//
// Created by nates on 6/11/2025.
//

#ifndef RENDERINGPROJECT_SCENES_H
#define RENDERINGPROJECT_SCENES_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include "maths.h"

namespace nsGraphics {

	class Texture {
	public:
		int width;
		int height;
		float3 **image;

		Texture(int width, int height, float3 **image);

		Texture(const char *fileName);

		Texture();        // default constructor gives 100x100 with random colors

		[[nodiscard]] float3 sample(float2 uv) const;
	};

	class Object {
	public:
		int numTriangles;
		std::vector<float3> points;
		std::vector<float2> uvCoords;
		std::vector<float3> normals;
		std::vector<float3> vertexColors;
		Texture texture;
		float scale;
		float3 offset;
		Rotation rotation;

		Object(int numTriangles, std::vector<float3> points, std::vector<float2> uvCoords, std::vector<float3> normals,
			   std::vector<float3> vertexColors);

		Object(const char *fileName);

		[[nodiscard]] float3 localToWorld(const float3 &p) const;
	};

	Object parseObj(const char *fileName);

	class Camera {
	public:
		float3 offset;
		Rotation rotation;
		float fov;

		Camera();

		Camera(float3 offset, Rotation rotation, float fov);
	};

	class Scene {
	public:
		std::vector<Object> objects;
		Camera camera;

		Scene(std::vector<Object> objects);
	};

}

#endif //RENDERINGPROJECT_SCENES_H