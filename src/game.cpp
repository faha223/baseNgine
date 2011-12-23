#include "game.h"
#include "included.h"
#include "userInput.h"
#include <iostream>
#include <cmath>
using namespace std;

#define pi 3.141592f
#define BUFFER_OFFSET(i) ((char*)NULL + (i))

#define ROUNDOFF(i) (i)

GLuint BMapTangent, BMapBumpMapTex, BMapDecalTex, BMapLightDir, PVLdiffuseMap;
bool bump = true;
bool getBool(const string& = NULL);
int getInt(const string& = NULL);
float getFloat(const string& = NULL);

char *textFileRead(const char* = NULL);
void FindInvTBN(vector3 Vertices[3], vector2 TexCoords[3], vector3 & InvNormal, vector3 & InvBinormal, vector3 & InvTangent);

void shaderBumpMap(const GLhandleARB& = 0, const vector3& = vector3(0.0f, -1.0f, 0.0f));

game::game()
{
	//Initialize the discrete variables that control the game, these will be replaced by a single funtion that reads them in from a file
	logfile.open("log.dat");
	cv_lastUpdate = 0;
	cv_lastFrameUpdate = 0;
	cv_drawReference = 0;

	cv_screenRes = vector3(640.0f, 480.0f, 60.0f);
	cv_fullscreen = true;
	cv_vsync = true;

	if(!loadConfig("./default.cfg"))		// This function loads the config file stored in this user's home directory
		loadConfig("bin/base/default.cfg");		// This function loads the config file stored in the game directory

	//Don't quit immediately, we've got stuff to do
	quit = false;

	//Initialize SDL and catch the error returned
	int i_error = SDL_Init(SDL_INIT_EVERYTHING);
	// If SDL had a problem initializing, report the error and exit
	if(i_error == -1)
	{
		printf("SDL_Init failed: %s\n", SDL_GetError());
		exit(1);
	}
	// Set up the screen based on whether we're fullscreen or not
	if(cv_fullscreen)
	{
		// We want a hardware accelerated opengl surface that uses double buffering, and we want it in fullscreen
		screen = SDL_SetVideoMode(cv_screenRes.x, cv_screenRes.y, 32, SDL_HWSURFACE | SDL_FULLSCREEN | SDL_OPENGL | SDL_DOUBLEBUF);
	}
	else
	{
		// We want a hardware accelerated opengl surface that uses double buffering, and we want it in a window
		screen = SDL_SetVideoMode(cv_screenRes.x, cv_screenRes.y, 32, SDL_HWSURFACE | SDL_NOFRAME | SDL_OPENGL | SDL_DOUBLEBUF);
	}
	// Make sure to grab all mouse input since we're hiding the mouse and using it for input
	SDL_ShowCursor(SDL_DISABLE);				// Hide the cursor
	SDL_WM_GrabInput(SDL_GRAB_ON);				// Make sure you can get the displacement of the cursor no matter what

	GL_Init();	//Initialize the GL state
			//And set up the viewport
	ResizeGLScene(cv_screenRes.x, cv_screenRes.y, 90);
	// We control buffer swaps
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

	// Load the TrafficCone prop model (It is my own invention), and setup the cone1 object to instance it
	trafficCone.load("props/crate");
	cone1 = new object(&trafficCone);

	//Center the camera
	cameraPosition = vector3(0.0f, 0.0f, 0.0f);
	// And make it aim forward
	cameraRotation = vector2(0.0f, 0.0f);

	light newLight;
	newLight.ambient[0] = 0.2f;
	newLight.ambient[1] = 0.2f;
	newLight.ambient[2] = 0.2f;
	newLight.ambient[3] = 1.0f;
	newLight.specular[0] = 0.2f;
	newLight.specular[1] = 0.2f;
	newLight.specular[2] = 0.2f;
	newLight.specular[3] = 1.0f;
	newLight.diffuse[0] = 1.0f;
	newLight.diffuse[1] = 1.0f;
	newLight.diffuse[2] = 1.0f;
	newLight.diffuse[3] = 1.0f;
	newLight.direction[0] = 0.0f;
	newLight.direction[1] = 1.0f;
	newLight.direction[2] = 0.0f;
	newLight.direction[3] = 1.0f;

	lights.push_back(newLight);
	for(size_t i = 0; i < lights.size(); ++i)
	{
		glLightfv(GL_LIGHT0, GL_AMBIENT, lights[i].ambient);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lights[i].specular);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lights[i].diffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, lights[i].direction);
		glEnable(GL_LIGHT0);
	}

	// Report the version of OpenGL that's available
	logfile << "GL Version: " << glGetString(GL_VERSION) << endl;
	cout << "GL Version: " << glGetString(GL_VERSION) << endl;
}

