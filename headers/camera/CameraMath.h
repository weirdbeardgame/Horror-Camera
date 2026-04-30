#pragma once
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/basis.hpp>
#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

using namespace godot;

#define PI 3.1415928f
#define DEG2RAD(x) ((float)(x) * PI / 180.0f)

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

static void GetTrgtRot(Vector3 p0, Vector3 p1, Vector3 rot, int id) {
	Vector3 dist;

	rot = Vector3(0.0f, 0.0f, 0.0f);

	dist = p1 - p0;

	if (id & 0x1) {
		dist[3] = p0.distance_to(p1);

		rot[0] = Math::atan2(-dist[1], dist[3]);

		RotLimitChk(rot);
	}

	if (id & 0x2) {
		rot[1] = Math::atan2(dist[0], dist[2]);

		RotLimitChk(&rot[1]);
	}
}

static void RotMatrixX(Transform3D &m0, const Transform3D &m1, float rx) {
	float s = Math::sin(rx);
	float c = Math::cos(rx);

	Transform3D out;
	for (int i = 0; i < 4; i++) {
		const Vector3 &col = m1.basis[i];
		out.basis[i] = Vector3(
				col.x, // 1*x + 0+0+0
				c * col.y - s * col.z, // 0 + cos*y - sin*z + 0
				s * col.y + c * col.z);
	}
	m0 = out;
}

static void RotMatrixY(Transform3D &m0, const Transform3D &m1, float ry) {
	float s = Math::sin(ry);
	float c = Math::cos(ry);

	Transform3D out;
	for (int i = 0; i < 4; i++) {
		const Vector3 &col = m1.basis[i];
		out.basis[i] = Vector3(
				col.x, // 1*x + 0+0+0
				c * col.y - s * col.z, // 0 + cos*y - sin*z + 0
				s * col.y + c * col.z);
	}
	m0 = out;
}

static void RotMatrixZ(Transform3D &m0, const Transform3D &m1, float rz) {
	float s = Math::sin(rz);
	float c = Math::cos(rz);

	Transform3D out;
	for (int i = 0; i < 4; i++) {
		const Vector3 &col = m1.basis[i];
		out.basis[i] = Vector3(
				col.x,
				c * col.y - s * col.z,
				s * col.y + c * col.z);
	}
	m0 = out;
}

static void RotFvector(Vector3 &rot, Vector3 &tv) {
	Transform3D work;
	Transform3D rot_matrix;

	work = Transform3D();
	// sceVu0RotMatrixZ equivalent
	if (rot.z != 0.0f) {
		RotMatrixZ(rot_matrix, work, rot.z);
	}

	// sceVu0RotMatrixX equivalent
	if (rot.x != 0.0f) {
		RotMatrixX(rot_matrix, work, rot.x);
	}

	// sceVu0RotMatrixY equivalent
	if (rot.y != 0.0f) {
		RotMatrixY(rot_matrix, work, rot.y);
	}
}

static float GetTrgtRotY(Vector3 p0, Vector3 p1) {
	return Math::atan2((p1[0] - p0[0]), (p1[2], p0[2]));
}