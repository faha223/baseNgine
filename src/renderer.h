#include <vector>
#include "matrices.h"

struct VertexBufferObject
{
};

typedef VertexBufferObject *VBO;
typedef unsigned int uint;

class matrixStack					// Stores all the transformation matrices in use at a given time
{
private:
	std::vector<mat4> stack;
public:
	matrixStack();
	bool pushMatrix(const mat4& = mat4::identity);	// Returns true if a matrix was able to be pushed onto the stack, false otherwise. Does not actually push the identity matrix if passed
	mat4 popMatrix();				// removes the last matrix to be successfully pushed onto the stack, if the stack is empty, returns the matrix that was removed from the stack
							//	returns identity matrix if the stack was empty
};

class Renderer
{
private:
	matrixStack Transforms;
	std::vector<VBO> vertexBuffers;
	std::vector<uint> VBOs;
public:
	Renderer();
	bool pushMatrix(const mat4& = mat4::identity);	// pushes a new matrix onto the stack
	mat4 popMatrix();				// pops the last matrix off the stack
	bool pushVBO(const VBO& = NULL);		// Applies the current transformations on the VBO passed, and pushes it onto the stack to be rendered
	void flush();					// clears the buffer and renders all the graphics to the screen
};
