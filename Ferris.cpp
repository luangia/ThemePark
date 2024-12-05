#include "Ferris.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Constructor
Ferris::Ferris() : displayListID(0) {}

// Destructor
Ferris::~Ferris() {
	if (initialized)
	{
		glDeleteLists(displayListID, 1);
	}
}

// Function to load an OBJ file
bool Ferris::loadOBJ(const std::string& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filepath << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream stream(line);
		std::string prefix;
		stream >> prefix;

		if (prefix == "v") { // Vertex position
			processVertex(line);
		} else if (prefix == "vn") { // Vertex normal
			processNormal(line);
		} else if (prefix == "f") { // Face
			processFace(line);
		}
	}

	file.close();
	return true;
}

// Initialize the model for rendering using display lists
bool Ferris::Initialize() {
	if (!loadOBJ("ferris.obj")) {
		std::cerr << "Failed to load OBJ file" << std::endl;
		return false;
	}

	// Generate a new display list ID
	displayListID = glGenLists(1);
	glNewList(displayListID, GL_COMPILE);

	glScalef(12.0f, 12.0f, 12.0f); // Scale by 10
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotate 90 degrees around the x-axis
	glTranslatef(0.0f, -0.1f, 0.0f);

	//glColor3f(0.4f, 0.6f, 1.0f);
	// Begin rendering the model
	glBegin(GL_TRIANGLES);


	// Compute max dimension for normalization
	float maxDimension = 0.0f;
	for (const auto& vertex : vertices) {
		maxDimension = max(maxDimension, fabs(vertex.position.x));
		maxDimension = max(maxDimension, fabs(vertex.position.y));
		maxDimension = max(maxDimension, fabs(vertex.position.z));
	}

	for (const auto& vertex : vertices) {
		// Apply artistic gradient color
		float blendFactor = (vertex.position.y + maxDimension) / (2 * maxDimension); // Normalize
		glColor3f(
			(1.0f - blendFactor) * 1.0f + blendFactor * 0.5f, // Blend red
			(1.0f - blendFactor) * 0.5f + blendFactor * 0.8f, // Blend green
			(1.0f - blendFactor) * 0.7f + blendFactor * 1.0f  // Blend blue
		);
		// Set normal
		glNormal3f(vertex.normal.x, vertex.normal.y, vertex.normal.z);

		// Set position
		glVertex3f(vertex.position.x, vertex.position.y, vertex.position.z);
	}

	glEnd();

	glEndList();
	initialized = true;
	return true;
}

// Render the model
void Ferris::Draw() {
	glPushMatrix();
	glCallList(displayListID);
	glPopMatrix();
}

// Helper function to process a vertex position
void Ferris::processVertex(const std::string& line) {
	std::istringstream stream(line);
	std::string prefix;
	Vec3 position;
	stream >> prefix >> position.x >> position.y >> position.z;
	tempPositions.push_back(position);
}

// Helper function to process a normal
void Ferris::processNormal(const std::string& line) {
	std::istringstream stream(line);
	std::string prefix;
	Vec3 normal;
	stream >> prefix >> normal.x >> normal.y >> normal.z;
	tempNormals.push_back(normal);
}

// Helper function to process a face
void Ferris::processFace(const std::string& line) {
	std::istringstream stream(line);
	std::string prefix;
	unsigned int posIdx[3], texIdx[3], normIdx[3];
	char slash;

	stream >> prefix;
	for (int i = 0; i < 3; i++) { // Assume triangular faces
		stream >> posIdx[i] >> slash >> texIdx[i] >> slash >> normIdx[i];

		// Create vertex
		FVertex vertex;
		vertex.position = tempPositions[posIdx[i] - 1]; // OBJ is 1-indexed
		vertex.normal = tempNormals[normIdx[i] - 1];

		// Add vertex and index
		vertices.push_back(vertex);
		indices.push_back(vertices.size() - 1);
	}
}
