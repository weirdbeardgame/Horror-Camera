#include "camera/SgCamera.h"
#include "camera/CameraEffects.h"
#include "camera/CameraMath.h"
#include "camera/MapCamDat.h"
#include "plyr_ctl.h"
#include <sys/types.h>

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/ray_cast3d.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/basis.hpp>
#include <godot_cpp/variant/plane.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <godot_cpp/classes/display_server.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/viewport.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/engine_ptrcall.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/classes/area3d.hpp>
#include <godot_cpp/classes/camera_attributes.hpp>
#include <godot_cpp/classes/compositor.hpp>
#include <godot_cpp/classes/environment.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/vector3i.hpp>

using namespace godot;

u_short drm_cam_no[12] = {
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
};

static u_short cam_no_save = 0;
static u_short compling = 0xFFFF;
static u_char cam_mode_old = 0;
static u_char req_rc_fade = 0;
static u_char drm_cam_req = 0;
static u_short drm_cam_tm = 0;

//static CameraEffects effects;

SgCamera::SgCamera() {
	isActive = true;
	drm_cam_req = 0;
	drm_cam_tm = 0;

	blend_frames = 0;
	camera_easing_mode = 0;
	camera_focus_enabled = false;
	camera_focus_data = 0;

	camera = SgCameraData();
	server = RenderingServer::get_singleton();
	plyr_wrk = Object::cast_to<Plyr_Wrk>(get_parent());

	mcd.instantiate();
	interestPoint = memnew(RayCast3D);
	camera_rid = server->camera_create();

	boundingBox = memnew(Plane);

	statusControl = memnew(Control);
	pointStatus = memnew(Label);

	add_child(statusControl);
	statusControl->set_anchors_preset(Control::PRESET_TOP_RIGHT);

	statusControl->add_child(pointStatus);
	pointStatus->set_offset(SIDE_RIGHT, -200);

	set_perspective(44.0f, 0.1f, 32768.0f);
	RenderingServer::get_singleton()->camera_set_cull_mask(camera_rid, layers);

	if (mcd.is_valid()) {
		godot::UtilityFunctions::print("Valid attributes");
		server->camera_set_camera_attributes(camera_rid, mcd->get_rid());
	}

	add_child(interestPoint);

	interestPoint->set_enabled(true);
	interestPoint->set_target_position(Vector3(0, 0, -100));

	set_notify_transform(true);
	set_disable_scale(true);
}

void SgCamera::_bind_methods() {
	ClassDB::bind_method(D_METHOD("make_current"), &SgCamera::make_current);
	ClassDB::bind_method(D_METHOD("clear_current"), &SgCamera::clear_current);
	ClassDB::bind_method(D_METHOD("set_current", "enabled"), &SgCamera::set_current);

	ClassDB::bind_method(D_METHOD("is_current"), &SgCamera::is_current);
	ClassDB::bind_method(D_METHOD("SetMapCamDat", "mcd"), &SgCamera::SetMapCamDat);

	ClassDB::bind_method(D_METHOD("get_camera"), &SgCamera::get_camera);

	ClassDB::bind_method(D_METHOD("GetMapCamDat"), &SgCamera::GetMapCamDat);

	ClassDB::bind_method(D_METHOD("get_fov"), &SgCamera::get_fov);
	ClassDB::bind_method(D_METHOD("set_fov", "fov"), &SgCamera::set_fov);
	ClassDB::bind_method(D_METHOD("get_farz"), &SgCamera::get_farz);
	ClassDB::bind_method(D_METHOD("set_farz", "farz"), &SgCamera::set_farz);
	ClassDB::bind_method(D_METHOD("set_nearz", "nearz"), &SgCamera::set_nearz);
	ClassDB::bind_method(D_METHOD("get_nearz"), &SgCamera::get_nearz);
	ClassDB::bind_method(D_METHOD("set_perspective", "fov", "z_near", "z_far"), &SgCamera::set_perspective);

	ClassDB::bind_method(D_METHOD("GetFocusData"), &SgCamera::GetFocusData);
	ClassDB::bind_method(D_METHOD("GetEasingMode"), &SgCamera::GetEasingMode);
	ClassDB::bind_method(D_METHOD("GetBlendFrames"), &SgCamera::GetBlendFrames);
	ClassDB::bind_method(D_METHOD("GetFocusEnabled"), &SgCamera::GetFocusEnabled);

	ClassDB::bind_method(D_METHOD("SetFocusData", "data"), &SgCamera::SetFocusData);
	ClassDB::bind_method(D_METHOD("SetEasingMode", "mode"), &SgCamera::SetEasingMode);
	ClassDB::bind_method(D_METHOD("SetBlendFrames", "frames"), &SgCamera::SetBlendFrames);
	ClassDB::bind_method(D_METHOD("SetFocusEnabled", "focusEnabled"), &SgCamera::SetFocusEnabled);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "current"), "set_current", "is_current");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "blend_frames"), "SetBlendFrames", "GetBlendFrames");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "camera_easing_mode"), "SetEasingMode", "GetEasingMode");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "camera_focus_enabled"), "SetFocusEnabled", "GetFocusEnabled");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "camera_focus_data"), "SetFocusData", "GetFocusData");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mcd", PROPERTY_HINT_RESOURCE_TYPE, "MapCamDat"), "SetMapCamDat", "GetMapCamDat");

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fov", PROPERTY_HINT_RANGE, "1,179,0.1,degrees"), "set_fov", "get_fov");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "nearz", PROPERTY_HINT_RANGE, "0.001,10,0.001,or_greater,exp,suffix:m"), "set_nearz", "get_nearz");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "farz", PROPERTY_HINT_RANGE, "0.01,4000,0.01,or_greater,exp,suffix:m"), "set_farz", "get_farz");
}

