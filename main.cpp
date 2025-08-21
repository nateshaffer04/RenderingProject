#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <chrono>

#include "raylib.h"
#include "renderer.h"
#include "scenes.h"

using namespace std;

// writes the content in the frame buffer on the given render target to a BMP file
void targetToBMP(nsGraphics::RenderTarget target, const string& name) {
	int dataOffset = 14 + 40;
	int rowSize = ((3 * target.width + 3) / 4) * 4;	// row size after padding
	int dataSize = rowSize * target.height;
	int fileSize = dataOffset + dataSize;

	const char* fileName = name.c_str();
	ofstream image(fileName, ios::binary);

	// --BMP Header--
	image.put('B'); image.put('M');									// file signature
	image.write((char*)&fileSize, 4);									// file size
	image.put(0); image.put(0); image.put(0); image.put(0);		// reserved
	image.write((char*)&dataOffset, 4);								// frameBuffer offset

	// --DIB Header--
	int headerSize = 40;
	image.write((char*)&headerSize, 4);								// DIB header size
	image.write((char*)&target.width, 4);								// image width
	image.write((char*)&target.height, 4);							// image height
	short planes = 1;
	image.write((char*)&planes, 2);									// num planes (?)
	short bpp = 24;
	image.write((char*)&bpp, 2);										// bits per pixel
	int compression = 0;
	image.write((char*)&compression, 4);								// rgb, no compression
	image.write((char*)&dataSize, 4);									// frameBuffer size
	char zeros[16] = {0};
	image.write(zeros, 16);											// unused

	// --Data--
	for (int row = 0; row < target.height;  row++) {
		vector<char> rowData(rowSize, 0);
		for (int col = 0; col < target.width; col++) {
			int xFirst = col * 3;
			char b = (char) target.frameBuffer[4 * (target.width * row + col) + 2];
			char g = (char) target.frameBuffer[4 * (target.width * row + col) + 1];
			char r = (char) target.frameBuffer[4 * (target.width * row + col) + 0];
			rowData[xFirst + 0] = b;
			rowData[xFirst + 1] = g;
			rowData[xFirst + 2] = r;
		}
		image.write(rowData.data(), rowSize);
	}

	image.close();
}


int main() {

	// set up the scene
	nsGraphics::Object model("models/monkey.obj");
	model.offset = float3(0, 0, 5);
	model.rotation = Rotation(M_PI * 0.9, 0);
	model.texture = nsGraphics::Texture("textures/rainbow.bmp");

//	nsGraphics::Object plane("models/plane.obj");
//	plane.offset = float3(0, -1, 0);
//	plane.texture = nsGraphics::Texture("textures/default.bmp");

//	nsGraphics::Object bunny("models/bunny.obj");
//	bunny.offset = float3(0, 2, 5);
//	bunny.texture = nsGraphics::Texture("textures/default.bmp");

	vector<nsGraphics::Object> objects = {model};
	nsGraphics::Scene scene(objects);

	// set up the renderer
	int renderWidth = 1280;
	int renderHeight = 720;
	nsGraphics::RenderTarget target(renderWidth, renderHeight);
	nsGraphics::Renderer renderer(scene, target);

	// setup raylib window
	int initScreenWidth = 1280;
	int initScreenHeight = 720;
	InitWindow(initScreenWidth, initScreenHeight, "RenderingProject");
	Texture2D screenTexture = LoadTextureFromImage(GenImageColor(renderWidth, renderHeight, SKYBLUE));

	// setting window to resizable and getting a RenderTexture2D to render to
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	RenderTexture2D renderTexture = LoadRenderTexture(renderWidth, renderHeight);

	float moveSpeed = 1;
	while (!WindowShouldClose()) {

		// update scene
		renderer.scene.objects[0].rotation.addYaw(GetFrameTime());


		// handle keyboard input
		float3 camRight = renderer.scene.camera.rotation.i;
		float3 camFwd = renderer.scene.camera.rotation.k;

		float3 moveDelta;

		if (IsKeyDown(KEY_A)) moveDelta -= camRight;
		if (IsKeyDown(KEY_D)) moveDelta += camRight;
		if (IsKeyDown(KEY_W)) moveDelta += camFwd;
		if (IsKeyDown(KEY_S)) moveDelta -= camFwd;

		renderer.scene.camera.offset += moveDelta * GetFrameTime();

		if (IsKeyDown(KEY_LEFT)) renderer.scene.camera.rotation.addYaw(moveSpeed * GetFrameTime());
		if (IsKeyDown(KEY_RIGHT)) renderer.scene.camera.rotation.addYaw(-moveSpeed * GetFrameTime());
		if (IsKeyDown(KEY_UP)) renderer.scene.camera.rotation.addPitch(moveSpeed * GetFrameTime());
		if (IsKeyDown(KEY_DOWN)) renderer.scene.camera.rotation.addPitch(- moveSpeed * GetFrameTime());

		// render and then write to the screen texture
		renderer.render();
		UpdateTexture(screenTexture, renderer.target.frameBuffer);

		// first writing to the RenderTexture2D
		BeginTextureMode(renderTexture);
			DrawTexture(screenTexture, 0, 0, WHITE);
		EndTextureMode();

		// writing render texture to screen
		BeginDrawing();
		DrawTexturePro(
				renderTexture.texture,
				Rectangle{ 0, 0, static_cast<float>(renderTexture.texture.width), static_cast<float>(renderTexture.texture.height) },
				Rectangle{ 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) },
				Vector2{ 0, 0 },
				0,
				WHITE);
		DrawFPS(10,10);
		EndDrawing();
	}

}
