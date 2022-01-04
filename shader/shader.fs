#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform float ourColor;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
   // FragColor = texture(ourTexture, texCoord) * vec4(0.0f, ourColor, 0.0f, 1.0f);
   // 两个纹理混合, 最后一个参数0.2是指两个纹理的混合配比, 第一个80%, 第二个20%
   FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2);
}