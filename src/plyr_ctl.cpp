#include "plyr_ctl.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/vector3.hpp>

Plyr_Wrk::Plyr_Wrk() {
	sta = 0;
	mvsta = 0;
	cp_old = Vector3();
	prot = 0.0f;
}

void Plyr_Wrk::_bind_methods() {
	ClassDB::bind_method(D_METHOD("GetSpeed"), &Plyr_Wrk::GetSpeed);
	ClassDB::bind_method(D_METHOD("SetSpeed", "s"), &Plyr_Wrk::SetSpeed);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "SetSpeed", "GetSpeed");
}

void Plyr_Wrk::_process(double delta) {
	if (!Engine::get_singleton()->is_editor_hint()) {
		if (Input::get_singleton()->is_action_pressed("ui_up")) {
			set_position(get_position() + Vector3(0, 0, -speed * (float)delta));
		}
		if (Input::get_singleton()->is_action_pressed("ui_down")) {
			set_position(get_position() + Vector3(0, 0, speed * (float)delta));
		}
	}
}