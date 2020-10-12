//#include "renderer/vk/TriangleApp.h"
//#include "renderer/vk/vkCore.hpp"
//#include "renderer/dx11/Context.hpp"
#include "renderer/Interface/Context.hpp"

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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
  // TriangleApp triangleApp;
  ////triangleApp.Run();
  // vk::Core core(true);
  // while (!glfwWindowShouldClose(core.mWindow)) {
  //  glfwPollEvents();
  //}
  // return 0;
  renderer::context::Init(MessageHandler, hInstance);
  auto window = renderer::context::MakeWindow(1920, 1080);

  MSG msg = {0};
  while (msg.message != WM_QUIT) {
    if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}
#else

#include <GLFW/glfw3.h>
#include <cstdio>
int main(int argc, char **argv)
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
