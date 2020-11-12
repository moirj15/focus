#pragma once

#include "../Interface/Handles.hpp"
#include "../Interface/IShader.hpp"

namespace renderer::gl::ShaderManager
{

// TODO: add functions to specify a shader path and a file to specify the list of shaders/pipelines
// void SetShaderPath(const char *absPath);

// TODO: overloads for other shader types
ShaderHandle AddShader(const char *name, const std::string &vSource, const std::string &fSource);

ShaderHandle GetShader(const std::string &name);

ShaderInfo GetInfo(ShaderHandle handle);

u32 GetProgram(ShaderHandle handle);

void DeleteShader(ShaderHandle handle);


}
