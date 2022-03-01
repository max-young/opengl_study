#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
  vec4 texColor = texture(texture1, TexCoords);
  // 如果四通道颜色的alpha值(透明度)小于0.1, 则不着色
  if(texColor.a < 0.1) {
    discard;
  }
  FragColor = texColor;
}