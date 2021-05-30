#include "ShaderManager.hpp"

#include "Utils.hpp"
#include "glad.h"

#include <algorithm>
#include <cassert>

namespace focus
{

// static ShaderHandle sCurrHandle;
// std::unordered_map<std::string, ShaderHandle> sNameToShaderHandles;
// std::unordered_map<ShaderHandle, u32> sShaderHandles;
// std::unordered_map<ShaderHandle, ShaderInfo> sShaderInfos;

void ShaderManager::CompileShader(u32 handle, const std::string &source, const std::string &type)
{
  const s32 sourceLen = source.size();
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

void ShaderManager::LinkShaderProgram(std::vector<u32> shaderHandles, u32 programHandle)
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

const char *ShaderManager::ShaderTypeToString(GLenum type)
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

std::unordered_map<std::string, InputBufferDescriptor> ShaderManager::GetInputBufferDescriptors(u32 program)
{
  std::unordered_map<std::string, InputBufferDescriptor> descriptorsMap;
  s32 attributeCount = 0;
  glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attributeCount);
  char attributeName[64] = {};
  s32 attribNameLen;
  s32 attribSize;
  GLenum attribType;
  std::vector<InputBufferDescriptor> descriptors;
  for (s32 i = 0; i < attributeCount; i++) {
    glGetActiveAttrib(program, i, sizeof(attributeName), &attribNameLen, &attribSize, &attribType, attributeName);
    u32 slot = glGetAttribLocation(program, attributeName);
    InputBufferDescriptor descriptor = {
        .name = attributeName,
        .type = glUtils::GLTypeToVarType(attribType),
        .slot = slot,
        .byteOffset = 0, // (u32)attribSize * glUtils::GLTypeSizeInBytes(attribType), // TODO: might not be correct
    };
    descriptors.push_back(descriptor);
  }
  std::sort(descriptors.begin(), descriptors.end(), [](const auto &a, const auto &b) { return a.slot < b.slot; });
  u32 currOffset = 0;
  descriptorsMap.emplace(descriptors[0].name, descriptors[0]);
  for (u32 i = 1; i < descriptors.size(); i++) {
    // TODO: this may cause issues for matrices
    auto &desc = descriptors[i];
    auto &prev = descriptors[i - 1];
    currOffset += glUtils::VarTypeSizeInBytes(prev.type);
    desc.byteOffset = currOffset;
    descriptorsMap.emplace(desc.name, desc);
  }
  return descriptorsMap;
}

ShaderHandle ShaderManager::AddShader(
    const char *name, const std::vector<std::string> &sources, const std::vector<u32> &types)
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
  mShaderHandles[sCurrHandle] = program;
  mShaderInfos[sCurrHandle] = shaderInfo;
  mNameToShaderHandles[name] = sCurrHandle;
  return sCurrHandle;
}

ShaderHandle ShaderManager::AddShader(const char *name, const std::string &vSource, const std::string &fSource)
{
  return AddShader(
      name, std::vector<std::string>{vSource, fSource}, std::vector<u32>{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER});
}

ShaderHandle ShaderManager::AddComputeShader(const char *name, const std::string &source)
{
  return AddShader(name, std::vector<std::string>{source}, std::vector<u32>{GL_COMPUTE_SHADER});
}

ShaderInfo ShaderManager::GetInfo(ShaderHandle handle)
{
  return mShaderInfos[handle];
}

u32 ShaderManager::GetProgram(ShaderHandle handle)
{
  return mShaderHandles[handle];
}

void ShaderManager::DeleteShader(ShaderHandle handle)
{
  glDeleteShader(mShaderHandles[handle]);
  mShaderHandles.erase(handle);
  mShaderInfos.erase(handle);
}

} // namespace focus
