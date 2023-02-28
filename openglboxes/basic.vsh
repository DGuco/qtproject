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
	//如果是渲染立方体
	if (render_type == 0)
	{
		//最终屏幕坐标 = 透视矩阵 * 观察矩阵 * 变换矩阵 * 局部空间坐标(从右向左做运算)
		gl_Position = projection_mat *  view_mat * model_mat * vec4(a_position, 1);
	}
	else  //如果是渲染天空壳
	{
		/*
		   天空盒很可能会渲染在所有其他对象之上，因为它只是一个1x1x1的立方体（意味着距离
		摄像机的距离也只有1），我们需要欺骗深度缓冲，让它认为天空盒有着最大的深度值1.0，只
		要它前面有一个物体，深度测试就会失败。
		   在坐标系统小节中我们说过，透视除法是在顶点着色器运行之后执行的，将gl_Position的xyz
		坐标除以w分量。我们又从深度测试小节中知道，相除结果的z分量等于顶点的深度值。使用这些信
		息，我们可以将输出位置的z分量等于它的w分量，让z分量永远等于1.0，这样子的话，当透视
		除法执行之后，z分量会变为w / w = 1.0。
		*/
		vec4 pos = projection_mat *  view_mat * model_mat * vec4(a_position, 1);
		gl_Position = pos.xyww;
	}
}
