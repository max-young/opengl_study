#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
}; 

struct Light {
  vec3 position;
  vec3 direction;
  float cutOff;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    vec3 lightDir = normalize(light.position - FragPos);

    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb * attenuation;
    float theta = dot(lightDir, normalize(-light.direction));
    if (theta > light.cutOff)
    {
      vec3 norm = normalize(Normal);
      // vec3 lightDir = normalize(-light.direction);
      float diff = max(dot(norm, lightDir), 0.0);
      vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb * attenuation;

      vec3 viewDir = normalize(viewPos - FragPos);
      vec3 reflectDir = reflect(-lightDir, norm);  
      float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
      vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb * attenuation;

      vec3 result = ambient + diffuse + specular;
      FragColor = vec4(result, 1.0f);
    }
    else
      FragColor = vec4(ambient, 1.0f);
}
