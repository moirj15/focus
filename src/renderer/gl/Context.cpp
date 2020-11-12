#include "Context.hpp"

#include "BufferManager.hpp"
#include "ShaderManager.hpp"
#include "Utils.hpp"
#include "glad.h"

#include <GLFW/glfw3.h>

#ifdef _WIN32
#include <GL/gl.h>
#include <Windows.h>
#include <gl/glext.h>
#include <gl/wglext.h>
#include <winuser.h>
#endif

namespace renderer::gl::context
{
#ifdef _WIN32
static WNDPROC sMessageHandler;
static HINSTANCE sInstanceHandle;
static BufferManager<VertexBufferHandle, VertexBufferDescriptor> sVBManager;
static BufferManager<IndexBufferHandle, IndexBufferDescriptor> sIBManager;
static BufferManager<ConstantBufferHandle, ConstantBufferDescriptor> sCBManager;
static u32 sVAO;

static void DBCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message, void const *userParam) 
{
  printf("DBG: %s\n", message);
}

void Init(WNDPROC messageHandler, HINSTANCE instanceHandle)
{
  sMessageHandler = messageHandler;
  sInstanceHandle = instanceHandle;
}

Window MakeWindow(s32 width, s32 height)
{

  WNDCLASS windowClass = {
      .style = CS_HREDRAW | CS_VREDRAW,
      .lpfnWndProc = sMessageHandler,
      .hInstance = sInstanceHandle,
      .hIcon = LoadIcon(0, IDI_APPLICATION),
      .hCursor = LoadCursor(0, IDC_ARROW),
      .hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH),
      .lpszClassName = "Core",
  };
  assert(RegisterClass(&windowClass) != 0);

  HWND fakeWindow = CreateWindow(
      "Core", "Fake Window", WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 1, 1, nullptr, nullptr, sInstanceHandle, nullptr);
  HDC fakeDeviceContext = GetDC(fakeWindow);
  PIXELFORMATDESCRIPTOR fakePFD = {
      .nSize = sizeof(fakePFD),
      .nVersion = 1,
      .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
      .iPixelType = PFD_TYPE_RGBA,
      .cColorBits = 32,
      .cAlphaBits = 8,
      .cDepthBits = 24,
  };
  s32 fakePFDID = ChoosePixelFormat(fakeDeviceContext, &fakePFD);
  assert(fakePFDID != 0);
  assert(SetPixelFormat(fakeDeviceContext, fakePFDID, &fakePFD) != false);

  auto fakeRenderContext = wglCreateContext(fakeDeviceContext);
  assert(fakeRenderContext != 0);
  assert(wglMakeCurrent(fakeDeviceContext, fakeRenderContext) != false);

  auto wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
  assert(wglChoosePixelFormatARB != nullptr);

  auto wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
  assert(wglCreateContextAttribsARB != nullptr);
  HWND windowHandle = CreateWindow("Core", "OpenGL Window", WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
      0, 0, width, height, nullptr, nullptr, sInstanceHandle, nullptr);
  HDC deviceContext = GetDC(windowHandle);

  const s32 pixelAttributes[] = {
      WGL_DRAW_TO_WINDOW_ARB,
      GL_TRUE,
      WGL_SUPPORT_OPENGL_ARB,
      GL_TRUE,
      WGL_DOUBLE_BUFFER_ARB,
      GL_TRUE,
      WGL_PIXEL_TYPE_ARB,
      WGL_TYPE_RGBA_ARB,
      WGL_ACCELERATION_ARB,
      WGL_FULL_ACCELERATION_ARB,
      WGL_COLOR_BITS_ARB,
      32,
      WGL_ALPHA_BITS_ARB,
      8,
      WGL_DEPTH_BITS_ARB,
      24,
      WGL_STENCIL_BITS_ARB,
      8,
      WGL_SAMPLE_BUFFERS_ARB,
      GL_TRUE,
      WGL_SAMPLES_ARB,
      4,
      0,
  };
  s32 pixelFormatID = 0;
  u32 numFormats = 0;
  bool status = wglChoosePixelFormatARB(deviceContext, pixelAttributes, nullptr, 1, &pixelFormatID, &numFormats);
  assert(status != false);
  assert(numFormats > 0);

  PIXELFORMATDESCRIPTOR pfd;
  DescribePixelFormat(deviceContext, pixelFormatID, sizeof(pfd), &pfd);
  SetPixelFormat(deviceContext, pixelFormatID, &pfd);

  const int major_min = 4, minor_min = 6;
  int contextAttribs[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB,
      major_min,
      WGL_CONTEXT_MINOR_VERSION_ARB,
      minor_min,
      WGL_CONTEXT_PROFILE_MASK_ARB,
      WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      0,
  };

  HGLRC renderContext = wglCreateContextAttribsARB(deviceContext, 0, contextAttribs);
  assert(renderContext != nullptr);

  wglMakeCurrent(nullptr, nullptr);
  wglDeleteContext(fakeRenderContext);
  ReleaseDC(fakeWindow, fakeDeviceContext);

  DestroyWindow(fakeWindow);
  assert(wglMakeCurrent(deviceContext, renderContext));

  if (!gladLoadGL()) {
    fprintf(stderr, "Failed to load glad\n");
    exit(EXIT_FAILURE);
  }

  SetWindowText(windowHandle, (LPCSTR)glGetString(GL_VERSION));
  ShowWindow(windowHandle, true);

