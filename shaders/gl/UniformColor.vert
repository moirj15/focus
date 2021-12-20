#version 460

//layout(std140, binding = 0) uniform CommonData
//{
const mat4 projection = mat4(1.0);
const mat4 view = mat4(1.0);
const mat4 model = mat4(1.0);
//} cb;

layout(std140, binding = 0) uniform Input
{
    vec3 color;
};

layout(location = 0) in vec3 aPosition;

out vec3 oColor;

void main()
{
    //  gl_Position = cb.projection * cb.view * cb.model * vec4(position, 1.0);
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
    oColor = color;
}