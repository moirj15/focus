#pragma once

#include "../../common.h"
#include "Types.hpp"
namespace renderer
{

enum class BufferType {
  SingleType, // just one type i.e (vvvvvvv)
  InterLeaved, // (vntvntvnt)
  Combined, // (vvvnnnttt)
};

// TODO: handle various buffer types (offsets, types, etc)
struct VertexBufferDescriptor {
  BufferType mBufferType;
  VarType mType;
  u32 mSizeInBytes;
};

enum class IndexBufferType {
  U8,
  U16,
  U32
};

struct IndexBufferDescriptor {
  IndexBufferType mType;
  u32 mSizeInBytes;
};

} // namespace renderer