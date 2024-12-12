#version 330 core
out vec4 FragColor;

in vec4 Color;
in vec3 fragPos;
in vec3 normal;

uniform vec3 lightPos;

void main()
{
  vec3 normal = vec3(0.5f, 0.5f, -0.5f);
  vec3 norm = normalize(normal);
  vec3 lightDir = normalize(lightPos - fragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec4 diffuse = vec4(diff * 2, diff * 2, diff * 2, 1);
  vec4 result = Color * diffuse;
  FragColor = result;
  //FragColor = Color;
}
