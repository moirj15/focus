#pragma once
#include "Types.hpp"
#include "Window.hpp"
#include "IShader.hpp"
#include "Handles.hpp"
#include "IBuffer.hpp"
#include "RenderState.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif


namespace renderer
{


enum class RendererAPI {
  OpenGL,
  Vulkan,
  DX11,
  DX12,
  Invalid
};

namespace context
{
#ifdef _WIN32
void Init(WNDPROC messageHandler, HINSTANCE instanceHandle);
#else
void Init();
#endif
Window MakeWindow(s32 width, s32 height);
ShaderHandle CreateShaderFromBinary(const char *vBinary, const char *fBinary);
ShaderHandle CreateShaderFromSource(const char *name, const std::string &vSource, const std::string &fSource);
ShaderHandle CreateComputeShaderFromSource(const char *name, const std::string &source);

VertexBufferHandle CreateVertexBuffer(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor);

void DestroyVertexBuffer(VertexBufferHandle handle);

IndexBufferHandle CreateIndexBuffer(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor);

void DestroyIndexBuffer(IndexBufferHandle handle);

void Draw(Primitive primitive, RenderState renderState, ShaderHandle shader, SceneState sceneState);

void Clear(ClearState clearState = {});

void DispatchCompute(u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, ComputeState computeState);

} // namespace context

} // namespace renderer