int game::run()
{
	//This is now the most recent update
	cv_lastUpdate = update(SDL_GetTicks());
	cv_lastFrameUpdate = drawScene(SDL_GetTicks());
	while(!quit)
	{
		if(cv_vsync)	//If we want vsync
		{
			do	//Update at least once
			{
				usleep(1);	// Sleep until it's time to work
			}while(float(SDL_GetTicks() - cv_lastFrameUpdate) < 1000.0f/cv_screenRes.z);	// clamp the framerate at the refresh rate
			cv_lastUpdate = update(cv_lastUpdate);
			cv_lastFrameUpdate = drawScene(cv_lastFrameUpdate);	// Then render the frame
		}
		else		// Otherwise, lock framerate at 125
		{
//			do	//Update at least once
//			{
//				usleep(1);
//			}while(float(SDL_GetTicks() - cv_lastFrameUpdate) < 1000.0f/125.0f);	// clamp the framerate at 125
			cv_lastUpdate = update(cv_lastUpdate);
			cv_lastFrameUpdate = drawScene(cv_lastFrameUpdate);	// Then render the frame
		}
	}
	return 0;	// This function doesn't really need to return anything fancy, though I should probably put in some fancy logic to keep track of what's going on, and crash if data gets corrupted
}

int game::update(const int& time)
{
	mouseDisp.x = 0.0f;			// Mouse may not have moved
	mouseDisp.y = 0.0f;			//
	int now = SDL_GetTicks();		// Find out what time it is
	int elapsed = now - time;		// Find out how much time has passed since the last frame update
	keystate = SDL_GetKeyState(NULL);	// Get the keystates
	SDL_PumpEvents();
	while(SDL_PollEvent(&Event))		// Poll for events
	{
		switch(Event.type)
		{
			case  SDL_KEYDOWN:		// A key was just pressed
			{
				keystate[Event.key.keysym.sym] = true;
				break;
			}
			case SDL_KEYUP:			// A key was just released
			{
				keystate[Event.key.keysym.sym] = false;
				break;
			}
			case SDL_QUIT:			// The game was told to quit
			{
				quit = true;
				break;
			}
			case SDL_MOUSEMOTION:		// The Mouse Moved
			{
				mouseDisp.x = Event.motion.xrel;
				mouseDisp.y = Event.motion.yrel;
				break;
			}
		}
	}
	if((KBD_LCTRL && KBD_X)||KBD_ESCAPE||KBD_Q)	// quit the game
		quit = true;
	if(KBD_W)	// move the camera forward
	{
		cameraPosition.z -= 0.001f*elapsed*cos(cameraRotation.x * pi/180.0f);
		cameraPosition.x += 0.001f*elapsed*sin(cameraRotation.x * pi/180.0f);
	}
	if(KBD_A)	// move the camera left
	{
		cameraPosition.z -= 0.001f*elapsed*sin(cameraRotation.x * pi/180.0f);
		cameraPosition.x -= 0.001f*elapsed*cos(cameraRotation.x * pi/180.0f);
	}
	if(KBD_S)	// move the camera back
	{
		cameraPosition.z += 0.001f*elapsed*cos(cameraRotation.x * pi/180.0f);
		cameraPosition.x -= 0.001f*elapsed*sin(cameraRotation.x * pi/180.0f);
	}
	if(KBD_D)	// move the camera right
	{
		cameraPosition.z += 0.001f*elapsed*sin(cameraRotation.x * pi/180.0f);
		cameraPosition.x += 0.001f*elapsed*cos(cameraRotation.x * pi/180.0f);
	}
	if(KBD_UP)	// rotate the camera up
		cameraRotation.y -= 0.01f*elapsed;
	if(KBD_LEFT)	// rotate the camera left
		cameraRotation.x -= 0.01f*elapsed;
	if(KBD_DOWN)	// rotate the camera down
		cameraRotation.y += 0.01f*elapsed;
	if(KBD_RIGHT)	// rotate the camera left
		cameraRotation.x += 0.01f*elapsed;
	if(KBD_SPACE)
		cameraPosition.y += 0.01f*elapsed;
	if(KBD_LCTRL)
		cameraPosition.y -= 0.01f*elapsed;
	if(KBD_F4)
	{
		KBD_F4 = false;
		bump = !bump;
	}
	if(KBD_F3)
	{
		KBD_F3 = false;
		cv_drawReference = !cv_drawReference;
	}
	if(KBD_F2)	// Toggle vsync
	{
		KBD_F2 = false;
		cv_vsync = !cv_vsync;
		if(cv_vsync)
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		else
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	}
	if(KBD_F1)	// Reset the camera
	{
		KBD_F1 = false;
		cameraRotation = vector2(0.0f, 0.0f);
		cameraPosition = vector3(0.0f, 0.0f, 0.0f);
	}

	cameraRotation.x += mouseDisp.x*abs(mouseDisp.x/2)*float(elapsed)*0.04f;	// Rotate the camera
	cameraRotation.y += mouseDisp.y*abs(mouseDisp.y/2)*float(elapsed)*0.04f;	// Rotate the camera

	if(cameraRotation.y < -88.0f)		// clamp the camera rotation
		cameraRotation.y = -88.0f;	//
	if(cameraRotation.y > 88.0f)		//
		cameraRotation.y = 88.0f;	//
	if(cameraRotation.x < 0.0f)		//
		cameraRotation.x += 360.0f;	//
	if(cameraRotation.x > 360.0f)		//
		cameraRotation.x -= 360.0f;	//

	logfile << "frameupdate: " << elapsed << endl;
	return now;	// Return the time it was when we started updating the game logic
}

