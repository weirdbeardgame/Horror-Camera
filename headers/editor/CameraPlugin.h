#pragma once
#include "camera/MapCamDat.h"
#include "camera/SgCamera.h"
#include <godot_cpp/classes/aspect_ratio_container.hpp>
#include <godot_cpp/classes/editor_inspector_plugin.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/foldable_container.hpp>
#include <godot_cpp/classes/margin_container.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/option_button.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>

using namespace godot;

class SgCameraPreview : public MarginContainer {
	GDCLASS(SgCameraPreview, MarginContainer)

protected:
	static void _bind_methods() {}

private:
	SgCamera *camera = nullptr;
	AspectRatioContainer *centering_container = nullptr;
	SubViewport *sub_viewport = nullptr;
	bool camera_preview_folded;

	Size2i get_camera_viewport_size(SgCamera *p_camera);

	void _update_sub_viewport_size();
	void _project_settings_changed();
	void _toggle_folding(bool p_folded);

public:
	SgCameraPreview() = default;
	SgCameraPreview(SgCamera *p_camera);
};

class SgCameraInspectorPlugin : public EditorInspectorPlugin {
	GDCLASS(SgCameraInspectorPlugin, EditorInspectorPlugin);

private:
	SgCamera *camera = nullptr;
	Ref<MapCamDat> dat = nullptr;

protected:
	static void _bind_methods() {}

public:
	virtual bool _can_handle(Object *p_object) const override;
	virtual void _parse_begin(Object *p_object) override;
	virtual void _parse_end(Object *p_object) override;
	virtual bool _parse_property(Object *p_object, Variant::Type p_type, const String &p_name, PropertyHint p_hint_type, const String &p_hint_string, BitField<PropertyUsageFlags> p_usage_flags, bool p_wide) override;
};

class SgCameraEditorPlugin : public EditorPlugin {
	GDCLASS(SgCameraEditorPlugin, EditorPlugin)

protected:
	static void _bind_methods() {}
	Ref<SgCameraInspectorPlugin> plugin;

public:
	String get_plugin_name() const { return "SgCamera"; }
	virtual void edit(Object *p_object);
	virtual bool handles(Object *p_object) const;
	virtual void make_visible(bool p_visible);

	SgCameraEditorPlugin();
	~SgCameraEditorPlugin();
};
