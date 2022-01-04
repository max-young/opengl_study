#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform float ourColor;
uniform sampler2D ourTexture;

void main()
{
   // FragColor = vec4(0.0f, ourColor, 0.0f, 1.0f);
   FragColor = texture(ourTexture, texCoord);
}