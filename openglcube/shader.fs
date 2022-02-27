#version 330 core
//像素着色器

uniform sampler2D texture;

in vec2 v_texcoord;

void main()
{
    //把纹理中对应的颜色 * vec4(0.1,0.1,0.1,1)使其变暗
    gl_FragColor = texture2D(texture, v_texcoord) * vec4(0.2,0.2,0.2,1);
}

