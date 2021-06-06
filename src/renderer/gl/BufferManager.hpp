#pragma once

#include "../Interface/Context.hpp"
#include "glad.h"

#include <unordered_map>
#include <vector>
#include <cassert>

namespace focus
{
template<typename Handle, typename Descriptor>
struct BufferManager {
  Handle mCurrHandle{0};
  std::unordered_map<Handle, u32> mHandles;
  std::unordered_map<Handle, Descriptor> mDescriptors;

  inline Handle Create(void *data, Descriptor descriptor)
  {
    // Create the buffer for OpenGL
    u32 handle;
    glGenBuffers(1, &handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glBufferData(GL_ARRAY_BUFFER, descriptor.size_in_bytes, data, GL_STATIC_DRAW);
    // Do the actual management of the buffer handle
    mCurrHandle++;
    mDescriptors[mCurrHandle] = descriptor;
    mHandles[mCurrHandle] = handle;
    return mCurrHandle;
  }

  inline u32 Get(Handle handle) { return mHandles[handle]; }

  inline void WriteTo(void *data, u32 size_in_bytes, Handle handle) { WriteTo(data, size_in_bytes, 0, handle); }
  inline void WriteTo(void *data, u32 size_in_bytes, u32 offset_in_bytes, Handle handle)
  {
    auto bufferHandle = mHandles[handle];
    auto descriptor = mDescriptors[handle];
    assert(descriptor.size_in_bytes >= (size_in_bytes + offset_in_bytes));
    glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
    glBufferSubData(GL_ARRAY_BUFFER, offset_in_bytes, size_in_bytes, data);
  }

  // TODO: refractor so less code is used
  inline std::vector<void *> ReadFrom(Handle handle)
  {
    const auto &descriptor = mDescriptors[handle];
    std::vector<void *> vertexBuffer(descriptor.mSizeInBytes);

    glGetNamedBufferSubData(mHandles[handle], 0, vertexBuffer.size(), vertexBuffer.data());
    return vertexBuffer;
  }

  inline std::vector<void *> ReadFrom(Handle handle, u32 length)
  {
    const auto &descriptor = mDescriptors[handle];
    assert(length < descriptor.mSizeInBytes);
    std::vector<void *> vertexBuffer(length);

    glGetNamedBufferSubData(mHandles[handle], 0, vertexBuffer.size(), vertexBuffer.data());
    return vertexBuffer;
  }

  inline std::vector<void *> ReadFrom(Handle handle, u32 start, u32 end)
  {
    const auto &descriptor = mDescriptors[handle];
    assert(end < descriptor.mSizeInBytes);
    std::vector<void *> vertexBuffer(end - start);

    glGetNamedBufferSubData(mHandles[handle], start, vertexBuffer.size(), vertexBuffer.data());
    return vertexBuffer;
  }

  inline void Destroy(Handle handle)
  {
    auto bufferHandle = mHandles[handle];
    mHandles.erase(handle);
    mDescriptors.erase(handle);
    glDeleteBuffers(1, &bufferHandle);
  }
};

} // namespace focus
