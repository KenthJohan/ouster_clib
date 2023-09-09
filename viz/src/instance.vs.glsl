#version 330

/*
[0] = sshape_position_vertex_attr_state(),
[1] = sshape_normal_vertex_attr_state(),
[2] = sshape_texcoord_vertex_attr_state(),
[3] = sshape_color_vertex_attr_state(),
[4] = { .format=SG_VERTEXFORMAT_FLOAT3, .buffer_index=1 }
*/

uniform vec4 vs_params[5];
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec4 color0;
layout(location = 4) in vec3 inst_pos;

out vec4 color;

void main()
{
    gl_Position = mat4(vs_params[0], vs_params[1], vs_params[2], vs_params[3]) * (position + vec4(inst_pos, 0.0));
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