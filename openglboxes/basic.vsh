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
uniform int  render_type;

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
	//�������Ⱦ������
	if (render_type == 0)
	{
		//������Ļ���� = ͸�Ӿ��� * �۲���� * �任���� * �ֲ��ռ�����(��������������)
		gl_Position = projection_mat *  view_mat * model_mat * vec4(a_position, 1);
	}
	else  //�������Ⱦ��տ�
	{
		/*
		   ��պкܿ��ܻ���Ⱦ��������������֮�ϣ���Ϊ��ֻ��һ��1x1x1�������壨��ζ�ž���
		������ľ���Ҳֻ��1����������Ҫ��ƭ��Ȼ��壬������Ϊ��պ������������ֵ1.0��ֻ
		Ҫ��ǰ����һ�����壬��Ȳ��Ծͻ�ʧ�ܡ�
		   ������ϵͳС��������˵����͸�ӳ������ڶ�����ɫ������֮��ִ�еģ���gl_Position��xyz
		�������w�����������ִ���Ȳ���С����֪������������z�������ڶ�������ֵ��ʹ����Щ��
		Ϣ�����ǿ��Խ����λ�õ�z������������w��������z������Զ����1.0�������ӵĻ�����͸��
		����ִ��֮��z�������Ϊw / w = 1.0��
		*/
		vec4 pos = projection_mat *  view_mat * model_mat * vec4(a_position, 1);
		gl_Position = pos.xyww;
	}
}
