uniform samplerCube env;
uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec4 material_specular;
uniform float material_shininess;

void main() 
{ 
	gl_FragColor = textureCube(env,gl_TexCoord[1].xyz);
}