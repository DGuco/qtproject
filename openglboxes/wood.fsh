varying vec3 position, normal;
varying vec4 specular, ambient, diffuse, lightDirection;

uniform sampler2D tex;
uniform sampler3D noise;

//const vec4 woodColors[2] = {vec4(0.37,0.24,0.20,1), vec4(0.8,0.6,0.4,1)};
uniform vec4 woodColors[2];
//const float woodTubulence = 0.1;
uniform float woodTubulence;

void main()
{
    float r = length(gl_TexCoord[1].yz);
    r += woodTubulence * texture3D(noise, 0.25 * gl_TexCoord[1].xyz).x;

    vec3 N = normalize(normal);
    // assume directional light

    gl_MaterialParameters M = gl_FrontMaterial;

    float NdotL = dot(N, lightDirection.xyz);
    float RdotL = dot(reflect(normalize(position), N), lightDirection.xyz);

    float f = fract(16.0 * r);
    vec4 unlitColor = mix(woodColors[0], woodColors[1], min(1.25 * f, 5.0 - 5.0 * f));
    gl_FragColor = (ambient + diffuse * max(NdotL, 0.0)) * unlitColor +
                    M.specular * specular * pow(max(RdotL, 0.0), M.shininess);
}
