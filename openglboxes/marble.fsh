varying vec3 position, normal,texcoord;
varying vec4 lightDirection;

uniform sampler2D tex;
uniform sampler3D noise;
//const vec4 marbleColors[2] = {vec4(0.9, 0.9, 0.9, 1), vec4(0.6, 0.5, 0.5, 1)};
uniform vec4 marbleColors[2];
uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec4 material_specular;
uniform float material_shininess;

void main()
{
    float turbulence = 0.0;
    float scale = 1.0;
    for (int i = 0; i < 4; ++i) {
        turbulence += scale * (texture3D(noise, 0.125 * texcoord.xyz / scale).x - 0.5);
        scale *= 0.5;
    }

    vec3 N = normalize(normal);
    // assume directional light

    float NdotL = dot(N, lightDirection.xyz);
    float RdotL = dot(reflect(normalize(position), N), lightDirection.xyz);

    vec4 unlitColor = mix(marbleColors[0], marbleColors[1], exp(-4.0 * abs(turbulence)));
    gl_FragColor = (light_ambient + light_diffuse * max(NdotL, 0.0)) * unlitColor +
                    material_specular * light_specular * pow(max(RdotL, 0.0), material_shininess);
}
