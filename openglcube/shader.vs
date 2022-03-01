#version 330 core
//顶点着色器

uniform mat4 mvp_matrix;
uniform mat4 model_matrix;

layout (location = 0) in vec3 a_position;   //空间坐标
layout (location = 1) in vec2 a_texcoord;   //纹理坐标
layout (location = 2) in vec3 a_normal;	    //法线向量

out vec2 v_texcoord;
out vec3 FragPos;
out vec3 Normal;

void main()
{
	FragPos = vec3(model_matrix * vec4(a_position, 1.0));
	Normal =  mat3(transpose(inverse(model_matrix))) * a_normal;  
    v_texcoord = a_texcoord;
    gl_Position = mvp_matrix * vec4(a_position,1);
}
