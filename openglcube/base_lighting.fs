#version 330 core
//������ɫ��
out vec4 FragColor;

uniform vec3 lightColor;

void main()
{
	//����cube����ɫֱ��ȡlightColor
	FragColor = vec4(lightColor,1.0); 
} 