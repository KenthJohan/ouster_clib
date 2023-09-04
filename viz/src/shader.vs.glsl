#version 330

uniform vec4 vs_params[4];
layout(location = 0) in vec3 pos;
layout(location = 2) in vec3 inst_pos;
out vec4 color;
layout(location = 1) in vec4 color0;

void main()
{
    gl_Position = mat4(vs_params[0], vs_params[1], vs_params[2], vs_params[3]) * vec4(pos + inst_pos, 1.0);
    color = color0;
}