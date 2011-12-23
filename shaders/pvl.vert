#version 110

varying vec3 normal, lightVec, halfAngle;
uniform sampler2D diffuseMap;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	normal = vec3(gl_Normal);
	lightVec = vec3(gl_LightSource[0].position);					// We don't subtract the vertex position because the light is not a point light
	halfAngle = vec3(gl_LightSource[0].halfVector);
	gl_TexCoord[0] = gl_MultiTexCoord0;						// Use the texture coordinates for this texture
}
