#version 460

layout(std140, binding = 1) uniform FragInput
{
  vec3 color2;
};

in vec3 oColor;
out vec4 FragColor;

void main()
{
  FragColor = vec4(oColor + color2, 1.0);
}