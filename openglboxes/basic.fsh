#version 410 core
//������ɫ��
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
	//��һ����������
    vec3 N = normalize(normal);

	//���պͷ������ļнǵ�cosֵ
    float NdotL = dot(N, lightDirection.xyz);
	//������պͷ������ļнǵ�cosֵ
    float RdotL = dot(reflect(normalize(position), N), lightDirection.xyz);

	//�����ľ���ֵ
    vec3 absN = abs(texcoord.xyz);
	//���������Ƶ��������ԭ��
	vec3 zerttexcoord = texcoord + 0.5;
    vec2 texCoord;
	//����������(����ȡ����yz)
    if (absN.x > absN.y && absN.x > absN.z)
        texCoord = vec2(zerttexcoord.yz);
	//����������
    else if (absN.y > absN.z)
        texCoord = vec2(zerttexcoord.zx);
	//ǰ��������
    else
        texCoord = vec2(zerttexcoord.xy);

	//��ȡ��Ӧ�����������ɫֵ
    vec4 texColor = texture(tex, texCoord.xy);
	//��ϻ�����ɫ������ɫ
    vec4 unlitColor = color * mix(basicColor, vec4(texColor.xyz, 1.0), texColor.w);
	//���������ɫ =(�������� + ��������� * ���ռн�cosֵ) * ��ʼ��ɫ + ���ʵ���ɫ
    FragColor = (light_ambient + light_diffuse * max(NdotL, 0.0)) * unlitColor +
                    material_specular * light_specular * pow(max(RdotL, 0.0), material_shininess);
}
