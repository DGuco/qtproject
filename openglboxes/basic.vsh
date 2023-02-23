#version 330 core
//¶¥µã×ÅÉ«Æ÷
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

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec4 a_color;

out vec3 position;
out vec3 normal;
out vec3 texcoord;
out vec4 lightDirection;
out vec4 color;

void main()
{
	lightDirection = lightview * light_position;

	normal = normal_mat * a_normal;
	texcoord = vec3(a_position);
	position = (view_mat * model_mat * vec4(a_position, 1)).xyz;

	color = vec4(a_color);
	gl_Position =  projection_mat *  view_mat * model_mat * vec4(a_position, 1);
}
