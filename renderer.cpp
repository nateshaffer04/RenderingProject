/*
 * Author: Nate Shaffer
 * Email: nshaffe4@u.rochester.edu
 * Date Created: 6/1/2025
 * 
 * Main renderer class, stores a collection of models and a camera, and renders them to a render target
 */

#include <sstream>
#include <utility>
#include "renderer.h"

using namespace std;

namespace nsGraphics {

	RenderTarget::RenderTarget(int w, int h) : width(w), height(h) {
		frameBuffer = new byte [4 * width * height];
		zBuffer = new float [width * height];
	}

	// clears a render target to all black with full alpha
	void RenderTarget::clear() {
		frameBuffer = new byte [4 * width * height];
		for (int row = 0; row < height; row++) {
			for (int col = 0; col < width; col++) {
				frameBuffer[4*(row*width + col) + 3] = (byte) 255;
			}
		}
		zBuffer = new float [width * height];
	}

	Renderer::Renderer(const Scene& scene, RenderTarget target) : scene(scene), target(target) {
		float screenWidthWorld = 2 * tanf(scene.camera.fov / 2);
		pixelsPerWorldUnit = (float) target.width / screenWidthWorld;
	}

	// converts from world space to screen space, storing inverse z coordinate in the third entry
	float3 Renderer::worldToScreen(const float3 &p) const {
		float3 pView = scene.camera.rotation.applyInv(p - scene.camera.offset);
		return {pView.x * pixelsPerWorldUnit / pView.z + (float) target.width / 2, pView.y * pixelsPerWorldUnit / pView.z + (float) target.height / 2, 1 / pView.z};
	}

	// computes the bounding box of p1, p2, and p3, within the width and height of target
	auto getBoundingBox(const float2& p1, const float2& p2, const float2& p3, const RenderTarget& target) {
		struct BoundingBox {int xMin; int xMax; int yMin; int yMax;};
		BoundingBox box{};

		box.xMin = (int) min(min(p1.x, p2.x), p3.x);
		box.xMin = clamp(box.xMin, 0, target.width);
		box.xMax = (int) max(max(p1.x, p2.x), p3.x) + 1;
		box.xMax = clamp(box.xMax, 0, target.width);
		box.yMin = (int) min(min(p1.y, p2.y), p3.y);
		box.yMin = clamp(box.yMin, 0, target.height);
		box.yMax = (int) max(max(p1.y, p2.y), p3.y) + 1;
		box.yMax = clamp(box.yMax, 0, target.height);

		return box;
	}

	void Renderer::drawObject(const Object& o) {
		for (int i = 0; i < o.numTriangles; i++) {

			// convert from local to world coordinates
			float3 p1World = o.localToWorld(o.points[3 * i + 0]);
			float3 p2World = o.localToWorld(o.points[3 * i + 1]);
			float3 p3World = o.localToWorld(o.points[3 * i + 2]);

			// convert from world to screen coordinates, storing inverse z in the last coordinate
			float3 p1Screen = worldToScreen(p1World);
			float3 p2Screen = worldToScreen(p2World);
			float3 p3Screen = worldToScreen(p3World);

			// orthogonal projection of screen space onto 2d space
			float2 p1 = float2(p1Screen.x, p1Screen.y);
			float2 p2 = float2(p2Screen.x, p2Screen.y);
			float2 p3 = float2(p3Screen.x, p3Screen.y);

			// back face culling
			float total = edgeFunc(p1, p2, p3);
			if (total <= 0) {
				continue;
			}

			// find the bounding box of the given triangle
			auto [xMin, xMax, yMin, yMax] = getBoundingBox(p1, p2, p3, target);

			// loop over the bounding box and draw each pixel
			for (int row = yMin; row < yMax; row++) {
				for (int col = xMin; col < xMax; col++) {
					float2 a(col + 0.5f, row + 0.5f);

					// get the barycentric coordinates (don't normalize until we know we are in the triangle)
					float b1 = edgeFunc(p2, p3, a);
					float b2 = edgeFunc(p3, p1, a);
					float b3 = edgeFunc(p1, p2, a);
					if (b1 >= 0 && b2 >= 0 && b3 >= 0) {
						b1 /= total;
						b2 /= total;
						b3 /= total;

						// compute the perspective barycentric coordinates
						float l1 = b1*p1Screen.z;
						float l2 = b2*p2Screen.z;
						float l3 = b3*p3Screen.z;

						// compute the z coordinate
						float zInv = l1 + l2 + l3;

						if (zInv > target.zBuffer[row * target.width + col]) {
							target.zBuffer[target.width * row + col] = zInv;

	//						// texturing using vertex colors
	//						float3 color = o.vertexColors[3 * i + 0] * b1 + o.vertexColors[3 * i + 1] * b2 + o.vertexColors[3 * i + 2] * b3;

							// texturing using UVs
							float2 uvSample = o.uvCoords[3*i+0]*l1 + o.uvCoords[3*i+1]*l2 + o.uvCoords[3*i+2]*l3;
							uvSample /= zInv;
							float3 color = o.texture.sample(uvSample);

							// using normal, compute shading level
							float3 normal1World = o.rotation.apply(o.normals[3 * i + 0]);
							float3 normal2World = o.rotation.apply(o.normals[3 * i + 1]);
							float3 normal3World = o.rotation.apply(o.normals[3 * i + 2]);
							float3 normal = normal1World * l1 + normal2World * l2 + normal3World * l3;
							normal /= zInv;
							normal.normalize();
							float shading = min(max((float) 0, dot(normal, float3(0, 0, -1))), (float) 1);        // TODO maybe rethink this
							shading = 0.5f * shading + 0.5f;
							color *= shading;

							target.frameBuffer[4 * (target.width * row + col) + 0] = (byte) color.z;
							target.frameBuffer[4 * (target.width * row + col) + 1] = (byte) color.y;
							target.frameBuffer[4 * (target.width * row + col) + 2] = (byte) color.x;
							target.frameBuffer[4 * (target.width * row + col) + 3] = (byte) 255;
						}
					}
				}
			}
		}
	}

	void Renderer::render() {
		target.clear();
		for (Object &o: scene.objects) {
			drawObject(o);
		}
	}

}
