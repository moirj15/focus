#include "ShaderManager.hpp"

#include "../Interface/IShader.hpp"
#include "Utils.hpp"
#include "glad.h"

#include <cassert>
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

constexpr const char *ShaderTypeToString(GLenum type)
{
  switch (type) {
  case GL_VERTEX_SHADER:
    return "Vertex";
  case GL_TESS_CONTROL_SHADER:
    return "Tesselation Control";
  case GL_TESS_EVALUATION_SHADER:
    return "Tesselation Evaluation";
  case GL_GEOMETRY_SHADER:
    return "Geometry";
  case GL_FRAGMENT_SHADER:
    return "Fragment";
  case GL_COMPUTE_SHADER:
    return "Compute";
  default:
    assert(0 && "Invalid shader type");
  }
}

static std::unordered_map<std::string, ConstantBufferDescriptor> GetConstantBuffers(u32 program)
{
  std::unordered_map<std::string, ConstantBufferDescriptor> descriptors;
  s32 uniformBlockCount = 0;
  glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &uniformBlockCount);
  char uniformBlockName[256] = {};
  s32 uniformBlockNameLength = 0;
  for (s32 i = 0; i < uniformBlockCount; i++) {
    glGetActiveUniformBlockName(program, i, sizeof(uniformBlockName), &uniformBlockNameLength, uniformBlockName);
    ConstantBuffer constantBuffer = {
        .mName = uniformBlockName,
        .mSlot = (u32)i,
    };
    descriptors.emplace(constantBuffer.mName, constantBuffer);
  }
  return descriptors;
}

static std::unordered_map<std::string, InputBufferDescriptor> GetInputBufferDescriptors(u32 program)
{
  std::unordered_map<std::string, InputBufferDescriptor> descriptors;
  s32 attributeCount = 0;
  glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attributeCount);
  char attributeName[64] = {};
  s32 attribNameLen;
  s32 attribSize;
  GLenum attribType;
  for (s32 i = 0; i < attributeCount; i++) {
    glGetActiveAttrib(program, i, sizeof(attributeName), &attribNameLen, &attribSize, &attribType, attributeName);
    InputBufferDescriptor descriptor = {
        .mName = attributeName,
        .mType = utils::GLTypeToVarType(attribType),
        .mSlot = (u32)i,
        .mByteOffset = (u32)attribSize, // TODO: might not be correct
    };
    descriptors.emplace(attributeName, descriptor);
  }
  return descriptors;
}

static ShaderHandle AddShader(const char *name, const std::vector<std::string> &sources, const std::vector<s32> &types)
{
  assert(sources.size() == types.size());
  std::vector<u32> stageHandles;
  for (u32 i = 0; i < sources.size(); i++) {
    stageHandles.emplace_back(glCreateShader(types[i]));
    CompileShader(stageHandles[i], sources[i], ShaderTypeToString(types[i]));
  }
  u32 program = glCreateProgram();
  LinkShaderProgram(stageHandles, program);
  ShaderInfo shaderInfo = {
      .mConstantBuffers = GetConstantBuffers(program),
      .mInputBufferDescriptors = GetInputBufferDescriptors(program),
  };
}

ShaderHandle AddShader(const char *name, const std::string &vSource, const std::string &fSource)
{
  u32 vHandle = glCreateShader(GL_VERTEX_SHADER);
  u32 fHandle = glCreateShader(GL_FRAGMENT_SHADER);

  CompileShader(vHandle, vSource, "Vertex");
  CompileShader(fHandle, fSource, "Fragment");

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

ShaderHandle AddComputeShader(const char *name, const std::string &source)
{
  return AddShader(name, {source}, {GL_COMPUTE_SHADER});
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
