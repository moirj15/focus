#version 450

//layout(std140, binding = 0) uniform CommonData
//{
  mat4 projection = mat4(1.0);
  mat4 view = mat4(1.0);
  mat4 model = mat4(1.0);
//} cb;

layout(location = 0) in vec3 aPosition;


void main()
{
//  gl_Position = cb.projection * cb.view * cb.model * vec4(position, 1.0);
  gl_Position = projection * view * model * vec4(aPosition, 1.0);
}