void SgCamera::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_WORLD: {
			viewport = get_viewport();
			if (Engine::get_singleton()->is_editor_hint()) {
				return;
			}

			if (current && camera_rid.is_valid()) {
				godot::UtilityFunctions::print("Attach Camera Safely");
				server->viewport_attach_camera(viewport->get_viewport_rid(), camera_rid);
			}

#ifdef TOOLS_ENABLED
			if (Engine::get_singleton()->is_editor_hint()) {
				viewport->connect(SNAME("size_changed"), callable_mp((Node3D *)this, &SgCamera::update_gizmos));
			}
#endif

		} break;

		case NOTIFICATION_TRANSFORM_CHANGED: {
			if (camera_rid.is_valid()) {
				cam_id_move.p = get_global_position();
				Transform3D t = get_global_transform();
				t.set_origin(cam_id_move.p);

				set_transform(t);

				//RenderingServer::get_singleton()->camera_set_transform(camera_rid, t);
			}
			_request_camera_update();
		} break;

		case NOTIFICATION_EXIT_WORLD: {
			if (!is_part_of_edited_scene()) {
				if (is_current()) {
					clear_current();
					current = true; //keep it true

				} else {
					current = false;
				}
			}

			if (viewport) {
				viewport = nullptr;
			}
		} break;
	}
}

Projection SgCamera::_get_camera_projection(real_t p_near) const {
	Size2 viewport_size = get_viewport()->get_visible_rect().size;
	Projection cm;

	cm.set_perspective(camera.fov, viewport_size.aspect(), p_near, camera.farz, keep_aspect == KEEP_WIDTH);

	return cm;
}

void SgCamera::DramaCameraReqCtrl() {
	u_char i;
	u_char chk;

	if (drm_cam_req != 0) {
		if (drm_cam_tm != 0) {
			drm_cam_tm--;

			if (drm_cam_tm == 0) {
				ClearDramaCamReq();
			}
		}

		if (drm_cam_req != 0) {
			for (i = 0, chk = 0; i < 12; i++) {
				if (drm_cam_no[i] == 0xffff) {
					break;
				}

				//if (plyr_wrk->pr_info.camera_drm == drm_cam_no[i]) {
				//	chk = 1;
				//	break;
				//}
			}

			if (!chk) {
				ClearDramaCamReq();
			}
		}
	}
}

void SgCamera::ClearDramaCamReq() {
	int i;

	drm_cam_req = 0;
	drm_cam_tm = 0;

	for (i = 0; i < 12; i++) {
		drm_cam_no[i] = -1;
	}
}

