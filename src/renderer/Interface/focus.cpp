#include "focus.hpp"

#include "../gl/GLContext.hpp"
#include "Context.hpp"

namespace focus
{

static Context *context = nullptr;

void Init(RendererAPI api)
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
Window MakeWindow(int32_t width, int32_t height)
{
  return context->MakeWindow(width, height);
}

ShaderHandle CreateShaderFromBinary(const std::vector<u8> &vBinary, const std::vector<u8> &fBinary)
{
  return context->CreateShaderFromBinary(vBinary, fBinary);
}
ShaderHandle CreateShaderFromSource(const char *name, const std::string &vSource, const std::string &fSource)
{
  return context->CreateShaderFromSource(name, vSource, fSource);
}
ShaderHandle CreateComputeShaderFromSource(const char *name, const std::string &source)
{
  return context->CreateComputeShaderFromSource(name, source);
}
VertexBufferHandle CreateVertexBuffer(void *data, VertexBufferDescriptor descriptor)
{
  return context->CreateVertexBuffer(data, descriptor);
}
IndexBufferHandle CreateIndexBuffer(void *data, IndexBufferDescriptor descriptor)
{
  return context->CreateIndexBuffer(data, descriptor);
}
ConstantBufferHandle CreateConstantBuffer(void *data, ConstantBufferDescriptor descriptor)
{
  return context->CreateConstantBuffer(data, descriptor);
}
ShaderBufferHandle CreateShaderBuffer(void *data, ShaderBufferDescriptor descriptor)
{
  return context->CreateShaderBuffer(data, descriptor);
}
void UpdateVertexBuffer(VertexBufferHandle handle, void *data, u32 size)
{
  context->UpdateVertexBuffer(handle, data, size);
}
void UpdateIndexBuffer(IndexBufferHandle handle, void *data, u32 size)
{
  context->UpdateIndexBuffer(handle, data, size);
}
void UpdateConstantBuffer(ConstantBufferHandle handle, void *data, u32 size)
{
  context->UpdateConstantBuffer(handle, data, size);
}
void UpdateShaderBuffer(ShaderBufferHandle handle, void *data, u32 size)
{
  context->UpdateShaderBuffer(handle, data, size);
}
void *MapBuffer(ShaderBufferHandle handle, AccessMode access_mode)
{
  return context->MapBuffer(handle, access_mode);
}
void UnmapBuffer(ShaderBufferHandle handle)
{
  context->UnmapBuffer(handle);
}
void DestroyVertexBuffer(VertexBufferHandle handle)
{
  context->DestroyVertexBuffer(handle);
}
void DestroyIndexBuffer(IndexBufferHandle handle)
{
  context->DestroyIndexBuffer(handle);
}
void DestroyShaderBuffer(ShaderBufferHandle handle)
{
  context->DestroyShaderBuffer(handle);
}
void DestroyConstantBuffer(ConstantBufferHandle handle)
{
  context->DestroyConstantBuffer(handle);
}
void Draw(Primitive primitive, RenderState render_state, ShaderHandle shader, const SceneState &scene_state)
{
  context->Draw(primitive, render_state, shader, scene_state);
}
void DispatchCompute(u32 x_groups, u32 y_groups, u32 z_groups, ShaderHandle shader, const ComputeState &compute_state)
{
  context->DispatchCompute(x_groups, y_groups, z_groups, shader, compute_state);
}
void WaitForMemory(u64 flags)
{
  context->WaitForMemory(flags);
}
void ClearBackBuffer(ClearState clear_state)
{
  context->ClearBackBuffer(clear_state);
}
void swap_buffers(const Window &window)
{
  context->SwapBuffers(window);
}

} // namespace focus
