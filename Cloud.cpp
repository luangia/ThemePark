#include "Cloud.h"
#include <cmath>
#include <GL/glu.h>
#include <iostream>
#include <map>
#include <vector>

// Constructor
bool Cloud::Initialize(const std::vector<float>& params) {
	subdivisionLevel = 0;
	needs_update = false;

	// Cloud position in space
	x = params[0];
	y = params[1];
	z = params[2];

	// Number of shapes and their size range
	int shapeCount = params[3];
	float maxRadius = params[4];
	float minRadius = params[5];

	// Clear existing data
	vertices.clear();
	triangles.clear();
	centers.clear();

	for (int i = 0; i < shapeCount; ++i) {
		// Random position and size for each shape center
		float centerX = dist(rng) * maxRadius * 1.7f + x;
		float centerY = dist(rng) * maxRadius * 1.7f + y;
		float centerZ = dist(rng) * maxRadius * 0.8f + z;

		// Add center to the centers vector
		centers.emplace_back(centerX, centerY, centerZ, 0.0f, -1);
		int centerIndex = centers.size() - 1;

		float radius = minRadius + (dist(rng) + 1.0f) / 2.0f * (maxRadius - minRadius);

		// Generate a shape at the random position
		generateShape(centerIndex, radius, subdivisionLevel);
	}

	// Generate the display list for rendering
	display_list = glGenLists(1);
	glNewList(display_list, GL_COMPILE);

	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_TRIANGLES);
	for (const auto& tri : triangles) {
		const auto& v1 = vertices[tri.v1];
		const auto& v2 = vertices[tri.v2];
		const auto& v3 = vertices[tri.v3];

		// Calculate normals for smooth shading
		glNormal3f(v1.x, v1.y, v1.z);
		glVertex3f(v1.x, v1.y, v1.z);

		glNormal3f(v2.x, v2.y, v2.z);
		glVertex3f(v2.x, v2.y, v2.z);

		glNormal3f(v3.x, v3.y, v3.z);
		glVertex3f(v3.x, v3.y, v3.z);
	}
	glEnd();

	glEndList();

	initialized = true;
	return true;
}


// Normalize a vertex to form a unit vector
Vertex Cloud::normalize(const Vertex& v) const {
	float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	return { v.x / length, v.y / length, v.z / length, v.radius, v.centerIndex };
}


// Perform a single level of subdivision
void Cloud::subdivide() {
	std::vector<Triangle> newTriangles;
	std::vector<Vertex> newVertices = vertices;
	std::map<std::pair<int, int>, int> midPointCache;

	auto getMidPointIndex = [&](int v1, int v2) -> int {
		int minVal = min(v1, v2);
		int maxVal = max(v1, v2);
		auto key = std::make_pair(minVal, maxVal);

		if (midPointCache.find(key) == midPointCache.end()) {
			// Compute midpoint
			Vertex mid = (vertices[v1] + vertices[v2]) * 0.5f;
			mid.radius = vertices[v1].radius;
			mid.centerIndex = vertices[v1].centerIndex;

			const Vertex& center = centers[mid.centerIndex];
			Vertex toCenter = normalize({ mid.x - center.x, mid.y - center.y, mid.z - center.z, mid.radius, mid.centerIndex });

			mid.x = toCenter.x * mid.radius + center.x;
			mid.y = toCenter.y * mid.radius + center.y;
			mid.z = toCenter.z * mid.radius + center.z;

			midPointCache[key] = newVertices.size();
			newVertices.push_back(mid);
		}
		return midPointCache[key];
		};
	for (const auto& tri : triangles) {
		int a = getMidPointIndex(tri.v1, tri.v2);
		int b = getMidPointIndex(tri.v2, tri.v3);
		int c = getMidPointIndex(tri.v3, tri.v1);

		newTriangles.push_back({ tri.v1, a, c });
		newTriangles.push_back({ tri.v2, b, a });
		newTriangles.push_back({ tri.v3, c, b });
		newTriangles.push_back({ a, b, c });
	}
	vertices = std::move(newVertices);
	triangles = std::move(newTriangles);

}


