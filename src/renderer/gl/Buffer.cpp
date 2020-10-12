#include "Buffer.hpp"

#include "glad.h"

namespace gl::buffer
{

// TODO: remove
#if 0
static void StoreInGLBuffer(void *data, u32 len, u32 handle)
{
  glNamedBufferSubData(handle, 0, len, data);
}

void CopyToGL(const std::vector<f32> &src, VertexBuffer *dst)
{
  StoreInGLBuffer((void*)src.data(), src.size() * sizeof(f32), dst->mHandle);
  dst->mBuffer.insert(dst->mBuffer.begin(), src.begin(), src.end());
}

void CopyToGL(const std::vector<u32> &src, IndexBuffer32 *dst)
{
  StoreInGLBuffer((void*)src.data(), src.size() * sizeof(f32), dst->mHandle);
  dst->mBuffer.insert(dst->mBuffer.begin(), src.begin(), src.end());
}

void CopyToGL(const std::vector<u16> &src, IndexBuffer16 *dst)
{
  StoreInGLBuffer((void*)src.data(), src.size() * sizeof(f32), dst->mHandle);
  dst->mBuffer.insert(dst->mBuffer.begin(), src.begin(), src.end());
}

void CopyToGL(const std::vector<u8> &src, IndexBuffer8 *dst)
{
  StoreInGLBuffer((void*)src.data(), src.size() * sizeof(f32), dst->mHandle);
  dst->mBuffer.insert(dst->mBuffer.begin(), src.begin(), src.end());
}

std::vector<f32> CopyFromGL(const VertexBuffer &src)
{
  return { src.mBuffer.begin(), src.mBuffer.end() };
}

std::vector<u32> CopyFromGL(const IndexBuffer32 &src)
{
  return { src.mBuffer.begin(), src.mBuffer.end() };
}

std::vector<u16> CopyFromGL(const IndexBuffer16 &src)
{
  return { src.mBuffer.begin(), src.mBuffer.end() };
}

std::vector<u8> CopyFromGL(const IndexBuffer8 &src)
{
  return { src.mBuffer.begin(), src.mBuffer.end() };
}
#endif

} // namespace gl::buffer