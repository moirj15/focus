#pragma once

#include "../../common.h"
#include "Types.hpp"
#include "Handles.hpp"

namespace renderer
{

enum class BufferType {
  SingleType, // just one type i.e (vvvvvvv)
  InterLeaved, // (vntvntvnt)
  Combined, // (vvvnnnttt)
};

// TODO: handle various buffer types (offsets, types, etc)
struct VertexBufferDescriptor {
  std::string inputDescriptorName;
  BufferType bufferType;
  VarType type;
  u32 sizeInBytes;
};

enum class IndexBufferType {
  U8,
  U16,
  U32
};

struct IndexBufferDescriptor {
  IndexBufferType type;
  u32 sizeInBytes;
};

struct ConstantBufferDescriptor {
  std::vector<VarType> types;
  u32 sizeInBytes;
};

} // namespace renderer