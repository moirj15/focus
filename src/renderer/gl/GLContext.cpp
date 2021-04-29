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
    assert(0 && "Failed to init video");
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

#if 0
  GLFWwindow *glfwWindow = nullptr;
  // need for i3
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindow = glfwCreateWindow(width, height, "OpenGL Renderer", nullptr, nullptr);
  if (glfwWindow == nullptr) {
    printf("Failed to create window\n");
    assert(0 && "Failed to create window");
  }
  glfwMakeContextCurrent(glfwWindow);
  assert(gladLoadGL());
  glViewport(0, 0, width, height);
#endif
  glGenVertexArrays(1, &mVAO);
  return {
      .mWidth = width,
      .mHeight = height,
      .mSDLWindow = window,
  };
}

ShaderHandle GLContext::CreateShaderFromBinary(const char *vBinary, const char *fBinary)
{
  return {};
}

ShaderHandle GLContext::CreateShaderFromSource(const char *name, const std::string &vSource, const std::string &fSource)
{
  return ShaderManager::AddShader(name, vSource, fSource);
}

ShaderHandle GLContext::CreateComputeShaderFromSource(const char *name, const std::string &source)
{
  return ShaderManager::AddComputeShader(name, source);
}

VertexBufferHandle GLContext::CreateVertexBuffer(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor)
{
  return mVBManager.Create(data, sizeInBytes, descriptor);
}

void GLContext::DestroyVertexBuffer(VertexBufferHandle handle)
{
  mVBManager.Destroy(handle);
}

IndexBufferHandle GLContext::CreateIndexBuffer(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor)
{
  return mIBManager.Create(data, sizeInBytes, descriptor);
}

