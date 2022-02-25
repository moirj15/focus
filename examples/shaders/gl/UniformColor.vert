#version 460

layout(std140, binding = 0) uniform Input
{
    vec4 color; // w component for padding
    mat4 mvp;
};

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aOffset;

out vec3 oColor;

void main()
{
    gl_Position = mvp * vec4(aPosition, 1.0);
    oColor = color.xyz;
}