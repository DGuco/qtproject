varying vec2 v_texcoord;

attribute vec3 a_position;   //�ռ�����
attribute vec2 a_texcoord;   //��������
attribute vec3 a_normal;	 //��������

uniform mat4 mvp_matrix;

void main()
{
	v_texcoord = a_texcoord;
	gl_Position = mvp_matrix * vec4(a_position,1);
}
