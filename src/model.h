#include <vector>
#include "included.h"
#include "texture.h"
#include "quaternion.h"
#include "vectorMath.h"

#ifndef _MODEL_H_
#define _MODEL_H_

enum jointType
{
	NORMAL_JOINT,
	HINGED_JOINT,
	BALL_SOCKET_JOINT
};

enum mapType
{
	DIFFUSE_MAP,
	NORMAL_MAP
};

#define FIRST			0
#define LAST			1
#define FPS			2

struct bone	//This is the base object for an animation and IK skeleton
{
	bone(	const float& length = 1.0f, const float& weight = 1.0f,				//This is a very thorough constructor
		const quaternion& rotation = quaternion(0.0f, 0.0f, 0.0f, 0.0f),
		const float& hingeAngle = 0.0f, const float& hingeMax = 180.0f,
		const float& hingeMin = 0.0f, int jointType = 1, const bone* parent = NULL,
		const bone* child = NULL, const char* name = "\0");
	bone(const bone&);									// this one is less thorough
	float length;										//length of the bone (displacement from each child
	float weight;										//used for IK
	quaternion rotation;									//direction to the children
	vector3 offset;										//I forgot
	float hingeAngle, hingeMax, hingeMin;							//angle about which it can rotate from its parent, and by how much
	int jointType;										//The type of joint this bone makes with its parent
	bone *parent, *child;									//pointers to this bone's parent and children
	char name[20];										//The name of the bone, could be useful
};

typedef vector3 vertex;		//A vertex is a vector3, and for that reason, you can do anything with a vertex that you can do with a vector3: WINNER!

struct triangle
{
	int vertIndex[3];	//An index to the vertices this triangle uses
	float vertST[6];
};

struct flatTriangle : public triangle		//Stores a single triangle, with 1 normal
{
	flatTriangle(const int&, const float&, const float&, const int&, const float&, const float&, const int&, const float&, const float&, const vector3&);	//constructor
	vector3 normal;		//Normal
};

struct roundedTriangle : public triangle	//stored a triangle with per vertex normals
{
	vector3 normal[3];	//one normal per vertex
};

struct animFrame	// Stores one frame of animation for a 3d model
{
	quaternion *skeleton;	// A tree of quaternions to store the rotations of each bone
};

struct model	// Stores a 3d model, complete with a full set of textures and skeletal animations
{
	std::vector<vertex> verts;		// A vector of vertex objects to store the vertices
	std::vector<flatTriangle> tris;		// A vector of triangles to store the geometry
	GLuint texture[2];			// Stores the textures: will add spots as necessary
	GLuint VBO;				// Stored the reference number of the Vertex Buffer Object
	std::vector<int[3]> animList;		// A list of the FIRST, LAST, and FPS of animation frames per animation
	bone *skeleton;				// A tree of bones for the skeleton
	animFrame *animationFrames;		// each animation frame contains a tree of quaternions in the same structure as the skeleton
	model();
	model(const char*);			// loads a model from a file
	bool load(const char*);			// makes a copy of another model (I can't think of a practical use for this, but that doesn't mean it wont come in handy)
	~model();				// gotta delete it sometime
	animFrame animate(const float& animation);	// the float is the number of the animation + the % complete the animation is, used for proper lerping
};

#endif
