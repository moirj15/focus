#pragma once

#include "../Interface/Types.hpp"

typedef unsigned int GLenum;

namespace renderer::gl::utils
{

VarType GLTypeToVarType(GLenum type);
}