void SgCamera::CompleCameraPos(SgCameraData *tc, SgCameraData *oc, MAP_CAM_INFO *mci) {
	Vector3 tc0;
	Vector3 tc1;
	float comple_div;
	float tca0;
	float tca1;
	u_char i;
	u_char chk;
	u_int *tbl;
	int cam_kind;
	static u_char rmax;
	static u_char pmax;
	static u_char comple_cnt;
	static u_char comple_spd_dwn;
	static u_char comple_dwn_tm;
	static Vector3 comple_adjr = { 0.0f, 0.0f, 0.0f };
	static Vector3 comple_adjp = { 0.0f, 0.0f, 0.0f };
	static float comple_adjfov = 0.0f;
	static float comple_adjroll = 0.0f;

	// Fill in SG_Cam
	if (CompleReqChk(mci) != 0) {
		// ToDo, reverse missing logic here:

		//

		/*if (camera_focus_enabled) {
			switch (camera_focus_data) {
				case 1:
					effects.CallFocus(0, 1, 50);
					break;
				case 2:
					effects.CallFocus(0, 2, 70);
					break;
				case 3:
					effects.CallFocus(0, 3, 100);
					break;
			}
		}*/

		compling = mci->no;

		if (blend_frames) {
			comple_div = blend_frames; // Blend Frames
			comple_cnt = comple_div;
			comple_spd_dwn = camera_easing_mode;

			if (comple_spd_dwn != 0) {
				comple_dwn_tm = comple_cnt / 2;
			}

			comple_adjr = tc->interest + camera.interest;
			comple_adjr /= comple_div;

			rmax = (fabsf(comple_adjr[0]) > fabsf(comple_adjr[1]))
					? 0
					: 1;
			rmax = (fabsf(comple_adjr[rmax]) > fabsf(comple_adjr[2]))
					? rmax
					: 2;

			comple_adjp = tc->position + camera.position;
			comple_adjp /= comple_div;

			pmax = (fabsf(comple_adjr[0]) > fabsf(comple_adjr[1]))
					? 0
					: 1;
			pmax = (fabsf(comple_adjr[pmax]) > fabsf(comple_adjr[2]))
					? pmax
					: 2;

			comple_adjfov = (tc->fov - camera.fov);
			comple_adjfov /= comple_div;

			comple_adjroll = tc->roll - camera.roll;

			RotLimitChk(&comple_adjroll);

			comple_adjroll = comple_adjroll / comple_div;
		}
	} else {
		if (mci->type == 2 || mci->type == 3 || mci->type == 4) {
			if (comple_spd_dwn) {
				comple_adjp = tc->position - oc->position;
			}

			tc0 = Vector3();

			tc1 = comple_adjp;
			tca0 = tc0.distance_to(tc1);
			tc1 = tc->position - camera.position;

			tc0 = Vector3();

			tca1 = tc0.distance_to(tc1);

			if (tca0 == 0.0f) {
				tca0 = 1.0f;
			}

			comple_adjr = tc->interest / oc->interest;
		}
		if (mci->type == 0x4) {
			if (comple_spd_dwn == 3) {
				comple_adjr = tc->interest - oc->interest;
			} else {
				tc0 = Vector3();
				tc1 = comple_adjr;

				tca0 = tc0.distance_to(tc1);
				tc1 = tc->interest - camera.interest;

				tc0 = Vector3();
				tca1 = tc0.distance_to(tc1);

				if (tca0 != 0.0f) {
					comple_adjr = tc1 / (tca1 / tca0);
				}
			}
		}
	}

	if (compling == 0xffff) {
		return;
	}

	if (((comple_spd_dwn & 0x1)) || comple_spd_dwn == 2) {
		if (comple_cnt <= comple_dwn_tm) {
			if (fabsf(comple_adjr[rmax]) > 0.1f) {
				comple_adjr *= 0.98f;
			}

			if (fabsf(comple_adjp[pmax]) > 0.1f) {
				comple_adjp *= 0.98f;
			}
		}
	}

	chk = 0;
	tc0 = tc->interest;

	for (i = 0; i < 3; i++) {
		if (comple_adjr[i] > 0.0f && tc->interest[i] > camera.interest[i]) {
			tc->interest[i] = camera.interest[i] + comple_adjr[i];

			if (tc0[i] <= tc->interest[i]) {
				tc->interest = tc0;
				comple_adjr = Vector3();
				break;
			}

			chk = 1;
		} else {
			if (comple_adjr[i] < 0.0f && tc->interest[i] < camera.interest[i]) {
				tc->interest[i] = camera.interest[i] + comple_adjr[i];

				if (tc->interest[i] <= tc0[i]) {
					tc->interest = tc0;
					comple_adjr = Vector3();
					break;
				}

				chk = 1;
			}
		}
	}

	tc0 = tc->position;

	for (i = 0; i < 3; i++) {
		if (comple_adjp[i] > 0.0f && tc->position[i] > camera.position[i]) {
			tc->position[i] = camera.position[i] + comple_adjp[i];

			if (tc0[i] <= tc->position[i]) {
				tc->position = tc0;

				comple_adjp = Vector3();

				break;
			}

			chk = 1;
		} else {
			if (comple_adjp[i] < 0.0f && tc->position[i] < camera.position[i]) {
				tc->position[i] = camera.position[i] + comple_adjp[i];

				if (tc->position[i] <= tc0[i]) {
					tc->position = tc0;
					comple_adjp = Vector3();
					break;
				}
				chk = 1;
			}
		}
	}

	if ((comple_adjfov > 0.0f && tc->fov > camera.fov) || (comple_adjfov < 0.0f && tc->fov < camera.fov)) {
		tc->fov = camera.fov + comple_adjfov;
		chk = 1;
	} else {
		comple_adjfov = 0.0f;
	}

	if ((comple_adjroll > 0.0f && tc->roll > camera.roll) || (comple_adjroll < 0.0f && tc->roll < camera.roll)) {
		tc->roll = camera.roll + comple_adjroll;
		chk = 1;
	} else {
		comple_adjroll = 0.0f;
	}

	if (chk) {
		if (comple_cnt != 0) {
			comple_cnt--;
		}
	} else {
		compling = -1;
	}
}

int SgCamera::CompleReqChk(MAP_CAM_INFO *mci) {
	int req;

	req = 0;

	if (mci->kind == 3) {
		return 0;
	}

	if (compling != 0xFFFF) {
		if (compling != mci->no) {
			compling = 0xFFFF;
		}
	}

	if (compling == 0xFFFF) {
		if (mci->change != 0) {
			if (mci->no_old != 0xFFFF) {
				req = 1;
			}
		}
	}

	return req;
}

void SgCamera::GetCameraData(u_char kind, MAP_CAM_INFO *mci) {
	mci->mcd = mcd;
	mci->type = mcd->cam_type;
}

