#pragma once

#include "../Interface/FocusBackend.hpp"
#include "glad.h"

#include <cassert>

typedef unsigned int GLenum;

namespace focus::glUtils
{

constexpr VarType GLTypeToVarType(GLenum type)
{
    switch (type) {
    case GL_FLOAT:
        return VarType::Float;
    case GL_FLOAT_VEC2:
        return VarType::Float2;
    case GL_FLOAT_VEC3:
        return VarType::Float3;
    case GL_FLOAT_VEC4:
        return VarType::Float4;
    case GL_FLOAT_MAT2:
        return VarType::Float2x2;
    case GL_FLOAT_MAT3:
        return VarType::Float3x3;
    case GL_FLOAT_MAT4:
        return VarType::Float4x4;
    default:
        return VarType::Invalid;
    }
}

inline uint32_t GLTypeSizeInBytes(GLenum type)
{
    switch (type) {
    case GL_FLOAT:
        return sizeof(float);
    case GL_FLOAT_VEC2:
        return sizeof(float) * 2;
    case GL_FLOAT_VEC3:
        return sizeof(float) * 3;
    case GL_FLOAT_VEC4:
        return sizeof(float) * 4;
    default:
        assert(0);
        return 0;
    }
}

inline GLint VarTypeSizeInBytes(VarType type)
{
    switch (type) {
    case VarType::Float:
        return sizeof(float);
    case VarType::Float2:
        return sizeof(float) * 2;
    case VarType::Float3:
        return sizeof(float) * 3;
    case VarType::Float4:
        return sizeof(float) * 4;
    default:
        assert(0);
        return 0;
    }
}

constexpr uint32_t VarTypeToSlotSizeGL(VarType varType)
{
    switch (varType) {
    case VarType::Float:
        return 1;
    case VarType::Float2:
        return 2;
    case VarType::Float3:
        return 3;
    case VarType::Float4:
        return 4;
    default:
        assert(0 && "Undefined Variable Type");
        return 0;
    }
}

constexpr GLenum VarTypeToIndividualTypeGL(VarType varType)
{
    switch (varType) {
    case VarType::Float:
    case VarType::Float2:
    case VarType::Float3:
    case VarType::Float4:
        return GL_FLOAT;
    default:
        assert(0 && "Undefined Variable Type");
        return GL_INVALID_ENUM;
    }
}

constexpr uint32_t ClearBufferToGL(ClearBuffer clearBuffer)
{
    uint32_t flags = 0;
    if ((uint32_t)clearBuffer & (uint32_t)ClearBuffer::Depth) {
        flags |= GL_DEPTH_BUFFER_BIT;
    }
    if ((uint32_t)clearBuffer & (uint32_t)ClearBuffer::Stencil) {
        flags |= GL_STENCIL_BUFFER_BIT;
    }
    if ((uint32_t)clearBuffer & (uint32_t)ClearBuffer::Color) {
        flags |= GL_COLOR_BUFFER_BIT;
    }
    return flags;
}

constexpr GLuint PrimitiveToGL(Primitive primitive)
{
    switch (primitive) {
    case Primitive::Points:
        return GL_POINTS;
    case Primitive::LineStrip:
        return GL_LINE_STRIP;
    case Primitive::LineLoop:
        return GL_LINE_LOOP;
    case Primitive::Lines:
        return GL_LINES;
    case Primitive::LineStripAdjacency:
        return GL_LINE_STRIP_ADJACENCY;
    case Primitive::LinesAdjacency:
        return GL_LINES_ADJACENCY;
    case Primitive::TriangleStrip:
        return GL_TRIANGLE_STRIP;
    case Primitive::TriangleFan:
        return GL_TRIANGLE_FAN;
    case Primitive::Triangles:
        return GL_TRIANGLES;
    case Primitive::TriangleStripAdjacency:
        return GL_TRIANGLE_STRIP_ADJACENCY;
    default:
        assert(0 && "Undefined Primitive");
        return GL_INVALID_ENUM;
    }
}

constexpr GLenum ComparisonFunctionToGL(ComparisonFunction func)
{
    switch (func) {
    case ComparisonFunction::Never:
        return GL_NEVER;
    case ComparisonFunction::Less:
        return GL_LESS;
    case ComparisonFunction::Equal:
        return GL_EQUAL;
    case ComparisonFunction::LessOrEqual:
        return GL_LEQUAL;
    case ComparisonFunction::Greater:
        return GL_GREATER;
    case ComparisonFunction::GreaterOrEqual:
        return GL_GEQUAL;
    case ComparisonFunction::NotEqual:
        return GL_NOTEQUAL;
    case ComparisonFunction::Always:
        return GL_ALWAYS;
    default:
        assert(0 && "Undefined Comparison Function");
        return GL_INVALID_ENUM;
    }
}
constexpr GLenum TriangleFaceToGL(TriangleFace face)
{
    switch (face) {
    case TriangleFace::Front:
        return GL_FRONT;
    case TriangleFace::Back:
        return GL_BACK;
    default:
        assert(0 && "Undefined Triangle Face");
        return GL_INVALID_ENUM;
    }
}

constexpr GLenum WindingOrderToGL(WindingOrder winding)
{
    switch (winding) {
    case WindingOrder::Clockwise:
        return GL_CW;
    case WindingOrder::CounterClockwise:
        return GL_CCW;
    default:
        assert(0 && "Undefined Winding Order");
        return GL_INVALID_ENUM;
    }
}

constexpr GLenum RasterizationModeToGL(RasterizationMode mode)
{
    switch (mode) {
    case RasterizationMode::Point:
        return GL_POINT;
    case RasterizationMode::Line:
        return GL_LINE;
    case RasterizationMode::Fill:
        return GL_FILL;
    default:
        assert(0 && "Undefined Rasterization Mode");
        return GL_INVALID_ENUM;
    }
}

constexpr GLbitfield AccessModeToGL(AccessMode mode)
{
    switch (mode) {
    case AccessMode::ReadOnly:
        return GL_READ_ONLY;
    case AccessMode::WriteOnly:
        return GL_WRITE_ONLY;
    case AccessMode::ReadWrite:
        return GL_READ_WRITE;
    default:
        assert(0 && "Undefined Access Mode");
        return 0xff;
    }
}

} // namespace focus::glUtils
