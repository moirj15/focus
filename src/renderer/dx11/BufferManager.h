#pragma once
#include "../Interface/FocusBackend.hpp"
#include "Utils.hpp"

#include <cassert>
#include <d3d11.h>
#include <dxgi.h>
#include <unordered_map>
#include <wrl/client.h>

namespace focus::dx11
{

using Microsoft::WRL::ComPtr;
template<typename HandleType, typename DescriptorType, D3D11_BIND_FLAG BindFlag>
struct BufferManager {
  HandleType mCurrHandle{0};
  std::unordered_map<HandleType, ComPtr<ID3D11Buffer>> mBuffers;
  std::unordered_map<HandleType, DescriptorType> mDescriptors;
  ID3D11Device *mDevice;
  // TODO: would have to switch to a deferred context for multithreading
  ID3D11DeviceContext *mContext;

  BufferManager() = default; // TODO: temp hack for constructing D3D11Context
  BufferManager(ID3D11Device *device, ID3D11DeviceContext *context) : mDevice(device), mContext(context) {}

  inline HandleType Create(void *data, DescriptorType descriptor)
  {
    return Create(HandleType{0}, descriptor, data, 0);
//    D3D11_BUFFER_DESC bufferDesc = {
//        .ByteWidth = sizeInBytes,
//        .Usage = D3D11_USAGE_DYNAMIC, // making everything dynamic for now, need to look at adding a usage member in the
//                                      // descriptors
//        .BindFlags = BindFlag,
//        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE, // just using write access for now for updating bufferers
//        .MiscFlags = 0,
//    };
//
//    D3D11_SUBRESOURCE_DATA initData = {
//        .pSysMem = data,
//        .SysMemPitch = 0,
//        .SysMemSlicePitch = 0,
//    };
//    ComPtr<ID3D11Buffer> buffer;
//    mDevice->CreateBuffer(&bufferDesc, &initData, &buffer);
//    // Do the actual management of the buffer handle
//    mCurrHandle++;
//    mDescriptors[mCurrHandle] = descriptor;
//    mBuffers[mCurrHandle] = buffer;
//    return mCurrHandle;
  }

  inline ID3D11Buffer *Get(HandleType handle) { return mBuffers[handle].Get(); }

  inline void Update(HandleType handle, void *data, uint32_t sizeInBytes)
  {
    auto buffer = mBuffers[handle];
    auto descriptor = mDescriptors[handle];
    if (descriptor.size_in_bytes < sizeInBytes) {
      buffer.Reset();
      descriptor.size_in_bytes = sizeInBytes;
      Create(handle, descriptor, data, 0);
    } else {
      D3D11_MAPPED_SUBRESOURCE mappedResource;
      Check(mContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
      auto *mappedData = (uint8_t*)mappedResource.pData;
      for (uint32_t i = 0; i < sizeInBytes; i += mappedResource.RowPitch) {
        memcpy(((uint8_t*)mappedResource.pData) + i, ((uint8_t*)data) + i, mappedResource.RowPitch);
      }
      mContext->Unmap(buffer.Get(), 0);
    }
  }

  inline void Destroy(HandleType handle)
  {
    mBuffers.erase(handle);
    mDescriptors.erase(handle);
  }

private:
  inline HandleType Create(HandleType handle, DescriptorType descriptor, void *data, uint32_t size_in_bytes)
  {
    D3D11_BUFFER_DESC bufferDesc = {
        .ByteWidth = size_in_bytes,
        .Usage = D3D11_USAGE_DYNAMIC, // making everything dynamic for now, need to look at adding a usage member in the
                                      // descriptors
        .BindFlags = BindFlag,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE, // just using write access for now for updating bufferers
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
    if (handle == 0) {
      mCurrHandle++;
      mDescriptors[mCurrHandle] = descriptor;
      mBuffers[mCurrHandle] = buffer;
      return mCurrHandle;
    } else {
      mDescriptors[handle] = descriptor;
      mBuffers[handle] = buffer;
      return handle;
    }
  }
};

} // namespace focus::dx11
