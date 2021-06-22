#pragma once

#include "focus.hpp"

struct SDL_Window;

namespace std
{

template<typename Tag, typename T, T default_value>
struct hash<focus::Handle<Tag, T, default_value>> {
  // template <typename Tag, typename T, T default_value>
  size_t operator()(const focus::Handle<Tag, T, default_value> &k) const { return k.Hash(); }
};

} // namespace std

namespace focus
{

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using f32 = float;
using f64 = double;

// TODO: need some better documentation.
class Context
{
public:
  // Window creation
  virtual Window MakeWindow(s32 width, s32 height) = 0;

  // Shader creation
  virtual ShaderHandle CreateShaderFromBinary(const std::vector<u8> &vBinary, const std::vector<u8> &fBinary) = 0;
  virtual ShaderHandle CreateShaderFromSource(
      const char *name, const std::string &vSource, const std::string &fSource) = 0;
  virtual ShaderHandle CreateComputeShaderFromSource(const char *name, const std::string &source) = 0;

  // Buffer Creation
  virtual VertexBufferHandle CreateVertexBuffer(void *data, VertexBufferDescriptor descriptor) = 0;
  virtual IndexBufferHandle CreateIndexBuffer(void *data, IndexBufferDescriptor descriptor) = 0;
  virtual ConstantBufferHandle CreateConstantBuffer(void *data, ConstantBufferDescriptor descriptor) = 0;
  virtual ShaderBufferHandle CreateShaderBuffer(void *data, ShaderBufferDescriptor descriptor) = 0;

  virtual void UpdateVertexBuffer(VertexBufferHandle handle, void *data, u32 size) = 0;
  virtual void UpdateIndexBuffer(IndexBufferHandle handle, void *data, u32 size) = 0;
  virtual void UpdateConstantBuffer(ConstantBufferHandle handle, void *data, u32 size) = 0;
  virtual void UpdateShaderBuffer(ShaderBufferHandle handle, void *data, u32 size) = 0;

  // Buffer Access
  // TODO: add partial buffer access too
  // TODO: consider adding a scoped pointer for mapped memory
  virtual void *MapBuffer(ShaderBufferHandle handle, AccessMode access_mode) = 0;
  virtual void UnmapBuffer(ShaderBufferHandle handle) = 0;

  // Buffer Destruction

  virtual void DestroyVertexBuffer(VertexBufferHandle handle) = 0;
  virtual void DestroyIndexBuffer(IndexBufferHandle handle) = 0;
  virtual void DestroyShaderBuffer(ShaderBufferHandle handle) = 0;
  virtual void DestroyConstantBuffer(ConstantBufferHandle handle) = 0;

  // draw call submission
  virtual void Draw(
      Primitive primitive, RenderState render_state, ShaderHandle shader, const SceneState &scene_state) = 0;

  // Compute shader dispatch
  virtual void DispatchCompute(
      u32 x_groups, u32 y_groups, u32 z_groups, ShaderHandle shader, const ComputeState &compute_state) = 0;

  // TODO: better naming
  virtual void WaitForMemory(u64 flags) = 0;

  // Screen clearing
  virtual void ClearBackBuffer(ClearState clear_state) = 0;

  virtual void SwapBuffers(const Window &window) = 0;
};

} // namespace focus
