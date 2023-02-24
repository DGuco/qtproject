#version 410 core
//顶点着色器
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
	//给像素着色器传递参数
	{
		//光照方向
		lightDirection = lightview * light_position;
		//法线向量
		normal = normal_mat * a_normal;
		//纹理坐标
		texcoord = a_texcoord;
		//颜色
		color = vec4(1.0f, 1.0f, 1.0f, 0.0f);
		//观察空间坐标(从右向左做运算) = 观察矩阵 * 变换矩阵 * 局部空间坐标
		position = (view_mat * model_mat * vec4(a_position, 1)).xyz;
	}
	//最终屏幕坐标 = 透视矩阵 * 观察矩阵 * 变换矩阵 * 局部空间坐标(从右向左做运算)
	gl_Position =  projection_mat *  view_mat * model_mat * vec4(a_position, 1);
}
