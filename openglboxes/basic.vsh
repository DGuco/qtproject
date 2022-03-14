#version 330 core
//像素着色器

uniform mat4 view;
uniform vec4 light_position;
uniform vec4 light_specular;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;

layout(location = 0) in vec3 a_position;   //空间坐标
layout(location = 1) in vec2 a_texcoord;   //纹理坐标
layout(location = 2) in vec3 a_normal;	    //法线向量

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
