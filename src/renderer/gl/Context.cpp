#include "Context.hpp"

#include "BufferManager.hpp"
#include <GLFW/glfw3.h>
#include "glad.h"

#ifdef WIN32
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

static constexpr VarType AttribTypeToVarType(GLenum attribType)
{
  switch (attribType) {
  case GL_FLOAT:
    return VarType::Float;
  case GL_FLOAT_VEC2:
    return VarType::Vec2;
  case GL_FLOAT_VEC3:
    return VarType::Vec3;
  case GL_FLOAT_VEC4:
    return VarType::Vec4;
  case GL_FLOAT_MAT2:
    return VarType::Mat2;
  case GL_FLOAT_MAT3:
    return VarType::Mat3;
  case GL_FLOAT_MAT4:
    return VarType::Mat4;
  }
}

ShaderHandle CreateShaderFromSource(const char *vSource, const char *fSource)
{

#if 0
  s32 vSourceLen = strlen(vSource);
  s32 fSourceLen = strlen(fSource);

  u32 vHandle = glCreateShader(GL_VERTEX_SHADER);
  u32 fHandle = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(vHandle, 1, &vSource, &vSourceLen);
  glShaderSource(fHandle, 1, &fSource, &fSourceLen);

  glCompileShader(vHandle);

  s32 success = 0;
  char compileLog[512] = {};
  glGetShaderiv(vHandle, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vHandle, 512, nullptr, compileLog);
    printf("Vertex Shader Compile Error: %s\n", compileLog);
    assert(0);
  }

  glCompileShader(fHandle);
  glGetShaderiv(fHandle, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fHandle, 512, nullptr, compileLog);
    printf("Fragment Shader Compile Error: %s\n", compileLog);
    assert(0);
  }

  u32 handle = glCreateProgram();
  glAttachShader(handle, vHandle);
  glAttachShader(handle, fHandle);
  glLinkProgram(handle);

  glGetProgramiv(handle, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(handle, sizeof(compileLog), nullptr, compileLog);
    printf("Shader Link Error: %s\n", compileLog);
  }

  glDeleteShader(vHandle);
  glDeleteShader(fHandle);

  s32 attributeCount = 0;
  glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &attributeCount);
  std::vector<VertexAttribute> attributes(attributeCount);
  char attributeName[64] = {};
  s32 attribNameLen;
  s32 attribSize;
  GLenum attribType;
  for (s32 i = 0; i < attributeCount; i++) {
    glGetActiveAttrib(handle, i, sizeof(attributeName), &attribNameLen, &attribSize, &attribType, attributeName);
    attributes[i] = {
        .mName = attributeName,
        .mLocation = (u32)i,
        .mType = AttribTypeToVarType(attribType),
    };
  }
#endif

  // TODO
  return INVALID_HANDLE;
}

VertexBufferHandle CreateVertexBuffer(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor)
{
  return VertexBufferManager::Create(data, sizeInBytes, descriptor);
}

void DestroyVertexBuffer(VertexBufferHandle handle)
{
  VertexBufferManager::Destroy(handle);
}

IndexBufferHandle CreateIndexBuffer(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor)
{
  return IndexBufferManager::Create(data, sizeInBytes, descriptor);
}

void DestroyIndexBuffer(IndexBufferHandle handle)
{
  IndexBufferManager::Destroy(handle);
}

} // namespace renderer::gl::context
