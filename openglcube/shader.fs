#version 330 core
//像素着色器
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform sampler2D texture;

in vec2 v_texcoord;

void main()
{
    //环境光照
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
  	
    // 漫反射光照 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
         
    // 镜面光照
    float specularStrength = 1.0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
    vec3 specular = specularStrength * spec * lightColor;  
        
	//获取纹理中对应的颜色
    vec4 textureColor = texture2D(texture, v_texcoord);
	//把纹理中的颜色和光照参数相乘
    FragColor = vec4(ambient + diffuse + specular, 1.0) * textureColor;
}

