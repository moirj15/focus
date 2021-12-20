#pragma once

#include "../Interface/FocusBackend.hpp"
#include "glad.h"

#include <cassert>
#include <unordered_map>
#include <vector>

namespace focus
{
template<typename Handle, typename BufferLayout>
struct BufferManager {
  Handle mCurrHandle{0};
  std::unordered_map<Handle, GLuint> mHandles;
  std::unordered_map<Handle, BufferLayout> mDescriptors;

  inline Handle Create(BufferLayout buffer_layout, void *data, uint32_t size_in_bytes)
  {
    // Create the buffer for OpenGL
    GLuint handle;
    glGenBuffers(1, &handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glBufferData(GL_ARRAY_BUFFER, size_in_bytes, data, GL_STATIC_DRAW);
    // Do the actual management of the buffer handle
    mCurrHandle++;
    mDescriptors[mCurrHandle] = buffer_layout;
    mHandles[mCurrHandle] = handle;
    return mCurrHandle;
  }

  inline GLuint Get(Handle handle) { return mHandles[handle]; }

  inline void WriteTo(void *data, uint32_t size_in_bytes, Handle handle) { WriteTo(data, size_in_bytes, 0, handle); }
  inline void WriteTo(void *data, uint32_t size_in_bytes, uint32_t offset_in_bytes, Handle handle)
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

  inline std::vector<void *> ReadFrom(Handle handle, uint32_t length)
  {
    const auto &descriptor = mDescriptors[handle];
    assert(length < descriptor.mSizeInBytes);
    std::vector<void *> vertexBuffer(length);

    glGetNamedBufferSubData(mHandles[handle], 0, vertexBuffer.size(), vertexBuffer.data());
    return vertexBuffer;
  }

  inline std::vector<void *> ReadFrom(Handle handle, uint32_t start, uint32_t end)
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
