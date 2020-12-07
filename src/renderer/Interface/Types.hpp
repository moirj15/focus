#pragma once

#include <stdint.h>
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

namespace renderer
{

enum class VarType {
  Float,
  Vec2,
  Vec3,
  Vec4,
  Mat2,
  Mat3,
  Mat4,
  Sampler1D,
  Sampler2D,
  Sampler3D,

  Invalid
};

enum class PrecisionType {
  Half,
  Single,
  Double,
};

enum class IndexType {
  U8,
  U16,
  U32,
};

} // namespace renderer
