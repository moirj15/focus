#pragma once

#include "../Interface/FocusBackend.hpp"
#include "glad.h"

#include <string>
#include <unordered_map>

namespace focus
{

class ShaderManager
{
  inline static Shader sCurrHandle{0};
  std::unordered_map<std::string, Shader> mNameToShaderHandles;
  std::unordered_map<Shader, GLuint> mShaderHandles;
  std::unordered_map<Shader, ShaderInfo> mShaderInfos;

public:
  Shader AddShader(const char *name, const std::string &vSource, const std::string &fSource);

  Shader AddComputeShader(const char *name, const std::string &source);

//  Shader GetShader(const std::string &name);

  ShaderInfo GetInfo(Shader handle);

  GLuint GetProgram(Shader handle);

  void DeleteShader(Shader handle);

private:
  void CompileShader(GLuint handle, const std::string &source, const std::string &type);
  void LinkShaderProgram(std::vector<GLuint> shaderHandles, GLuint programHandle);
  const char *ShaderTypeToString(GLenum type);
  std::unordered_map<std::string, InputBufferDescriptor> GetInputBufferDescriptors(GLuint program);
  Shader AddShader(const char *name, const std::vector<std::string> &sources, const std::vector<uint32_t> &types);
};

} // namespace focus
