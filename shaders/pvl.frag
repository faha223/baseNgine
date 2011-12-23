#version 110

varying vec3 normal, lightVec, halfAngle;
uniform sampler2D diffuseMap;

void main()
{
	const float specularLevel = 10.0;
	const vec4 ambientColor = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 diffuseColor = texture2D(diffuseMap, gl_TexCoord[0].xy);
	vec3 norm = normalize(normal);
	vec3 halfV = normalize(halfAngle);
	vec3 lV = normalize(lightVec);
	float diffuse = clamp(dot(norm, lV), 0.0, 1.0);
	float specular = pow(clamp(dot(norm,halfV), 0.0, 1.0), specularLevel);
	gl_FragColor = (ambientColor + diffuseColor)*diffuse + vec4(1.0)*specular;
}
