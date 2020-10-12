#pragma once

#include <Windows.h>
#include "../../common.h"
#include "../Interface/Window.hpp"
#include "Shader.hpp"
#include <string>
//#include "../Interface/Context.hpp"

namespace dx11::context
{

void Init(WNDPROC messageHandler, HINSTANCE instanceHandle);

renderer::Window MakeWindow(s32 width, s32 height);

Shader *CreateShaderFromSource(const std::string &vSource, const std::string &fSource);

//Shader CreateShader(const char *vBinary, const char *pBinary);

} // namespace dx11