varying vec3 position, normal;
varying vec4 specular, ambient, diffuse, lightDirection;

uniform sampler2D tex;
uniform sampler3D noise;

//const vec4 marbleColors[2] = {vec4(0.9, 0.9, 0.9, 1), vec4(0.6, 0.5, 0.5, 1)};
uniform vec4 marbleColors[2];

void main()
{
    float turbulence = 0.0;
    float scale = 1.0;
    for (int i = 0; i < 4; ++i) {
        turbulence += scale * (texture3D(noise, 0.125 * gl_TexCoord[1].xyz / scale).x - 0.5);
        scale *= 0.5;
    }

    vec3 N = normalize(normal);
    // assume directional light

    gl_MaterialParameters M = gl_FrontMaterial;

    float NdotL = dot(N, lightDirection.xyz);
    float RdotL = dot(reflect(normalize(position), N), lightDirection.xyz);

    vec4 unlitColor = mix(marbleColors[0], marbleColors[1], exp(-4.0 * abs(turbulence)));
    gl_FragColor = (ambient + diffuse * max(NdotL, 0.0)) * unlitColor +
                    M.specular * specular * pow(max(RdotL, 0.0), M.shininess);
}
