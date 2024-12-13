#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 color[];
in vec3 FragPos[];

out vec3 pos1;
out vec3 pos2;
out vec3 pos3;
out vec4 Color;
out vec3 fragPos;

void main() {
    Color = (color[0] + color[1] + color[2]) / 3;
    fragPos = min(FragPos[0], min(FragPos[1], FragPos[2]));
    gl_Position = gl_in[0].gl_Position;
    pos1 = vec3(gl_in[0].gl_Position);
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    pos2 = vec3(gl_in[1].gl_Position);
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    pos3 = vec3(gl_in[2].gl_Position);
    EmitVertex();
    EndPrimitive();
}
