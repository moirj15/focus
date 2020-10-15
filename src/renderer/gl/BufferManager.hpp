#pragma once

#include "../Interface/Handles.hpp"
#include "../Interface/IBuffer.hpp"
#include "../Interface/Types.hpp"

#include <vector>

namespace renderer::gl
{
template<typename Handle, typename Descriptor>
struct BufferManager {
  Handle mCurrHandle = 0;
  std::unordered_map<Handle, u32> mHandles;
  std::unordered_map<Handle, Descriptor> mDescriptors;

  Handle Create(void *data, u32 sizeInBytes, Descriptor descriptor)
  {
    // Create the buffer for OpenGL
    u32 handle;
    glCreateBuffers(1, &handle);
    glNamedBufferStorage(handle, sizeInBytes, data, GL_DYNAMIC_STORAGE_BIT);
    // Do the actual management of the buffer handle
    mCurrHandle++;
    mDescriptors[mCurrHandle] = descriptor;
    mHandles[mCurrHandle] = handle;
    return mCurrHandle;
  }

  u32 Get(Handle handle) { return mHandles[handle]; }

  void WriteTo(void *data, u32 sizeInBytes, Handle handle)
  {
    auto bufferHandle = mHandles[handle];
    auto descriptor = mDescriptors[handle];
    assert(descriptor.mSizeInBytes >= sizeInBytes);
    glNamedBufferSubData(bufferHandle, 0, sizeInBytes, data);
  }
  void WriteTo(void *data, u32 sizeInBytes, u32 offsetInBytes, Handle handle)
  {
    auto bufferHandle = mHandles[handle];
    auto descriptor = mDescriptors[handle];
    assert(descriptor.mSizeInBytes >= (sizeInBytes + offsetInBytes));
    glNamedBufferSubData(bufferHandle, offsetInBytes, sizeInBytes, data);
  }

  // TODO: refractor so less code is used
  std::vector<void *> ReadFrom(Handle handle)
  {
    const auto &descriptor = mDescriptors[handle];
    std::vector<void *> vertexBuffer(descriptor.mSizeInBytes);

    glGetNamedBufferSubData(mHandles[handle], 0, vertexBuffer.size(), vertexBuffer.data());
    return vertexBuffer;
  }

  std::vector<void *> ReadFrom(Handle handle, u32 length)
  {
    const auto &descriptor = mDescriptors[handle];
    assert(length < descriptor.mSizeInBytes);
    std::vector<void *> vertexBuffer(length);

    glGetNamedBufferSubData(mHandles[handle], 0, vertexBuffer.size(), vertexBuffer.data());
    return vertexBuffer;
  }

  std::vector<void *> ReadFrom(Handle handle, u32 start, u32 end)
  {
    const auto &descriptor = mDescriptors[handle];
    assert(end < descriptor.mSizeInBytes);
    std::vector<void *> vertexBuffer(end - start);

    glGetNamedBufferSubData(mHandles[handle], start, vertexBuffer.size(), vertexBuffer.data());
    return vertexBuffer;
  }

  void Destroy(Handle handle)
  {
    auto bufferHandle = mHandles[handle];
    mHandles.erase(handle);
    mDescriptors.erase(handle);
    glDeleteBuffers(1, &bufferHandle);
  }
};

extern BufferManager<VertexBufferHandle, VertexBufferDescriptor> gVBManager;
extern BufferManager<IndexBufferHandle, IndexBufferDescriptor> gIBManager;

} // namespace renderer::gl

