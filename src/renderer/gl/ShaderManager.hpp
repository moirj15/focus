#pragma once

#include "../Interface/Context.hpp"
#include "glad.h"

#include <string>
#include <unordered_map>

namespace focus
{

class ShaderManager
{
  inline static ShaderHandle sCurrHandle{0};
  std::unordered_map<std::string, ShaderHandle> mNameToShaderHandles;
  std::unordered_map<ShaderHandle, u32> mShaderHandles;
  std::unordered_map<ShaderHandle, ShaderInfo> mShaderInfos;

public:
  ShaderHandle AddShader(const char *name, const std::string &vSource, const std::string &fSource);

  ShaderHandle AddComputeShader(const char *name, const std::string &source);

  ShaderHandle GetShader(const std::string &name);

  ShaderInfo GetInfo(ShaderHandle handle);

  u32 GetProgram(ShaderHandle handle);

  void DeleteShader(ShaderHandle handle);

private:
  void CompileShader(u32 handle, const std::string &source, const std::string &type);
  void LinkShaderProgram(std::vector<u32> shaderHandles, u32 programHandle);
  const char *ShaderTypeToString(GLenum type);
  std::unordered_map<std::string, InputBufferDescriptor> GetInputBufferDescriptors(u32 program);
  ShaderHandle AddShader(const char *name, const std::vector<std::string> &sources, const std::vector<u32> &types);
};

} // namespace focus
