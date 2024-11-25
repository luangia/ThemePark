#ifndef CLOUD_H
#define CLOUD_H

#include "SceneObject.h" 
#include <Fl/gl.h>
#include <random>
#include <vector>

struct Vertex {
	float x, y, z;
	float radius; // Radius of the sphere this vertex belongs to
	int centerIndex; // Center of the sphere this vertex belongs to

	Vertex operator-(const Vertex& other) const {
		return { x - other.x, y - other.y, z - other.z, radius, centerIndex };
	}

	Vertex operator+(const Vertex& other) const {
		return { x + other.x, y + other.y, z + other.z, radius, centerIndex };
	}

	Vertex operator*(float scalar) const {
		return { x * scalar, y * scalar, z * scalar, radius, centerIndex };
	}
	// Constructor to initialize all fields
	Vertex(float x, float y, float z, float radius, int centerIndex)
		: x(x), y(y), z(z), radius(radius), centerIndex(centerIndex) {
	}
};

struct Triangle {
	int v1, v2, v3; // Indices of vertices
};

class Cloud : public SceneObject {
private:
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
	std::vector<Vertex> centers;

	std::vector<Vertex> normals;
	int subdivisionLevel;
	mutable bool needs_update; // Flag to track if display list needs regeneration

	mutable GLubyte display_list;   // The display list that does all the work. 
	bool    initialized;    // Whether or not we have been initialised.

	float x, y, z;

	void generateShape(int centerIndex, float radius, int subdivisions);
	void subdivide();
	void RegenerateDisplayList();

	Vertex normalize(const Vertex& v) const;

	// Random number generator
	std::mt19937 rng;
	std::uniform_real_distribution<float> dist;

public:
	Cloud(void) {};
	bool Initialize(const std::vector<float>& params) override;	// Gets everything set up for drawing.
	void refine();
	void Draw() const override;
	void Update();
};

#endif // CLOUD_H
#pragma once
