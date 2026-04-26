#pragma once

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/godot.hpp>

namespace godot {

class CameraAttributes;
class Compositor;
class Environment;

typedef struct {
	Vector4 p;
	Vector4 i;
	float roll;
	float fov;
	float nearz;
	float farz;
	float ax;
	float ay;
	float cx;
	float cy;
	float zmin;
	float zmax;
	float pad[2];
	// ToDo: Find proper matrix types for these
	Vector4 vs[4];
	Vector4 vc[4];
	Vector4 vcv[4];
	Vector4 wv[4];
	Vector4 ws[4];
	Vector4 wc[4];
	Vector4 wcv[4];
	Vector4 zd;
	Vector4 yd;
} SgCAMERA;

typedef struct {
	u_char type;
	u_char id;
	u_short p0[3];
	u_short p1[3];
	u_short p2[3];
	u_short p3[3];
	float roll[2];
	float fov[2];
} MAP_CAM_DAT;

typedef struct {
	u_short no;
	u_short no_old;
	u_char kind;
	u_char type;
	u_char change;
	u_char pad;
	MAP_CAM_DAT *mcd;
} MAP_CAM_INFO;

typedef struct {
	Vector4 i;
	Vector4 p;
	Vector4 rot_y;
	Vector4 rot_x;
} CAM_ID_MOVE;

class HorrorCamera : public Node3D {
	GDCLASS(HorrorCamera, Node3D)

protected:
	static void _bind_methods();

private:
	void _on_camera_input(const Ref<InputEvent> &event);

public:
	void _ready() override;
	void _process(double delta) override;
	void print_type(const Variant &p_variant) const;
};

} //namespace godot