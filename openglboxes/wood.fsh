#version 410 core
//������ɫ��
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
	//ȡ����������(����ȡ����yz�ĳ���)��Ϊ�뾶
    float r = length(texcoord.yz);
	//�뾶 + ���� * ���Ʋ���
    r += woodTubulence * texture(noise, 0.25 * texcoord.xyz).x;

	//��������׼��
    vec3 N = normalize(normal);

	//���պͷ������ļнǵ�cosֵ
    float NdotL = dot(N, lightDirection.xyz);
	//������պͷ������ļнǵ�cosֵ
    float RdotL = dot(reflect(normalize(position), N), lightDirection.xyz);

    float f = fract(16.0 * r);
	//������ɫ
    vec4 unlitColor = mix(woodColors[0], woodColors[1], min(1.25 * f, 5.0 - 5.0 * f));
	//���������ɫ =(�������� + ��������� * ���ռн�cosֵ) * ��ʼ��ɫ + ���ʵ���ɫ
    FragColor = (light_ambient + light_diffuse * max(NdotL, 0.0)) * unlitColor +
                    material_specular * light_specular * pow(max(RdotL, 0.0), material_shininess);
}
