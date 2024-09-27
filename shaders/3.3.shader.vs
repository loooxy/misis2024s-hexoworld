#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

//out VS_OUT{
//    vec4 color;
//} vs_out;

out vec4 oColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
  gl_Position = projection * view * vec4(aPos, 1.0);
  oColor = aColor;
}