 #version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

//in VS_OUT{
//   vec4 color;
//} gs_in[];

in vec4 oColor[];

out vec4 fColor;

void main() {
    fColor = oColor[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    //gl_Position = gl_in[0].gl_Position + vec4(0f, 0f, 0f, 1f);
    //EmitVertex();
    EndPrimitive();
}