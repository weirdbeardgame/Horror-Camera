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

typedef struct { // 0x28
	/* 0x00 */ u_int flow;
	/* 0x04 */ u_int cnt;
	/* 0x08 */ u_int in;
	/* 0x0c */ u_int keep;
	/* 0x10 */ u_int out;
	/* 0x14 */ u_int alp;
	/* 0x18 */ float scl;
	/* 0x1c */ float rot;
	/* 0x20 */ float cx;
	/* 0x24 */ float cy;
} EFF_BLUR;

typedef struct { // 0x18
	/* 0x00 */ u_int flow;
	/* 0x04 */ u_int cnt;
	/* 0x08 */ u_int in;
	/* 0x0c */ u_int keep;
	/* 0x10 */ u_int out;
	/* 0x14 */ u_int max;
} EFF_FOCUS;

typedef struct { // 0x4
	/* 0x0 */ u_char type;
	/* 0x1 */ u_char otype;
	/* 0x2 */ u_char init;
	/* 0x3 */ u_char pass;
} EFF_DEFORM;

typedef struct { // 0x18
	/* 0x00 */ float rrr;
	/* 0x04 */ float lll;
	/* 0x08 */ float mm1;
	/* 0x0c */ float mm2;
	/* 0x10 */ float sss;
	/* 0x14 */ float ccc;
} DEFWORK;

typedef struct { // 0xc
	/* 0x0 */ float scl;
	/* 0x4 */ short int rot;
	/* 0x6 */ short int bx;
	/* 0x8 */ short int by;
	/* 0xa */ u_char flip;
	/* 0xb */ u_char alp;
} ONE_HAND;

class CameraEffects {
private:
	SgCamera *camera;
	QUAKE_CTRL quake;
	EFF_FOCUS eff_focus = {
		.flow = 3,
		.cnt = 0,
		.in = 0,
		.keep = 0,
		.out = 0,
		.max = 0,
	};

	float motGetRandom(float upper, float lower);

public:
	CameraEffects() = default;
	CameraEffects(SgCamera *c) { camera = c; }
	void SubFocus(int ef);
	void SetFocus(EFFECT_CONT *ec);
	void RunFocus(EFFECT_CONT *ec);
	void CallFocus(int type, int wait, int gap);
	void CallFocus2(int in, int keep, int out, int max);

	void StopQuake();
	int QuakeCamera();
	void ReqQuake(float pow, u_int time, u_int loop, u_int stop_flg);
};