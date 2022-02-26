//像素着色器

#ifdef GL_ES
#version 330 core
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform sampler2D texture;

varying vec2 v_texcoord;

void main()
{
    //把纹理中对应的颜色 * vec4(0.1,0.1,0.1,1)使其变暗
    gl_FragColor = texture2D(texture, v_texcoord)/* * vec4(0.2,0.2,0.2,1)*/;
}

