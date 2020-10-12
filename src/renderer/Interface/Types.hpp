#pragma once

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
