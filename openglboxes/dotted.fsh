varying vec3 position, normal,texcoord;
varying vec4 lightDirection,color;

uniform sampler2D tex;
uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec4 material_specular;
uniform float material_shininess;

void main()
{
    vec3 N = normalize(normal);

    // assume directional light
    float NdotL = dot(N, lightDirection.xyz);
    float RdotL = dot(reflect(normalize(position), N), lightDirection.xyz);

    float r1 = length(fract(7.0 * texcoord.xyz) - 0.5);
    float r2 = length(fract(5.0 * texcoord.xyz + 0.2) - 0.5);
    float r3 = length(fract(11.0 * texcoord.xyz + 0.7) - 0.5);
    vec4 rs = vec4(r1, r2, r3, 0.0);

    vec4 unlitColor = color * (0.8 - clamp(10.0 * (0.4 - rs), 0.0, 0.2));
    unlitColor.w = 1.0;
    gl_FragColor = (light_ambient + light_diffuse * max(NdotL, 0.0)) * unlitColor +
                    material_specular * light_specular * pow(max(RdotL, 0.0), material_shininess);
}
