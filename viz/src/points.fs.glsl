// https://stackoverflow.com/questions/25780145/gl-pointsize-corresponding-to-world-space-size

#version 330

flat in vec2 centre;
flat in float radiusPixels;

layout(location = 0) out vec4 fragColor;
in vec4 color;

void main()
{
    vec2 coord = (gl_FragCoord.xy - centre) / radiusPixels;
    float l = length(coord);
    if (l > 1.0)
        discard;
    vec3 pos = vec3(coord, sqrt(1.0-l*l));
    fragColor = vec4(vec3(pos.z), 1.0) * color;
    //fragColor = color;
}

