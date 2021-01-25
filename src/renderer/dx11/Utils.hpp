#pragma once
#include "../Interface/Context.hpp"

#include <cassert>
#include <d3d11.h>
#include <dxgi.h>

namespace focus::dx11
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

constexpr u32 ClearBufferToD3D11(ClearBuffer clearBuffer)
{
  u32 flags = 0;
  if ((u32)clearBuffer & (u32)ClearBuffer::Depth) {
    flags |= D3D11_CLEAR_DEPTH;
  }
  if ((u32)clearBuffer & (u32)ClearBuffer::Stencil) {
    flags |= D3D11_CLEAR_STENCIL;
  }
  return flags;
}

} // namespace focus::dx11
