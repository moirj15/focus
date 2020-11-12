#pragma once
#include "../../common.h"
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

VertexBufferHandle CreateVertexBuffer(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor);

void DestroyVertexBuffer(VertexBufferHandle handle);

IndexBufferHandle CreateIndexBuffer(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor);

void DestroyIndexBuffer(IndexBufferHandle handle);

void Draw(Primitive primitive, RenderState renderState, ShaderHandle shader, SceneState sceneState);

void Clear(ClearState clearState);

// PixelBuffer CreatePixelBuffer(u32 width, u32 height, Components components);
//
// Texture2D CreateTexture2D(u32 width, u32 height, Components components);
} // namespace context

} // namespace renderer
