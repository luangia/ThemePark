#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <vector>

class SceneObject {
public:
	virtual void Draw() const = 0;  // Pure virtual function for drawing
	virtual ~SceneObject() = default;  // Virtual destructor for cleanup
	// Virtual Initialize function to set up object-specific parameters
	virtual bool Initialize(const std::vector<float>& params) = 0;
};

#endif // SCENE_OBJECT_H
