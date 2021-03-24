#pragma once

#include "../../common.h"
#include "../Interface/Context.hpp"
#include "Buffer.hpp"
#include "BufferManager.hpp"
#include "Shader.hpp"

struct GLFWwindow;

namespace focus
{

class GLContext final : public Context
{
  BufferManager<VertexBufferHandle, VertexBufferDescriptor> mVBManager;
  BufferManager<IndexBufferHandle, IndexBufferDescriptor> mIBManager;
  BufferManager<ConstantBufferHandle, ConstantBufferDescriptor> mCBManager;
  BufferManager<BufferHandle, ShaderBufferDescriptor> mSBManager;
  RenderState mCachedRenderState;
  u32 mVAO;

public:
  GLContext();
  // Window creation
  Window MakeWindow(s32 width, s32 height) override;

  // Shader creation

  // TODO: need to create a shader compile tool
  ShaderHandle CreateShaderFromBinary(const char *vBinary, const char *fBinary) override;
  ShaderHandle CreateShaderFromSource(
      const char *name, const std::string &vSource, const std::string &fSource) override;
  ShaderHandle CreateComputeShaderFromSource(const char *name, const std::string &source) override;

  // Buffer Creation
  VertexBufferHandle CreateVertexBuffer(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor) override;
  IndexBufferHandle CreateIndexBuffer(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor) override;
  ConstantBufferHandle CreateConstantBuffer(void *data, u32 sizeInBytes, ConstantBufferDescriptor descriptor) override;
  BufferHandle CreateShaderBuffer(void *data, u32 sizeInBytes, ShaderBufferDescriptor descriptor) override;
  ConstantBufferHandle CreateConstantBuffer(void* data, u32 sizeInBytes, ConstantBufferDescriptor descriptor) override;

  void UpdateVertexBuffer(VertexBufferHandle handle, void *data, u32 size);
  void UpdateIndexBuffer(IndexBufferHandle handle, void *data, u32 size);
  void UpdateConstantBuffer(ConstantBufferHandle handle, void *data, u32 size);
  void UpdateShaderBuffer(BufferHandle handle, void *data, u32 size);

  void *MapBufferPtr(BufferHandle handle, AccessMode accessMode) override;
  void UnmapBufferPtr(BufferHandle handle) override;

  std::vector<u8> ReadShaderBuffer(BufferHandle handle) override;
  // Buffer Destruction

  void DestroyVertexBuffer(VertexBufferHandle handle) override;
  void DestroyIndexBuffer(IndexBufferHandle handle) override;
  void DestroyShaderBuffer(BufferHandle handle) override;
  void DestroyConstantBuffer(ConstantBufferHandle handle) override;

  // Draw call submission
  void Draw(Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState &sceneState) override;

  // Compute shader dispatch
  void DispatchCompute(
      u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, const ComputeState &computeState) override;
  void WaitForMemory(u64 flags) override;

  // Screen clearing
  void Clear(ClearState clearState = {}) override;

  void SwapBuffers(const Window &window) override;


};

} // namespace focus
