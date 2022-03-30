varying vec3 position, normal,texcoord;
varying vec4 lightDirection,color;

uniform sampler2D tex;
uniform vec4 basicColor;
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

    vec3 absN = abs(texcoord.xyz);
    vec3 texCoord;
    if (absN.x > absN.y && absN.x > absN.z)
        texCoord = texcoord.yzx;
    else if (absN.y > absN.z)
        texCoord = texcoord.zxy;
    else
        texCoord = texcoord.xyz;
    texCoord.y *= -sign(texCoord.z);
    texCoord += 0.5;

    vec4 texColor = texture2D(tex, texCoord.xy);
    vec4 unlitColor = color * mix(basicColor, vec4(texColor.xyz, 1.0), texColor.w);
    gl_FragColor = (light_ambient + light_diffuse * max(NdotL, 0.0)) * unlitColor +
                    material_specular * light_specular * pow(max(RdotL, 0.0), material_shininess);
}
