#include "Cloud.h"
#include <GL/glu.h>
#include <cmath>
#include <map>

// Constructor
bool Cloud::Initialize(const std::vector<float>& params) {
    subdivisionLevel = 0;

    x = params[0];
    y = params[1];
    z = params[2];
    int shapeCount = params[3];
    float maxRadius = params[4];
    float minRadius = params[5];

    vertices.clear();
    triangles.clear();

    for (int i = 0; i < shapeCount; ++i) {
        // Random position and size for each shape
        Vertex center = { dist(rng) * maxRadius + x,
                          dist(rng) * maxRadius + y,
                          dist(rng) * maxRadius + z };
        float radius = minRadius + (dist(rng) + 1.0f) / 2.0f * (maxRadius - minRadius);

        // Generate a shape at the random position
        generateShape(center, radius, subdivisionLevel);
    }

    // Now do the geometry. Create the display list.
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

    initialized = true;

    return true;
}

// Normalize a vertex to form a unit vector
Vertex Cloud::normalize(const Vertex& v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return { v.x / length, v.y / length, v.z / length };
}

// Find the midpoint between two vertices
Vertex Cloud::midPoint(const Vertex& v1, const Vertex& v2) {
    return normalize({ (v1.x + v2.x) / 2.0f, (v1.y + v2.y) / 2.0f, (v1.z + v2.z) / 2.0f });
}

// Perform a single level of subdivision
void Cloud::subdivide() {
    std::vector<Triangle> newTriangles;
    std::vector<Vertex> newVertices = vertices;
    std::map<std::pair<int, int>, int> midPointCache;

    auto getMidPointIndex = [&](int v1, int v2) -> int {
        int minVal = (((v1) < (v2)) ? (v1) : (v2));
        int maxVal = (((v1) > (v2)) ? (v1) : (v2));
        auto key = std::make_pair(minVal, maxVal);
        if (midPointCache.find(key) == midPointCache.end()) {
            Vertex mid = midPoint(vertices[key.first], vertices[key.second]);
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
}

// Draw the cloud
void Cloud::Draw() const {
    glPushMatrix();
    glCallList(display_list);
    glPopMatrix();
}

// Generate a single sphere/ellipsoid and add it to the cloud
void Cloud::generateShape(const Vertex& center, float radius, int subdivisions) {
    // Start with an icosahedron for the base shape
    std::vector<Vertex> shapeVertices = {
        { -1,  1.618f, 0 }, { 1,  1.618f, 0 }, { -1, -1.618f, 0 }, { 1, -1.618f, 0 },
        { 0, -1,  1.618f }, { 0,  1,  1.618f }, { 0, -1, -1.618f }, { 0,  1, -1.618f },
        { 1.618f, 0, -1 }, { 1.618f, 0,  1 }, { -1.618f, 0, -1 }, { -1.618f, 0,  1 },
    };

    std::vector<Triangle> shapeTriangles = {
        { 0, 11, 5 }, { 0, 5, 1 }, { 0, 1, 7 }, { 0, 7, 10 }, { 0, 10, 11 },
        { 1, 5, 9 }, { 5, 11, 4 }, { 11, 10, 2 }, { 10, 7, 6 }, { 7, 1, 8 },
        { 3, 9, 4 }, { 3, 4, 2 }, { 3, 2, 6 }, { 3, 6, 8 }, { 3, 8, 9 },
        { 4, 9, 5 }, { 2, 4, 11 }, { 6, 2, 10 }, { 8, 6, 7 }, { 9, 8, 1 }
    };

    // Normalize vertices and scale to radius
    for (auto& vertex : shapeVertices) {
        vertex = normalize(vertex);
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
