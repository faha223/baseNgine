#include "texture.h"

GLuint loadTexBMP(const char* filename, GLenum filter)
{
        GLuint texture = 0;
        SDL_Surface *surface = NULL;
	if(filename != NULL)
	        surface = SDL_LoadBMP(filename);
        if(surface == NULL)
                return 0;
        glGenTextures(1, &texture);                                             // allocate a texture name
	if(texture == 0)
		return 0;
        glBindTexture(GL_TEXTURE_2D, texture);                                  // select our current texture
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);            // select modulate to mix texture with color for shading
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);       // when texture area is small, bilinear filter the closest mipmap
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);       // when texture area is large, bilinear filter the first mipmap
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);           // wrap the textures Horizontally
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);           // wrap the textures Vertically
        switch(surface->format->BitsPerPixel)
        {
                case 32:
                {
                        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, surface->w, surface->h, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
                        break;
                }
                case 24:
                {
                        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, surface->w, surface->h, GL_BGR_EXT, GL_UNSIGNED_BYTE, surface->pixels);
                        break;
                }
                case 16:
                {
                        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, surface->w, surface->h, GL_BGR_EXT, GL_UNSIGNED_BYTE, surface->pixels);
                        break;
                }
                case 8:
                {
                        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, surface->w, surface->h, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
                        break;
                }
                default:
                        break;
        }
        SDL_FreeSurface(surface);

        return texture;
};

GLuint loadBumpBMP(const char* filename, GLenum filter)
{
        GLuint texture = 0;
        SDL_Surface *surface = NULL;
	if(filename != NULL)
	        surface = SDL_LoadBMP(filename);
        if(surface == NULL)
                return 0;
        glGenTextures(1, &texture);                                             // allocate a texture name
	if(texture == 0)
		return 0;
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, texture);                                  // select our current texture
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);            // select modulate to mix texture with color for shading
        glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, filter);       // when texture area is small, bilinear filter the closest mipmap
        glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, filter);       // when texture area is large, bilinear filter the first mipmap
        glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_REPEAT);           // wrap the textures Horizontally
        glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_REPEAT);           // wrap the textures Vertically
        switch(surface->format->BitsPerPixel)
        {
                case 32:
                {
                        gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_ARB, 4, surface->w, surface->h, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
                        break;
                }
                case 24:
                {
                        gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_ARB, 3, surface->w, surface->h, GL_BGR_EXT, GL_UNSIGNED_BYTE, surface->pixels);
                        break;
                }
                case 16:
                {
                        gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_ARB, 3, surface->w, surface->h, GL_BGR_EXT, GL_UNSIGNED_BYTE, surface->pixels);
                        break;
                }
                case 8:
                {
                        gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_ARB, 4, surface->w, surface->h, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
                        break;
                }
                default:
                        break;
        }
        SDL_FreeSurface(surface);

        return texture;
};
