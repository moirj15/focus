#include "GLContext.hpp"

#include "BufferManager.hpp"
#include "ShaderManager.hpp"
#include "Utils.hpp"
#include "glad.h"

#include <SDL2/SDL.h>

namespace focus
{

static void DBCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message,
    void const *userParam)
{
  printf("DBG: %s\n", message);
}

GLContext::GLContext()
{
  if (SDL_Init(SDL_INIT_VIDEO)) {
    assert(0 && "Failed to Init video");
  }
  SDL_GL_LoadLibrary(nullptr);
}

Window GLContext::MakeWindow(s32 width, s32 height)
{
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

  SDL_Window *window =
      SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);

  assert(window != nullptr);

  // Don't resize window so it doesn't mess with tiling window managers
  SDL_SetWindowResizable(window, SDL_FALSE);

  auto *context = SDL_GL_CreateContext(window);
  assert(context);
  assert(gladLoadGL());

#ifdef _DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(DBCallBack, nullptr);
#endif

  printf("%s", glGetString(GL_VERSION));

  glGenVertexArrays(1, &mVAO);
  return {
      .width = width,
      .height = height,
      .sdl_window = window,
  };
}

ShaderHandle GLContext::CreateShaderFromBinary(const std::vector<u8> &vBinary, const std::vector<u8> &fBinary)
{
  assert(0);
  return {};
}

ShaderHandle GLContext::CreateShaderFromSource(
    const char *name, const std::string &vSource, const std::string &fSource)
{
  return mShaderManager.AddShader(name, vSource, fSource);
}

ShaderHandle GLContext::CreateComputeShaderFromSource(const char *name, const std::string &source)
{
  return mShaderManager.AddComputeShader(name, source);
}

VertexBufferHandle GLContext::CreateVertexBuffer(void *data, VertexBufferDescriptor descriptor)
{
  return mVBManager.Create(data, descriptor);
}

void GLContext::DestroyVertexBuffer(VertexBufferHandle handle)
{
  mVBManager.Destroy(handle);
}

IndexBufferHandle GLContext::CreateIndexBuffer(void *data, IndexBufferDescriptor descriptor)
{
  return mIBManager.Create(data, descriptor);
}

ConstantBufferHandle GLContext::CreateConstantBuffer(void *data, ConstantBufferDescriptor descriptor)
{
  return mCBManager.Create(data, descriptor);
}

ShaderBufferHandle GLContext::CreateShaderBuffer(void *data, ShaderBufferDescriptor descriptor)
{
  return mSBManager.Create(data, descriptor);
}

void GLContext::UpdateVertexBuffer(VertexBufferHandle handle, void *data, u32 size)
{
  mVBManager.WriteTo(data, size, handle);
}
void GLContext::UpdateIndexBuffer(IndexBufferHandle handle, void *data, u32 size)
{
  mIBManager.WriteTo(data, size, handle);
}
void GLContext::UpdateConstantBuffer(ConstantBufferHandle handle, void *data, u32 size)
{
  mCBManager.WriteTo(data, size, handle);
}
void GLContext::UpdateShaderBuffer(ShaderBufferHandle handle, void *data, u32 size)
{
  assert(0);
}

void *GLContext::MapBuffer(ShaderBufferHandle handle, AccessMode accessMode)
{
  return glMapNamedBuffer(mSBManager.Get(handle), glUtils::AccessModeToGL(accessMode));
}
void GLContext::UnmapBuffer(ShaderBufferHandle handle)
{
  glUnmapNamedBuffer(mSBManager.Get(handle));
}

void GLContext::DestroyIndexBuffer(IndexBufferHandle handle)
{
  mIBManager.Destroy(handle);
}

void GLContext::DestroyShaderBuffer(ShaderBufferHandle handle)
{
  mSBManager.Destroy(handle);
}
void GLContext::DestroyConstantBuffer(ConstantBufferHandle handle)
{
  mCBManager.Destroy(handle);
}

// TODO: do automatic batching

