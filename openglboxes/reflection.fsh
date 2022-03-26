varying vec3 position, normal;
varying vec4 specular, ambient, diffuse, lightDirection;

uniform sampler2D tex;
uniform samplerCube env;
uniform mat4 view_mat;

void main()
{
    vec3 N = normalize(normal);
    vec3 R = 2.0 * dot(-position, N) * N + position;
    gl_FragColor = textureCube(env, R * mat3(view_mat[0].xyz, view_mat[1].xyz, view_mat[2].xyz));
}
