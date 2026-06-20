#pragma once

#include "CameraMath.h"
#include "MapCamDat.h"
#include "plyr_ctl.h"

#include <sys/types.h>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/ray_cast3d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/variant/plane.hpp>
#include <godot_cpp/variant/projection.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/variant/typed_array.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/vector3.hpp>

// UI Includes
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/label3d.hpp>

#define CAMERA_NO (tbl)((tbl >> 8) & 0x1ff)
#define CAMERA_ACTIVE (tbl)(tbl & 0xff)
#define CAMERA_FOCUS_ENABLED (tbl)(tbl & 0x60000000)
#define CAMERA_FOCUS_DATA(tbl) ((tbl >> 28) & 0x3)

namespace godot {

class CameraAttributes;
class Compositor;
class Environment;

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

struct SgCameraData {
	Vector3 position;
	Vector3 interest;
	float roll;
	real_t fov;
	real_t nearz;
	real_t farz;
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

enum KeepAspect {
	KEEP_WIDTH,
	KEEP_HEIGHT
};

class SgCamera : public Node3D {
	GDCLASS(SgCamera, Node3D)

protected:
	void _update_camera();

	void _update_camera_mode();
	static void _bind_methods();
	void _notification(int p_what);
	virtual void _request_camera_update();
	Projection _get_camera_projection(real_t p_near) const;
	virtual Transform3D _get_adjusted_camera_transform(const Transform3D &p_xform) const;

private:
	bool isActive;

	int drm_cam_req;
	int drm_cam_tm;

	CAM_ID_MOVE cam_id_move;

	short int plyr_adj[4] = { 0 };

	bool force_change = false;
	bool current = true;

	int blend_frames;
	int camera_easing_mode;
	bool camera_focus_enabled;
	int camera_focus_data;

	/// The actual camera
	RID camera;

	RID attributes;

	friend class Viewport;
	Viewport *viewport;
	RenderingServer *server;

	Plane *boundingBox;

	Plyr_Wrk *plyr_wrk;

	// Culling layer mask
	uint32_t layers = 0xfffff;

	Ref<Environment> environment;
	Ref<Compositor> compositor;

	Ref<MapCamDat> mcd;
	SgCameraData cameraData;

	Node3D *interestNode;
	Projection projectionMatrix;

	// UI Stuff
	Control *statusControl;
	Label *pointStatus;

	KeepAspect keep_aspect;

	void _update_projection();
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

	void KonwakuCamCtrl();
	void NormalCameraCtrl();

	void DramaCameraReqCtrl();
	void ClearDramaCamReq();

	void CompleCameraPos(SgCameraData *tc, SgCameraData *oc, MAP_CAM_INFO *mci);
	int CompleReqChk(MAP_CAM_INFO *mci);

	int GetBlendFrames() { return blend_frames; }
	int GetEasingMode() { return camera_easing_mode; }
	bool GetFocusEnabled() { return camera_focus_enabled; }
	int GetFocusData() { return camera_focus_data; }

	Vector3 GetPosition() { return cameraData.position; }
	Vector3 GetInterest() { return cameraData.interest; }

	SgCameraData GetCameraData() { return cameraData; }

	short *GetPlyrAdj() { return plyr_adj; }

	void SetBlendFrames(int frames) { blend_frames = frames; }
	void SetEasingMode(int mode) { camera_easing_mode = mode; }
	void SetFocusEnabled(bool focusEnabled) { camera_focus_enabled = focusEnabled; }
	void SetFocusData(int data) { camera_focus_data = data; }

	float GetMCLocalPosPer(u_short cn, u_char kind, u_char id);
	Transform3D get_camera_transform() const;

	int GetCameraInfo(MAP_CAM_INFO *mci);
	void GetCameraData(u_char kind, MAP_CAM_INFO *mci);
	void SetCamPos0(SgCameraData *tc);
	void SetCamPos1(SgCameraData *tc);
	void SetCamPos2(SgCameraData *tc, MAP_CAM_INFO *mci);
	void SetCamPos3(SgCameraData *tc, MAP_CAM_INFO *mci);
	void SetCamPos4(SgCameraData *tc, MAP_CAM_INFO *mci);
	void SetCamPos5(SgCameraData *tc, MAP_CAM_INFO *mci);

	void SetPosition(Vector3 position) { cameraData.position = position; }
	void SetInterest(Vector3 interest) { cameraData.interest = interest; }

	// EDITOR CODE:
	void SetMapCamDat(Ref<MapCamDat> m) { mcd = m; }
	Ref<MapCamDat> GetMapCamDat() { return mcd; }

	void CameraIdMoveCtrl();
	void set_fov(real_t fov);
	real_t get_fov() { return cameraData.fov; }

	void set_nearz(real_t near);
	real_t get_nearz() { return cameraData.nearz; }

	void set_farz(real_t far);
	real_t get_farz() { return cameraData.farz; }

	int SetMapCamDat0(Ref<MapCamDat> mcd);
	int SetMapCamDat1(Ref<MapCamDat> mcd);
	int SetMapCamDat2(Ref<MapCamDat> mcd, int id);
	int SetMapCamDat3(Ref<MapCamDat> mcd, int id);
	int SetMapCamDat4(Ref<MapCamDat> mcd, int id);
	int SetMapCamDat5(Ref<MapCamDat> mcd);

	~SgCamera();
};

} //namespace godot