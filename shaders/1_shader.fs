#version 330 core
out vec4 FragColor;

in vec4 Color;
in vec3 fragPos;
in vec3 pos1;
in vec3 pos2;
in vec3 pos3;

uniform vec3 lightPos;

void main()
{
  vec3 vector_1 = pos2 - pos1;
  vec3 vector_2 = pos3 - pos2;
  vec3 normal = abs(vec3(vector_1.y * vector_2.z - vector_1.z * vector_2.y, -(vector_1.x * vector_2.z - vector_1.z * vector_2.x), vector_1.x * vector_2.y - vector_1.y * vector_2.x));
  
  //vec3 normal = vec3(0.5f, 0.5f, -0.5f);
  vec3 norm = normalize(normal);
  vec3 lightDir = normalize(lightPos - fragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  float k = 1.1;
  vec4 diffuse = vec4(diff * k, diff * k, diff * k, 1);
  vec4 result = Color * diffuse;
  FragColor = result;
  //FragColor = Color;
}
