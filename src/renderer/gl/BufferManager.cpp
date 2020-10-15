#include "BufferManager.hpp"

#include <unordered_map>
#include "glad.h"

namespace renderer::gl
{
BufferManager<VertexBufferHandle, VertexBufferDescriptor> gVBManager;
BufferManager<IndexBufferHandle, IndexBufferDescriptor> gIBManager;
}

#if 0
  template<typename Handle, typename Descriptor>
  Handle BufferManager::Create(void *data, u32 sizeInBytes, Descriptor descriptor)
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

  void BufferManager::WriteTo(void *data, u32 sizeInBytes, Handle handle)
  {
    auto bufferHandle = mHandles[handle];
    auto descriptor = mDescriptors[handle];
    assert(descriptor.mSizeInBytes >= sizeInBytes);
    glNamedBufferSubData(bufferHandle, 0, sizeInBytes, data);
  }
  void BufferManager::WriteTo(void *data, u32 sizeInBytes, u32 offsetInBytes, Handle handle)
  {
    auto bufferHandle = mHandles[handle];
    auto descriptor = mDescriptors[handle];
    assert(descriptor.mSizeInBytes >= (sizeInBytes + offsetInBytes));
    glNamedBufferSubData(bufferHandle, offsetInBytes, sizeInBytes, data);
  }

  // TODO: refractor so less code is used 
  std::vector<void*> BufferManager::ReadFrom(Handle handle)
  {
    const auto &descriptor = mDescriptors[handle];
    std::vector<void*> vertexBuffer(descriptor.mSizeInBytes);
    
    glGetNamedBufferSubData(mHandles[handle], 0, vertexBuffer.size(), vertexBuffer.data());
    return vertexBuffer; 
  }

  std::vector<void*> BufferManager::ReadFrom(Handle handle, u32 length)
  {
    const auto &descriptor = mDescriptors[handle];
    assert(length < descriptor.mSizeInBytes);
    std::vector<void*> vertexBuffer(length);
    
    glGetNamedBufferSubData(mHandles[handle], 0, vertexBuffer.size(), vertexBuffer.data());
    return vertexBuffer; 
  }

  std::vector<void*> BufferManager::ReadFrom(Handle handle, u32 start, u32 end)
  {
    const auto &descriptor = mDescriptors[handle];
    assert(end < descriptor.mSizeInBytes);
    std::vector<void*> vertexBuffer(end - start);
    
    glGetNamedBufferSubData(mHandles[handle], start, vertexBuffer.size(), vertexBuffer.data());
    return vertexBuffer; 
  }


  void BufferManager::Destroy(Handle handle)
  {
    auto bufferHandle = mHandles[handle];
    mHandles.erase(handle);
    mDescriptors.erase(handle);
    glDeleteBuffers(1, &bufferHandle);
  }
template class BufferManager<VertexBufferHandle, VertexBufferDescriptor>;

namespace renderer::gl::VertexBufferManager
{
static BufferManager<VertexBufferHandle, VertexBufferDescriptor> sVBManager;

renderer::VertexBufferHandle Create(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor)
{
  return sVBManager.Create(data, sizeInBytes, descriptor);
}

void WriteTo(void *data, u32 sizeInBytes, VertexBufferHandle handle)
{
  sVBManager.WriteTo(data, sizeInBytes, handle);
}

void WriteTo(void *data, u32 sizeInBytes, u32 offsetInBytes, VertexBufferHandle handle)
{
  sVBManager.WriteTo(data, sizeInBytes, offsetInBytes, handle);
}

// TODO: figure out how to implement
std::vector<void *> ReadFrom(VertexBufferHandle handle)
{
  return sVBManager.ReadFrom(handle);
}
std::vector<void *> ReadFrom(VertexBufferHandle handle, u32 length)
{
  return sVBManager.ReadFrom(handle, length);
}
std::vector<void *> ReadFrom(VertexBufferHandle handle, u32 start, u32 end)
{
  return sVBManager.ReadFrom(handle, start, end);
}

void Destroy(VertexBufferHandle handle)
{
  sVBManager.Destroy(handle);
}

VertexBufferDescriptor GetDescriptor(VertexBufferHandle handle)
{
  return sVBManager.mDescriptors[handle];
}

// Only for use by the Draw*() methods
void BindToPipeline(VertexBufferHandle handle)
{
  auto bufferHandle = sVBManager.mHandles[handle];
  glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
}

u32 GetGLHandle(VertexBufferHandle handle)
{
  return sVBManager.mHandles[handle];
}

} // namespace gl::VertexBufferManager


namespace renderer::gl::IndexBufferManager
{
static BufferManager<IndexBufferHandle, IndexBufferDescriptor> sIBManager;

renderer::VertexBufferHandle Create(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor)
{
  return sIBManager.Create(data, sizeInBytes, descriptor);
}

void WriteTo(void *data, u32 sizeInBytes, VertexBufferHandle handle)
{
  sIBManager.WriteTo(data, sizeInBytes, handle);
}

void WriteTo(void *data, u32 sizeInBytes, u32 offsetInBytes, IndexBufferHandle handle)
{
  sIBManager.WriteTo(data, sizeInBytes, offsetInBytes, handle);
}

// TODO: figure out how to implement
std::vector<void *> ReadFrom(IndexBufferHandle handle)
{
  return sIBManager.ReadFrom(handle);
}
std::vector<void *> ReadFrom(IndexBufferHandle handle, u32 length)
{
  return sIBManager.ReadFrom(handle, length);
}
std::vector<void *> ReadFrom(IndexBufferHandle handle, u32 start, u32 end)
{
  return sIBManager.ReadFrom(handle, start, end);
}

void Destroy(IndexBufferHandle handle)
{
  sIBManager.Destroy(handle);
}

IndexBufferDescriptor GetDescriptor(IndexBufferHandle handle)
{
  return sIBManager.mDescriptors[handle];
}

u32 GetGLHandle(IndexBufferHandle handle)
{
  return sIBManager.mHandles[handle];
}


} // namespace gl::VertexBufferManager
#endif

