#pragma once

#include "../../common.h"

struct GLFWwindow;

namespace renderer
{

struct Window {
  s32 mWidth;
  s32 mHeight;
#ifdef _WIN32
  HWND mWindowHandle;
#else
  GLFWwindow *mGLFWWindow;
#endif
};
} // namespace renderer
