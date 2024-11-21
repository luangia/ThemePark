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
Wall::Initialize(const std::vector<float>& params)
{
    x = params[0];
    y = params[1];
    z = params[2];
    width = params[3];
    height = params[4];
    depth = params[5];

    ubyte* image_data;
    int	    image_height, image_width;

    // Load the image for the texture. The texture file has to be in
    // a place where it will be found.
    if (!(image_data = (ubyte*)tga_load("brick.tga", &image_width,
        &image_height, TGA_TRUECOLOR_24)))
    {
        fprintf(stderr, "Wall::Initialize: Couldn't load grass.tga\n");
        return false;
    }

    // This creates a texture object and binds it, so the next few operations
    // apply to this texture.
    glGenTextures(1, &texture_obj);
    glBindTexture(GL_TEXTURE_2D, texture_obj);

    // This sets a parameter for how the texture is loaded and interpreted.
    // basically, it says that the data is packed tightly in the image array.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // This sets up the texture with high quality filtering. First it builds
    // mipmaps from the image data, then it sets the filtering parameters
    // and the wrapping parameters. We want the grass to be repeated over the
    // ground.
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image_width, image_height,
        GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        GL_NEAREST_MIPMAP_LINEAR);

    // This says what to do with the texture. Modulate will multiply the
    // texture by the underlying color.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Now do the geometry. Create the display list.
    display_list = glGenLists(1);
    glNewList(display_list, GL_COMPILE);
    // Use white, because the texture supplies the color.
    glColor3f(1.0, 1.0, 1.0);

    // Turn on texturing and bind the grass texture.
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_obj);

    glBegin(GL_QUADS);
    // Front face
    glNormal3f(0.0f, 0.0f, 1.0f); // Normal pointing outward along the Z-axis
    glTexCoord2f(1000.0, 1000.0); glVertex3f(x, y, z);
    glTexCoord2f(-1000.0, 1000.0); glVertex3f(x + width, y, z);
    glTexCoord2f(-1000.0, -1000.0); glVertex3f(x + width, y + height, z);
    glTexCoord2f(1000.0, -1000.0); glVertex3f(x, y + height, z);

    // Back face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1000.0, 1000.0); glVertex3f(x, y + height, z - depth);
    glTexCoord2f(-1000.0, 1000.0); glVertex3f(x + width, y + height, z - depth);
    glTexCoord2f(-1000.0, -1000.0); glVertex3f(x + width, y, z - depth);
    glTexCoord2f(1000.0, -1000.0); glVertex3f(x, y, z - depth);

    // Left face
    glNormal3f(-1.0f, 0.0f, 0.0f); // Normal pointing inward along the X-axis
    glTexCoord2f(1000.0, 1000.0); glVertex3f(x, y + height, z);
    glTexCoord2f(-1000.0, 1000.0); glVertex3f(x, y + height, z - depth);
    glTexCoord2f(-1000.0, -1000.0); glVertex3f(x, y, z - depth);
    glTexCoord2f(1000.0, -1000.0); glVertex3f(x, y, z);

    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f); // Normal pointing outward along the X-axis
    glTexCoord2f(1000.0, 1000.0); glVertex3f(x + width, y, z);
    glTexCoord2f(-1000.0, 1000.0); glVertex3f(x + width, y, z - depth);
    glTexCoord2f(-1000.0,-1000.0); glVertex3f(x + width, y + height, z - depth);
    glTexCoord2f(1000.0, -1000.0); glVertex3f(x + width, y + height, z);

    // Top face
    glNormal3f(0.0f, 1.0f, 0.0f); // Normal pointing outward along the Y-axis
    glTexCoord2f(1000.0, 1000.0); glVertex3f(x, y + height, z);
    glTexCoord2f(-1000.0, 1000.0); glVertex3f(x + width, y + height, z);
    glTexCoord2f(-1000.0, -1000.0); glVertex3f(x + width, y + height, z - depth);
    glTexCoord2f(1000.0, -1000.0); glVertex3f(x, y + height, z - depth);

    // Bottom face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1000.0, 1000.0); glVertex3f(x, y, z - depth);
    glTexCoord2f(-1000.0, 1000.0); glVertex3f(x + width, y, z - depth);
    glTexCoord2f(-1000.0, -1000.0); glVertex3f(x + width, y, z);
    glTexCoord2f(1000.0, -1000.0); glVertex3f(x, y, z);

    glEnd();

    glEndList();

    // We only do all this stuff once, when the GL context is first set up.
    initialized = true;

    return true;
}


// Draw just calls the display list we set up earlier.
void
Wall::Draw(void) const
{
    glPushMatrix();
    glCallList(display_list);
    glPopMatrix();
}


