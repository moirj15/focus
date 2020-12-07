#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif
#include <cassert>
#include <filesystem>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tuple>
#include <utility>


#ifdef _WIN32
using WindowsMessageHandler = LRESULT (*)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

#define ArraySize(X) (sizeof(X) / sizeof(X[0]))
#define NODISCARD    [[nodiscard]]
#define IMPLEMENTME()                                                                                                  \
  printf("IMPLEMENT %s %s %d\n", __FUNCTION__, __FILE__, __LINE__);                                                    \
  assert(0);

#define passert(MESSAGE, EVAL)                                                                                         \
  if (!(EVAL)) {                                                                                                       \
    printf(MESSAGE);                                                                                                   \
  }                                                                                                                    \
  assert(EVAL);
