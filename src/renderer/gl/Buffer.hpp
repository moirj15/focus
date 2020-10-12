#pragma once

#include "../../common.h"
#include "Types.hpp"
#include "../Interface/IBuffer.hpp"

#include <vector>

namespace gl
{

// TODO: remove
//using Handle = u32;
//struct VertexBuffer : public renderer::IVertexBuffer {
//  Handle mHandle;
//};
//
//template<typename T>
//struct IndexBuffer : public renderer::IIndexBuffer<T> {
//  std::vector<T> mBuffer;
//  Handle mHandle;
//  bool mDirty;
//  static constexpr u32 mSize = sizeof(T);
//};

//using IndexBuffer32 = IndexBuffer<u32>;
//using IndexBuffer16 = IndexBuffer<u16>;
//using IndexBuffer8 = IndexBuffer<u8>;

//struct PixelBuffer {
//  // TODO
//  Handle mHandle;
//  bool mDirty;
//};
//
//struct UniformBuffer {
//  Handle mHandle;
//  bool mDirty;
//};
//
//namespace buffer
//{
//
//void CopyToGL(const std::vector<f32> &src, VertexBuffer *dst);
//void CopyToGL(const std::vector<u32> &src, IndexBuffer32 *dst);
//void CopyToGL(const std::vector<u16> &src, IndexBuffer16 *dst);
//void CopyToGL(const std::vector<u8> &src, IndexBuffer8 *dst);
//
//std::vector<f32> CopyFromGL(const VertexBuffer &src);
//std::vector<u32> CopyFromGL(const IndexBuffer32 &src);
//std::vector<u16> CopyFromGL(const IndexBuffer16 &src);
//std::vector<u8> CopyFromGL(const IndexBuffer8 &src);
//
//} // namespace buffer

} // namespace gl