void SgCamera::NormalCameraCtrl() {
	MAP_CAM_INFO mci;
	int cam_id;
	SgCameraData tc;
	SgCameraData tc2;
	static SgCameraData oc;

	int debug = false;
	int cd_edit_end;

	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}

	DramaCameraReqCtrl();

	if (GetCameraInfo(&mci) != 0) {
		return;
	}

	if (mci.change != 0) {
		plyr_wrk->cp_old[0] = camera.position[0];
		plyr_wrk->cp_old[1] = camera.position[1];
		plyr_wrk->cp_old[2] = camera.position[2];
	}

	// Type of angles?
	switch (mci.type) {
		case 0:
			SetCamPos0(&tc);
			break;
		case 1:
			SetCamPos1(&tc);
			break;
		case 2:
			SetCamPos2(&tc, &mci);
			break;
		case 3:
			SetCamPos3(&tc, &mci);
			break;
		case 4:
			SetCamPos4(&tc, &mci);
			break;
		case 5:
			SetCamPos5(&tc, &mci);
			break;
	}
	tc2 = tc;

	CompleCameraPos(&tc, &oc, &mci);

	if (plyr_wrk)
		plyr_wrk->prot = GetTrgtRotY(camera.position, plyr_wrk->get_global_position());

	oc = tc2;

	camera.interest = tc.interest;
	camera.position = tc.position;

	camera.roll = tc.roll;
	camera.fov = tc.fov;

	Transform3D trans = get_global_transform();
	trans.set_origin(camera.position);

	set_transform(trans);
}

void SgCamera::_ready() {
	godot::UtilityFunctions::print("my extension is initialised");
	viewport = get_viewport();
	if (get_owner() != nullptr)
		// ToDo, GetBody from Area3D
		plyr_wrk = get_owner()->get_node<Plyr_Wrk>("plyr_wrk");
}

void SgCamera::_request_camera_update() {
	_update_camera();
}

void SgCamera::set_fov(real_t fov) {
	camera.fov = fov;
	set_perspective(camera.fov, camera.nearz, camera.farz);
}

void SgCamera::set_nearz(real_t near) {
	camera.nearz = near;
	set_perspective(camera.fov, camera.nearz, camera.farz);
}

void SgCamera::set_farz(real_t far) {
	camera.farz = far;
	set_perspective(camera.fov, camera.nearz, camera.farz);
}

void SgCamera::print_type(const Variant &p_variant) const {
	print_line(vformat("Type: %d", p_variant.get_type()));
}

RID SgCamera::get_camera() const {
	return camera_rid;
}

void SgCamera::_update_camera_mode() {
	set_process_internal(true);
	set_physics_process_internal(false);
}

Transform3D SgCamera::_get_adjusted_camera_transform(const Transform3D &p_xform) const {
	Transform3D tr = p_xform.orthonormalized();
	tr.origin += tr.basis.get_column(1); //* v_offset;
	tr.origin += tr.basis.get_column(0); //* h_offset;
	return tr;
}

Transform3D SgCamera::get_camera_transform() const {
	return _get_adjusted_camera_transform(get_global_transform());
}

void SgCamera::_process(double delta) {
	//godot::UtilityFunctions::print("Interest: ", camera.interest);

	Transform3D t;

	t = get_global_transform();

	if (Engine::get_singleton()->is_editor_hint()) {
		cam_id_move.i = interestPoint->to_global(Vector3()).direction_to(
				interestPoint->to_global(interestPoint->get_target_position()));

		GetTrgtRot(camera.position, camera.interest, &cam_id_move.rot_x, 1);
		GetTrgtRot(camera.position, camera.interest, &cam_id_move.rot_y, 2);

		RotFvector(cam_id_move.rot_x, t);
		RotFvector(cam_id_move.rot_y, t);

		camera.interest = cam_id_move.i;
		camera.position = cam_id_move.p;

		plyr_adj[0] = plyr_adj[1] = plyr_adj[2] = plyr_adj[3] = 0;

		mcd->type = 0;
		//cd_step = 0;
		//cam_id = 0;
		cd_edit_end = 0;
	}

	else {
		NormalCameraCtrl();
		//effects.QuakeCamera();
	}
}

void SgCamera::_update_camera() {
	if (!is_inside_tree()) {
		return;
	}

	if (!is_physics_interpolated_and_enabled()) {
		RenderingServer::get_singleton()->camera_set_transform(camera_rid, get_camera_transform());
	}

	if (is_part_of_edited_scene() || !is_current()) {
		return;
	}
}

void SgCamera::make_current() {
	current = true;

	godot::UtilityFunctions::print("make_current");

	if (!is_inside_tree()) {
		godot::UtilityFunctions::print("returned");
		return;
	}
}

void SgCamera::clear_current(bool p_enable_next) {
	current = false;
	if (!is_inside_tree()) {
		return;
	}

	//if (get_viewport()->get_camera_3d() == this) {
	//	server->viewport_attach_camera(viewport, camera);
	//}
}

void SgCamera::set_current(bool p_enabled) {
	if (p_enabled) {
		make_current();
	} else {
		clear_current();
	}
}

bool SgCamera::is_current() const {
	if (is_inside_tree() && !is_part_of_edited_scene()) {
		//return get_viewport()->get_camera_3d() == this;
	} else {
		return current;
	}

	godot::UtilityFunctions::print("is_current");
	return 0;
}

