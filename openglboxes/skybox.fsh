uniform samplerCube env;

void main() 
{ 
	gl_FragColor = textureCube(env,gl_TexCoord[1].xyz);
}