int game::drawScene(const int& time)	// Draw the scene(tell it the time it was last called)
{
	int now = SDL_GetTicks();	// What time is it now

	if(bump)
		shaderBumpMap(bumpMap, vector3(1.0f, 2.0f, 1.0f));
	else
	{
		glUseProgramObjectARB(perVertex);
		glUniform1iARB(PVLdiffuseMap, 0);
	}
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	// Clear the scene

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();					// And load the identity matrix

	glRotatef(cameraRotation.y, 1.0f, 0.0f, 0.0f);					// rotate the camera about the x-axis (to look up/down)
	glRotatef(cameraRotation.x, 0.0f, 1.0f, 0.0f);					// rotate the camera about the y-axis (to look left/right)
	glTranslatef(-cameraPosition.x, -cameraPosition.y, -cameraPosition.z);		// translate the camera to it's current position

	glEnable(GL_TEXTURE_2D);	// Enable textures
	cone1->renderFrame();		// Render the vertex object
	glDisable(GL_TEXTURE_2D);	// Disable textures

	if(cv_drawReference)
	{
		glPushMatrix();			// push the current world matrix onto the stack
		glBegin(GL_LINES);		// begin rendering lines

		glColor3f(1.0f, 0.0f, 0.0f);	// in red

		for(int i = -10; i <= 10; ++i)				//
		{
			glVertex3f(-1.0f, 0.0f, float(i)/10.0f);	//
			glVertex3f(1.0f, 0.0f, float(i)/10.0f);		//
			glVertex3f(0.0f, -1.0f, float(i)/10.0f);	// render the lines
			glVertex3f(0.0f, 1.0f, float(i)/10.0f);		// do it now
			glVertex3f(float(i)/10.0f, 0.0f, -1.0f);	//
			glVertex3f(float(i)/10.0f, 0.0f, 1.0f);		//
		}

		glEnd();			// no more drawing the lines

		glPopMatrix();			// pop the world matrix back off the stack
	}
	SDL_GL_SwapBuffers();		// Swap Buffers
	glFinish();
	return now;			// Return the time we started rendering
}

