#include "Utils.hpp"
#include "glad.h"

namespace renderer::gl::utils
{

VarType GLTypeToVarType(GLenum type)
{
  switch (type) {
  case GL_FLOAT:
    return VarType::Float;
  case GL_FLOAT_VEC2:
    return VarType::Vec2;
  case GL_FLOAT_VEC3:
    return VarType::Vec3;
  case GL_FLOAT_VEC4:
    return VarType::Vec4;
  case GL_FLOAT_MAT2:
    return VarType::Mat2;
  case GL_FLOAT_MAT3:
    return VarType::Mat3;
  case GL_FLOAT_MAT4:
    return VarType::Mat4;
  default:
    return VarType::Invalid;
  }
}

}