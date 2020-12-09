#pragma once

#include "../../common.h"
#include "Shader.hpp"
#include "../Interface/Window.hpp"
#include "../Interface/IBuffer.hpp"
#include "../Interface/Handles.hpp"
#include "../Interface/RenderState.hpp"
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
ShaderHandle CreateShaderFromSource(const char * name, const std::string &vSource, const std::string &fSource);

ShaderHandle CreateComputeShaderFromSource(const char *name, const std::string &source);

// TODO: interleved vertex buffers
VertexBufferHandle CreateVertexBuffer(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor);

void DestroyVertexBuffer(VertexBufferHandle handle);

IndexBufferHandle CreateIndexBuffer(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor);

void DestroyIndexBuffer(IndexBufferHandle handle);

void Draw(Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState &sceneState);

void Clear(ClearState clearState);

void DispatchCompute(u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, const ComputeState &computeState);

} // namespace gl::Context
