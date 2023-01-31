varying vec2 v_texcoord;

uniform sampler2D texture;

void main()
{
	//获取纹理中对应的颜色
    vec4 textureColor = texture2D(texture, v_texcoord);
	//把纹理中的颜色和光照参数相乘
    gl_FragColor = textureColor;
}

