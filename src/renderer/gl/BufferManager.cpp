#include "BufferManager.hpp"

#include <unordered_map>
#include "glad.h"

namespace renderer::gl
{
BufferManager<VertexBufferHandle, VertexBufferDescriptor> gVBManager;
BufferManager<IndexBufferHandle, IndexBufferDescriptor> gIBManager;
}

