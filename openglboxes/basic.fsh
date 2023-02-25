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
uniform vec4 basicColor;
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

	//向量的绝对值
    vec3 absN = abs(texcoord.xyz);
	//把立方体移到坐标轴的原点
	vec3 zerttexcoord = texcoord + 0.5;
    vec2 texCoord;
	//左右两个面(纹理取向量yz)
    if (absN.x > absN.y && absN.x > absN.z)
        texCoord = vec2(zerttexcoord.yz);
	//上下两个面
    else if (absN.y > absN.z)
        texCoord = vec2(zerttexcoord.zx);
	//前后两个面
    else
        texCoord = vec2(zerttexcoord.xy);

	//获取对应坐标纹理的颜色值
    vec4 texColor = texture(tex, texCoord.xy);
	//混合基础颜色和纹理色
    vec4 unlitColor = color * mix(basicColor, vec4(texColor.xyz, 1.0), texColor.w);
	//输出最终颜色 =(环境光照 + 漫反射光照 * 光照夹角cos值) * 初始颜色 + 材质的颜色
    FragColor = (light_ambient + light_diffuse * max(NdotL, 0.0)) * unlitColor +
                    material_specular * light_specular * pow(max(RdotL, 0.0), material_shininess);
}
