#include "HorrorCamera.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/engine_ptrcall.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/classes/camera_attributes.hpp>
#include <godot_cpp/classes/compositor.hpp>
#include <godot_cpp/classes/environment.hpp>

using namespace godot;

void HorrorCamera::_bind_methods() {
}

void HorrorCamera::_ready() {
	godot::UtilityFunctions::print("my extension is initialised");
}

void HorrorCamera::_process(double delta) {
	// Example of rotating the camera over time
	rotate_y(delta * 0.5); // Rotate around the Y-axis
}

void HorrorCamera::print_type(const Variant &p_variant) const {
	print_line(vformat("Type: %d", p_variant.get_type()));
}

void HorrorCamera::DramaCameraReqCtrl() {
	u_char i;
	u_char chk;

	if (drm_cam_req != 0) {
		if (drm_cam_tm != 0) {
			drm_cam_tm--;

			if (drm_cam_tm == 0) {
				ClearDramaCamReq();
			}
		}

		/*if (drm_cam_req != 0) {
			for (i = 0, chk = 0; i < 12; i++) {
				if (drm_cam_no[i] == 0xffff) {
					break;
				}

				if (plyr_wrk.pr_info.camera_drm == drm_cam_no[i]) {
					chk = 1;
					break;
				}
			}*/

		if (!chk) {
			ClearDramaCamReq();
		}
		//}
	}
}

void HorrorCamera::ClearDramaCamReq() {
	u_char i;

	drm_cam_req = 0;
	drm_cam_tm = 0;

	for (i = 0; i < 12; i++) {
		//drm_cam_no[i] = -1;
	}
}

void HorrorCamera::NormalCameraCtrl() {
	MAP_CAM_INFO mci;
	SgCAMERA tc;
	SgCAMERA tc2;
	static SgCAMERA oc;

	DramaCameraReqCtrl();

	if (GetCameraInfo(&mci) != 0) {
		return;
	}

	if (mci.change != 0) {
		plyr_wrk.cp_old[0] = camera.p[0];
		plyr_wrk.cp_old[1] = camera.p[1];
		plyr_wrk.cp_old[2] = camera.p[2];
		plyr_wrk.cp_old[3] = camera.p[3];
	}

	switch (mci.type) {
		case 0:
			SetCamPos0(&tc, &mci);
			break;
		case 1:
			SetCamPos1(&tc, &mci);
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

	//CompleCameraPos(&tc, &oc, &mci);

	oc = tc2;

	/*camera.i[0] = tc.i[0];
	camera.i[1] = tc.i[1];
	camera.i[2] = tc.i[2];
	camera.i[3] = tc.i[3];

	camera.p[0] = tc.p[0];
	camera.p[1] = tc.p[1];
	camera.p[2] = tc.p[2];
	camera.p[3] = tc.p[3];

	camera.roll = tc.roll;
	camera.fov = tc.fov;*/

	//plyr_wrk.prot = GetTrgtRotY(camera.p, plyr_wrk.move_box.pos);
}
