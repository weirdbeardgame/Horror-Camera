#pragma once

#include <godot_cpp/variant/plane.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace godot {
struct [[nodiscard]] ABB {
	Vector2 pointX;
	Vector2 pointY;
	Vector2 size;
};
} //namespace godot