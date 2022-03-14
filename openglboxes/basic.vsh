#version 330 core
//������ɫ��

uniform mat4 view;
uniform vec4 light_position;
uniform vec4 light_specular;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;

layout(location = 0) in vec3 a_position;   //�ռ�����
layout(location = 1) in vec2 a_texcoord;   //��������
layout(location = 2) in vec3 a_normal;	    //��������

out vec3 position;
out vec3 normal;
out vec4 specular;
out vec4 ambient;
out vec4 diffuse;
out vec4 lightDirection;

void main()
{	
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[1] = gl_Vertex;
    specular = light_specular;
    ambient = light_ambient;
    diffuse = light_diffuse;
    lightDirection = view * light_position;

    normal = gl_NormalMatrix * gl_Normal;
    position = (gl_ModelViewMatrix * gl_Vertex).xyz;

    gl_FrontColor = gl_Color;
    gl_Position = ftransform();
}
