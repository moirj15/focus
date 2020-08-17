#pragma once
#include <cassert>
#include <filesystem>
#include <fmt/core.h>
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

#define ArraySize(X) (sizeof(X) / sizeof(X[0]))
#define NODISCARD    [[nodiscard]]
#define IMPLEMENTME()                                                                                                  \
  printf("IMPLEMENT %s %s %d\n", __FUNCTION__, __FILE__, __LINE__);                                                    \
  assert(0);

#define passert(MESSAGE, EVAL)                                                                                         \
  if (!EVAL) {                                                                                                         \
    fmt::print(MESSAGE);                                                                                               \
  }                                                                                                                    \
  assert(EVAL);

inline FILE *OpenFile(const char *file, const char *perm)
{
  FILE *ret = NULL;
  ret = fopen(file, perm);
  if (!ret) {
    printf("FAILED TO OPEN FILE: %s\n", file);
    exit(EXIT_FAILURE);
  }
  return ret;
}

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
