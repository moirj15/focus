#include "ShaderManager.hpp"

#include "Utils.hpp"
#include "glad.h"

#include <algorithm>
#include <cassert>

namespace focus
{

void ShaderManager::CompileShader(GLuint handle, const std::string &source, const std::string &type)
{
    const int32_t sourceLen = source.size();
    auto shaderSource = source.c_str();
    glShaderSource(handle, 1, &shaderSource, &sourceLen);
    glCompileShader(handle);
    GLint success = 0;
    char compileLog[512] = {};
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(handle, 512, nullptr, compileLog);
        printf("%s Shader Compile Error: %s\n", type.c_str(), compileLog);
        assert(0);
    }
}

void ShaderManager::LinkShaderProgram(std::vector<GLuint> shaderHandles, GLuint programHandle)
{
    for (GLuint sHandle : shaderHandles) {
        glAttachShader(programHandle, sHandle);
    }
    glLinkProgram(programHandle);

    GLint success = 0;
    char compileLog[512] = {};
    glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programHandle, sizeof(compileLog), nullptr, compileLog);
        printf("Shader Link Error: %s\n", compileLog);
        assert(0);
    }
    for (GLuint sHandle : shaderHandles) {
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

Shader ShaderManager::AddShader(
    const char *name, const std::vector<std::string> &sources, const std::vector<GLenum> &types)
{
    assert(sources.size() == types.size());
    std::vector<GLuint> stageHandles;
    for (uint32_t i = 0; i < sources.size(); i++) {
        stageHandles.emplace_back(glCreateShader(types[i]));
        CompileShader(stageHandles[i], sources[i], ShaderTypeToString(types[i]));
    }
    GLuint program = glCreateProgram();
    LinkShaderProgram(stageHandles, program);

    sCurrHandle++;
    mShaderHandles[sCurrHandle] = program;
    mNameToShaderHandles[name] = sCurrHandle;
    return sCurrHandle;
}

Shader ShaderManager::AddShader(const char *name, const std::string &vSource, const std::string &fSource)
{
    return AddShader(
        name, std::vector<std::string>{vSource, fSource}, std::vector<GLenum>{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER});
}

Shader ShaderManager::AddComputeShader(const char *name, const std::string &source)
{
    return AddShader(name, std::vector<std::string>{source}, std::vector<GLenum>{GL_COMPUTE_SHADER});
}

GLuint ShaderManager::GetProgram(Shader handle)
{
    return mShaderHandles[handle];
}

void ShaderManager::DeleteShader(Shader handle)
{
    glDeleteShader(mShaderHandles[handle]);
    mShaderHandles.erase(handle);
}

} // namespace focus
