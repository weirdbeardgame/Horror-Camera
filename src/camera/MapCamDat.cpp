#include "camera/MapCamDat.h"
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/resource_saver.hpp>

MapCamDat::MapCamDat() {
	type = 0;
	cam_type = 0;
	cam_kind = 0;
	id = 0;
	pointID = 0;
	p0 = Vector3i();
	p1 = Vector3i();
	p2 = Vector3i();
	p3 = Vector3i();

	roll = Vector2();
	fov = Vector2();

	String path = get_path();

	if (!path.is_empty() && path.begins_with("res://")) {
		ResourceSaver *save = ResourceSaver::get_singleton();
		save->save(this, path);
	}
}

void MapCamDat::_bind_methods() {
	ClassDB::bind_method(D_METHOD("GetCamType"), &MapCamDat::GetCamType);
	ClassDB::bind_method(D_METHOD("GetCamKind"), &MapCamDat::GetCamKind);
	ClassDB::bind_method(D_METHOD("SetCamType", "0"), &MapCamDat::SetCamType);
	ClassDB::bind_method(D_METHOD("SetCamKind", "0"), &MapCamDat::SetCamKind);
	ClassDB::bind_method(D_METHOD("GetID"), &MapCamDat::GetID);
	ClassDB::bind_method(D_METHOD("SetID", "0"), &MapCamDat::SetID);
	ClassDB::bind_method(D_METHOD("GetPointID"), &MapCamDat::GetPointID);
	ClassDB::bind_method(D_METHOD("SetPointID", "0"), &MapCamDat::SetPointID);
	ClassDB::bind_method(D_METHOD("GetP0"), &MapCamDat::GetP0);
	ClassDB::bind_method(D_METHOD("SetP0", "p0"), &MapCamDat::SetP0);
	ClassDB::bind_method(D_METHOD("GetP1"), &MapCamDat::GetP1);
	ClassDB::bind_method(D_METHOD("SetP1", "p1"), &MapCamDat::SetP1);
	ClassDB::bind_method(D_METHOD("GetP2"), &MapCamDat::GetP2);
	ClassDB::bind_method(D_METHOD("SetP2", "p2"), &MapCamDat::SetP2);
	ClassDB::bind_method(D_METHOD("GetP3"), &MapCamDat::GetP3);
	ClassDB::bind_method(D_METHOD("SetP3", "p3"), &MapCamDat::SetP3);
	ClassDB::bind_method(D_METHOD("GetRoll"), &MapCamDat::GetRoll);
	ClassDB::bind_method(D_METHOD("SetRoll", "roll"), &MapCamDat::SetRoll);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "camType"), "SetCamType", "GetCamType");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "camKind"), "SetCamKind", "GetCamKind");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "id"), "SetID", "GetID");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "pointID"), "SetPointID", "GetPointID");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3I, "p0"), "SetP0", "GetP0");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3I, "p1"), "SetP1", "GetP1");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3I, "p2"), "SetP2", "GetP2");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3I, "p3"), "SetP3", "GetP3");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "roll"), "SetRoll", "GetRoll");
}

void MapCamDat::SetCamKind(int k) {
	cam_kind = k;
	emit_changed();
}
void MapCamDat::SetType(int t) {
	type = t;
	emit_changed();
}
void MapCamDat::SetCamType(int t) {
	cam_type = t;
	emit_changed();
}
void MapCamDat::SetID(int i) {
	id = i;
	emit_changed();
}

void MapCamDat::SetPointID(int i) {
	pointID = i;
	emit_changed();
}

void MapCamDat::SetP0(Vector3i point) {
	p0 = point;
	emit_changed();
}
void MapCamDat::SetP1(Vector3i point) {
	p1 = point;
	emit_changed();
}
void MapCamDat::SetP2(Vector3i point) {
	p2 = point;
	emit_changed();
}
void MapCamDat::SetP3(Vector3i point) {
	p0 = point;
	emit_changed();
}
void MapCamDat::SetRoll(Vector2 r) {
	roll = r;
	emit_changed();
}
void MapCamDat::SetFOV(Vector2 f) {
	fov = f;
	emit_changed();
}
