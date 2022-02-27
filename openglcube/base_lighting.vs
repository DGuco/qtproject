#version 330 core
//¶¥µã×ÅÉ«Æ÷

uniform mat4 mvp_matrix;

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texcoord;
layout (location = 2) in vec3 a_normal;

out vec2 v_texcoord;

void main()
{
    gl_Position = mvp_matrix * vec4(a_position,1);

    v_texcoord = a_texcoord;
}