void SgCamera::set_perspective(real_t p_fovy_degrees, real_t p_z_near, real_t p_z_far) {
	camera.fov = p_fovy_degrees;
	camera.nearz = p_z_near;
	camera.farz = p_z_far;

	RenderingServer::get_singleton()->camera_set_perspective(camera_rid, camera.fov, camera.nearz, camera.farz);
	update_gizmos();
	force_change = false;
}

int SgCamera::GetCameraInfo(MAP_CAM_INFO *mci) {
	static int kind_old;

	mci->change = 0;
	GetCameraData(kind_old, mci);
	if ((kind_old != 2 && drm_cam_req != 0) || ((kind_old == 2 && drm_cam_req == 0) || /*|| plyr_wrk.pr_info.camera_door != 0xffff*/ mci->no != mci->no_old)) {
		mci->change = 1;
		if (drm_cam_req != 0) {
			if (kind_old == 2) {
				mci->change = 0;
			}

			mci->kind = 2;
		}
		if (mci->kind != kind_old) {
			compling = 0xffff;
			GetCameraData(mci->kind, mci);
		}

		kind_old = mci->kind;
	} else {
		mci->kind = kind_old;
	}

	return 0;
}

void SgCamera::KonwakuCamCtrl() {
	Vector3 rv;
	Vector3 tv;

	Vector3 pos = Vector3(plyr_wrk->get_global_position().x, plyr_wrk->get_global_position().y, plyr_wrk->get_global_position().z);
	Vector3 rot = Vector3(plyr_wrk->get_global_rotation().x, plyr_wrk->get_global_rotation().y, plyr_wrk->get_global_rotation().z);

	Transform3D t = get_global_transform();

	tv = Vector3(0.0f, -500.0f, 800.0f);

	RotFvector(rot, t);
	camera.interest = pos + tv;

	tv = Vector3(0.0f, -950.0f, -400.0f);

	RotFvector(rot, t);
	camera.position = pos + tv;

	camera.roll = PI;
	camera.fov = 44.0f;
}

float SgCamera::GetMCLocalPosPer(u_short cn, u_char kind, u_char id) {
	u_short xmin;
	u_short xmax;
	u_short zmin;
	u_short zmax;

	Point2 minMax;

	u_char kind_tbl[4] = { 1, 2, 3, 4 };
	static float min;
	static float max;
	static u_short req_cam_no_save = 0xFFFF;
	static u_char req_cam_kind_save = 0xFF;

	if (id == 0xff) {
		req_cam_no_save = -1;
		req_cam_kind_save = 0xff;

		return 0.0f;
	}

	if (cn != req_cam_no_save || kind != req_cam_kind_save) {
		req_cam_no_save = cn;
		req_cam_kind_save = kind;

		//MapCameraCdivideAB(cn, &xmin, &xmax, &zmin, &zmax, kind_tbl[kind]);

		if (id == 0) {
			min = xmin;
			max = xmax;
		} else {
			min = zmin;
			max = zmax;
		}
	}

	if (id == 0) {
		return __builtin_fabsf((plyr_wrk->bwp[0] - min) / (max - min));
	}

	return __builtin_fabsf((plyr_wrk->bwp[2] - min) / (max - min));
}

// Used ingame
void SgCamera::SetCamPos0(SgCameraData *tc) {
	Vector3 tv = Vector3();

	// Replace this with the player's global position
	GetMCLocalPosPer(0, 0, 0xff);
	tc->interest = Vector3((short)mcd->p0.y, mcd->p0.x, mcd->p0.z);

	godot::UtilityFunctions::print("Interest: ", tc->interest);

	tv = Vector3(mcd->p1[0], (short)mcd->p1[1], mcd->p1[2]);
	tc->position = Vector3(tv[0], tv[1], tv[2]);

	godot::UtilityFunctions::print("Position: ", tc->position);

	tc->roll = mcd->roll[0];
	tc->fov = mcd->fov[0];
}

void SgCamera::SetCamPos1(SgCameraData *tc) {
	Vector3 tv2;
	static Vector3 tv = Vector3();
	Vector3 pos = plyr_wrk->get_global_position();
	Vector3 rot = plyr_wrk->get_global_rotation();
	Transform3D t = get_global_transform();

	GetMCLocalPosPer(0, 0, 0xff);

	tv = Vector3((short)mcd->p1[0], (short)mcd->p1[1], (short)mcd->p1[2]);
	RotFvector(rot, t);

	tv2 = pos + tv;
	tc->interest = Vector3(tv2[0], tv2[1] + -400.0f, tv2[2]);
	tv2 = Vector3(mcd->p0[0], (short)mcd->p0[1], mcd->p0[2]);
	tc->position = Vector3(tv2[0], tv2[1], tv2[2]);
	tc->roll = mcd->roll.x;
	tc->fov = mcd->fov.x;
}

