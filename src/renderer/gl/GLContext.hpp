#pragma once

#include "../../common.h"
#include "../Interface/Context.hpp"
#include "BufferManager.hpp"
#include "ShaderManager.hpp"

struct GLFWwindow;

namespace focus
{

class GLContext final : public Context
{
  BufferManager<VertexBufferHandle, VertexBufferDescriptor> mVBManager;
  BufferManager<IndexBufferHandle, IndexBufferDescriptor> mIBManager;
  BufferManager<ConstantBufferHandle, ConstantBufferDescriptor> mCBManager;
  BufferManager<BufferHandle, ShaderBufferDescriptor> mSBManager;
  ShaderManager mShaderManager;
  RenderState mCachedRenderState;
  u32 mVAO;

public:
  GLContext();
  // Window creation
  Window make_window(s32 width, s32 height) override;

  // Shader creation

  // TODO: need to create a shader compile tool
  ShaderHandle create_shader_from_binary(const std::vector<u8> &vBinary, const std::vector<u8> &fBinary) override;
  ShaderHandle create_shader_from_source(
      const char *name, const std::string &vSource, const std::string &fSource) override;
  ShaderHandle create_compute_shader_from_source(const char *name, const std::string &source) override;

  // Buffer Creation
  VertexBufferHandle create_vertex_buffer(void *data, VertexBufferDescriptor descriptor) override;
  IndexBufferHandle create_index_buffer(void *data, IndexBufferDescriptor descriptor) override;
  ConstantBufferHandle create_constant_buffer(void *data, ConstantBufferDescriptor descriptor) override;
  BufferHandle create_shader_buffer(void *data, ShaderBufferDescriptor descriptor) override;

  // Buffer Updates
  void update_vertex_buffer(VertexBufferHandle handle, void *data, u32 size) override;
  void update_index_buffer(IndexBufferHandle handle, void *data, u32 size) override;
  void update_constant_buffer(ConstantBufferHandle handle, void *data, u32 size) override;
  void update_shader_buffer(BufferHandle handle, void *data, u32 size) override;

  void *map_buffer(BufferHandle handle, AccessMode accessMode) override;
  void unmap_buffer(BufferHandle handle) override;

  // Buffer Destruction
  void destroy_vertex_buffer(VertexBufferHandle handle) override;
  void destroy_index_buffer(IndexBufferHandle handle) override;
  void destroy_shader_buffer(BufferHandle handle) override;
  void destroy_constant_buffer(ConstantBufferHandle handle) override;

  // draw call submission
  void draw(Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState &sceneState) override;

  // Compute shader dispatch
  void dispatch_compute(
      u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, const ComputeState &computeState) override;
  void wait_for_memory(u64 flags) override;

  // Screen clearing
  void clear_back_buffer(ClearState clearState = {}) override;

  void swap_buffers(const Window &window) override;
};

} // namespace focus
