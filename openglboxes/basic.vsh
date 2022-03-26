varying vec3 position, normal;
varying vec4 specular, ambient, diffuse, lightDirection;

attribute vec3 a_position;
attribute vec2 a_texcoord;
attribute vec3 a_normal;

uniform mat4 view_mat;
uniform mat4 projection_mat;
uniform mat4 modelview_mat;

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = vec4(a_position,1);
	specular =  gl_LightSource[0].specular;
	ambient  =  gl_LightSource[0].ambient;
	diffuse  =  gl_LightSource[0].diffuse;
	lightDirection = view_mat * gl_LightSource[0].position;

	normal = gl_NormalMatrix * a_normal;
	position = (gl_ModelViewMatrix * vec4(a_position, 1)).xyz;

	gl_FrontColor = gl_Color;

	//gl_Position =  projection_mat * modelview_mat * vec4(a_position, 1);
	gl_Position = ftransform();
}
