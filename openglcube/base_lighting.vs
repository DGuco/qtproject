#version 330 core
//¶¥µã×ÅÉ«Æ÷

uniform mat4 mvp_matrix;
layout (location = 0) in vec3 a_position;

void main()
{
	//mvp_matrix = projection * viewmatrix * modelmatrix
    gl_Position = mvp_matrix * vec4(a_position,1);
}
