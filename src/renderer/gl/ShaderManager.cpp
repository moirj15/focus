#include "ShaderManager.hpp"

#include "../Interface/IShader.hpp"
#include "Utils.hpp"
#include "glad.h"

#include <unordered_map>

namespace renderer::gl::ShaderManager
{

static ShaderHandle sCurrHandle;
std::unordered_map<std::string, ShaderHandle> sNameToShaderHandles;
std::unordered_map<ShaderHandle, u32> sShaderHandles;
std::unordered_map<ShaderHandle, ShaderInfo> sShaderInfos;

static void CompileShader(u32 handle, const std::string &source, const std::string &type)
{
  s32 sourceLen = source.size();
  auto shaderSource = source.c_str();
  glShaderSource(handle, 1, &shaderSource, &sourceLen);
  glCompileShader(handle);
  s32 success = 0;
  char compileLog[512] = {};
  glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(handle, 512, nullptr, compileLog);
    printf("%s Shader Compile Error: %s\n", type.c_str(), compileLog);
    assert(0);
  }
  
}

static void LinkShaderProgram(std::vector<u32> shaderHandles, u32 programHandle)
{
  for (u32 sHandle : shaderHandles) {
    glAttachShader(programHandle, sHandle);
  }
  glLinkProgram(programHandle);

  s32 success = 0;
  char compileLog[512] = {};
  glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(programHandle, sizeof(compileLog), nullptr, compileLog);
    printf("Shader Link Error: %s\n", compileLog);
  }
  for (u32 sHandle : shaderHandles) {
    glDeleteShader(sHandle);
  }
}

renderer::ShaderHandle AddShader(const char *name, const std::string &vSource, const std::string &fSource)
{
  u32 vHandle = glCreateShader(GL_VERTEX_SHADER);
  u32 fHandle = glCreateShader(GL_FRAGMENT_SHADER);

  CompileShader(vHandle, vSource, "Vertex");
  CompileShader(fHandle, fSource, "Fragment");

  #if 0
  auto vsh = vSource.c_str();
  auto fsh = fSource.c_str();

  glShaderSource(vHandle, 1, &vsh, &vSourceLen);
  glShaderSource(fHandle, 1, &fsh, &fSourceLen);

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
  #endif

  u32 handle = glCreateProgram();
  LinkShaderProgram({vHandle, fHandle}, handle);

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
        .mType = utils::GLTypeToVarType(attribType),
        .mSlot = (u32)i,
        .mByteOffset = (u32)attribSize, // TODO: might not be correct
    };
    shaderInfo.mInputBufferDescriptors.emplace(attributeName, descriptor);
  }
  s32 uniformBlockCount = 0;
  glGetProgramiv(handle, GL_ACTIVE_UNIFORM_BLOCKS, &uniformBlockCount);
  char uniformBlockName[256] = {};
  s32 uniformBlockNameLength = 0;
  for (s32 i = 0; i < uniformBlockCount; i++) {
    glGetActiveUniformBlockName(handle, i, sizeof(uniformBlockName), &uniformBlockNameLength, uniformBlockName);
    ConstantBuffer constantBuffer = {
        .mName = uniformBlockName,
        .mSlot = (u32)i,
    };
  }

  sCurrHandle++;
  sShaderHandles[sCurrHandle] = handle;
  sShaderInfos[sCurrHandle] = shaderInfo;
  sNameToShaderHandles[name] = handle;
  return sCurrHandle;
}

ShaderHandle GetShaderHandle(const std::string &name)
{
  return sNameToShaderHandles[name];
}

ShaderInfo GetInfo(ShaderHandle handle)
{
  return sShaderInfos[handle];
}

u32 GetProgram(ShaderHandle handle)
{
  return sShaderHandles[handle];
}

void DeleteShader(ShaderHandle handle)
{
  glDeleteShader(sShaderHandles[handle]);
  sShaderHandles.erase(handle);
  sShaderInfos.erase(handle);
}

} // namespace renderer::gl::ShaderManager
