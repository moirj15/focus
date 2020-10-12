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

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef float f32;
typedef double f64;
typedef size_t Size;

namespace fs = std::filesystem;

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


// inline std::tuple<char *, Size> ReadFile(const char *filename)
// {
//   FILE *fp = OpenFile(filename, "r");
//   char *data = nullptr;
//   Size len = 0;
//
//   fseek(fp, 0, SEEK_END);
//   len = ftell(fp);
//   rewind(fp);
//
//   if (len == 0)
//   {
//     printf("failed to get file size");
//   }
//
//   data = new char[len + 1]();
//   len = fread(data, sizeof(char), len, fp);
//
//   fclose(fp);
//
//   return {data, len};
// }
