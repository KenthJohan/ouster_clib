#version 330

uniform vec4 vs_params[5];
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color0;
out vec4 color;

void main()
{
	gl_Position = mat4(vs_params[0], vs_params[1], vs_params[2], vs_params[3]) * vec4(position, 1.0);
	gl_PointSize = vs_params[4].x;
	color = color0;
}