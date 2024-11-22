#ifndef CLOUD_H
#define CLOUD_H

#include <vector>
#include <Fl/gl.h>
#include "SceneObject.h" 
#include <random>

struct Vertex {
    float x, y, z;
};

struct Triangle {
    int v1, v2, v3; // Indices of vertices
};

class Cloud : public SceneObject {
private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    int subdivisionLevel;

    GLubyte display_list;   // The display list that does all the work.
    GLuint  texture_obj;    // The object for the grass texture.
    bool    initialized;    // Whether or not we have been initialised.

    float x, y, z;

    void generateShape(const Vertex& center, float radius, int subdivisions);
    void subdivide();

    Vertex normalize(const Vertex& v);
    Vertex midPoint(const Vertex& v1, const Vertex& v2);

    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;

public:
    Cloud(void) { };
    bool Initialize(const std::vector<float>& params) override;	// Gets everything set up for drawing.
    void refine();
    void Draw() const override;
};

#endif // CLOUD_H
