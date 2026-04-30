#include "camera/CameraEffects.h"
#include <cstdlib>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/variant/vector3.hpp>

float CameraEffects::motGetRandom(float upper, float lower) {
	return (rand() / (float)RAND_MAX) * (upper - lower) + lower;
}

int CameraEffects::QuakeCamera() {
	float v0;
	float v1;
	float v2;
	Vector3 val[2];
	Vector3 xd;
	int i;
	QUAKE_CTRL *qk_p;
	float dat;

	qk_p = &quake;

	if (qk_p->req == 0) {
		return 0;
	}
	if (qk_p->cnt >= qk_p->all_cnt) {
		qk_p->cnt--;
		return 0;
	}

	qk_p->cnt--;

	if (qk_p->cnt <= 0) {
		if (qk_p->loop == 0) {
			qk_p->req = 0;
			StopQuake();
			return 0;
		} else {
			qk_p->cnt = motGetRandom(600.0f, 1200.0f);
			qk_p->loop--;
			return 0;
		}
	}

	xd = camera.GetCameraData().zd.cross(camera.GetCameraData().yd);

	for (i = 0; i < 2; i++) {
		dat = qk_p->pow[i];

		v0 = motGetRandom(dat, -dat);
		v1 = motGetRandom(dat, -dat);
		v2 = motGetRandom(dat, -dat);

		if (qk_p->cnt < 30) {
			v0 = v0 * (qk_p->cnt / 30.0f);
			v1 = v1 * (qk_p->cnt / 30.0f);
			v2 = v2 * (qk_p->cnt / 30.0f);
		}

		val[i][0] = (xd[0] * v0) + (camera.GetCameraData().yd[0] * v1) + (camera.GetCameraData().zd[0] * v2);
		val[i][1] = (xd[1] * v0) + (camera.GetCameraData().yd[1] * v1) + (camera.GetCameraData().zd[1] * v2);
		val[i][2] = (xd[2] * v0) + (camera.GetCameraData().yd[2] * v1) + (camera.GetCameraData().zd[2] * v2);
		val[i][3] = 0;
	}

	camera.GetCameraData().p += val[0];
	camera.GetCameraData().i += val[1];
}

void CameraEffects::StopQuake() {
	quake.req = 0;
}

// ToDo: Make a signal call for this
void CameraEffects::ReqQuake(float pow, u_int time, u_int loop, u_int stop_flg) {
	quake.pow[0] = pow;
	quake.pow[1] = pow * 1.62f;
	quake.cnt = time;
	quake.all_cnt = time;
	quake.loop = loop;
	quake.req = 1;
}

void CameraEffects::CallFocus(int type, int wait, int gap) {
	eff_focus.flow = type != 0;
	eff_focus.cnt = 0;
	eff_focus.max = gap;
	eff_focus.in = 30;
	eff_focus.keep = wait;
	eff_focus.out = 30;
}

void CameraEffects::CallFocus2(int in, int keep, int out, int max) {
	eff_focus.flow = in < 1 ? (keep < 1 ? (out > 0 ? 2 : 3) : 1) : 0;
	eff_focus.cnt = 0;
	eff_focus.max = max;
	eff_focus.in = in;
	eff_focus.keep = keep;
	eff_focus.out = out;
}