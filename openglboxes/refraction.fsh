#version 410 core
//ÏñËØ×ÅÉ«Æ÷
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

// Arrays don't work here on glsl < 120, apparently.
//const float coeffs[6] = float[6](1.0/2.0, 1.0/2.1, 1.0/2.2, 1.0/2.3, 1.0/2.4, 1.0/2.5);
float coeffs(int i)
{
	return 1.0 / (2.0 + 0.1 * float(i));
}

void main()
{
    vec3 N = normalize(normal);
    vec3 I = -normalize(position);
    float IdotN = dot(I, N);
    float scales[6];
    vec3 C[6];
    for (int i = 0; i < 6; ++i) {
        scales[i] = (IdotN - sqrt(1.0 - coeffs(i) + coeffs(i) * (IdotN * IdotN)));
        C[i] = textureCube(env, (-I + coeffs(i) * N) * mat3(lightview[0].xyz, lightview[1].xyz, lightview[2].xyz)).xyz;
    }

    FragColor = 0.25 * vec4(C[5].x + 2.0*C[0].x + C[1].x, C[1].y + 2.0*C[2].y + C[3].y,
                   C[3].z + 2.0*C[4].z + C[5].z, 4.0);
}
