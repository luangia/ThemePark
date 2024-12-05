#ifndef FERRIS_H
#define FERRIS_H

#include <vector>
#include <string>
#include <FL/gl.h>

// Custom structures for storing 3D data
struct Vec3 {
	float x, y, z;
};

struct Vec2 {
	float u, v;
};

struct FVertex {
	Vec3 position;
	Vec3 normal;
};

class Ferris {
public:
	// Public member variables
	std::vector<FVertex> vertices;
	std::vector<unsigned int> indices;
	bool    initialized;

	// Constructor and Destructor
	Ferris();
	~Ferris();

	// Load an OBJ file
	bool loadOBJ(const std::string& filepath);

	// Initialize the model for rendering using display lists
	bool Initialize();

	// Render the model
	void Draw();

private:
	GLuint displayListID; // ID for the OpenGL display list

	// Temporary storage for parsing
	std::vector<Vec3> tempPositions;
	std::vector<Vec3> tempNormals;

	// Helper functions to parse OBJ data
	void processVertex(const std::string& line);
	void processNormal(const std::string& line);
	void processFace(const std::string& line);
};

#endif // FERRIS_H
