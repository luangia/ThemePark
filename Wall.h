/*
 * Ground.h: Header file for a class that draws the ground.
 *
 * (c) 2001-2002: Stephen Chenney, University of Wisconsin at Madison.
 */


#ifndef _WALL_H_
#define _WALL_H_

#include <Fl/gl.h>
#include "SceneObject.h"

class Wall : public SceneObject {
private:
	GLubyte display_list;   // The display list that does all the work.
	GLuint  texture_obj;    // The object for the grass texture.
	bool    initialized;    // Whether or not we have been initialised.
	float x, y, z;
	float width, height, depth;

public:
	// Constructor. Can't do initialization here because we are
	// created before the OpenGL context is set up.
	Wall(void) { display_list = 0; initialized = false; };

	// Destructor. Frees the display lists and texture object.
	~Wall(void);

	// Initializer. Creates the display list.
	bool    Initialize(const std::vector<float>& params) override;

	// Does the drawing.
	void    Draw(void) const override;
};


#endif
