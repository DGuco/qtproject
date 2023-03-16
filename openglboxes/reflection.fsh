#version 410 core
//像素着色器
#extension GL_NV_shadow_samplers_cube : enable
out vec4 FragColor;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 texcoord;
layout(location = 3) in vec4 lightDirection;
layout(location = 4) in vec4 color;

uniform sampler2D tex;
uniform samplerCube env;
uniform mat4 lightview;
uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec4 material_specular;
uniform float material_shininess;

void main()
{
	//归一化法向量
    vec3 N = normalize(normal);
	//
    vec3 R = 2.0 * dot(-position, N) * N + position;
    FragColor = textureCube(env, R * mat3(lightview[0].xyz, lightview[1].xyz, lightview[2].xyz));
}