bool game::GL_Init()
{
	glShadeModel(GL_SMOOTH);

//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);							// Black
	glClearColor(0.390625f, 0.58203125f, 0.92578125f, 1.0f);                                // Cornflower Blue
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_LIGHTING);

	int loadShade = loadShader("bumpMap", bumpMap);
	if(loadShade != 0)
		logfile << "Failed to load bumpMap shader: error " << loadShade << endl;
	else
	{
		BMapTangent = glGetAttribLocationARB(bumpMap,"tangent");
	        BMapDecalTex = glGetUniformLocationARB(bumpMap,"DecalTex");
	        BMapBumpMapTex = glGetUniformLocationARB(bumpMap,"BumpTex");
	        BMapLightDir = glGetUniformLocationARB(bumpMap,"lightdir");
	}
	loadShade = loadShader("pvl", perVertex);
	if(loadShade != 0)
		logfile << "Failed to load pvl shader: error " << loadShade << endl;
	else
	{
		PVLdiffuseMap = glGetUniformLocationARB(perVertex, "diffuseMap");
	}

	return true;
}

bool game::ResizeGLScene(const int& resx, const int& resy, const int& fov)	// This is the only uncommented function in my program
{
	float x = 1, y = 1;				// Make sure we have some kind of viewport
	if(resx != 0)
		x = float(resx);
	if(resy != 0)
		y = float(resy);

	glViewport(0, 0, GLsizei(x), GLsizei(y));	// Create the viewport

	glMatrixMode(GL_PROJECTION);			// Set the matix mode to projection
	glLoadIdentity();				// load the identity matrix

	gluPerspective(45, x/y, 0.10f, 10000.0f);	// Set the current perspective (currently a static FOV, and with a new clipping plane at 0.1 depth, and far at 10000)

	glMatrixMode(GL_MODELVIEW);			// Set the matrix mode to the modelview matrix
	glLoadIdentity();				// And load the identity matrix again

	return true;					// return true
}

bool game::loadConfig(const char* filename)
{
	char buffer[1024];
	string command, parameter1;
	ifstream infile;
	infile.open(filename);
	if(!infile.is_open())
		return false;
	while(!infile.eof())
	{
		while((infile.peek() == '#')||(infile.peek() == '\n'))
			infile.getline(buffer, 1024, '\n');
		infile >> command;
		if(command == "seta")
		{
			infile >> parameter1;
			if(parameter1 == "r_width")
				infile >> cv_screenRes.x;
			if(parameter1 == "r_height")
				infile >> cv_screenRes.y;
			if(parameter1 == "r_refresh")
				infile >> cv_screenRes.z;
			if(parameter1 == "r_vsync")
				infile >> cv_vsync;
			if(parameter1 == "r_fullscreen")
				infile >> cv_fullscreen;
			if(parameter1 == "sv_cheats")
				infile >> sv_cheats;
		}
		infile.getline(buffer, 1024, '\n');				// get rid of any garbage at the end of the line
	}
	return true;
}

game::~game()
{
	logfile.close();
	glDeleteObjectARB(bumpMap);
	glDeleteObjectARB(perVertex);
	delete cone1;					// destroy the cone
	glDisableClientState(GL_VERTEX_ARRAY);		// then disable vertex arrays
	SDL_Quit();					// then quit the SDL state
}