void GLContext::Draw(Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState &sceneState)
{
  GLenum glPrimitive = glUtils::PrimitiveToGL(primitive);
  // TODO: sort based on bit flags?
  if (renderState.depth_test != mCachedRenderState.depth_test) {
    if (renderState.depth_test.enabled) {
      glEnable(GL_DEPTH_TEST);
      GLenum compFunc = glUtils::ComparisonFunctionToGL(renderState.depth_test.function);
      glDepthFunc(compFunc);
      glDepthMask(renderState.depth_test.write_to_depth_buffer);
    } else {
      glDisable(GL_DEPTH_TEST);
    }
  }
  if (renderState.cull_state != mCachedRenderState.cull_state) {
    if (renderState.cull_state.enabled) {
      glEnable(GL_CULL_FACE);
      GLenum cullFace = glUtils::TriangleFaceToGL(renderState.cull_state.face);
      glCullFace(cullFace);
      GLenum frontFace = glUtils::WindingOrderToGL(renderState.cull_state.front_face);
      glFrontFace(frontFace);
    } else {
      glDisable(GL_CULL_FACE);
    }
  }
  if (renderState.rasterization_mode != mCachedRenderState.rasterization_mode) {
    GLenum rasterizationMode = glUtils::RasterizationModeToGL(renderState.rasterization_mode);
    // TODO: add the face and rasterization mode to a struct?
    glPolygonMode(GL_FRONT_AND_BACK, rasterizationMode);
  }
  if (renderState.stencil_test != mCachedRenderState.stencil_test) {
    if (renderState.stencil_test.enabled) {
      glEnable(GL_STENCIL_TEST);
    } else {
      glDisable(GL_STENCIL_TEST);
    }
  }
  if (renderState.depth_range != mCachedRenderState.depth_range) {
    glDepthRange(renderState.depth_range.mNear, renderState.depth_range.mNear);
  }
  if (renderState.blend_state != mCachedRenderState.blend_state) {
    if (renderState.blend_state.mEnabled) {
      glEnable(GL_BLEND);
    } else {
      glDisable(GL_BLEND);
    }
  }
  auto ibDesc = mIBManager.mDescriptors[sceneState.ib_handle];
  auto shaderInfo = mShaderManager.GetInfo(shader);
  u32 program = mShaderManager.GetProgram(shader);
  glUseProgram(program);
  glBindVertexArray(mVAO);
  // TODO: for now I'll assume that there is either one interleaved vbo or multiple single-type buffers
  for (const auto &vbHandle : sceneState.vb_handles) {
    const auto &vbDesc = mVBManager.mDescriptors[vbHandle];
    u32 glVBHandle = mVBManager.Get(vbHandle);
    if (vbDesc.buffer_type == BufferType::SingleType) {
      const auto &inputDesc = shaderInfo.mInputBufferDescriptors[vbDesc.input_descriptor_name[0]];
      glBindBuffer(GL_ARRAY_BUFFER, glVBHandle);
      glEnableVertexAttribArray(inputDesc.slot);

      // glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (const void *)0);
      glVertexAttribPointer(inputDesc.slot, glUtils::VarTypeToSlotSizeGL(vbDesc.types[0]),
          glUtils::VarTypeToIndividualTypeGL(inputDesc.type), false, 0, (const void *)0);
    } else if (vbDesc.buffer_type == BufferType::InterLeaved) {
      glBindBuffer(GL_ARRAY_BUFFER, glVBHandle);
      for (u32 i = 0; i < shaderInfo.mInputBufferDescriptors.size(); i++) {
        const auto &inputDesc = shaderInfo.mInputBufferDescriptors[vbDesc.input_descriptor_name[i]];
        glEnableVertexAttribArray(inputDesc.slot);

        // glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (const void *)0);
        glVertexAttribPointer(inputDesc.slot, glUtils::VarTypeToSlotSizeGL(vbDesc.types[i]),
            glUtils::VarTypeToIndividualTypeGL(inputDesc.type), false, vbDesc.element_size_in_bytes,
            (const void *)inputDesc.byteOffset);
      }
    } else {
      assert(0);
    }
  }
  // setup all the uniform buffers
  for (const auto &cbHandle : sceneState.cb_handles) {
    const auto &cbDesc = mCBManager.mDescriptors[cbHandle];
    u32 glCBHandle = mCBManager.Get(cbHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, glCBHandle);
    // TODO: figure out multiple binding points
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, glCBHandle);
    glUniformBlockBinding(program, cbDesc.slot, 0);
  }
  if (sceneState.indexed) {
    u32 glIBHandle = mIBManager.Get(sceneState.ib_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIBHandle);
    glDrawElements(glPrimitive, ibDesc.size_in_bytes / 4, GL_UNSIGNED_INT, (void *)0);
  } else {
    // TODO: pretty ugly, need to re-write. Maybe add an element count to the sceneState or some descriptor?
    glDrawArrays(glPrimitive, 0,
        (mVBManager.mDescriptors[sceneState.vb_handles[0]].size_in_bytes
            / mVBManager.mDescriptors[sceneState.vb_handles[0]].element_size_in_bytes)
            / 3);
  }
}

void GLContext::ClearBackBuffer(ClearState clearState)
{
  glClearColor(
      clearState.clear_color.red, clearState.clear_color.green, clearState.clear_color.blue, clearState.clear_color.alpha);
  glClear(glUtils::ClearBufferToGL(clearState.to_clear));
}

void GLContext::DispatchCompute(
    u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, const ComputeState &computeState)
{
  u32 program = mShaderManager.GetProgram(shader);
  glUseProgram(program);
  for (auto bufferHandle : computeState.buffer_handles) {
    const auto &bufferDesc = mSBManager.mDescriptors[bufferHandle];
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSBManager.Get(bufferHandle));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bufferDesc.slot, mSBManager.Get(bufferHandle));
  }
  for (const auto &cbHandle : computeState.cb_handles) {
    const auto &cbDesc = mCBManager.mDescriptors[cbHandle];
    glBindBuffer(GL_UNIFORM_BUFFER, mCBManager.Get(cbHandle));
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, mCBManager.Get(cbHandle));
  }
  glDispatchCompute(xGroups, yGroups, zGroups);
}

void GLContext::WaitForMemory(u64 flags)
{
  // TODO: actually use input
  glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void GLContext::SwapBuffers(const Window &window)
{
  SDL_GL_SwapWindow(window.sdl_window);
}

} // namespace focus
