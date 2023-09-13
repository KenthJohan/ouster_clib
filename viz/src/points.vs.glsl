#version 330

uniform vec4 vs_params[5];
layout(location = 0) in vec2 position;
out vec4 color;
layout(location = 1) in vec4 color0;

void main()
{
	gl_Position = mat4(vs_params[0], vs_params[1], vs_params[2], vs_params[3]) * vec4(position, 0.0, 1.0);
	gl_PointSize = vs_params[4].x;
	color = color0;
}