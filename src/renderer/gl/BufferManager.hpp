#pragma once

#include "../Interface/Handles.hpp"
#include "../Interface/Types.hpp"
#include "../Interface/IBuffer.hpp"

#include <vector>

namespace renderer::gl::VertexBufferManager
{


VertexBufferHandle Create(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor);

void WriteTo(void *data, u32 sizeInBytes, VertexBufferHandle handle);
void WriteTo(void *data, u32 sizeInBytes, u32 offsetInBytes, VertexBufferHandle handle);

std::vector<void *> ReadFrom(VertexBufferHandle handle);
std::vector<void *> ReadFrom(VertexBufferHandle handle, u32 length);
std::vector<void *> ReadFrom(VertexBufferHandle handle, u32 start, u32 end);

void Destroy(VertexBufferHandle handle);


VertexBufferDescriptor GetDescriptor(VertexBufferHandle handle);

// Only for use by the Draw*() methods
void BindToPipeline(VertexBufferHandle handle);

} // namespace gl::VertexBufferManager

namespace renderer::gl::IndexBufferManager
{
VertexBufferHandle Create(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor);

void WriteTo(void *data, u32 sizeInBytes, IndexBufferHandle handle);
void WriteTo(void *data, u32 sizeInBytes, u32 offset, IndexBufferHandle handle);

std::vector<void *> ReadFrom(IndexBufferHandle handle);
std::vector<void *> ReadFrom(IndexBufferHandle handle, u32 length);
std::vector<void *> ReadFrom(IndexBufferHandle handle, u32 start, u32 end);

void Destroy(renderer::IndexBufferHandle handle);

// TODO: need way to query information about the buffer contents (type, component count, size, etc).
// can either do this by returning a descriptor with all the information or have a query function for each content type


IndexBufferDescriptor GetDescriptor(IndexBufferHandle handle);

// Only for use by the Draw*() methods
void BindToPipeline(IndexBufferHandle handle);

}
