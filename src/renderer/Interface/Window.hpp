#pragma once
#if 0
#include "Types.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

struct SDL_Window;

namespace focus
{

struct Window {
  s32 mWidth;
  s32 mHeight;

  SDL_Window *mSDLWindow;
};

} // namespace focus

#endif