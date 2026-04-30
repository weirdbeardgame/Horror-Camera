#pragma once

#include "CameraMath.h"
#include "plyr_ctl.h"

#include <sys/types.h>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/variant/plane.hpp>
#include <godot_cpp/variant/projection.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/vector3i.hpp>

#include <godot_cpp/core/class_db.hpp>

#define CAMERA_NO (tbl)((tbl >> 8) & 0x1ff)
#define CAMERA_ACTIVE (tbl)(tbl & 0xff)
#define CAMERA_FOCUS_ENABLED (tbl)(tbl & 0x60000000)
#define CAMERA_FOCUS_DATA(tbl) ((tbl >> 28) & 0x3)

namespace godot {

class CameraAttributes;
class Compositor;
class Environment;

enum CamKind : int {
	NORMAL,
	BATTLE,
	DRAMA,
	DOOR
};

class MapCamDat : public Resource {
	GDCLASS(MapCamDat, Resource)

protected:
	static void _bind_methods();

public:
	int type;
	int cam_type;
	int id;

	// Points:
	Vector3i p0;
	Vector3i p1;
	Vector3i p2;
	Vector3i p3;

	Vector2 roll;
	Vector2 fov;

	int cd_edit_end;

	MapCamDat();
	void emit_changed();

	int GetType() { return type; }
	void SetType(int t) { type = t; }
	int GetCamType() { return cam_type; }
	void SetCamType(int t) { cam_type = t; }
	int GetID() { return id; }
	void SetID(int i) { type = i; }
	Vector3i GetP0() { return p0; }
	void SetP0(Vector3i point) { p0 = point; }
	Vector3i GetP1() { return p1; }
	void SetP1(Vector3i point) { p1 = point; }
	Vector3i GetP2() { return p2; }
	void SetP2(Vector3i point) { p2 = point; }
	Vector3i GetP3() { return p3; }
	void SetP3(Vector3i point) { p0 = point; }
	Vector2 GetRoll() { return roll; }
	void SetRoll(Vector2 r) { roll = r; }
	Vector2 GetFOV() { return fov; }
	void SetFOV(Vector2 f) { fov = f; }
};

struct MAP_CAM_INFO {
	u_short no;
	u_short no_old;
	int kind;
	int type;
	int change;
	int pad;
	Ref<MapCamDat> mcd;
};

struct CAM_ID_MOVE {
	Vector3 i;
	Vector3 p;
	Vector3 rot_y;
	Vector3 rot_x;
};

static int cam_kind;
static int cam_type;
static int cd_step;
static int cam_id;
static short int plyr_adj[4];

struct SgCameraData {
	Vector3 p;
	Vector3 i;
	float roll;
	float fov = DEG2RAD(44.0f);
	float nearz;
	float farz;
	float ax;
	float ay;
	float cx;
	float cy;
	float zmin;
	float zmax;
	float pad[2];
	Transform3D vs;
	Transform3D vc;
	Transform3D vcv;
	Transform3D wv;
	Transform3D ws;
	Transform3D wc;
	Transform3D wcv;
	Vector3 zd;
	Vector3 yd;
};

class SgCamera : public Node3D {
	GDCLASS(SgCamera, Node3D)

protected:
	void _update_camera();
	virtual void _request_camera_update();
	void _update_camera_mode();

	static void _bind_methods();

	void _notification(int p_what);

private:
	bool isActive;

	int drm_cam_req;
	int drm_cam_tm;

	CAM_ID_MOVE cam_id_move;

	CamKind cam_kind;
	int cam_type = 0;
	int cd_step = 0;
	int cam_id = 0;
	short int plyr_adj[4] = { 0 };

	bool force_change = false;
	bool current = true;

	int blend_frames;
	int camera_id;
	int camera_focus_data;
	int camera_easing_mode;
	bool camera_focus_enabled;

	RID scenario_id;
	RID camera_rid;
	Ref<Environment> environment;
	Ref<CameraAttributes> attributes;
	Ref<Compositor> compositor;

	friend class Viewport;
	Viewport *viewport;
	RenderingServer *server;

	Plyr_Wrk *plyr_wrk;

	// Culling layer mask
	uint32_t layers = 0xfffff;

	Ref<MapCamDat> mcd;

	SgCameraData camera;

	void _attributes_changed();

public:
	SgCamera();

	void _ready() override;
	void _process(double delta) override;
	void print_type(const Variant &p_variant) const;

	void make_current();
	void clear_current(bool p_enable_next = true);
	void set_current(bool p_enabled);
	bool is_current() const;
	RID get_camera() const;
	void set_perspective(real_t p_fovy_degrees, real_t p_z_near, real_t p_z_far);
	void _on_body_entered(Node3D *body);

	void CameraMain();
	void KonwakuCamCtrl();
	void NormalCameraCtrl();

	void DramaCameraReqCtrl();
	void ClearDramaCamReq();

	void CompleCameraPos(SgCameraData *tc, SgCameraData *oc, MAP_CAM_INFO *mci);
	int CompleReqChk(MAP_CAM_INFO *mci);

	void StopQuake();
	void ReqQuake(u_int furn_id, float pow, u_int time, u_int loop, u_int stop_flg);
	int QuakeCamera();

	int GetBlendFrames() { return blend_frames; }
	int GetCameraID() { return camera_id; }
	int GetFocusData() { return camera_focus_data; }
	int GetEasingMode() { return camera_easing_mode; }
	bool GetFocusEnabled() { return camera_focus_enabled; }

	void SetBlendFrames(int frames) { blend_frames = frames; }
	void SetCameraID(int id) { camera_id = id; }
	void SetFocusData(int data) { camera_focus_data = data; }
	void SetEasingMode(int mode) { camera_easing_mode = mode; }
	void SetFocusEnabled(bool focusEnabled) { camera_focus_enabled = focusEnabled; }

	float GetMCLocalPosPer(u_short cn, u_char kind, u_char id);
	Transform3D _get_adjusted_camera_transform(const Transform3D &p_xform);
	Transform3D get_camera_transform() const;

	int GetCameraInfo(MAP_CAM_INFO *mci);
	void GetCameraData(u_char kind, MAP_CAM_INFO *mci);
	void SetCamPos0(SgCameraData *tc, MAP_CAM_INFO *mci);
	void SetCamPos1(SgCameraData *tc, MAP_CAM_INFO *mci);
	void SetCamPos2(SgCameraData *tc, MAP_CAM_INFO *mci);
	void SetCamPos3(SgCameraData *tc, MAP_CAM_INFO *mci);
	void SetCamPos4(SgCameraData *tc, MAP_CAM_INFO *mci);
	void SetCamPos5(SgCameraData *tc, MAP_CAM_INFO *mci);

	int SetMapCamDat0(Ref<MapCamDat> mcd);
	int SetMapCamDat1(Ref<MapCamDat> mcd);
	int SetMapCamDat2(Ref<MapCamDat> mcd, int id);
	int SetMapCamDat3(Ref<MapCamDat> mcd, int id);
	int SetMapCamDat4(Ref<MapCamDat> mcd, int id);
	int SetMapCamDat5(Ref<MapCamDat> mcd);

	void SetMapCamDat(Ref<MapCamDat> m) { mcd = m; }
	Ref<MapCamDat> GetMapCamDat() { return mcd; }

	void CameraIdMoveCtrl();

	~SgCamera();
};

} //namespace godot