#pragma once
#include "../Interface/focus.hpp"

#include <cassert>
#include <d3d11.h>
#include <dxgi.h>

namespace focus::dx11::utils
{

#define Check(x) assert(x == S_OK)

constexpr D3D11_PRIMITIVE_TOPOLOGY PrimitiveToD3D11(Primitive primitive)
{
  switch (primitive) {
  case Primitive::Points:
    return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
  case Primitive::LineStrip:
    return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
  case Primitive::LineLoop:
    assert(0 && "TODO");
    return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
  case Primitive::Lines:
    return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
  case Primitive::LineStripAdjacency:
    return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
  case Primitive::LinesAdjacency:
    return D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
  case Primitive::TriangleStrip:
    return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
  case Primitive::TriangleFan:
    assert(0 && "TODO");
    return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
  case Primitive::Triangles:
    return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
  case Primitive::TriangleStripAdjacency:
    return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
  default:
    assert(0 && "Undefined Primitive");
    return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
  }
}

constexpr uint32_t ClearBufferToD3D11(ClearBuffer clearBuffer)
{
    uint32_t flags = 0;
  if ((uint32_t)clearBuffer & (uint32_t)ClearBuffer::Depth) {
    flags |= D3D11_CLEAR_DEPTH;
  }
  if ((uint32_t)clearBuffer & (uint32_t)ClearBuffer::Stencil) {
    flags |= D3D11_CLEAR_STENCIL;
  }
  return flags;
}

constexpr DXGI_FORMAT FocusToDXGIFormat(VarType type)
{
  switch (type)
  {
  case VarType::Float:
    return DXGI_FORMAT_R32_FLOAT;
  case VarType::Int:
    return DXGI_FORMAT_R32_SINT;
  case VarType::UInt:
    return DXGI_FORMAT_R32_UINT;
  case VarType::Float2:
    return DXGI_FORMAT_R32G32_FLOAT;
  case VarType::Float3:
    return DXGI_FORMAT_R32G32B32_FLOAT;
  case VarType::Float4:
    return DXGI_FORMAT_R32G32B32A32_FLOAT;
  case VarType::Int2:
    return DXGI_FORMAT_R32G32_SINT;
  case VarType::Int3:
    return DXGI_FORMAT_R32G32B32_SINT;
  case VarType::Int4:
    return DXGI_FORMAT_R32G32B32A32_SINT;
  case VarType::UInt2:
    return DXGI_FORMAT_R32G32_UINT;
  case VarType::UInt3:
    return DXGI_FORMAT_R32G32B32_UINT;
  case VarType::UInt4:
    return DXGI_FORMAT_R32G32B32A32_UINT;
  case VarType::Float2x2:
    assert(0 && "TODO");
    return DXGI_FORMAT_FORCE_UINT;
  case VarType::Float3x3:
    assert(0 && "TODO");
    return DXGI_FORMAT_FORCE_UINT;
  case VarType::Float4x4:
    assert(0 && "TODO");
    return DXGI_FORMAT_FORCE_UINT;
  case VarType::Sampler1D:
    assert(0 && "TODO");
    return DXGI_FORMAT_FORCE_UINT;
  case VarType::Sampler2D:
    assert(0 && "TODO");
    return DXGI_FORMAT_FORCE_UINT;
  case VarType::Sampler3D:
    assert(0 && "TODO");
    return DXGI_FORMAT_FORCE_UINT;

  default:
    assert(0 && "Undefined Format");
    return DXGI_FORMAT_FORCE_UINT;
  }
}

constexpr uint32_t BytesPerVarType(VarType type)
{
  switch (type)
  {
  case VarType::Float:
    return sizeof(float);
  case VarType::Int:
    return sizeof(int32_t);
  case VarType::UInt:
    return sizeof(uint32_t);
  case VarType::Float2:
    return sizeof(float) * 2;
  case VarType::Float3:
    return sizeof(float) * 3;
  case VarType::Float4:
    return sizeof(float) * 4;
  case VarType::Int2:
    return DXGI_FORMAT_R32G32_SINT;
    return sizeof(int32_t) * 2;
  case VarType::Int3:
    return sizeof(int32_t) * 3;
  case VarType::Int4:
    return sizeof(int32_t) * 4;
  case VarType::UInt2:
    return sizeof(uint32_t) * 2;
  case VarType::UInt3:
    return sizeof(uint32_t) * 3;
  case VarType::UInt4:
    return sizeof(uint32_t) * 4;
  case VarType::Float2x2:
    assert(0 && "TODO");
    return DXGI_FORMAT_FORCE_UINT;
  case VarType::Float3x3:
    assert(0 && "TODO");
    return DXGI_FORMAT_FORCE_UINT;
  case VarType::Float4x4:
    assert(0 && "TODO");
    return DXGI_FORMAT_FORCE_UINT;
  case VarType::Sampler1D:
    assert(0 && "TODO");
    return DXGI_FORMAT_FORCE_UINT;
  case VarType::Sampler2D:
    assert(0 && "TODO");
    return DXGI_FORMAT_FORCE_UINT;
  case VarType::Sampler3D:
    assert(0 && "TODO");
    return DXGI_FORMAT_FORCE_UINT;

  default:
    assert(0 && "Undefined Format");
    return DXGI_FORMAT_FORCE_UINT;
  }

}

} // namespace focus::dx11
