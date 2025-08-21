//
// Created by nates on 6/1/2025.
//

#ifndef RENDERINGPROJECT_RENDERER_H
#define RENDERINGPROJECT_RENDERER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>

#include "scenes.h"

namespace nsGraphics {

	class RenderTarget {
	public:
		int width;
		int height;
		std::byte *frameBuffer;
		float *zBuffer;

		RenderTarget(int w, int h);

		void clear();
	};

	class Renderer {
		float pixelsPerWorldUnit;
	public:
		Scene scene;
		RenderTarget target;

		Renderer(const Scene& scene, RenderTarget target);

		[[nodiscard]] float3 worldToScreen(const float3 &p) const;

		void drawObject(const Object &o);

		void render();
	};

}

#endif //RENDERINGPROJECT_RENDERER_H