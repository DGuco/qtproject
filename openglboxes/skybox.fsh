#version 330 core
//ÏñËØ×ÅÉ«Æ÷
#extension GL_NV_shadow_samplers_cube : enable
out vec4 FragColor;

in vec3 texcoord;

uniform samplerCube env;
uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec4 material_specular;
uniform float material_shininess;

void main() 
{ 
	FragColor = textureCube(env,texcoord.xyz);
}