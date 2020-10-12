#pragma once

#include "../../common.h"
#include "Types.hpp"
#include <vector>
#include <unordered_map>
#include <string>

namespace renderer
{

struct ConstantBuffer {
  std::vector<VarType> mTypes;
};

struct InputBufferDescriptor {
  const char *mName; // name of the variable, not the semantic used.
  VarType mType;
  u32 mSlot;
  u32 mByteOffset;
};

struct ShaderInfo {
  // TODO: maybe use the position in the shader for the key?
  std::unordered_map<std::string, ConstantBuffer> mConstantBuffers;
  std::unordered_map<std::string, InputBufferDescriptor> mInputBufferDescriptors;
};

}
