#version 330 core
//������ɫ��

uniform vec3 lightColor;

void main()
{
	//����cube����ɫֱ��ȡlightColor
	gl_FragColor = vec4(lightColor,1.0); 
} 