void SgCamera::SetCamPos2(SgCameraData *tc, MAP_CAM_INFO *mci) {
	Vector3 tv;
	Vector3 bv;
	float per;

	tv = Vector3((u_short)mcd->p0[0], (short)mcd->p0[1], (u_short)mcd->p0[2]);
	tc->interest = tv;

	per = GetMCLocalPosPer(mci->no, mci->kind, mcd->id);

	tv = Vector3i(((u_short)mcd->p2[0] - (u_short)mcd->p1[0]) * per,
				  ((short)mcd->p2[1] - (short)mci->mcd->p1[1]) * per,
				  ((u_short)mcd->p2[2] - (u_short)mcd->p1[2]) * per);

	bv = Vector3((u_short)mcd->p1[0], (short)mcd->p1[1], (u_short)mcd->p1[2]);

	tc->position = bv + tv;

	if (mcd->roll[1] != 0.0f) {
		bv[0] = mcd->roll[1] - mcd->roll[0];

		RotLimitChk(bv);

		bv[0] *= per;
		tc->roll = mcd->roll[0] + bv[0];

		RotLimitChk(&tc->roll);
	} else {
		tc->roll = mcd->roll[0];
	}

	if (mcd->fov[1] != 0.0f) {
		tv[0] = (mcd->fov[1] - mcd->fov[0]) * per;
		tc->fov = mcd->fov[0] + tv[0];
	} else {
		tc->fov = mcd->fov[0];
	}
}

void SgCamera::SetCamPos3(SgCameraData *tc, MAP_CAM_INFO *mci) {
	Vector3 tv2 = Vector3();
	Vector3 bv = Vector3();
	float per;
	static Vector3 tv = { 0.0f, 0.0f, 0.0f };

	Vector3 pos = plyr_wrk->get_global_position();
	Vector3 rot = plyr_wrk->get_global_rotation();

	Transform3D t = get_global_transform();

	tv = Vector3((short)mcd->p2[0], (short)mcd->p2[1], (short)mcd->p2[2]);
	RotFvector(rot, t);

	tv2 = pos + t.origin;
	tv2[1] = tv2[1] + -400.0f;

	tc->interest = tv2;

	per = GetMCLocalPosPer(mci->no, mci->kind, mci->mcd->id);

	tv2 = Vector3(((u_short)mci->mcd->p1[0] - (u_short)mci->mcd->p0[0]) * per,
				  ((short)mci->mcd->p1[1] - (short)mci->mcd->p0[1]) * per,
				  ((u_short)mci->mcd->p1[2] - (u_short)mci->mcd->p0[2]) * per);

	bv = Vector3((u_short)mci->mcd->p0[0], (short)mci->mcd->p0[1], (u_short)mci->mcd->p0[2]);

	tc->position = bv + tv2;

	if (mci->mcd->roll[1] != 0.0f) {
		bv[0] = mci->mcd->roll[1] - mci->mcd->roll[0];

		RotLimitChk(bv);

		bv[0] *= per;

		tc->roll = mci->mcd->roll[0] + bv[0];

		RotLimitChk(&tc->roll);
	} else {
		tc->roll = mci->mcd->roll[0];
	}

	if (mci->mcd->fov[1] != 0.0f) {
		tv[0] = (mci->mcd->fov[1] - mci->mcd->fov[0]) * per;

		tc->fov = mci->mcd->fov[0] + tv[0];
	} else {
		tc->fov = mci->mcd->fov[0];
	}
}

void SgCamera::SetCamPos4(SgCameraData *tc, MAP_CAM_INFO *mci) {
	Vector3 tv;
	Vector3 bv;
	float per;

	// Bounds check
	per = GetMCLocalPosPer(mci->no, mci->kind, mci->mcd->id);

	tv[0] = ((u_short)mci->mcd->p1[0] - (u_short)mci->mcd->p0[0]) * per;
	tv[1] = ((short)mci->mcd->p1[1] - (short)mci->mcd->p0[1]) * per;
	tv[2] = ((u_short)mci->mcd->p1[2] - (u_short)mci->mcd->p0[2]) * per;

	bv[0] = (u_short)mci->mcd->p0[0];
	bv[1] = (short)mci->mcd->p0[1];
	bv[2] = (u_short)mci->mcd->p0[2];

	tc->interest = bv + tv;

	tv[0] = ((u_short)mci->mcd->p3[0] - (u_short)mci->mcd->p2[0]) * per;
	tv[1] = ((short)mci->mcd->p3[1] - (short)mci->mcd->p2[1]) * per;
	tv[2] = ((u_short)mci->mcd->p3[2] - (u_short)mci->mcd->p2[2]) * per;

	bv[0] = (u_short)mci->mcd->p2[0];
	bv[1] = (short)mci->mcd->p2[1];
	bv[2] = (u_short)mci->mcd->p2[2];

	tc->position = bv + tv;

	if (mci->mcd->roll[1] != 0.0f) {
		bv[0] = mci->mcd->roll[1] - mci->mcd->roll[0];

		RotLimitChk(bv);

		bv[0] = bv[0] * per;

		tc->roll = mci->mcd->roll[0] + bv[0];

		RotLimitChk(&tc->roll);
	} else {
		tc->roll = mci->mcd->roll[0];
	}

	if (mci->mcd->fov[1] != 0.0f) {
		tv[0] = (mci->mcd->fov[1] - mci->mcd->fov[0]) * per;

		tc->fov = mci->mcd->fov[0] + tv[0];
	} else {
		tc->fov = mci->mcd->fov[0];
	}
}

