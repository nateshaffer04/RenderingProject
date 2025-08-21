//
// Created by nates on 6/1/2025.
//

#ifndef RENDERINGPROJECT_MATHS_H
#define RENDERINGPROJECT_MATHS_H

#include <cmath>
#include <cstdlib>
#include <cmath>

struct float3 {
public:
	float x;
	float y;
	float z;

	float3();
	float3(float x, float y, float z);

	inline float3 operator+(const float3 &v) const {
		return {v.x+x,v.y+y,v.z+z};
	}

	float3 operator-(const float3 &v) const;

	inline float3 operator*(const float &c) const {
		return {c*x, c*y, c*z};
	}

	float3 operator/(const float &c) const;

	inline void operator+=(const float3 &v) {
		x += v.x;
		y += v.y;
		z += v.z;
	}

	inline void operator-=(const float3 &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	inline void operator*=(const float c) {
		x *= c;
		y *= c;
		z *= c;
	}

	inline void operator/=(const float c) {
		x /= c;
		y /= c;
		z /= c;
	}

	inline void normalize() {
		float invNorm = 1/std::sqrt(x*x + y*y + z*z);
		x *= invNorm;
		y *= invNorm;
		z *= invNorm;
	}

};

float3 randColor();

struct float2 {
public:
	float x;
	float y;

	float2();
	float2(float x, float y);

	float2 operator+(const float2 &v) const;

	float2 operator-(const float2 &v) const;

	float2 operator*(const float &c) const;

	float2 operator/(const float &c) const;

	void operator+=(const float2 &v);

	inline void operator/=(const float c) {
		x /= c;
		y /= c;
	}
};

inline float dot(float3 v1, float3 v2) {
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

float dot(float2 v1, float2 v2);

inline float edgeFunc(float2 a, float2 b, float2 p) {
	return (p.x-a.x)*(b.y-a.y) - (p.y-a.y)*(b.x-a.x);
}

int clamp(int x, int min, int max);

class Rotation {
public:
	float3 i;
	float3 j;
	float3 k;

	float3 iInv;
	float3 jInv;
	float3 kInv;

	float yaw;
	float pitch;

	Rotation(float yaw, float pitch);
	Rotation();

	float3 apply(float3 v) const;
	float3 applyInv(float3 v) const;

	void addYaw(float delta);
	void addPitch(float delta);
};

#endif //RENDERINGPROJECT_MATHS_H