BufferHandle GLContext::CreateShaderBuffer(void *data, u32 sizeInBytes, ShaderBufferDescriptor descriptor)
{
  return mSBManager.Create(data, sizeInBytes, descriptor);
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
void GLContext::UpdateShaderBuffer(BufferHandle handle, void *data, u32 size)
{

  assert(0);
}
std::vector<u8> GLContext::ReadShaderBuffer(BufferHandle handle)
{
  throw std::logic_error("The method or operation is not implemented.");
}

ConstantBufferHandle GLContext::CreateConstantBuffer(void *data, u32 sizeInBytes, ConstantBufferDescriptor descriptor)
{
  return mCBManager.Create(data, sizeInBytes, descriptor);
}

void *GLContext::MapBufferPtr(BufferHandle handle, AccessMode accessMode)
{
  return glMapNamedBuffer(mSBManager.Get(handle), glUtils::AccessModeToGL(accessMode));
}
void GLContext::UnmapBufferPtr(BufferHandle handle)
{
  glUnmapNamedBuffer(mSBManager.Get(handle));
}

void GLContext::DestroyIndexBuffer(IndexBufferHandle handle)
{
  mIBManager.Destroy(handle);
}

void GLContext::DestroyShaderBuffer(BufferHandle handle)
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
  if (renderState.depthTest != mCachedRenderState.depthTest) {
    if (renderState.depthTest.mEnabled) {
      glEnable(GL_DEPTH_TEST);
      GLenum compFunc = glUtils::ComparisonFunctionToGL(renderState.depthTest.mFunction);
      glDepthFunc(compFunc);
      glDepthMask(renderState.depthTest.mWriteToDepthBuffer);
    } else {
      glDisable(GL_DEPTH_TEST);
    }
  }
  if (renderState.cullState != mCachedRenderState.cullState) {
    if (renderState.cullState.mEnabled) {
      glEnable(GL_CULL_FACE);
      GLenum cullFace = glUtils::TriangleFaceToGL(renderState.cullState.mFace);
      glCullFace(cullFace);
      GLenum frontFace = glUtils::WindingOrderToGL(renderState.cullState.mFrontFace);
      glFrontFace(frontFace);
    } else {
      glDisable(GL_CULL_FACE);
    }
  }
  if (renderState.rasterizationMode != mCachedRenderState.rasterizationMode) {
    GLenum rasterizationMode = glUtils::RasterizationModeToGL(renderState.rasterizationMode);
    // TODO: add the face and rasterization mode to a struct?
    glPolygonMode(GL_FRONT_AND_BACK, rasterizationMode);
  }
  if (renderState.stencilTest != mCachedRenderState.stencilTest) {
    if (renderState.stencilTest.mEnabled) {
      glEnable(GL_STENCIL_TEST);
    } else {
      glDisable(GL_STENCIL_TEST);
    }
  }
  if (renderState.depthRange != mCachedRenderState.depthRange) {
    glDepthRange(renderState.depthRange.mNear, renderState.depthRange.mNear);
  }
  if (renderState.blendState != mCachedRenderState.blendState) {
    if (renderState.blendState.mEnabled) {
      glEnable(GL_BLEND);
    } else {
      glDisable(GL_BLEND);
    }
  }
  auto ibDesc = mIBManager.mDescriptors[sceneState.ibHandle];
  auto shaderInfo = ShaderManager::GetInfo(shader);
  u32 program = ShaderManager::GetProgram(shader);
  glUseProgram(program);
  glBindVertexArray(mVAO);
  // TODO: for now I'll assume that there is either one interleaved vbo or multiple single-type buffers
  for (const auto &vbHandle : sceneState.vbHandles) {
    const auto &vbDesc = mVBManager.mDescriptors[vbHandle];
    u32 glVBHandle = mVBManager.Get(vbHandle);
    if (vbDesc.bufferType == BufferType::SingleType) {
      const auto &inputDesc = shaderInfo.mInputBufferDescriptors[vbDesc.inputDescriptorName[0]];
      glBindBuffer(GL_ARRAY_BUFFER, glVBHandle);
      glEnableVertexAttribArray(inputDesc.slot);

      // glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (const void *)0);
      glVertexAttribPointer(inputDesc.slot, glUtils::VarTypeToSlotSizeGL(vbDesc.types[0]),
          glUtils::VarTypeToIndividualTypeGL(inputDesc.type), false, 0, (const void *)0);
    } else if (vbDesc.bufferType == BufferType::InterLeaved) {
      glBindBuffer(GL_ARRAY_BUFFER, glVBHandle);
      for (u32 i = 0; i < shaderInfo.mInputBufferDescriptors.size(); i++) {
        const auto &inputDesc = shaderInfo.mInputBufferDescriptors[vbDesc.inputDescriptorName[i]];
        glEnableVertexAttribArray(inputDesc.slot);

        // glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (const void *)0);
        glVertexAttribPointer(inputDesc.slot, glUtils::VarTypeToSlotSizeGL(vbDesc.types[i]),
            glUtils::VarTypeToIndividualTypeGL(inputDesc.type), false, vbDesc.elementSizeInBytes, (const void *)inputDesc.byteOffset);
      }
    } else {
      assert(0);
    }
  }
  // setup all the uniform buffers
  for (const auto &cbHandle : sceneState.cbHandles) {
    const auto &cbDesc = mCBManager.mDescriptors[cbHandle];
    u32 glCBHandle = mCBManager.Get(cbHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, glCBHandle);
    // TODO: figure out multiple binding points
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, glCBHandle);
    glUniformBlockBinding(program, cbDesc.slot, 0);
  }
  if (sceneState.indexed) {
    u32 glIBHandle = mIBManager.Get(sceneState.ibHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIBHandle);
    glDrawElements(glPrimitive, ibDesc.sizeInBytes / 4, GL_UNSIGNED_INT, (void *)0);
  } else {
    glDrawArrays(glPrimitive, 0, (mVBManager.mDescriptors[sceneState.vbHandles[0]].sizeInBytes / mVBManager.mDescriptors[sceneState.vbHandles[0]].elementSizeInBytes) / 3);
  }
}

void GLContext::Clear(ClearState clearState)
{
  glClearColor(
      clearState.clearColor.red, clearState.clearColor.green, clearState.clearColor.blue, clearState.clearColor.alpha);
  glClear(glUtils::ClearBufferToGL(clearState.toClear));
}

void GLContext::DispatchCompute(
    u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, const ComputeState &computeState)
{
  u32 program = ShaderManager::GetProgram(shader);
  glUseProgram(program);
  for (auto bufferHandle : computeState.bufferHandles) {
    const auto &bufferDesc = mSBManager.mDescriptors[bufferHandle];
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSBManager.Get(bufferHandle));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bufferDesc.slot, mSBManager.Get(bufferHandle));
  }
  for (const auto &cbHandle : computeState.cbHandles) {
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
  SDL_GL_SwapWindow(window.mSDLWindow);
}


} // namespace focus
