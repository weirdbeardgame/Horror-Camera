#ifndef EXAMPLE_REGISTER_TYPES_H
#define EXAMPLE_REGISTER_TYPES_H

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_gdextension_types(ModuleInitializationLevel p_level);
void uninitialize_gdextension_types(ModuleInitializationLevel p_level);

void register_editorplugin_types(ModuleInitializationLevel p_level);
void unregister_editorplugin_types(ModuleInitializationLevel p_level);

#endif // EXAMPLE_REGISTER_TYPES_H