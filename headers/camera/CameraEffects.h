#pragma once
#include "camera/SgCamera.h"
#include <godot_cpp/classes/node3d.hpp>

using namespace godot;

typedef struct {
	float pow[2];
	int cnt;
	int all_cnt;
	short int req;
	char loop;
	char flg;
} QUAKE_CTRL;

typedef struct {
	void *pnt[6];
	float fw[3];
	u_int z;
	u_int flow;
	u_int cnt;
	u_int in;
	u_int keep;
	u_int out;
	u_int max;
} EFFECT_CONT;

class CameraEffects {
private:
	SgCamera camera;
	QUAKE_CTRL quake;
	EFFECT_CONT eff_focus;

	float motGetRandom(float upper, float lower);

public:
	CameraEffects() = default;
	void CallFocus(int type, int wait, int gap);
	void CallFocus2(int in, int keep, int out, int max);

	void StopQuake();
	int QuakeCamera();
	void ReqQuake(float pow, u_int time, u_int loop, u_int stop_flg);
};