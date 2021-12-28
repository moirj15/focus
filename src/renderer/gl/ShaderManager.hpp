#pragma once

#include "../Interface/focus.hpp"
#include "glad.h"

#include <string>
#include <unordered_map>

namespace focus
{

class ShaderManager
{
  Shader sCurrHandle{0};
  std::unordered_map<std::string, Shader> mNameToShaderHandles;
  std::unordered_map<Shader, GLuint> mShaderHandles;

public:
  Shader AddShader(const char *name, const std::string &vSource, const std::string &fSource);

  Shader AddComputeShader(const char *name, const std::string &source);

  GLuint GetProgram(Shader handle);

  void DeleteShader(Shader handle);

private:
  void CompileShader(GLuint handle, const std::string &source, const std::string &type);
  void LinkShaderProgram(std::vector<GLuint> shaderHandles, GLuint programHandle);
  const char *ShaderTypeToString(GLenum type);
  Shader AddShader(const char *name, const std::vector<std::string> &sources, const std::vector<uint32_t> &types);
};

} // namespace focus
