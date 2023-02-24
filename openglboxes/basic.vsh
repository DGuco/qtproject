#version 410 core
//������ɫ��
uniform mat4 lightview;
uniform mat4 projection_mat;
uniform mat4 view_mat;
uniform mat4 model_mat;
uniform mat3 normal_mat;
uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec4 material_specular;
uniform float material_shininess;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_texcoord;
layout(location = 2) in vec3 a_normal;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 texcoord;
layout(location = 3) out vec4 lightDirection;
layout(location = 4) out vec4 color;

void main()
{
	//��������ɫ�����ݲ���
	{
		//���շ���
		lightDirection = lightview * light_position;
		//��������
		normal = normal_mat * a_normal;
		//��������
		texcoord = a_texcoord;
		//��ɫ
		color = vec4(1.0f, 1.0f, 1.0f, 0.0f);
		//�۲�ռ�����(��������������) = �۲���� * �任���� * �ֲ��ռ�����
		position = (view_mat * model_mat * vec4(a_position, 1)).xyz;
	}
	//������Ļ���� = ͸�Ӿ��� * �۲���� * �任���� * �ֲ��ռ�����(��������������)
	gl_Position =  projection_mat *  view_mat * model_mat * vec4(a_position, 1);
}