object::object(model* mod, const vector3& pos, const quaternion& rot)
{
	// Commented function calls are not working properly
	mdl = mod;
	vertexBuffer = NULL;
	position = pos;
	rotation = rot;
	VBO = 0;
	GLuint n00b = 0;
	glGenBuffersARB(1, &n00b);
	if(n00b == 0)
	{
		cout << "FAIL" << endl;
		return;
	}
	VBO = n00b;
	// Bind the buffer so we can put data in it
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	if(mdl != NULL)	// If the model is specified, we know how many triangles and therefore vertices to account for
		glBufferData(GL_ARRAY_BUFFER, 24*mdl->tris.size()*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
}

void object::renderFrame()
{
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);							// Make it fairly bright and opaque

	glEnableVertexAttribArrayARB(BMapTangent);

	if(glIsBuffer(VBO) && (mdl != NULL))							// If we really do have a model and VBO associated with this object
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);						// Bind the VBO
		if(vertexBuffer == NULL)
			vertexBuffer = new GLfloat[24*mdl->tris.size()];			// Make a new vector of vertices to feed into the VBO
		if(vertexBuffer == NULL)
		{
			// OMGWTF?!?!?
			return;
		}
		for(size_t i = 0; i < mdl->tris.size(); ++i)					// and for each triangle of the model
			for(size_t j = 0; j < 3; ++j)						// and each vertex of the triangle
			{
				vertexBuffer[24*i+8*j]   = mdl->tris[i].vertST[2*j];			// s
				vertexBuffer[24*i+8*j+1] = mdl->tris[i].vertST[2*j+1];			// t
				vertexBuffer[24*i+8*j+2] = mdl->tris[i].normal.x;			// nx
				vertexBuffer[24*i+8*j+3] = mdl->tris[i].normal.y;			// ny
				vertexBuffer[24*i+8*j+4] = mdl->tris[i].normal.z;			// nz
				vertexBuffer[24*i+8*j+5] = mdl->verts[mdl->tris[i].vertIndex[j]].x;	// x
				vertexBuffer[24*i+8*j+6] = mdl->verts[mdl->tris[i].vertIndex[j]].y;	// y
				vertexBuffer[24*i+8*j+7] = mdl->verts[mdl->tris[i].vertIndex[j]].z;	// z
			}
		GLfloat *triangleTangents = new GLfloat[mdl->tris.size()*9];
		vector3 normal, tangent, binormal;
		vector3 verts[3];
		vector2 texcoords[3];
		for(size_t i = 0; i < mdl->tris.size(); ++i)
		{
			verts[0].x = mdl->verts[mdl->tris[i].vertIndex[0]].x;
			verts[0].y = mdl->verts[mdl->tris[i].vertIndex[0]].y;
			verts[0].z = mdl->verts[mdl->tris[i].vertIndex[0]].z;
			verts[1].x = mdl->verts[mdl->tris[i].vertIndex[1]].x;
			verts[1].y = mdl->verts[mdl->tris[i].vertIndex[1]].y;
			verts[1].z = mdl->verts[mdl->tris[i].vertIndex[1]].z;
			verts[2].x = mdl->verts[mdl->tris[i].vertIndex[2]].x;
			verts[2].y = mdl->verts[mdl->tris[i].vertIndex[2]].y;
			verts[2].z = mdl->verts[mdl->tris[i].vertIndex[2]].z;
			texcoords[0].x = mdl->tris[i].vertST[0];
			texcoords[0].y = mdl->tris[i].vertST[1];
			texcoords[1].x = mdl->tris[i].vertST[2];
			texcoords[1].y = mdl->tris[i].vertST[3];
			texcoords[2].x = mdl->tris[i].vertST[4];
			texcoords[2].y = mdl->tris[i].vertST[5];
			FindInvTBN(verts, texcoords, normal, tangent, binormal);
			triangleTangents[9*i] = tangent.x;
			triangleTangents[9*i+1] = tangent.y;
			triangleTangents[9*i+2] = tangent.z;
			triangleTangents[9*i+3] = tangent.x;
			triangleTangents[9*i+4] = tangent.y;
			triangleTangents[9*i+5] = tangent.z;
			triangleTangents[9*i+6] = tangent.x;
			triangleTangents[9*i+7] = tangent.y;
			triangleTangents[9*i+8] = tangent.z;
		}
		glBufferData(GL_ARRAY_BUFFER, 24*mdl->tris.size()*sizeof(GLfloat), vertexBuffer, GL_DYNAMIC_DRAW);	// Fill the buffer
		glInterleavedArrays(GL_T2F_N3F_V3F, 0, NULL);								// Set up all pointers at once
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glBindTexture(GL_TEXTURE_2D, mdl->texture[DIFFUSE_MAP]);						// bind the texture
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glBindTexture(GL_TEXTURE_2D, mdl->texture[NORMAL_MAP]);						// bind the texture

		glVertexAttribPointerARB(BMapTangent, 3, GL_FLOAT, 0, 0, triangleTangents);

		glDrawArrays(GL_TRIANGLES, 0, 3*mdl->tris.size());							// Render the VBO
	}
}

