#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 color[];
in vec3 FragPos[];

out vec3 normal;
out vec4 Color;
out vec3 fragPos;

void main() {
    Color = (color[0] + color[1] + color[2]) / 3;
    fragPos = min(FragPos[0], min(FragPos[1], FragPos[2]));
    gl_Position = gl_in[0].gl_Position;
    vec3 pos1 = vec3(gl_Position);
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    vec3 pos2 = vec3(gl_Position);
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    vec3 pos3 = vec3(gl_Position);
    EmitVertex();
    vec3 vector_1 = pos2 - pos1;
    vec3 vector_2 = pos3 - pos2;
    normal = vec3(vector_1.y * vector_2.z - vector_1.z * vector_2.y, -(vector_1.x * vector_2.z - vector_1.z * vector_2.x), vector_1.x * vector_2.y - vector_1.y * vector_2.x);
    if (normal.z < 0){
        normal.z = -normal.z;
    }
    EndPrimitive();
}