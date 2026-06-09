#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/vector3i.hpp>

using namespace godot;

enum CamKind : int {
	NORMAL,
	BATTLE,
	DRAMA,
	DOOR
};

class MapCamDat : public Resource {
	GDCLASS(MapCamDat, Resource)

private:
	RID map_cam_dat;
	int cam_kind;

protected:
	static void _bind_methods();

public:
	int type;
	int cam_type;
	int id;

	int pointID;

	// Points:
	Vector3i p0;
	Vector3i p1;
	Vector3i p2;
	Vector3i p3;

	Vector2 roll;
	Vector2 fov;

	int cd_edit_end;

	MapCamDat();

	void SetCamKind(int k);
	int GetCamKind() { return cam_kind; }
	int GetType() { return type; }
	void SetType(int t);
	int GetCamType() { return cam_type; }
	void SetCamType(int t);

	int GetID() { return id; }
	void SetID(int i);

	int GetPointID() { return pointID; }
	void SetPointID(int i);

	Vector3i GetP0() { return p0; }
	void SetP0(Vector3i point);
	Vector3i GetP1() { return p1; }
	void SetP1(Vector3i point);
	Vector3i GetP2() { return p2; }
	void SetP2(Vector3i point);
	Vector3i GetP3() { return p3; }
	void SetP3(Vector3i point);
	Vector2 GetRoll() { return roll; }
	void SetRoll(Vector2 r);
	Vector2 GetFOV() { return fov; }
	void SetFOV(Vector2 f);
};

VARIANT_ENUM_CAST(CamKind);