// Refine the model by one subdivision
void Cloud::refine() {
	++subdivisionLevel;
	subdivide();
	needs_update = true;
}

void Cloud::RegenerateDisplayList() {
	if (display_list != 0) {
		glDeleteLists(display_list, 1); // Delete the old display list
	}

	// Generate a new display list
	display_list = glGenLists(1);
	glNewList(display_list, GL_COMPILE);

	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_TRIANGLES);

	for (const auto& tri : triangles) {
		const auto& v1 = vertices[tri.v1];
		const auto& v2 = vertices[tri.v2];
		const auto& v3 = vertices[tri.v3];


		glNormal3f(v1.x, v1.y, v1.z);
		glVertex3f(v1.x, v1.y, v1.z);

		glNormal3f(v2.x, v2.y, v2.z);
		glVertex3f(v2.x, v2.y, v2.z);

		glNormal3f(v3.x, v3.y, v3.z);
		glVertex3f(v3.x, v3.y, v3.z);
	}
	glEnd();

	glEndList();

	// Mark as up-to-date
	needs_update = false;
}

// Draw the cloud
void Cloud::Draw() const {
	glPushMatrix();
	glCallList(display_list);
	glPopMatrix();

}

void Cloud::Update() {
	if (needs_update) {
		std::cout << "increase subdivision:" << std::endl;
		RegenerateDisplayList();
	}
	//Draw();
}

void Cloud::generateShape(int centerIndex, float radius, int subdivisions) {
	const Vertex& center = centers[centerIndex];

	// Start with an icosahedron for the base shape
	std::vector<Vertex> shapeVertices = {
		{ -1,  1.618f, 0, radius, centerIndex }, { 1,  1.618f, 0, radius, centerIndex },
		{ -1, -1.618f, 0, radius, centerIndex }, { 1, -1.618f, 0, radius, centerIndex },
		{ 0, -1,  1.618f, radius, centerIndex }, { 0,  1,  1.618f, radius, centerIndex },
		{ 0, -1, -1.618f, radius, centerIndex }, { 0,  1, -1.618f, radius, centerIndex },
		{ 1.618f, 0, -1, radius, centerIndex }, { 1.618f, 0,  1, radius, centerIndex },
		{ -1.618f, 0, -1, radius, centerIndex }, { -1.618f, 0,  1, radius, centerIndex },
	};

	std::vector<Triangle> shapeTriangles = {
		{ 0, 11, 5 }, { 0, 5, 1 }, { 0, 1, 7 }, { 0, 7, 10 }, { 0, 10, 11 },
		{ 1, 5, 9 }, { 5, 11, 4 }, { 11, 10, 2 }, { 10, 7, 6 }, { 7, 1, 8 },
		{ 3, 9, 4 }, { 3, 4, 2 }, { 3, 2, 6 }, { 3, 6, 8 }, { 3, 8, 9 },
		{ 4, 9, 5 }, { 2, 4, 11 }, { 6, 2, 10 }, { 8, 6, 7 }, { 9, 8, 1 }
	};

	// Normalize vertices and scale to radius
	for (auto& vertex : shapeVertices) {
		vertex = normalize(vertex); // Normalize to the unit sphere
		vertex.x = vertex.x * radius + center.x;
		vertex.y = vertex.y * radius + center.y;
		vertex.z = vertex.z * radius + center.z;
	}

	// Add vertices and triangles to the cloud
	int vertexOffset = vertices.size();
	vertices.insert(vertices.end(), shapeVertices.begin(), shapeVertices.end());

	for (const auto& tri : shapeTriangles) {
		triangles.push_back({ tri.v1 + vertexOffset, tri.v2 + vertexOffset, tri.v3 + vertexOffset });
	}
}

