#include "focus.hpp"

#include "../gl/GLContext.hpp"
#include "Context.hpp"

namespace focus
{

static Context *context = nullptr;

void init(RendererAPI api)
{
  if (context != nullptr) {
    printf("Focus was already initialized, terminating program\n");
    std::exit(1);
  }
  if (api == RendererAPI::OpenGL) {
    context = new GLContext();
  } else if (api == RendererAPI::Vulkan) {

  } else if (api == RendererAPI::DX11) {

  } else if (api == RendererAPI::DX12) {

  } else if (api == RendererAPI::Invalid) {
  }
}
ShaderHandle create_shader_from_binary(const std::vector<u8> &vBinary, const std::vector<u8> &fBinary)
{
  return context->create_shader_from_binary(vBinary, fBinary);
}
ShaderHandle create_shader_from_source(const char *name, const std::string &vSource, const std::string &fSource)
{
  return context->create_shader_from_source(name, vSource, fSource);
}
ShaderHandle create_compute_shader_from_source(const char *name, const std::string &source)
{
  return context->create_compute_shader_from_source(name, source);
}
VertexBufferHandle create_vertex_buffer(void *data, VertexBufferDescriptor descriptor)
{
  return context->create_vertex_buffer(data, VertexBufferDescriptor());
}
IndexBufferHandle create_index_buffer(void *data, IndexBufferDescriptor descriptor)
{
  return context->create_index_buffer(data, descriptor);
}
ConstantBufferHandle create_constant_buffer(void *data, ConstantBufferDescriptor descriptor)
{
  return context->create_constant_buffer(data, descriptor);
}
BufferHandle create_shader_buffer(void *data, ShaderBufferDescriptor descriptor)
{
  return context->create_shader_buffer(data, descriptor);
}
void update_vertex_buffer(VertexBufferHandle handle, void *data, u32 size)
{
  context->update_vertex_buffer(handle, data, size);
}
void update_index_buffer(IndexBufferHandle handle, void *data, u32 size)
{
  context->update_index_buffer(handle, data, size);
}
void update_constant_buffer(ConstantBufferHandle handle, void *data, u32 size)
{
  context->update_constant_buffer(handle, data, size);
}
void update_shader_buffer(BufferHandle handle, void *data, u32 size)
{
  context->update_shader_buffer(handle, data, size);
}
void *map_buffer(BufferHandle handle, AccessMode access_mode)
{
  return context->map_buffer(handle, access_mode);
}
void unmap_buffer(BufferHandle handle)
{
  context->unmap_buffer(handle);
}
void destroy_vertex_buffer(VertexBufferHandle handle)
{
  context->destroy_vertex_buffer(handle);
}
void destroy_index_buffer(IndexBufferHandle handle)
{
  context->destroy_index_buffer(handle);
}
void destroy_shader_buffer(BufferHandle handle)
{
  context->destroy_shader_buffer(handle);
}
void destroy_constant_buffer(ConstantBufferHandle handle)
{
  context->destroy_constant_buffer(handle);
}
void draw(Primitive primitive, RenderState render_state, ShaderHandle shader, const SceneState &scene_state)
{
  context->draw(primitive, render_state, shader, scene_state);
}
void dispatch_compute(u32 x_groups, u32 y_groups, u32 z_groups, ShaderHandle shader, const ComputeState &compute_state)
{
  context->dispatch_compute(x_groups, y_groups, z_groups, shader, compute_state);
}
void wait_for_memory(u64 flags)
{
  context->wait_for_memory(flags);
}
void clear_back_buffer(ClearState clear_state)
{
  context->clear_back_buffer(clear_state);
}
void swap_buffers(const Window &window)
{
  context->swap_buffers(window);
}

} // namespace focus
