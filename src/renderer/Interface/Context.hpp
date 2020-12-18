#pragma once
#include "Handles.hpp"
#include "IBuffer.hpp"
#include "IShader.hpp"
#include "RenderState.hpp"
#include "Types.hpp"
#include "Window.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace focus
{

enum class RendererAPI {
  OpenGL,
  Vulkan,
  DX11,
  DX12,
  Invalid,
};


class Context
{
public:
  /**
   * @brief Initialize the global render context (gContext) with the specified Render API.
   *        Should only be called once, calling multiple times will result in an assertion in debug builds.
   * @param api The render API that will the global render context will use.
   * @param messageHandler The windows message handler.
   * @param instanceHandle The windows application's HINSTANCE value that is passed to WinMain.
   */
  static void Init(RendererAPI api, WNDPROC messageHandler, HINSTANCE instanceHandle);

  // Window creation
  virtual Window MakeWindow(s32 width, s32 height) = 0;

  // Shader creation

  virtual ShaderHandle CreateShaderFromBinary(const char *vBinary, const char *fBinary) = 0;
  virtual ShaderHandle CreateShaderFromSource(
      const char *name, const std::string &vSource, const std::string &fSource) = 0;
  virtual ShaderHandle CreateComputeShaderFromSource(const char *name, const std::string &source) = 0;

  // Buffer Creation
  virtual VertexBufferHandle CreateVertexBuffer(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor) = 0;
  virtual IndexBufferHandle CreateIndexBuffer(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor) = 0;
  virtual BufferHandle CreateShaderBuffer(void *data, u32 sizeInBytes, ShaderBufferDescriptor descriptor) = 0;

  // Buffer Access
  // TODO: add partial buffer access too
  // TODO: consider adding a scoped pointer for mapped memory
  virtual void *MapBufferPtr(BufferHandle handle, AccessMode accessMode) = 0;
  virtual void UnmapBufferPtr(BufferHandle handle) = 0;

  // Buffer Destruction

  virtual void DestroyVertexBuffer(VertexBufferHandle handle) = 0;
  virtual void DestroyIndexBuffer(IndexBufferHandle handle) = 0;

  // Draw call submission
  virtual void Draw(
      Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState &sceneState) = 0;

  // Compute shader dispatch
  virtual void DispatchCompute(
      u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, const ComputeState &computeState) = 0;

  // TODO: better naming
  virtual void WaitForMemory(u64 flags) = 0;

  // Screen clearing
  virtual void Clear(ClearState clearState = {}) = 0;
};
extern Context *gContext;

#if 0
  namespace context
  {
#ifdef _WIN32
    void Init(WNDPROC messageHandler, HINSTANCE instanceHandle);
#else
    void Init();
#endif

    // Window creation
    Window MakeWindow(s32 width, s32 height);

    // Shader creation

    ShaderHandle CreateShaderFromBinary(const char* vBinary, const char* fBinary);
    ShaderHandle CreateShaderFromSource(const char* name, const std::string& vSource, const std::string& fSource);
    ShaderHandle CreateComputeShaderFromSource(const char* name, const std::string& source);

    // Buffer Creation
    VertexBufferHandle CreateVertexBuffer(void* data, u32 sizeInBytes, VertexBufferDescriptor descriptor);
    IndexBufferHandle CreateIndexBuffer(void* data, u32 sizeInBytes, IndexBufferDescriptor descriptor);
    BufferHandle CreateShaderBuffer(void* data, u32 sizeInBytes, ShaderBufferDescriptor descriptor);

    // Buffer Destruction

    void DestroyVertexBuffer(VertexBufferHandle handle);
    void DestroyIndexBuffer(IndexBufferHandle handle);

    // Draw call submission
    void Draw(Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState& sceneState);

    // Compute shader dispatch
    void DispatchCompute(u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, const ComputeState& computeState);

    // Screen clearing
    void Clear(ClearState clearState = {});


  } // namespace context
#endif

} // namespace focus
