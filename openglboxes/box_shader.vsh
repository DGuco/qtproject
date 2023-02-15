varying vec2 v_texcoord;

attribute vec3 a_position;   //�ռ�����
attribute vec2 a_texcoord;   //��������
attribute vec3 a_normal;	 //��������
attribute vec4 a_color;

uniform mat4 mvp_matrix;

void main()
{
	v_texcoord = a_texcoord;
	gl_Position = mvp_matrix * vec4(a_position,1);
}
