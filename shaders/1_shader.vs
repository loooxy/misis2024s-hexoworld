#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 Color;

uniform mat4 view;
uniform mat4 projection;

void main()
{
  vec3 Pos = vec3(aPos.x, aPos.y, -aPos.z);
  gl_Position = projection * view * vec4(Pos, 1.0);
  Color = aColor;
}