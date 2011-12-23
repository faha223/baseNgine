#include "included.h"

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

GLuint loadTexBMP(const char* = NULL, GLenum filter = GL_LINEAR);	// loads a texture from a bitmap file
GLuint loadBumpBMP(const char* = NULL, GLenum filter = GL_LINEAR);	// loads a texture from a bitmap file

#endif
