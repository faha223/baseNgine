#version 110

attribute vec3 tangent;
uniform sampler2D DecalTex;
uniform sampler2D BumpTex;
varying vec4 passcolor;
varying vec3 normal;
varying vec3 position;
varying vec3 LightDir;
varying mat3 rotmat;

void main()
{
	// get the color passed to the opengl context
	vec3 tan = normalize(tangent * gl_NormalMatrix);
	position = vec3(gl_Vertex);
	passcolor = gl_Color;
	LightDir = normalize(vec3(gl_LightSource[0].position));
	normal = normalize(gl_Normal * gl_NormalMatrix);
	vec3 binormal = normalize(cross(normal, tangent));
//	rotmat = gl_NormalMatrix;
	rotmat = mat3(tan.x, tan.y, tan.z,
		      binormal.x, binormal.y, binormal.z,
		      normal.x, normal.y, normal.z);
	gl_Position = ftransform();
	// Get the texture coordinates for the fragment shader
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
