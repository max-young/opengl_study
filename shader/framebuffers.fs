#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

uniform vec3 cameraPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_reflection1;
uniform samplerCube skybox;

void main()
{
  // diffuse
  vec4 diffuse_color = texture(texture_diffuse1, TexCoords);
  // reflection
  vec3 I = normalize(Position - cameraPos);
  vec3 R = reflect(I, normalize(Normal));
  float reflect_intensity = texture(texture_reflection1, TexCoords).r;
  vec4 reflect_color;
  if (reflect_intensity > 0.01)
    // reflect_color = texture(skybox, R) * reflect_intensity;
    reflect_color = vec4(texture(skybox, R).rgb, 1.0) * reflect_intensity;

  FragColor = diffuse_color + reflect_color;
  // FragColor = reflect_color;
}