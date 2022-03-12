#version 330 core
layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceMatrix;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
  // vec2 pos = aPos * (gl_InstanceID / 100.0);
  // gl_Position = vec4(pos + aOffset, 0.0, 1.0);
  // fColor = aColor;
  TexCoords = aTexCoord;
  gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
}