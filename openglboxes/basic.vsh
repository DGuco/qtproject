varying vec3 position,normal,texcoord;
varying vec4 lightDirection,color;

attribute vec3 a_position;
attribute vec2 a_texcoord;
attribute vec3 a_normal;
attribute vec4 a_color;

uniform mat4 lightview;
uniform mat4 projection_mat;
uniform mat4 view_mat;
uniform mat4 model_mat;
uniform mat3 normal_mat;
uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec4 material_specular;
uniform float material_shininess;

void main()
{
	lightDirection = lightview * light_position;

	normal = normal_mat * a_normal;
	texcoord = vec3(a_position);
	position = (view_mat * model_mat * vec4(a_position, 1)).xyz;

	color = a_color;
	gl_Position =  projection_mat *  view_mat * model_mat * vec4(a_position, 1);
}
