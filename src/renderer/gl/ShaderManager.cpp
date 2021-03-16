#include "ShaderManager.hpp"

#include "../Interface/IShader.hpp"
#include "Utils.hpp"
#include "glad.h"

#include <cassert>
#include <unordered_map>

namespace focus::ShaderManager
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
    assert(0);
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
    return "Tessellation Control";
  case GL_TESS_EVALUATION_SHADER:
    return "Tessellation Evaluation";
  case GL_GEOMETRY_SHADER:
    return "Geometry";
  case GL_FRAGMENT_SHADER:
    return "Fragment";
  case GL_COMPUTE_SHADER:
    return "Compute";
  default:
    assert(0 && "Invalid shader type");
    return "";
  }
}

template<typename DescriptorType>
static std::unordered_map<std::string, DescriptorType> GetBuffers(u32 program, GLenum resourceType)
{
  std::unordered_map<std::string, DescriptorType> descriptors;
  s32 blockCount = 0;
  glGetProgramInterfaceiv(program, resourceType, GL_ACTIVE_RESOURCES, &blockCount);
  char blockName[256] = {};
  s32 blockNameLength = 0;
  for (s32 i = 0; i < blockCount; i++) {
    glGetProgramResourceName(program, resourceType, i, sizeof(blockName), &blockNameLength, blockName);
    DescriptorType constantBuffer = {
        .name = blockName,
        .slot = (u32)i,
    };
    descriptors.emplace(blockName, constantBuffer);
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
        .name = attributeName,
        .type = glUtils::GLTypeToVarType(attribType),
        .slot = (u32)i,
        .byteOffset = (u32)attribSize, // TODO: might not be correct
    };
    descriptors.emplace(attributeName, descriptor);
  }
  return descriptors;
}

static ShaderHandle AddShader(const char *name, const std::vector<std::string> &sources, const std::vector<u32> &types)
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
      .mConstantBufferDescriptors = GetBuffers<ConstantBufferDescriptor>(program, GL_UNIFORM_BLOCK),
      .mShaderBufferDescriptors = GetBuffers<ShaderBufferDescriptor>(program, GL_SHADER_STORAGE_BLOCK),
      .mInputBufferDescriptors = GetInputBufferDescriptors(program),
  };

  sCurrHandle++;
  sShaderHandles[sCurrHandle] = program;
  sShaderInfos[sCurrHandle] = shaderInfo;
  sNameToShaderHandles[name] = program;
  return sCurrHandle;
}

ShaderHandle AddShader(const char *name, const std::string &vSource, const std::string &fSource)
{
  return AddShader(
      name, std::vector<std::string>{vSource, fSource}, std::vector<u32>{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER});
}

ShaderHandle AddComputeShader(const char *name, const std::string &source)
{
  return AddShader(name, std::vector<std::string>{source}, std::vector<u32>{GL_COMPUTE_SHADER});
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
