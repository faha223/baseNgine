#version 110

uniform sampler2D BumpTex; //The bump-map
uniform sampler2D DecalTex; //The texture
varying vec4 passcolor; //Receiving the vertex color from the vertex shader
varying vec3 LightDir; //Receiving the transformed light direction
varying vec3 position;
varying vec3 normal;
varying mat3 rotmat;
const float minLight = 0.2;
const float maxLight = 1.0;

void main()
{
	// Get the colors from the textures
	vec3 DecalCol = vec3(texture2D(DecalTex, gl_TexCoord[0].xy));
	vec3 BumpNorm = vec3(texture2D(BumpTex, gl_TexCoord[0].xy));
	//Convert the BumpNorm from RGB color to XYZ normal in 3d space
	BumpNorm = normalize(2.0 * (BumpNorm -0.5));

	vec3 light = normalize(rotmat * LightDir);

//	LightDir = LightDir * rotmat;

	// get the diffuse value of the light from the dot product of the tangent space (BumpNorm) and the tangent space light vector

	float diffuse = max(dot(light, BumpNorm), 0.2);
//	float specular = pow(clamp(dot(light - position, BumpNorm), 0.0, 1.0), 10.0);

	// Get the fragment color from the light value, and the decal color and the passed color
	vec3 color = DecalCol * vec3(passcolor) * (diffuse);// + specular);
	gl_FragColor = vec4(color, 1.0);
//	gl_FragColor = vec4(normal, 1.0);
}
