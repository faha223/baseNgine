#include "model.h"
#include <fstream>
using namespace std;

model::model()
{
	skeleton = NULL;
	animationFrames = NULL;
	texture[DIFFUSE_MAP] = 0;
	texture[NORMAL_MAP] = 0;
}

model::model(const char *filename)
{
	load(filename);
}

bool model::load(const char *filename)
{
	int numVerts, numPolys, x, y, z;
	float fx, fy, fz, s0, s1, s2, t0, t1, t2;
	vector3 normal;
	ifstream file((string("bin/base/")+string(filename)+string("/mesh")).c_str());
	if(!file.is_open())
	{
		return false;
	}
	file >> numVerts >> numPolys;
	for(int i = 0; i < numVerts; i++)
	{
		file >> fx >> fy >> fz;
		verts.push_back(vertex(fx, fy, fz));
	}
	for(int i = 0; i < numPolys; i++)
	{
		file >> x >> s0 >> t0 >> y >> s1 >> t1 >> z >> s2 >> t2 >> normal.x >> normal.y >> normal.z;
		tris.push_back(flatTriangle(x, s0, t0, y, s1, t1, z, s2, t2, normal));
	}
	file.close();
	system((string("ar -x bin/base/")+string(filename)+string("/texture diffuse.bmp")).c_str());
	glActiveTextureARB(GL_TEXTURE0_ARB);
	texture[DIFFUSE_MAP] = loadTexBMP("diffuse.bmp");
	system("rm diffuse.bmp");
	system((string("ar -x bin/base/")+string(filename)+string("/texture normal.bmp")).c_str());
	glActiveTextureARB(GL_TEXTURE1_ARB);
	texture[NORMAL_MAP] = loadTexBMP("normal.bmp");
	system("rm normal.bmp");
	glActiveTextureARB(GL_TEXTURE0_ARB);
	return true;
}

flatTriangle::flatTriangle(const int& vertIndex1, const float& s0, const float& t0,
			const int& vertIndex2, const float& s1, const float& t1,
			const int& vertIndex3, const float& s2, const float& t2, const vector3& norm)
{
	vertIndex[0] = vertIndex1;
	vertIndex[1] = vertIndex2;
	vertIndex[2] = vertIndex3;
	vertST[0] = s0;
	vertST[1] = t0;
	vertST[2] = s1;
	vertST[3] = t1;
	vertST[4] = s2;
	vertST[5] = t2;
	normal = norm;
}

model::~model()
{
	if(texture[DIFFUSE_MAP] != 0)
		glDeleteTextures(1, &texture[DIFFUSE_MAP]);
	if(texture[NORMAL_MAP] != 0)
		glDeleteTextures(1, &texture[NORMAL_MAP]);
}
