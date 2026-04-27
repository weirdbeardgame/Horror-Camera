#pragma once

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/plane.hpp>
#include <godot_cpp/variant/projection.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <godot_cpp/core/class_db.hpp>

#include <type_traits>

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
	Projection vs;
	Projection vc;
	Projection vcv;
	Projection wv;
	Projection ws;
	Projection wc;
	Projection wcv;
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
	Vector2 roll;
	Vector2 fov;
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
	bool isActive;

	int drm_cam_req;
	int drm_cam_tm;

public:
	void _ready() override;
	void _process(double delta) override;
	void print_type(const Variant &p_variant) const;

	void SetCamPos0(SgCAMERA *tc, MAP_CAM_INFO *mci);
	void SetCamPos1(SgCAMERA *tc, MAP_CAM_INFO *mci);
	void SetCamPos2(SgCAMERA *tc, MAP_CAM_INFO *mci);
	void SetCamPos3(SgCAMERA *tc, MAP_CAM_INFO *mci);
	void SetCamPos4(SgCAMERA *tc, MAP_CAM_INFO *mci);
	void SetCamPos5(SgCAMERA *tc, MAP_CAM_INFO *mci);

	void NormalCameraCtrl();

	void DramaCameraReqCtrl();
	void ClearDramaCamReq();
};

} //namespace godot