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

void TriangleTest(const renderer::Window& window)
{
  char path[256];
  GetModuleFileName(GetModuleHandle(nullptr), path, sizeof(path));
  printf("%s\n", path);

  auto handle = renderer::context::CreateShaderFromSource("UniformColor",
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

  u32 indices[] = { 0, 1, 2 };

  renderer::VertexBufferDescriptor vbDescriptor = {
      .inputDescriptorName = "aPosition",
      .bufferType = renderer::BufferType::SingleType,
      .type = renderer::VarType::Float,
      .sizeInBytes = sizeof(points),
  };

  renderer::IndexBufferDescriptor ibDescriptor = {
      .type = renderer::IndexBufferType::U32,
      .sizeInBytes = sizeof(indices),
  };

  renderer::SceneState sceneState = {
      .vbHandles = {renderer::context::CreateVertexBuffer(points, sizeof(points), vbDescriptor)},
      .ibHandle = renderer::context::CreateIndexBuffer(indices, sizeof(indices), ibDescriptor),
      .indexed = true,
  };

  MSG msg = {};
  auto dc = GetDC(window.mWindowHandle);
  while (msg.message != WM_QUIT) {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    renderer::context::Clear({});

    renderer::context::Draw(renderer::Primitive::Triangles, {}, handle, sceneState);

    SwapBuffers(dc);
  }
}

void ComputeTest(const renderer::Window& window)
{

  auto handle = renderer::context::CreateComputeShaderFromSource("TestCompute", utils::ReadEntireFileAsString("shaders/gl/test.comp"));
  MSG msg = {};
  auto dc = GetDC(window.mWindowHandle);
  while (msg.message != WM_QUIT) {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    renderer::context::Clear({});

    SwapBuffers(dc);
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
  // TODO: maybe switch to an ECS type system and have a window as a component?
  renderer::context::Init(MessageHandler, hInstance);
  auto window = renderer::context::MakeWindow(1920, 1080);

  TriangleTest(window);
  ComputeTest(window);

}
#else

#include <GLFW/glfw3.h>
#include <cstdio>
int main(int argc, char** argv)
{
  (void)argc;
  (void)argv;
  printf("entere\n");
  renderer::context::Init();
  auto window = renderer::context::MakeWindow(1920, 1080);
  printf("made window\n");
  while (!glfwWindowShouldClose(window.mGLFWWindow)) {
    glfwPollEvents();
  }
}

#endif