object::~object()				// Destroy object
{
	if(vertexBuffer == NULL)
		delete [] vertexBuffer;
	if(glIsBuffer(VBO))		// if there is a vbo
		glDeleteBuffers(1, &VBO);	// delete it
}

char *textFileRead(const char* filename)
{
	if(filename == NULL)
		return NULL;
	char *content = NULL;
	int count;

	ifstream file(filename, ios::in);

	if(!file.is_open())
		return NULL;

	file.seekg(0, ios::end);	//move to the end of the file
	count = file.tellg();		//record the current point
	file.seekg(0, ios::beg);	//move back to the beginning;

	content = new char[count + 1];

	for(int i = 0; i < count; i++)
		content[i] = file.get();
	content[count] = '\0';
	file.close();
	return content;
}

int game::loadShader(const char* shadername, GLhandleARB &shader)
{
	if(shadername == NULL)			// if no filename given
		return false;			// cant load the shader
	ifstream file((string("shaders/") + string(shadername) + string(".vert")).c_str(), ios::in);	// try to open the vertex shader
	if(!file.is_open())			// if the file doesn't exist
		return 1;			// cant load the shader
	file.close();
	file.open((string("shaders/") + string(shadername) + string(".frag")).c_str(), ios::in);	// try to open the fragment shader
	if(!file.is_open())			// if the file doesn't exist
		return 2;			// cant load the shader
	file.close();
	char *vs, *fs;
	GLhandleARB vertBMap, fragBMap;

	shader = glCreateProgramObjectARB();
	vertBMap = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragBMap = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	vs = textFileRead((string("shaders/") + string(shadername) + string(".vert")).c_str());	// load the file
	fs = textFileRead((string("shaders/") + string(shadername) + string(".frag")).c_str());	// load the file
	const char* vBMap = vs;
	const char* fBMap = fs;

	glShaderSourceARB(vertBMap, 1, &vBMap, NULL);
	glShaderSourceARB(fragBMap, 1, &fBMap, NULL);

	free(vs); free(fs);

	char *infoLog = NULL;
	GLint length = 0;

	glCompileShaderARB(fragBMap);
	glCompileShaderARB(vertBMap);

	glGetShaderiv(fragBMap, GL_INFO_LOG_LENGTH, &length);
	infoLog = new char[length + 1];
	glGetShaderInfoLog(fragBMap, length, &length, infoLog);
	infoLog[length] = '\0';
	logfile << shadername << " frag-compilelog: " << infoLog << endl;
	delete [] infoLog;

	length = 0;
	glGetShaderiv(vertBMap, GL_INFO_LOG_LENGTH, &length);
	infoLog = new char[length + 1];
	glGetShaderInfoLog(vertBMap, length, &length, infoLog);
	infoLog[length] = '\0';
	logfile << shadername << " vert-compilelog: " << infoLog << endl;
	delete [] infoLog;

	glAttachObjectARB(shader, vertBMap);
	glAttachObjectARB(shader, fragBMap);

	glLinkProgramARB(shader);

	length = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
	infoLog = new char[length + 1];
	glGetShaderInfoLog(shader, length, &length, infoLog);
	infoLog[length] = '\0';
	logfile << shadername << " linklog: " << infoLog << endl;
	delete [] infoLog;
		return 0;
}

