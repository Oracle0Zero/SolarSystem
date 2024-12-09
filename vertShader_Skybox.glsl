#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 pMat;
uniform mat4 vMat;

void main()
{
    TexCoords = aPos;
    gl_Position = pMat * vMat * vec4(aPos, 1.0);
}  