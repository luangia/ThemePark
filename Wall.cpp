/*
 * Stuff.cpp: A class for drawing a cube.
 *
 * 
 */


#include "Wall.h"
#include "libtarga.h"
#include <stdio.h>
#include <GL/glu.h>

 // Destructor
Wall::~Wall(void)
{
    if (initialized)
    {
        glDeleteLists(display_list, 1);
        glDeleteTextures(1, &texture_obj);
    }
}


// Initializer. Returns false if something went wrong, like not being able to
// load the texture.
bool
Wall::Initialize(float x, float y, float z, float width, float height, float depth)
{
    ubyte* image_data;
    int	    image_height, image_width;

    // Now do the geometry. Create the display list.
    display_list = glGenLists(1);
    glNewList(display_list, GL_COMPILE);
    // Use white, because the texture supplies the color.
    glColor3f(1.0, 1.0, 1.0);

    // The surface normal is up for the ground.
    glNormal3f(0.0, 0.0, 1.0);

    // Draw the ground as a quadrilateral, specifying texture coordinates.
    glBegin(GL_QUADS);
    // Front face
    glVertex3f(x, y, z);
    glVertex3f(x + width, y, z);
    glVertex3f(x + width, y + height, z);
    glVertex3f(x, y + height, z);

    // Back face
    glVertex3f(x, y, z - depth);
    glVertex3f(x + width, y, z - depth);
    glVertex3f(x + width, y + height, z - depth);
    glVertex3f(x, y + height, z - depth);

    // Left face
    glVertex3f(x, y, z);
    glVertex3f(x, y, z - depth);
    glVertex3f(x, y + height, z - depth);
    glVertex3f(x, y + height, z);

    // Right face
    glVertex3f(x + width, y, z);
    glVertex3f(x + width, y, z - depth);
    glVertex3f(x + width, y + height, z - depth);
    glVertex3f(x + width, y + height, z);

    // Top face
    glVertex3f(x, y + height, z);
    glVertex3f(x + width, y + height, z);
    glVertex3f(x + width, y + height, z - depth);
    glVertex3f(x, y + height, z - depth);

    // Bottom face
    glVertex3f(x, y, z);
    glVertex3f(x + width, y, z);
    glVertex3f(x + width, y, z - depth);
    glVertex3f(x, y, z - depth);
    glEnd();

    glEndList();

    // We only do all this stuff once, when the GL context is first set up.
    initialized = true;

    return true;
}


// Draw just calls the display list we set up earlier.
void
Wall::Draw(void)
{
    glPushMatrix();
    glCallList(display_list);
    glPopMatrix();
}


