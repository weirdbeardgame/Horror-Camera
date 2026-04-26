#include "HorrorCamera.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/engine_ptrcall.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/classes/camera_attributes.hpp>
#include <godot_cpp/classes/compositor.hpp>
#include <godot_cpp/classes/environment.hpp>

using namespace godot;

void HorrorCamera::_bind_methods() {
}

void HorrorCamera::_ready() {
	godot::UtilityFunctions::print("my extension is initialised");
}

void HorrorCamera::_process(double delta) {
	// Example of rotating the camera over time
	rotate_y(delta * 0.5); // Rotate around the Y-axis
}

void HorrorCamera::print_type(const Variant &p_variant) const {
	print_line(vformat("Type: %d", p_variant.get_type()));
}
