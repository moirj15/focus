#pragma once

#include "../../common.h"
#include "Types.hpp"

#include <unordered_map>
#include <vector>

namespace gl
{

struct VertexAttribute {
  const char *mName;
  u32 mLocation;
  VarType mType;
};

struct UniformBufferObject {
  const char *mBlockName;
  u32 mLocation;
  std::vector<VarType> mTypes;
};

struct Shader {
  u32 mHandle;
  std::unordered_map<std::string_view, VertexAttribute> mAttributes;
  std::unordered_map<std::string_view, UniformBufferObject> mUBOs;
};

} // namespace gl
