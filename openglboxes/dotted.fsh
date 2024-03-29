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
uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec4 material_specular;
uniform float material_shininess;

void main()
{
	//归一化法线向量
    vec3 N = normalize(normal);

	//光照和法向量的夹角的cos值
    float NdotL = dot(N, lightDirection.xyz);
	//反射光照和法向量的夹角的cos值
    float RdotL = dot(reflect(normalize(position), N), lightDirection.xyz);

    float r1 = length(fract(7.0 * texcoord.xyz) - 0.5);
    float r2 = length(fract(5.0 * texcoord.xyz + 0.2) - 0.5);
    float r3 = length(fract(11.0 * texcoord.xyz + 0.7) - 0.5);
    vec4 rs = vec4(r1, r2, r3, 0.0);

    vec4 unlitColor = color * (0.8 - clamp(10.0 * (0.4 - rs), 0.0, 0.2));
    unlitColor.w = 1.0;
    FragColor = (light_ambient + light_diffuse * max(NdotL, 0.0)) * unlitColor +
                    material_specular * light_specular * pow(max(RdotL, 0.0), material_shininess);
}
