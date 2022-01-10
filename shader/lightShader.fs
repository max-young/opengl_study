#version 330 core
out vec4 FragColor;

struct Material {
  sampler2D diffuse;
  sampler2D specular;    
  float shininess;
}; 

// 定向光
struct DirLight {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
uniform DirLight dirLight;
vec3 CalDirLight(DirLight light, vec3 normal, vec3 viewDir);

// 点光源
struct PointLight {
  vec3 position;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct SpotLight {
  vec3 position;  
  vec3 direction;
  float cutOff;
  float outerCutOff;
  
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
	
  float constant;
  float linear;
  float quadratic;
};
uniform SpotLight spotLight;
vec3 CalSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 viewPos;
uniform Material material;

void main()
{
  vec3 norm = normalize(Normal);
  vec3 viewDir = normalize(viewPos - FragPos);

  // 定向光
  vec3 result = CalDirLight(dirLight, norm, viewDir);
  // 聚光灯
  result += CalSpotLight(spotLight, norm, FragPos, viewDir);

  FragColor = vec4(result, 1.0f);
}

// 计算定向光源
vec3 CalDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
  vec3 lightDir = normalize(-light.direction);
  float diff = max(dot(normal, lightDir), 0.0f);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
  vec3 specular = light.specular * vec3(texture(material.specular, TexCoords));
  return ambient + diffuse + specular;
}

// 计算点光源
vec3 CalPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0f);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

  ambient *= attenuation;
  diff *= attenuation;
  specular *= attenuation;

  return ambient + diffuse + specular;
}

// 计算聚光灯
vec3 CalSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
  // ambient
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
  
  // diffuse 
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(light.position - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
  
  // specular
  vec3 reflectDir = reflect(-lightDir, norm);  
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  
  
  // spotlight (soft edges)
  float theta = dot(lightDir, normalize(-light.direction)); 
  float epsilon = (light.cutOff - light.outerCutOff);
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  diffuse  *= intensity;
  specular *= intensity;
  
  // attenuation
  float distance    = length(light.position - FragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
  ambient  *= attenuation; 
  diffuse  *= attenuation;
  specular *= attenuation;   
      
  return ambient + diffuse + specular;
}