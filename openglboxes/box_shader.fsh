varying vec2 v_texcoord;

uniform sampler2D texture;

void main()
{
	//��ȡ�����ж�Ӧ����ɫ
    vec4 textureColor = texture2D(texture, v_texcoord);
	//�������е���ɫ�͹��ղ������
    gl_FragColor = textureColor;
}

