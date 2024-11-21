#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

class SceneObject {
public:
    virtual void Draw() const = 0;  // Pure virtual function for drawing
    virtual ~SceneObject() = default;  // Virtual destructor for cleanup
};

#endif // SCENE_OBJECT_H
