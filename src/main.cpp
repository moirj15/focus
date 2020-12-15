//#include "renderer/vk/TriangleApp.h"
//#include "renderer/vk/vkCore.hpp"
//#include "renderer/dx11/Context.hpp"
#include "renderer/Interface/Context.hpp"
#include "utils/FileUtils.hpp"

#include <filesystem>

#ifdef _WIN32
#include <Windows.h>
LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_CLOSE:
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hWnd, msg, wParam, lParam);
}

void TriangleTest(const focus::Window &window)
{
  char path[256];
  GetModuleFileName(GetModuleHandle(nullptr), path, sizeof(path));
  printf("%s\n", path);

  auto handle = focus::gContext->CreateShaderFromSource("UniformColor",
      utils::ReadEntireFileAsString("shaders/gl/UniformColor.vert"),
      utils::ReadEntireFileAsString("shaders/gl/UniformColor.frag"));

  f32 points[] = {
      0.0f,
      1.0f,
      0.0f,
      -1.0f,
      -1.0f,
      0.0f,
      1.0f,
      -1.0f,
      0.0f,
  };

  u32 indices[] = {0, 1, 2};

  focus::VertexBufferDescriptor vbDescriptor = {
      .inputDescriptorName = "aPosition",
      .bufferType = focus::BufferType::SingleType,
      .type = focus::VarType::Float,
      .sizeInBytes = sizeof(points),
  };

  focus::IndexBufferDescriptor ibDescriptor = {
      .type = focus::IndexBufferType::U32,
      .sizeInBytes = sizeof(indices),
  };

  focus::SceneState sceneState = {
      .vbHandles = {focus::gContext->CreateVertexBuffer(points, sizeof(points), vbDescriptor)},
      .ibHandle = focus::gContext->CreateIndexBuffer(indices, sizeof(indices), ibDescriptor),
      .indexed = true,
  };

  MSG msg = {};
  auto dc = GetDC(window.mWindowHandle);
  while (msg.message != WM_QUIT) {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    focus::gContext->Clear({});

    focus::gContext->Draw(focus::Primitive::Triangles, {}, handle, sceneState);

    SwapBuffers(dc);
  }
}

void ComputeTest(const focus::Window &window)
{

  auto handle = focus::gContext->CreateComputeShaderFromSource(
      "TestCompute", utils::ReadEntireFileAsString("shaders/gl/test.comp"));

  focus::ShaderBufferDescriptor sDesc = {
    .mName = "color_buf",
    .mSlot = 0
  };
  auto sHandle = focus::gContext->CreateShaderBuffer(nullptr, 4 * sizeof(float) * 256 * 256, sDesc);

  MSG msg = {};
  auto dc = GetDC(window.mWindowHandle);
  while (msg.message != WM_QUIT) {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    focus::gContext->Clear({});
    focus::gContext->DispatchCompute(256, 256, 1, sHandle, {{sHandle}, {}});
    focus::gContext->WaitForMemory(0);
    float *contents = (float*)focus::gContext->GetBufferPtr(sHandle, focus::AccessMode::ReadOnly);

    SwapBuffers(dc);
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
  // TODO: maybe switch to an ECS type system and have a window as a component?
  focus::Context::Init(focus::RendererAPI::OpenGL, MessageHandler, hInstance);
  auto window = focus::gContext->MakeWindow(1920, 1080);

  //TriangleTest(window);
  ComputeTest(window);
}
#else

#include <GLFW/glfw3.h>
#include <cstdio>
int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  printf("entere\n");
  focus::context::Init();
  auto window = focus::context::MakeWindow(1920, 1080);
  printf("made window\n");
  while (!glfwWindowShouldClose(window.mGLFWWindow)) {
    glfwPollEvents();
  }
}

#endif
