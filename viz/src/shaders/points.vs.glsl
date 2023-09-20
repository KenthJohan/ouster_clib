// https://stackoverflow.com/questions/25780145/gl-pointsize-corresponding-to-world-space-size

#version 330

uniform vec4 vs_params[5];
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color0;
out vec4 color;

flat out vec2 centre;
flat out float radiusPixels;

//const float radius = 1.0;

void main()
{
	mat4 mvp = mat4(vs_params[0], vs_params[1], vs_params[2], vs_params[3]);
	gl_Position = mvp * vec4(position.xyz, 1.0);
	vec2 viewport = vs_params[4].xy;
	color = color0;
	centre = (0.5 * gl_Position.xy / gl_Position.w + 0.5) * viewport;
	float radius = position.w;
	gl_PointSize = viewport.y * radius / gl_Position.w;
	radiusPixels = gl_PointSize / 2.0;
}


