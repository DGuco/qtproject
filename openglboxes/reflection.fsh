varying vec3 position, normal;
varying vec4 lightDirection;

uniform sampler2D tex;
uniform samplerCube env;
uniform mat4 lightview;
uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec4 material_specular;
uniform float material_shininess;

void main()
{
    vec3 N = normalize(normal);
    vec3 R = 2.0 * dot(-position, N) * N + position;
    gl_FragColor = textureCube(env, R * mat3(lightview[0].xyz, lightview[1].xyz, lightview[2].xyz));
}
