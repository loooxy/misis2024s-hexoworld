#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 color;
out vec3 FragPos;

uniform vec4 lightColor;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  gl_Position = projection * view * vec4(aPos.x, aPos.y, -aPos.z, 1.0);
  FragPos = aPos;
  //Color = aColor * lightColor;
  color = aColor;
}
