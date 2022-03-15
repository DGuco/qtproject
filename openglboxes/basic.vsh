varying vec3 position, normal;
varying vec4 specular, ambient, diffuse, lightDirection;

attribute vec4 a_position;
attribute vec2 a_texcoord;
attribute vec3 a_normal;

uniform mat4 view;

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_Vertex;
	specular = gl_LightSource[0].specular;
	ambient = gl_LightSource[0].ambient;
	diffuse = gl_LightSource[0].diffuse;
	lightDirection = view * gl_LightSource[0].position;

	normal = gl_NormalMatrix * gl_Normal;
	position = (gl_ModelViewMatrix * gl_Vertex).xyz;

	gl_FrontColor = gl_Color;
	gl_Position = ftransform();
}
