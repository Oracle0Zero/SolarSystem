#version 430

in vec2 tc;

out vec4 color;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
layout (binding=0) uniform sampler2D samp;

void main(void)
{
    color = vec4(1.0f, 1.0f, 1.0f, 0.4f);
};