void SgCamera::SetCamPos5(SgCameraData *tc, MAP_CAM_INFO *mci) {
	Vector3 tv2;
	static Vector3 tv = { 0.0f, 0.0f, 0.0f };
	Vector3 pos = plyr_wrk->get_global_position();
	Vector3 rot = plyr_wrk->get_global_rotation();

	Transform3D t = get_global_transform();

	GetMCLocalPosPer(0, 0x0, 0xff);

	if (mci->change != 0x0) {
		tv = Vector3((short)mci->mcd->p1[0], (short)mci->mcd->p1[1], (short)mci->mcd->p1[2]);
		RotFvector(rot, t);
	}

	tv2 = pos + t.origin;

	tc->interest = Vector3(tv2[0], tv2[1] + -400.0f, tv2[2]);
	tv2 = Vector3((short)mci->mcd->p0[0], (short)mci->mcd->p0[1], (short)mci->mcd->p0[2]);

	tc->position = pos + tv2;
	tc->roll = mci->mcd->roll[0];
	tc->fov = mci->mcd->fov[0];
}

SgCamera::~SgCamera() {
	// Clean up the RID to prevent memory leaks
	if (camera_rid.is_valid()) {
		server->free_rid(camera_rid);
	}
}

// These are used in the original editor
// These assign positions for "map cam dat"
void SgCamera::CameraIdMoveCtrl() {
	Vector3 tv = { 0.0f, 0.0f, 0.0f };
	Vector3 rav = { 0.0f, 0.0f, 1000.0f };

	switch (mcd->cam_type) {
		case 0:
			cd_edit_end = SetMapCamDat0(mcd);
			break;
		case 1:
			cd_edit_end = SetMapCamDat1(mcd);
			break;
		case 2:
			cd_edit_end = SetMapCamDat2(mcd, 0);
			break;
		case 3:
			cd_edit_end = SetMapCamDat3(mcd, 0);
			break;
		case 4:
			cd_edit_end = SetMapCamDat4(mcd, 0);
			break;
		case 5:
			cd_edit_end = SetMapCamDat5(mcd);
			break;
	}

	//RotFvector(cam_id_move.rot_x, tv);
	//RotFvector(cam_id_move.rot_y, tv);
	//RotFvector(cam_id_move.rot_x, rav);
	//RotFvector(cam_id_move.rot_y, rav);

	tv = cam_id_move.p;
	rav = cam_id_move.i;
	camera.interest = cam_id_move.i;
	camera.position = cam_id_move.p;
}

int SgCamera::SetMapCamDat0(Ref<MapCamDat> mcd) {
	int i;
	godot::UtilityFunctions::print("SetMapCamDat0");

	mcd->type = 0;
	mcd->id = 0;

	for (i = 0; i < 3; i++) {
		mcd->p0[i] = camera.interest[i];
	}

	for (i = 0; i < 3; i++) {
		mcd->p1[i] = camera.position[i];
	}

	mcd->p2 = Vector3i();
	mcd->p3 = Vector3i();

	mcd->roll = Vector2(camera.roll, 0.0f);
	mcd->fov = Vector2(camera.fov, 0.0f);

	return 1;
}

int SgCamera::SetMapCamDat1(Ref<MapCamDat> mcd) {
	mcd->type = 1;
	mcd->id = 0;

	for (int i = 0; i < 3; i++) {
		mcd->p0[i] = camera.position[i];
	}

	for (int i = 0; i < 3; i++) {
		mcd->p1[i] = plyr_adj[i];
	}

	mcd->p2 = Vector3i();
	mcd->p3 = Vector3i();

	mcd->roll = Vector2(camera.roll, 0.0f);
	mcd->fov = Vector2(camera.fov, 0.0f);

	return 1;
}

int SgCamera::SetMapCamDat2(Ref<MapCamDat> mcd, int id) {
	int end;
	static Ref<MapCamDat> tmpd;

	tmpd.instantiate();

	end = 0;

	for (int i = 0; i < 3; i++) {
		tmpd->p0[i] = (u_short)camera.interest[i];
	}

	for (int i = 0; i < 3; i++) {
		tmpd->p1[i] = (u_short)camera.position[i];
	}

	switch (mcd->pointID) {
		case 0:
			mcd->type = 2;

			for (int i = 0; i < 3; i++) {
				tmpd->p0[i] = (u_short)(int)camera.interest[i];
			}

			for (int i = 0; i < 3; i++) {
				tmpd->p1[i] = camera.position[i];
			}

			mcd->roll[0] = camera.roll;
			mcd->fov[0] = camera.fov;

			mcd->pointID++;
			break;
		case 1:
			for (int i = 0; i < 3; i++) {
				tmpd->p2[i] = camera.position[i];
			}

			if (mcd->roll[0] != camera.roll) {
				mcd->roll[1] = camera.roll;
			} else {
				mcd->roll[1] = 0.0f;
			}

			if (mcd->fov[0] != camera.fov) {
				mcd->fov[1] = camera.fov;
			} else {
				mcd->fov[1] = 0.0f;
			}

			mcd->pointID++;
			break;
		case 2:
			mcd->id = id;

			if (tmpd->p0[0] != 0 || tmpd->p0[1] != 0 || tmpd->p0[2] != 0) {
				mcd->p0 = tmpd->p0;
			}

			if (tmpd->p1[0] != 0 || tmpd->p1[1] != 0 || tmpd->p1[2] != 0) {
				mcd->p1 = tmpd->p1;
			}

			if (tmpd->p2[0] != 0 || tmpd->p2[1] != 0 || tmpd->p2[2] != 0) {
				mcd->p2 = tmpd->p2;
			}

			mcd->p3 = Vector3i();

			end = 1;
			break;
		default:
			end = 1;
			break;
	}

	return end;
}

