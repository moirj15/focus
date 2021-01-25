#ifndef FOCUS_BUFFERMANAGER_H
#define FOCUS_BUFFERMANAGER_H

#include "../Interface/Context.hpp"

#include <d3d11.h>
#include <dxgi.h>
#include <unordered_map>
#include <wrl/client.h>

namespace focus::dx11
{

using Microsoft::WRL::ComPtr;
template<typename HandleType, typename DescriptorType, D3D11_BIND_FLAG BindFlag>
struct BufferManager
{
  HandleType mCurrHandle = 0;
  std::unordered_map<HandleType, ComPtr<ID3D11Buffer>> mBuffers;
  std::unordered_map<HandleType, DescriptorType> mDescriptors;
  ID3D11Device *mDevice;

  BufferManager() = default; // TODO: temp hack for constructing D3D11Context
  BufferManager(ID3D11Device *device) : mDevice(device) {}

  inline HandleType Create(void *data, u32 sizeInBytes, DescriptorType descriptor)
  {
    D3D11_BUFFER_DESC bufferDesc = {
        .ByteWidth = sizeInBytes,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = BindFlag,
        .CPUAccessFlags = 0, // TODO: figure out api for dyanmic buffers
        .MiscFlags = 0,
    };

    D3D11_SUBRESOURCE_DATA initData = {
        .pSysMem = data,
        .SysMemPitch = 0,
        .SysMemSlicePitch = 0,
    };
    ComPtr<ID3D11Buffer> buffer;
    mDevice->CreateBuffer(&bufferDesc, &initData, &buffer);
    // Do the actual management of the buffer handle
    mCurrHandle++;
    mDescriptors[mCurrHandle] = descriptor;
    mBuffers[mCurrHandle] = buffer;
    return mCurrHandle;
  }

  inline ID3D11Buffer *Get(HandleType handle) { return mBuffers[handle].Get(); }

  // TODO: read-write access
#if 0
  inline void WriteTo(void *data, u32 sizeInBytes, Handle handle) { WriteTo(data, sizeInBytes, 0, handle); }
  inline void WriteTo(void *data, u32 sizeInBytes, u32 offsetInBytes, Handle handle)
  {
    auto bufferHandle = mHandles[handle];
    auto descriptor = mDescriptors[handle];
    assert(descriptor.mSizeInBytes >= (sizeInBytes + offsetInBytes));
    glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
    glBufferSubData(GL_ARRAY_BUFFER, offsetInBytes, sizeInBytes, data);
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
#endif

  inline void Destroy(HandleType handle)
  {
    mBuffers.erase(handle);
    mDescriptors.erase(handle);
  }


};

} // namespace focus::dx11
#endif // FOCUS_BUFFERMANAGER_H
