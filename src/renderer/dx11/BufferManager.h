#pragma once
#include "../Interface/Context.hpp"
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
  HandleType mCurrHandle = 0;
  std::unordered_map<HandleType, ComPtr<ID3D11Buffer>> mBuffers;
  std::unordered_map<HandleType, DescriptorType> mDescriptors;
  ID3D11Device *mDevice;
  // TODO: would have to switch to a deferred context for multithreading
  ID3D11DeviceContext *mContext;

  BufferManager() = default; // TODO: temp hack for constructing D3D11Context
  BufferManager(ID3D11Device *device) : mDevice(device) {}

  inline HandleType Create(void *data, u32 sizeInBytes, DescriptorType descriptor)
  {
    return Create(data, sizeInBytes, descriptor, 0);
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

  inline void Update(HandleType handle, void *data, u32 sizeInBytes)
  {
    auto buffer = mBuffers[handle];
    auto descriptor = mDescriptors[handle];
    if (descriptor.sizeInBytes < sizeInBytes) {
      buffer.Reset();
      Create(data, sizeInBytes, descriptor, handle);
    } else {
      D3D11_MAPPED_SUBRESOURCE mappedResource;
      Check(mContext->Map(buffer.Get(), 0, D3D11_MAP_READ_WRITE, 0, &mappedResource));
      auto *mappedData = (u8*)mappedResource.pData;
      for (u32 i = 0; i < sizeInBytes; i += mappedResource.RowPitch) {
        memcpy(((u8*)mappedResource.pData) + i, ((u8*)data) + i, mappedResource.RowPitch);
      }
      mContext->Unmap(mBuffers[handle].Get(), 0);
    }
  }

  inline void Destroy(HandleType handle)
  {
    mBuffers.erase(handle);
    mDescriptors.erase(handle);
  }

private:
  inline HandleType Create(void *data, u32 sizeInBytes, DescriptorType descriptor, HandleType handle)
  {
    D3D11_BUFFER_DESC bufferDesc = {
        .ByteWidth = sizeInBytes,
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
