//������ɫ��

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
    //�������ж�Ӧ����ɫ * vec4(0.1,0.1,0.1,1)ʹ��䰵
    gl_FragColor = texture2D(texture, v_texcoord)/* * vec4(0.2,0.2,0.2,1)*/;
}

