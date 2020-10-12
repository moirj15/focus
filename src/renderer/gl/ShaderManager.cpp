#include "ShaderManager.hpp"

#include "../Interface/IShader.hpp"
#include "glad.h"

#include <unordered_map>

namespace renderer::gl::ShaderManager
{

static ShaderHandle sCurrHandle;
std::unordered_map<ShaderHandle, u32> sShaderHandles;
std::unordered_map<ShaderHandle, ShaderInfo> sShaderInfos;

static VarType GLTypeToMyType(GLenum type)
{
  // TODO
}

ShaderHandle AddShader(const std::string &name, const char *vSource, const char *fSource)
{
  s32 vSourceLen = strlen(vSource);
  s32 fSourceLen = strlen(fSource);

  u32 vHandle = glCreateShader(GL_VERTEX_SHADER);
  u32 fHandle = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(vHandle, 1, &vSource, &vSourceLen);
  glShaderSource(fHandle, 1, &fSource, &fSourceLen);

  glCompileShader(vHandle);

  s32 success = 0;
  char compileLog[512] = {};
  glGetShaderiv(vHandle, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vHandle, 512, nullptr, compileLog);
    printf("Vertex Shader Compile Error: %s\n", compileLog);
    assert(0);
  }

  glCompileShader(fHandle);
  glGetShaderiv(fHandle, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fHandle, 512, nullptr, compileLog);
    printf("Fragment Shader Compile Error: %s\n", compileLog);
    assert(0);
  }

  u32 handle = glCreateProgram();
  glAttachShader(handle, vHandle);
  glAttachShader(handle, fHandle);
  glLinkProgram(handle);

  glGetProgramiv(handle, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(handle, sizeof(compileLog), nullptr, compileLog);
    printf("Shader Link Error: %s\n", compileLog);
  }

  glDeleteShader(vHandle);
  glDeleteShader(fHandle);

  ShaderInfo shaderInfo;

  s32 attributeCount = 0;
  glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &attributeCount);
  char attributeName[64] = {};
  s32 attribNameLen;
  s32 attribSize;
  GLenum attribType;
  for (s32 i = 0; i < attributeCount; i++) {
    glGetActiveAttrib(handle, i, sizeof(attributeName), &attribNameLen, &attribSize, &attribType, attributeName);
    InputBufferDescriptor descriptor = {
        .mName = attributeName,
        .mType = GLTypeToMyType(attribType),
        .mSlot = (u32)i,
        .mByteOffset = (u32)attribSize, // TODO: might not be correct
    };

    shaderInfo.mInputBufferDescriptors.emplace(attributeName, descriptor);
  }

  sCurrHandle++;
  sShaderHandles[sCurrHandle] = handle;
  sShaderInfos[sCurrHandle] = shaderInfo;
  return sCurrHandle;
}

ShaderHandle GetShader(const std::string &name)
{
}

void DeleteShader(ShaderHandle handle)
{
  glDeleteShader(sShaderHandles[handle]);
  sShaderHandles.erase(handle);
  sShaderInfos.erase(handle);
}

} // namespace renderer::gl::ShaderManager
