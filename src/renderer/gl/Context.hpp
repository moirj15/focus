#pragma once

#include "../../common.h"
#include "Shader.hpp"
#include "../Interface/Window.hpp"
#include "../Interface/IBuffer.hpp"
#include "../Interface/Handles.hpp"
#include "Buffer.hpp"



struct GLFWwindow;


namespace renderer::gl::context
{

#ifdef _WIN32
void Init(WNDPROC messageHandler, HINSTANCE instanceHandle);
#else
void Init();
#endif

Window MakeWindow(s32 width, s32 height);

// TODO: tesselation, geometry, compute shaders
ShaderHandle CreateShaderFromSource(const char *vSource, const char *fSource);

// TODO: interleved vertex buffers
VertexBufferHandle CreateVertexBuffer(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor);

void DestroyVertexBuffer(VertexBufferHandle handle);

IndexBufferHandle CreateIndexBuffer(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor);

void DestroyIndexBuffer(IndexBufferHandle handle);

//PixelBuffer CreatePixelBuffer(u32 width, u32 height, Components components);

//Texture2D CreateTexture2D(u32 width, u32 height, Components components);

} // namespace gl::Context
