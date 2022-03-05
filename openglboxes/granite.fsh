varying vec3 position, normal;
varying vec4 specular, ambient, diffuse, lightDirection;

uniform sampler2D tex;
uniform sampler3D noise;

//const vec4 graniteColors[3] = {vec4(0.0, 0.0, 0.0, 1), vec4(0.30, 0.15, 0.10, 1), vec4(0.80, 0.70, 0.75, 1)};
uniform vec4 graniteColors[3];

float steep(float x)
{
    return clamp(5.0 * x - 2.0, 0.0, 1.0);
}

void main()
{
    vec2 turbulence = vec2(0, 0);
    float scale = 1.0;
    for (int i = 0; i < 4; ++i) {
        turbulence += scale * (texture3D(noise, gl_TexCoord[1].xyz / scale).xy - 0.5);
        scale *= 0.5;
    }

    vec3 N = normalize(normal);
    // assume directional light

    gl_MaterialParameters M = gl_FrontMaterial;

    float NdotL = dot(N, lightDirection.xyz);
    float RdotL = dot(reflect(normalize(position), N), lightDirection.xyz);

    vec4 unlitColor = mix(graniteColors[1], mix(graniteColors[0], graniteColors[2], steep(0.5 + turbulence.y)), 4.0 * abs(turbulence.x));
    gl_FragColor = (ambient + diffuse * max(NdotL, 0.0)) * unlitColor +
                    M.specular * specular * pow(max(RdotL, 0.0), M.shininess);
}
