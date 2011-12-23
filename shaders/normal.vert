#version 110

varying vec3 normal;

void main()
{
	normal = gl_Normal * gl_NormalMatrix;
	gl_Position = ftransform();
}
