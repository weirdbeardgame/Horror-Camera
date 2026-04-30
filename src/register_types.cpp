#include "register_types.h"

#include "camera/SgCamera.h"
#include "editor/CameraPlugin.h"
#include "plyr_ctl.h"
#include <godot_cpp/godot.hpp>

void initialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level == godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(SgCamera);
		GDREGISTER_CLASS(MapCamDat);
		GDREGISTER_CLASS(Plyr_Wrk);
	} else if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		godot::UtilityFunctions::print("Register Plugins");

		GDREGISTER_CLASS(SgCameraPreview);
		GDREGISTER_CLASS(SgCameraInspectorPlugin);
		GDREGISTER_CLASS(SgCameraEditorPlugin);
		EditorPlugins::add_by_type<SgCameraEditorPlugin>();
	}
}

void uninitialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
// Initialization
GDExtensionBool GDE_EXPORT SgCamera_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
	init_obj.register_initializer(initialize_gdextension_types);
	init_obj.register_terminator(uninitialize_gdextension_types);

	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_EDITOR);

	return init_obj.init();
}
}