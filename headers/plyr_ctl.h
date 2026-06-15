#pragma once
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/collision_object3d.hpp>

using namespace godot;

class Plyr_Wrk : public CharacterBody3D {
	GDCLASS(Plyr_Wrk, CharacterBody3D)

private:
	float speed;

protected:
	static void _bind_methods();

public:
	u_int sta;
	u_int mvsta;
	Vector3 cp_old;
	float prot;
	/// Bounding Waist Position
	Vector3 bwp;
	Plyr_Wrk();

	float GetSpeed() { return speed; }
	void SetSpeed(float s) { speed = s; }

	virtual void _process(double delta) override;
};