#ifdef _DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(DBCallBack, nullptr);
#endif

  glGenVertexArrays(1, &sVAO);

  return {
      .mWidth = width,
      .mHeight = height,
      .mWindowHandle = windowHandle,
  };
}
#else
void Init()
{
  printf("Initializing OpenGL Renderer");
  assert(glfwInit());
}

Window MakeWindow(s32 width, s32 height)
{
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
  return {
      .mWidth = width,
      .mHeight = height,
      .mGLFWWindow = glfwWindow,
  };
}
#endif

renderer::ShaderHandle CreateShaderFromSource(const char *name, const std::string &vSource, const std::string &fSource)
{
  return ShaderManager::AddShader(name, vSource, fSource);
}

VertexBufferHandle CreateVertexBuffer(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor)
{
  return sVBManager.Create(data, sizeInBytes, descriptor);
}

void DestroyVertexBuffer(VertexBufferHandle handle)
{
  sVBManager.Destroy(handle);
}

IndexBufferHandle CreateIndexBuffer(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor)
{
  return sIBManager.Create(data, sizeInBytes, descriptor);
}

void DestroyIndexBuffer(IndexBufferHandle handle)
{
  sIBManager.Destroy(handle);
}

// TODO: do automatic batching
static RenderState sCachedRenderState;

void Draw(Primitive primitive, RenderState renderState, ShaderHandle shader, SceneState sceneState)
{
  GLenum glPrimitive = utils::PrimitiveToGL(primitive);
  // TODO: sort based on bit flags?
  if (renderState.depthTest != sCachedRenderState.depthTest) {
    if (renderState.depthTest.mEnabled) {
      glEnable(GL_DEPTH_TEST);
      GLenum compFunc = utils::ComparisonFunctionToGL(renderState.depthTest.mFunction);
      glDepthFunc(compFunc);
      glDepthMask(renderState.depthTest.mWriteToDepthBuffer);
    } else {
      glDisable(GL_DEPTH_TEST);
    }
  }
  if (renderState.cullState != sCachedRenderState.cullState) {
    if (renderState.cullState.mEnabled) {
      glEnable(GL_CULL_FACE);
      GLenum cullFace = utils::TriangleFaceToGL(renderState.cullState.mFace);
      glCullFace(cullFace);
      GLenum frontFace = utils::WindingOrderToGL(renderState.cullState.mFrontFace);
      glFrontFace(frontFace);
    } else {
      glDisable(GL_CULL_FACE);
    }
  }
  if (renderState.rasterizationMode != sCachedRenderState.rasterizationMode) {
    GLenum rasterizationMode = utils::RasterizationModeToGL(renderState.rasterizationMode);
    // TODO: add the face and rasterization mode to a struct?
    glPolygonMode(GL_FRONT_AND_BACK, rasterizationMode);
  }
  if (renderState.stencilTest != sCachedRenderState.stencilTest) {
    if (renderState.stencilTest.mEnabled) {
      glEnable(GL_STENCIL_TEST);
    } else {
      glDisable(GL_STENCIL_TEST);
    }
  }
  if (renderState.depthRange != sCachedRenderState.depthRange) {
    glDepthRange(renderState.depthRange.mNear, renderState.depthRange.mNear);
  }
  if (renderState.blendState != sCachedRenderState.blendState) {
    if (renderState.blendState.mEnabled) {
      glEnable(GL_BLEND);
    } else {
      glDisable(GL_BLEND);
    }
  }
  auto ibDesc = sIBManager.mDescriptors[sceneState.ibHandle];
  auto shaderInfo = ShaderManager::GetInfo(shader);
  u32 program = ShaderManager::GetProgram(shader);
  glUseProgram(program);
  glBindVertexArray(sVAO);
  for (const auto &vbHandle : sceneState.vbHandles) {
    const auto &vbDesc = sVBManager.mDescriptors[vbHandle];
    const auto &inputDesc = shaderInfo.mInputBufferDescriptors[vbDesc.inputDescriptorName];
    u32 glVBHandle = sVBManager.Get(vbHandle);
    glBindBuffer(GL_ARRAY_BUFFER, glVBHandle);
    glEnableVertexAttribArray(inputDesc.mSlot);

    // glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (const void *)0);
    glVertexAttribPointer(inputDesc.mSlot, utils::VarTypeToSlotSizeGL(inputDesc.mType),
        utils::VarTypeToIndividualTypeGL(inputDesc.mType), false, 0, (const void *)0);
  }
  if (sceneState.indexed) {
    u32 glIBHandle = sIBManager.Get(sceneState.ibHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIBHandle);
    glDrawElements(GL_TRIANGLES, ibDesc.sizeInBytes / 4, GL_UNSIGNED_INT, (void *)0);
  } else {
    // TODO
  }
}

void Clear(ClearState clearState)
{
  glClearColor(
      clearState.clearColor.red, clearState.clearColor.green, clearState.clearColor.blue, clearState.clearColor.alpha);
  glClear(utils::ClearBufferToGL(clearState.toClear));
}

} // namespace renderer::gl::context
