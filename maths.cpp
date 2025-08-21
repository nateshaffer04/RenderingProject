/*
 * Author: Nate Shaffer
 * Email: nshaffe4@u.rochester.edu
 * Date Created: 6/1/2025
 * 
 * Primarily vector operations, as well as rotation operations on those vectors
 */

#include "maths.h"

float3::float3() : x(0), y(0), z(0) { }

float3::float3(float x, float y, float z) : x(x), y(y), z(z) { }

//float3 float3::operator+(const float3 &v) const {
//	return {v.x+x,v.y+y,v.z+z};
//}

float3 float3::operator-(const float3 &v) const {
	return {x-v.x,y-v.y,z-v.z};
}

//float3 float3::operator*(const float &c) const {
//	return {c*x, c*y, c*z};
//}

float3 float3::operator/(const float &c) const {
	return {x/c, y/c, z/c};
}

//void float3::operator+=(const float3 &v) {
//	x += v.x;
//	y += v.y;
//	z += v.z;
//}

float3 randColor() {
	float b = 255 * ((float) rand() / (float) RAND_MAX);
	float g = 255 * ((float) rand() / (float) RAND_MAX);
	float r = 255 * ((float) rand() / (float) RAND_MAX);
	return {b, g, r};
}

float2::float2() : x(0), y(0) { }

float2::float2(float x, float y) : x(x), y(y) { }

float2 float2::operator+(const float2 &v) const {
	return {v.x+x,v.y+y};
}

float2 float2::operator-(const float2 &v) const {
	return {v.x-x,v.y-y};
}

float2 float2::operator*(const float &c) const {
	return {x*c, y*c};
}

float2 float2::operator/(const float &c) const {
	return {x/c, y/c};
}

void float2::operator+=(const float2 &v) {
	x += v.x;
	y += v.y;
}


//float dot(float3 v1, float3 v2) {
//	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
//}

float dot(float2 v1, float2 v2) {
	return v1.x*v2.x + v1.y*v2.y;
}

// returns the signed area of the triangle formed by a,b, and p
//float edgeFunc(float2 a, float2 b, float2 p) {
//	return (p.x-a.x)*(b.y-a.y) - (p.y-a.y)*(b.x-a.x);
//}

// clamps x to lie in [min, max]
int clamp(int x, int min, int max) {
	if (x<min) {
		return min;
	} else if (x > max) {
		return max;
	} else {
		return x;
	}
}

Rotation::Rotation(float yaw, float pitch) : yaw(yaw), pitch(pitch) {
	i = float3(cosf(yaw), 0, sinf(yaw));
	j = float3(-sinf(yaw)*sinf(pitch), cosf(pitch), -sinf(pitch));
	k = float3(-sinf(yaw)*cosf(pitch), -sinf(pitch), cosf(yaw)*cosf(pitch));

	iInv = float3(i.x, j.x, k.x);
	jInv = float3(i.y, j.y, k.y);
	kInv = float3(i.z, j.z, k.z);
}

float3 Rotation::apply(float3 v) const {
	return {i.x*v.x + j.x*v.y + k.x*v.z, i.y*v.x + j.y*v.y + k.y*v.z, i.z*v.x + j.z*v.y + k.z*v.z};
}

float3 Rotation::applyInv(float3 v) const {
	return {iInv.x*v.x + jInv.x*v.y + kInv.x*v.z, iInv.y*v.x + jInv.y*v.y + kInv.y*v.z, iInv.z*v.x + jInv.z*v.y + kInv.z*v.z};
}

Rotation::Rotation() : yaw(0), pitch(0) {
	i = float3(1, 0, 0);
	iInv = float3(1, 0, 0);
	j = float3(0, 1, 0);
	jInv = float3(0, 1, 0);
	k = float3(0, 0, 1);
	kInv = float3(0, 0, 1);
}

void Rotation::addYaw(float delta) {
	yaw -= delta;	// TODO fix this hack
	i = float3(cosf(yaw), 0, -sinf(yaw));
	j = float3(sinf(yaw)*sinf(pitch), cosf(pitch), cosf(yaw)*sinf(pitch));
	k = float3(sinf(yaw)*cosf(pitch), -sinf(pitch), cosf(yaw)*cosf(pitch));

	iInv = float3(i.x, j.x, k.x);
	jInv = float3(i.y, j.y, k.y);
	kInv = float3(i.z, j.z, k.z);
}

void Rotation::addPitch(float delta) {
	pitch -= delta;	// TODO fix this hack
	i = float3(cosf(yaw), 0, -sinf(yaw));
	j = float3(sinf(yaw)*sinf(pitch), cosf(pitch), cosf(yaw)*sinf(pitch));
	k = float3(sinf(yaw)*cosf(pitch), -sinf(pitch), cosf(yaw)*cosf(pitch));

	iInv = float3(i.x, j.x, k.x);
	jInv = float3(i.y, j.y, k.y);
	kInv = float3(i.z, j.z, k.z);
}

