#pragma once

#include "../Interface/Handles.hpp"
#include "../Interface/IShader.hpp"

namespace renderer::gl::ShaderManager
{

// TODO: add functions to specify a shader path and a file to specify the list of shaders/pipelines
// void SetShaderPath(const char *absPath);

// TODO: overloads for other shader types
ShaderHandle AddShader(const std::string &name, const char *vSource, const char *fSource);

ShaderHandle GetShader(const std::string &name);

void DeleteShader(ShaderHandle handle);

}