void FindInvTBN(vector3 Vertices[3], vector2 TexCoords[3], vector3 & InvNormal, vector3 & InvBinormal, vector3 & InvTangent)
{
	/* Calculate the vectors from the current vertex
	to the two other vertices in the triangle */

	vector3 v2v1 = Vertices[0] - Vertices[2];
        vector3 v3v1 = Vertices[1] - Vertices[2];

        //Calculate the “direction” of the triangle based on texture coordinates.

        // Calculate c2c1_T and c2c1_B
        float c2c1_T = TexCoords[0].x - TexCoords[2].x;
        float c2c1_B = TexCoords[0].y - TexCoords[2].y;

	// Calculate c3c1_T and c3c1_B
        float c3c1_T = TexCoords[1].x - TexCoords[2].x;
        float c3c1_B = TexCoords[1].y - TexCoords[2].y;

        //Look at the references for more explanation for this one.
        float fDenominator = c2c1_T * c3c1_B - c3c1_T * c2c1_B;
        /*ROUNDOFF here is a macro that sets a value to 0.0f if the value is a very small
        value, such as > -0.001f and < 0.001. */

        /* EDIT by c programmer: you should NEVER perform an equality test against a floating point value, even if
        your macro has set fDenominator to 0.0f.  The comparison can still fail.  The code needs fixed.
        Instead you should check if fDenominator is within an epsilon value of 0.0f. */

	if (ROUNDOFF(fDenominator) == 0.0f)
        {
        	/* We won't risk a divide by zero, so set the tangent matrix to the
        	identity matrix */
                InvTangent = vector3(1.0f, 0.0f, 0.0f);
        }
        else
        {
        	// Calculate the reciprocal value once and for all (to achieve speed)
                float fScale1 = 1.0f / fDenominator;

                /* Time to calculate the tangent, binormal, and normal.
                Look at Søren’s article for more information. */
                vector3 T, B, N;
                T = vector3((c3c1_B * v2v1.x - c2c1_B * v3v1.x) * fScale1, (c3c1_B * v2v1.y - c2c1_B * v3v1.y) * fScale1, (c3c1_B * v2v1.z - c2c1_B * v3v1.z) * fScale1);

                B = vector3((-c3c1_T * v2v1.x + c2c1_T * v3v1.x) * fScale1, (-c3c1_T * v2v1.y + c2c1_T * v3v1.y) * fScale1, (-c3c1_T * v2v1.z + c2c1_T * v3v1.z) * fScale1);

                N = (T%B); //Cross product!
		/*This is where programmers should break up the function to smooth the tangent, binormal and
		normal values. */

		//Look at “Derivation of the Tangent Space Matrix” for more information.

               float fScale2 = 1.0f / ((T.x * B.y * N.z - T.z * B.y * N.x) + (B.x * N.y * T.z - B.z * N.y * T.x) + (N.x * T.y * B.z - N.z * T.y * B.x));

               InvTangent = vector3((B%N).x * fScale2, ((-1.0f * N)%T).x * fScale2, (T%B).x * fScale2);
               InvTangent.normalize();
	}
}

void shaderBumpMap(const GLhandleARB& shader, const vector3& lightDirection)
{
	glUseProgramObjectARB(shader);
	glUniform1iARB(BMapDecalTex,0);
	glUniform1iARB(BMapBumpMapTex, 1);
	glUniform3fARB(BMapLightDir, lightDirection.x, lightDirection.y, lightDirection.z);
}
