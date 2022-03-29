varying vec3 position, normal;
varying vec4 lightDirection;

attribute vec3 a_position;
attribute vec2 a_texcoord;
attribute vec3 a_normal;

uniform mat4 lightview;
uniform mat4 projection_mat;
uniform mat4 view_mat;
uniform mat4 model_mat;
uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = vec4(a_position,1);
	lightDirection = lightview * light_position;

	normal = gl_NormalMatrix * a_normal;
	position = (gl_ModelViewMatrix * vec4(a_position, 1)).xyz;

	gl_FrontColor = gl_Color;

	gl_Position =  projection_mat *  view_mat * model_mat * vec4(a_position, 1);
}
