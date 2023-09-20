#version 330

uniform vec4 vs_params[5];
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec4 color0;
out vec4 color;

void main()
{
    gl_Position = mat4(vs_params[0], vs_params[1], vs_params[2], vs_params[3]) * position;
    if (vs_params[4].x == 0.0)
    {
        color = vec4((normal + vec3(1.0)) * 0.5, 1.0);
    }
    else
    {
        if (vs_params[4].x == 1.0)
        {
            color = vec4(texcoord, 0.0, 1.0);
        }
        else
        {
            color = color0;
        }
    }
}