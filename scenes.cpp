/*
 * Author: Nate Shaffer
 * Email: nshaffe4@u.rochester.edu
 * Date Created: 6/11/2025
 * 
 * Contains classes for objects, textures, as well as the scene and camera
 */

#include "scenes.h"

using namespace std;
using namespace nsGraphics;

vector<string> splitString(string s, string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

Object::Object(int numTriangles, vector<float3> points, vector<float2> uvCoords, std::vector<float3> normals,
			   vector<float3> vertexColors) :
		numTriangles(numTriangles),
		points(std::move(points)),
		uvCoords(std::move(uvCoords)),
		normals(std::move(normals)),
		vertexColors(std::move(vertexColors)),
		scale(1),
		offset(float3()),
		rotation(Rotation()) {
}

float3 Object::localToWorld(const float3 &p) const {
	float3 pWorld = rotation.apply(p);
	pWorld *= scale;
	pWorld += offset;
	return pWorld;
}

Object::Object(const char *fileName) {
	ifstream file(fileName);

	numTriangles = 0;
	scale = 1;
	vector<float3> vertices;    // total collection of vertices
	vector<float2> vertUVCoords;    // total collection of uv coordinates
	vector<float3> vertNormals;        // total collection of normals

	vector<float3> triangleColors;

	string line;
	while (getline(file, line)) {
		stringstream lineStream(line);
		string type;
		lineStream >> type;
		if (type == "v") {
			string xString;
			lineStream >> xString;
			string yString;
			lineStream >> yString;
			string zString;
			lineStream >> zString;
			float x = stof(xString);
			float y = stof(yString);
			float z = stof(zString);
			vertices.emplace_back(x, y, z);
			vertexColors.push_back(randColor());
		}
		if (type == "vt") {
			string uString;
			lineStream >> uString;
			string vString;
			lineStream >> vString;
			float u = stof(uString);
			float v = stof(vString);
			vertUVCoords.emplace_back(u, v);
		}
		if (type == "vn") {
			string xString;
			lineStream >> xString;
			string yString;
			lineStream >> yString;
			string zString;
			lineStream >> zString;
			float x = stof(xString);
			float y = stof(yString);
			float z = stof(zString);
			vertNormals.emplace_back(x, y, z);
		}
		if (type == "f") {
			vector<float3> faceVertices;
			vector<float2> faceUVs;
			vector<float3> faceNormals;

			vector<float3> faceVertexColors;        // TODO temp alsoo

			string faceGroup;
			while (lineStream >> faceGroup) {
				vector<string> indexGroup = splitString(faceGroup, "/");
				int pointIndex = stoi(indexGroup[0]) - 1;
				faceVertices.push_back(vertices[pointIndex]);
				if (indexGroup.size() > 1) {
					int uvIndex = stoi(indexGroup[1]) - 1;
					faceUVs.push_back(vertUVCoords[uvIndex]);
				} else {
					faceUVs.emplace_back();
				}
				if (indexGroup.size() > 2) {
					int normalIndex = stoi(indexGroup[2]) - 1;
					faceNormals.push_back(vertNormals[normalIndex]);
				} else {
					faceNormals.emplace_back(0, 0, 1);
				}
				faceVertexColors.push_back(vertexColors[pointIndex]);    // TODO temp
			}
			size_t sizeFace = faceVertices.size();
			for (int i = 2; i < sizeFace; i++) {
				numTriangles++;
				triangleColors.push_back(faceVertexColors[0]);
				triangleColors.push_back(faceVertexColors[i - 1]);
				triangleColors.push_back(faceVertexColors[i]);
				points.push_back(faceVertices[0]);
				points.push_back(faceVertices[i - 1]);
				points.push_back(faceVertices[i]);
				uvCoords.push_back(faceUVs[0]);
				uvCoords.push_back(faceUVs[i - 1]);
				uvCoords.push_back(faceUVs[i]);
				normals.push_back(faceNormals[0]);
				normals.push_back(faceNormals[i - 1]);
				normals.push_back(faceNormals[i]);
			}
		}
	}
	file.close();
}

// parse an obj file into an object, splitting faces into triangles
Object parseObj(const char *fileName) {
	ifstream file(fileName);

	int numTriangles = 0;
	vector<float3> vertices;    // total collection of vertices
	vector<float3> points;        // ordered collection of points in each triangle
	vector<float2> vertUVCoords;    // total collection of uv coordinates
	vector<float2> uvCoords;        // ordered collection of uv coordinates at each point
	vector<float3> vertNormals;        // total collection of normals
	vector<float3> normals;            // ordered collection of normals at each point

	vector<float3> triangleColors;
	vector<float3> vertexColors;        // TODO temp for rainbow rand colors

	string line;
	while (getline(file, line)) {
		stringstream lineStream(line);
		string type;
		lineStream >> type;
		if (type == "v") {
			string xString;
			lineStream >> xString;
			string yString;
			lineStream >> yString;
			string zString;
			lineStream >> zString;
			float x = stof(xString);
			float y = stof(yString);
			float z = stof(zString);
			vertices.emplace_back(x, y, z);
			vertexColors.push_back(randColor());
		}
		if (type == "vt") {
			string uString;
			lineStream >> uString;
			string vString;
			lineStream >> vString;
			float u = stof(uString);
			float v = stof(vString);
			vertUVCoords.emplace_back(u, v);
		}
		if (type == "vn") {
			string xString;
			lineStream >> xString;
			string yString;
			lineStream >> yString;
			string zString;
			lineStream >> zString;
			float x = stof(xString);
			float y = stof(yString);
			float z = stof(zString);
			vertNormals.emplace_back(x, y, z);
		}
		if (type == "f") {
			vector<float3> faceVertices;
			vector<float2> faceUVs;
			vector<float3> faceNormals;

			vector<float3> faceVertexColors;        // TODO temp alsoo

			string faceGroup;
			while (lineStream >> faceGroup) {
				vector<string> indexGroup = splitString(faceGroup, "/");
				int pointIndex = stoi(indexGroup[0]) - 1;
				faceVertices.push_back(vertices[pointIndex]);
				if (indexGroup.size() > 1) {
					int uvIndex = stoi(indexGroup[1]) - 1;
					faceUVs.push_back(vertUVCoords[uvIndex]);
				} else {
					faceUVs.emplace_back();
				}
				if (indexGroup.size() > 2) {
					int normalIndex = stoi(indexGroup[2]) - 1;
					faceNormals.push_back(vertNormals[normalIndex]);
				} else {
					faceNormals.emplace_back(0, 0, 1);
				}
				faceVertexColors.push_back(vertexColors[pointIndex]);    // TODO temp
			}
			size_t sizeFace = faceVertices.size();
			for (int i = 2; i < sizeFace; i++) {
				numTriangles++;
				triangleColors.push_back(faceVertexColors[0]);
				triangleColors.push_back(faceVertexColors[i - 1]);
				triangleColors.push_back(faceVertexColors[i]);
				points.push_back(faceVertices[0]);
				points.push_back(faceVertices[i - 1]);
				points.push_back(faceVertices[i]);
				uvCoords.push_back(faceUVs[0]);
				uvCoords.push_back(faceUVs[i - 1]);
				uvCoords.push_back(faceUVs[i]);
				normals.push_back(faceNormals[0]);
				normals.push_back(faceNormals[i - 1]);
				normals.push_back(faceNormals[i]);
			}
		}
	}
	file.close();
	return {numTriangles, points, uvCoords, normals, triangleColors};
}

Scene::Scene(vector<Object> objects) : objects(std::move(objects)), camera(Camera()) {}

Texture::Texture(int width, int height, float3 **image) : width(width), height(height), image(image) {}

Texture::Texture(const char *fileName) {
	ifstream data(fileName, ios::binary);
	data.ignore(10);
	int pixelOffset;
	data.read((char *) &pixelOffset, 4);
	data.ignore(4);
	data.read((char *) &width, 4);                    // read in the width
	data.read((char *) &height, 4);                    // read in the height
	data.ignore(2);
	short bpp;
	data.read((char *) &bpp, 2);                    // read in bits per pixel

	int rowWidth = (int) ((bpp * width + 31) / 32) * 4;
	data.seekg(pixelOffset);

	image = new float3 *[height];
	for (int row = 0; row < height; row++) {
		image[row] = new float3[width];
		byte rowPixels[rowWidth];
		data.read((char *) rowPixels, rowWidth);
		for (int col = 0; col < width; col++) {
			float3 color;
			if (bpp == 24) {
				color = float3((float) rowPixels[3 * col + 0], (float) rowPixels[3 * col + 1],
							   (float) rowPixels[3 * col + 2]);
			} else if (bpp == 32) {
				color = float3((float) rowPixels[4 * col + 0], (float) rowPixels[4 * col + 1],
							   (float) rowPixels[4 * col + 2]);
			}
			image[row][col] = color;
		}
	}
}

// by default, make a 20x20 grey texture
Texture::Texture() : width(20), height(20) {
	image = new float3 *[height];
	for (int row = 0; row < height; row++) {
		image[row] = new float3[width];
		for (int col = 0; col < width; col++) {
			float3 color(200, 200, 200);
			image[row][col] = color;
		}
	}
}

float3 Texture::sample(float2 uv) const {
	int s = clamp((int) (uv.x * (float) (width - 1)), 0, width - 1);
	int t = clamp((int) (uv.y * (float) (height - 1)), 0, height - 1);
	return image[t][s];
}

Camera::Camera() : offset(float3()), rotation(Rotation()), fov(2*M_PI/3) {}

Camera::Camera(float3 offset, Rotation rotation, float fov) : offset(offset), rotation(rotation), fov(fov) {}