int SgCamera::SetMapCamDat3(Ref<MapCamDat> mcd, int id) {
	int i;
	int end;

	static Ref<MapCamDat> tmpd;

	end = 0;

	switch (mcd->pointID) {
		case 0:
			tmpd.instantiate();

			mcd->type = 3;

			for (i = 0; i < 3; i++) {
				tmpd->p0[i] = camera.position[i];
			}

			mcd->roll[0] = camera.roll;
			mcd->fov[0] = camera.fov;

			mcd->pointID++;
			break;
		case 1:
			for (i = 0; i < 3; i++) {
				tmpd->p1[i] = camera.position[i];
			}

			if (mcd->roll[0] != camera.roll) {
				mcd->roll[1] = camera.roll;
			} else {
				mcd->roll[1] = 0.0f;
			}

			if (mcd->fov[0] != camera.fov) {
				mcd->fov[1] = camera.fov;
			} else {
				mcd->fov[1] = 0.0f;
			}

			mcd->pointID++;
			break;
		case 2:
			mcd->id = id;

			if (tmpd->p0 != Vector3i()) {
				mcd->p0 = tmpd->p0;
			}

			if (tmpd->p1 != Vector3i()) {
				mcd->p1 = tmpd->p1;
			}

			for (i = 0; i < 3; i++) {
				mcd->p2[i] = GetPlyrAdj()[i];
			}

			mcd->p3 = Vector3i();

			end = 1;
			break;
		default:
			end = 1;
			break;
	}

	return end;
}

int SgCamera::SetMapCamDat4(Ref<MapCamDat> mcd, int id) {
	int i;
	int end;
	static Ref<MapCamDat> tmpd;

	end = 0;

	switch (mcd->pointID) {
		case 0:
			tmpd.instantiate();

			mcd->type = 4;

			for (i = 0; i < 3; i++) {
				tmpd->p0[i] = camera.interest[i];
			}

			for (i = 0; i < 3; i++) {
				tmpd->p2[i] = camera.position[i];
			}

			mcd->roll[0] = camera.roll;
			mcd->fov[0] = camera.fov;

			pointStatus->set_text("Point 1 Set");
			mcd->pointID++;
			break;
		case 1:
			for (i = 0; i < 3; i++) {
				tmpd->p1[i] = (u_short)(int)camera.interest[i];
			}

			for (i = 0; i < 3; i++) {
				tmpd->p3[i] = (u_short)(int)camera.position[i];
			}

			if (mcd->roll[0] != camera.roll) {
				mcd->roll[1] = camera.roll;
			} else {
				mcd->roll[1] = 0.0f;
			}

			if (mcd->fov[0] != camera.fov) {
				mcd->fov[1] = camera.fov;
			} else {
				mcd->fov[1] = 0.0f;
			}

			pointStatus->set_text("Point 2 Set");
			mcd->pointID++;
			break;
		case 2:
			mcd->id = id;

			if (tmpd->p0 != Vector3i()) {
				mcd->p0 = tmpd->p0;
			}

			if (tmpd->p1 != Vector3i()) {
				mcd->p1 = tmpd->p1;
			}

			if (tmpd->p2 != Vector3i()) {
				mcd->p2 = tmpd->p2;
			}

			if (tmpd->p3 != Vector3i()) {
				mcd->p3 = tmpd->p3;
			}

			pointStatus->set_text("Saved Points");
			end = 1;
			break;
		default:
			pointStatus->set_text("");
			end = 1;
			break;
	}

	return end;
}

int SgCamera::SetMapCamDat5(Ref<MapCamDat> mcd) {
	int i;

	mcd->type = 5;
	mcd->id = 0;

	Plyr_Wrk *plyr_wrk;

	for (i = 0; i < 3; i++) {
		mcd->p0[i] = camera.position[i] - plyr_wrk->get_global_position()[i];
	}

	for (i = 0; i < 3; i++) {
		mcd->p1[i] = GetPlyrAdj()[i];
	}

	mcd->p2 = Vector3();
	mcd->p3 = Vector3();

	mcd->roll = Vector2(camera.roll, 0.0f);
	mcd->fov = Vector2(camera.fov, 0.0f);

	return 1;
}
