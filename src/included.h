#include <SDL/SDL.h>
//#include <SDL/SDL_opengl.h>	//I was using this one, but then it became apparent that I need extensions to use VBOs
//#ifdef GL_VERSION_1_5		// If the gl version is already defined
//#undef GL_VERSION_1_5		// undefine it
//#define GL_VERSION_1_5 1	// and redefine it as though Version 1.5 is installed (fight the power!)
//#endif
#define GL_GLEXT_PROTOTYPES	// Use the glext prototypes
#include <GL/gl.h>		//
#include <GL/glu.h>		//
#include <GL/glext.h>		//
