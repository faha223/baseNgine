#include "included.h"
#include "vectorMath.h"
#include "model.h"
#include <fstream>
#include <vector>

#ifndef _GAME_H_
#define _GAME_H_

struct object
{
	object(model* = NULL, const vector3& = vector3(0.0f, 0.0f, 0.0f), const quaternion& = quaternion());
	~object();
	model* mdl;
	GLfloat* vertexBuffer;
	GLuint VBO;
	int state;
	quaternion rotation;
	vector3 position;
	void renderFrame();
};

struct light
{
	GLfloat ambient[4], diffuse[4], direction[4], specular[4];
};

struct prop : public object
{
	// This object is not animated and as such does not need any fancy animation frames;
	float inverseMass;
	//TODO: Add objects to store collision geometry
};

struct avatar : public object
{
	avatar();
	~avatar();
	animFrame frame;
	//TODO: Add objects to store collision geometry
};

class game
{
private:
	std::ofstream logfile;
	SDL_Surface *screen;
	int cv_lastUpdate;
	int cv_lastFrameUpdate;
	bool quit;
	bool sv_cheats;
	bool cv_fullscreen;
	bool cv_showfps;
	bool cv_vsync;
	bool con_background;
	bool cv_drawReference;
	unsigned int con_consoleHistory;
	// shaders
	GLhandleARB bumpMap, perVertex;
	vector2 cameraRotation;
	vector3 cameraPosition;
	vector3 cv_screenRes;
	std::vector<light> lights;
	bool loadConfig(const char* = NULL);	// returns true if config file loaded successfully, false if no config file found
	bool GL_Init();
	bool ResizeGLScene(const int& resx, const int& resy, const int& fov);
	int loadShader(const char* shadername, GLhandleARB &shader);
	Uint8 *keystate;
	vector2 mouseDisp;
	SDL_Event Event;
	model trafficCone;
	object* cone1;
public:
	game();
	~game();
	int run();
	int update(const int& time);
	int drawScene(const int& time);
};

#endif
