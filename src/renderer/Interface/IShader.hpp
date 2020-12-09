#pragma once

#include "Types.hpp"
#include <vector>
#include <unordered_map>
#include <string>

namespace renderer
{

struct ConstantBufferDescriptor {
  std::string mName;
  std::vector<VarType> mTypes; // TODO: consider if this is necessary
  u32 mSlot;
};

using ShaderBufferDescriptor = ConstantBufferDescriptor;

struct InputBufferDescriptor {
  std::string mName; // name of the variable, not the semantic used.
  VarType mType;
  u32 mSlot;
  u32 mByteOffset;
};


struct ShaderInfo {
  // TODO: maybe use the position in the shader for the key?
  std::unordered_map<std::string, ConstantBufferDescriptor> mConstantBufferDescriptors;
  std::unordered_map<std::string, ShaderBufferDescriptor> mShaderBufferDescriptors;
  std::unordered_map<std::string, InputBufferDescriptor> mInputBufferDescriptors;
};

}
