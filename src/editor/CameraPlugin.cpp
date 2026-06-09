#include "editor/CameraPlugin.h"
#include "camera/MapCamDat.h"
#include "camera/SgCamera.h"
#include "plyr_ctl.h"

#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/vector2i.hpp>

// UI Includes:
#include <godot_cpp/classes/aspect_ratio_container.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/canvas_item.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/slider.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/sub_viewport_container.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/window.hpp>

SgCameraPreview::SgCameraPreview(SgCamera *p_camera) {
	camera = p_camera;

	FoldableContainer *folder = memnew(FoldableContainer);
	folder->set_title("Camera Preview");
	folder->set_title_text_overrun_behavior(TextServer::OVERRUN_TRIM_ELLIPSIS);
	folder->set_folded(camera_preview_folded);
	folder->connect("folding_changed", callable_mp(this, &SgCameraPreview::_toggle_folding));
	add_child(folder);

	centering_container = memnew(AspectRatioContainer);
	centering_container->set_custom_minimum_size(Size2(0.0, 256.0) * EditorInterface::get_singleton()->get_editor_scale());
	folder->add_child(centering_container);

	SubViewportContainer *sub_viewport_container = memnew(SubViewportContainer);
	sub_viewport_container->set_stretch(true);
	sub_viewport_container->set_texture_filter(TEXTURE_FILTER_NEAREST_WITH_MIPMAPS);
	centering_container->add_child(sub_viewport_container);

	sub_viewport = memnew(SubViewport);
	sub_viewport_container->add_child(sub_viewport);

	RenderingServer::get_singleton()->viewport_attach_camera(sub_viewport->get_viewport_rid(), camera->get_camera());

	sub_viewport->set_use_hdr_2d(true);

	ProjectSettings::get_singleton()->connect("settings_changed", callable_mp(this, &SgCameraPreview::_project_settings_changed));
	_update_sub_viewport_size();
}

Size2i SgCameraPreview::get_camera_viewport_size(SgCamera *p_camera) {
	Viewport *viewport = p_camera->get_viewport();

	Window *window = Object::cast_to<Window>(viewport);
	if (window) {
		return window->get_size();
	}

	SubViewport *sub_viewport = Object::cast_to<SubViewport>(viewport);
	ERR_FAIL_NULL_V(sub_viewport, Size2i());

	if (sub_viewport == EditorInterface::get_singleton()->get_edited_scene_root()->get_viewport()) {
		return Size2i(ProjectSettings::get_singleton()->get_setting("display/window/size/viewport_width"), ProjectSettings::get_singleton()->get_setting("display/window/size/viewport_height"));
	}

	return sub_viewport->get_size();
}

void SgCameraPreview::_update_sub_viewport_size() {
	const Size2i camera_size = get_camera_viewport_size(camera);
	centering_container->set_ratio(camera_size.aspect());
}

void SgCameraPreview::_toggle_folding(bool p_folded) {
	camera_preview_folded = p_folded;
}

void SgCameraPreview::_project_settings_changed() {
	if (ProjectSettings::get_singleton()->check_changed_settings_in_group("display/window/size")) {
		_update_sub_viewport_size();
	}
}

void SgCameraEditorPlugin::edit(Object *p_object) {
	//Node3DEditor::get_singleton()->set_can_preview(Object::cast_to<Camera3D>(p_object));
}

bool SgCameraEditorPlugin::handles(Object *p_object) const {
	return p_object->is_class("Camera3D");
}

void SgCameraEditorPlugin::make_visible(bool p_visible) {
	if (!p_visible) {
		//Node3DEditor::get_singleton()->set_can_preview(nullptr);
	}
}

SgCameraEditorPlugin::SgCameraEditorPlugin() {
	plugin.instantiate();
	add_inspector_plugin(plugin);
}

SgCameraEditorPlugin::~SgCameraEditorPlugin() {
	remove_inspector_plugin(plugin);
}

bool SgCameraInspectorPlugin::_parse_property(Object *p_object, Variant::Type p_type, const String &p_name, PropertyHint p_hint_type, const String &p_hint_string, BitField<PropertyUsageFlags> p_usage_flags, bool p_wide) {
	if (Object::cast_to<SgCamera>(p_object)) {
		SgCamera *cam = Object::cast_to<SgCamera>(p_object);

		if (!cam)
			return false;
	}

	if (Object::cast_to<MapCamDat>(p_object)) {
		MapCamDat *mcd = Object::cast_to<MapCamDat>(p_object);
		if (p_name == "camType") {
			Label *label = memnew(Label);
			label->set_text("Cam Type: ");

			OptionButton *opt = memnew(OptionButton);
			opt->add_item("0");
			opt->add_item("1");
			opt->add_item("2");
			opt->add_item("3");
			opt->add_item("4");
			opt->add_item("5");
			opt->select(mcd->GetCamType());

			opt->connect("item_selected", callable_mp(mcd, &MapCamDat::SetCamType));
			add_custom_control(label);
			add_property_editor("camType", opt);
			return true;
		}

		if (p_name == "camKind") {
			Label *label = memnew(Label);
			label->set_text("Cam Kind: ");

			OptionButton *opt = memnew(OptionButton);
			opt->add_item("NORMAL");
			opt->add_item("BATTLE");
			opt->add_item("DRAMA");
			opt->add_item("DOOR");
			opt->select(mcd->GetCamKind());

			opt->connect("item_selected", callable_mp(mcd, &MapCamDat::SetCamKind));
			add_custom_control(label);
			add_property_editor("camKind", opt);
			return true;
		}
	}
	return false;
}

bool SgCameraInspectorPlugin::_can_handle(Object *p_object) const {
	return Object::cast_to<SgCamera>(p_object) != nullptr || Object::cast_to<MapCamDat>(p_object) != nullptr;
}

void SgCameraInspectorPlugin::_parse_begin(Object *p_object) {
	camera = Object::cast_to<SgCamera>(p_object);
	if (!camera)
		return;

	dat = camera->GetMapCamDat();

	SgCameraPreview *preview = memnew(SgCameraPreview(camera));
	add_custom_control(preview);
}

void SgCameraInspectorPlugin::_parse_end(Object *p_object) {
	camera = Object::cast_to<SgCamera>(p_object);
	if (!camera)
		return;

	dat = camera->GetMapCamDat();

	Button *setPoint = memnew(Button);
	setPoint->set_text("Set Point");
	setPoint->connect("pressed", callable_mp(camera, &SgCamera::CameraIdMoveCtrl));
	add_custom_control(setPoint);
}