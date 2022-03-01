#version 330 core
//像素着色器

uniform vec3 lightColor;

void main()
{
	//光照cube的颜色直接取lightColor
	gl_FragColor = vec4(lightColor,1.0); 
} 