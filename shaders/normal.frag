#version 110

varying vec3 normal;

void main()
{
	vec3 color = normalize(normal) + vec3(1.0);
	color = color/2.0;
//	vec3 color = vec3(1.0);
	gl_FragColor = vec4(color, 1.0);
}
