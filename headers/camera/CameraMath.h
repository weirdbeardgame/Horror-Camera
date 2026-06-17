#pragma once
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/basis.hpp>
#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

using namespace godot;

#define PI 3.1415928f
#define DEG2RAD(x) ((float)(x) * PI / 180.0f)
#define HALF_PI 1.57079637050628662109e0f

static void RotLimitChk(Vector3 rot) {
	if (PI < rot[0]) {
		rot[0] -= PI * 2;
	} else if (rot[0] < -PI) {
		rot[0] += PI * 2;
	}
}

static void RotLimitChk(float *rot) {
	if (PI < rot[0]) {
		rot[0] -= PI * 2;
	} else if (rot[0] < -PI) {
		rot[0] += PI * 2;
	}
}

static void GetTrgtRot(Vector3 p0, Vector3 p1, Vector3 *rot, int id) {
	Vector3 dist;

	Vector3 r;

	r = Vector3(0.0f, 0.0f, 0.0f);

	dist = p1 - p0;

	if (id & 0x1) {
		dist[2] = p0.distance_to(p1);

		r[0] = Math::atan2(-dist[1], dist[2]);

		RotLimitChk(&r[0]);
	}

	if (id & 0x2) {
		r[1] = Math::atan2(dist[0], dist[2]);

		RotLimitChk(&r[1]);
	}

	*rot = r;
}

static void RotMatrixX(Transform3D &m0, const Transform3D &m1, float rx) {
	float sin;
	float cos;
	int sign;
	Basis out;

	if (rx < 0.0f) {
		rx = HALF_PI + rx;
		sign = 1;
	} else {
		rx = HALF_PI - rx;
		sign = 0;
	}

	sin = Math::sin(rx);
	cos = Math::cos(rx);

	out[0][0] = 1.0f * m1.basis[0][0] + 0.0f * m1.basis[0][1] + 0.0f * m1.basis[0][2];
	out[0][1] = 0.0f * m1.basis[0][0] + cos * m1.basis[0][1] + -sin * m1.basis[0][2] + 0.0f;
	out[0][2] = 0.0f * m1.basis[0][0] + 0.0f * m1.basis[0][1] + 1.0f * m1.basis[0][2];

	m0.basis = out;
}

static void RotMatrixY(Transform3D &m0, const Transform3D &m1, float ry) {
	float sin;
	float cos;
	int sign;
	Basis out;

	if (ry < 0.0f) {
		ry = HALF_PI + ry;
		sign = 1;
	} else {
		ry = HALF_PI - ry;
		sign = 0;
	}

	sin = Math::sin(ry);
	cos = Math::cos(ry);

	out[1][0] = cos * m1.basis[1][0] + 0.0f * m1.basis[1][1] + sin * m1.basis[1][2];
	out[1][1] = 0.0f * m1.basis[1][0] + 1.0f * m1.basis[1][1] + 0.0f * m1.basis[1][2];
	out[1][2] = -sin * m1.basis[1][0] + 0.0f * m1.basis[1][1] + cos * m1.basis[1][2];

	m0.basis = out;
}

static void RotMatrixZ(Transform3D &m0, const Transform3D &m1, float rz) {
	float sin;
	float cos;
	int sign;
	Basis out;

	if (rz < 0.0f) {
		rz = HALF_PI + rz;
		sign = 1;
	} else {
		rz = HALF_PI - rz;
		sign = 0;
	}

	sin = Math::sin(rz);
	cos = Math::cos(rz);

	out[2][0] = cos * m1.basis[2][0] + -sin * m1.basis[2][1] + 0.0f * m1.basis[2][2];
	out[2][1] = sin * m1.basis[2][0] + cos * m1.basis[2][1] + 0.0f * m1.basis[2][2];
	out[2][2] = 0.0f * m1.basis[2][0] + 0.0f * m1.basis[2][1] + 1.0f * m1.basis[2][2];

	m0.basis = out;
}

static void RotFvector(Vector3 &rot, Transform3D &rot_matrix) {
	Transform3D work;

	work = Transform3D();
	if (rot.z != 0.0f) {
		RotMatrixZ(rot_matrix, work, rot.z);
	}

	if (rot.x != 0.0f) {
		RotMatrixX(rot_matrix, work, rot.x);
	}

	if (rot.y != 0.0f) {
		RotMatrixY(rot_matrix, work, rot.y);
	}
}

static float GetTrgtRotY(Vector3 p0, Vector3 p1) {
	return Math::atan2((p1[0] - p0[0]), (p1[2], p0[2]));
}