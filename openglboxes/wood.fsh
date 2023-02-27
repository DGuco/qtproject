#version 410 core
//像素着色器
#extension GL_NV_shadow_samplers_cube : enable
out vec4 FragColor;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 texcoord;
layout(location = 3) in vec4 lightDirection;

uniform sampler2D tex;
uniform sampler3D noise;

//const vec4 woodColors[2] = {vec4(0.37,0.24,0.20,1), vec4(0.8,0.6,0.4,1)};
//const float woodTubulence = 0.1;

uniform vec4 woodColors[2];
uniform float woodTubulence;
uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec4 ma_specular;
uniform vec4 material_specular;
uniform float material_shininess;

void main()
{
	//取左右两个面(纹理取向量yz的长度)作为半径
    float r = length(texcoord.yz);
	//半径 + 噪声 * 波纹参数
    r += woodTubulence * texture(noise, 0.25 * texcoord.xyz).x;

	//法向量标准化
    vec3 N = normalize(normal);

	//光照和法向量的夹角的cos值
    float NdotL = dot(N, lightDirection.xyz);
	//反射光照和法向量的夹角的cos值
    float RdotL = dot(reflect(normalize(position), N), lightDirection.xyz);

    float f = fract(16.0 * r);
	//像素颜色
    vec4 unlitColor = mix(woodColors[0], woodColors[1], min(1.25 * f, 5.0 - 5.0 * f));
	//输出最终颜色 =(环境光照 + 漫反射光照 * 光照夹角cos值) * 初始颜色 + 材质的颜色
    FragColor = (light_ambient + light_diffuse * max(NdotL, 0.0)) * unlitColor +
                    material_specular * light_specular * pow(max(RdotL, 0.0), material_shininess);
}
