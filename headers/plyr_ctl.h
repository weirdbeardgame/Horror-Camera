#pragma once
#include <godot_cpp/classes/character_body3d.hpp>

using namespace godot;

class Plyr_Wrk : public CharacterBody3D {
	GDCLASS(Plyr_Wrk, CharacterBody3D)

protected:
	static void _bind_methods() {}

public:
	u_int sta;
	u_int mvsta;
	Vector3 cp_old;
	float prot;
	/// Bounding Waist Position
	Vector3 bwp;
	Plyr_Wrk();
};