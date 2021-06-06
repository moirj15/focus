#pragma once

#include "../Interface/Context.hpp"
#include "Utils.hpp"

#include <d3d11_3.h>
#include <unordered_map>
#include <wrl/client.h>
namespace focus::dx11
{
using Microsoft::WRL::ComPtr;

// TODO: there must be a way to combine this and the other buffer manager
// TODO: figure out a better way to do this for different types of resource views
struct ShaderBufferManager {
  ID3D11Device3 *mDevice;
  ID3D11DeviceContext3 *mContext;
  std::unordered_map<ShaderBufferHandle, ComPtr<ID3D11Buffer>> mBuffers;
  std::unordered_map<ShaderBufferHandle, ShaderBufferDescriptor> mDescriptors;
  std::unordered_map<ShaderBufferHandle, ComPtr<ID3D11ShaderResourceView>> mResources;
  std::unordered_map<ShaderBufferHandle, ComPtr<ID3D11UnorderedAccessView1>> mRWResources;
  // TODO: temporary hacky solution for synchronous reads from compute shader results
  ComPtr<ID3D11Buffer> mStagingBuffer;
  u32 mStagingBufferSize = 0;
  ShaderBufferHandle mCurrentHandle{0};

  ShaderBufferManager() = default; // TODO: temp hack for constructing D3D11Context
  explicit ShaderBufferManager(ID3D11Device3 *device, ID3D11DeviceContext3 *context) :
      mDevice(device), mContext(context)
  {
  }

  inline ShaderBufferHandle Create(void *data, const ShaderBufferDescriptor &descriptor)
  {
    return Create(data, descriptor, ShaderBufferHandle{0});
  }

  void Update(ShaderBufferHandle handle, void *data, u32 sizeInBytes)
  {
    auto descriptor = mDescriptors[handle];
    if (descriptor.size_in_bytes < sizeInBytes) {
      Destroy(handle);
      descriptor.size_in_bytes = sizeInBytes;
      Create(data, descriptor, handle);
    } else {
      auto buffer = mBuffers[handle];
      D3D11_MAPPED_SUBRESOURCE mappedResource;
      Check(mContext->Map(buffer.Get(), 0, D3D11_MAP_READ_WRITE, 0, &mappedResource));
      auto *mappedData = (u8 *)mappedResource.pData;
      for (u32 i = 0; i < sizeInBytes; i += mappedResource.RowPitch) {
        memcpy(((u8 *)mappedResource.pData) + i, ((u8 *)data) + i, mappedResource.RowPitch);
      }
      mContext->Unmap(mBuffers[handle].Get(), 0);
    }
  }

  std::vector<u8> ReadAll(ShaderBufferHandle handle)
  {
    if (!mDescriptors.contains(handle)) {
      return {};
    }
    auto descriptor = mDescriptors[handle];
    if (mStagingBufferSize < descriptor.size_in_bytes) {
      mStagingBufferSize = descriptor.size_in_bytes;

      // TODO: cleanup
      mStagingBuffer.Reset();
      D3D11_BUFFER_DESC bufferDesc = {
          .ByteWidth = descriptor.size_in_bytes,
          .Usage = D3D11_USAGE_STAGING,
          .BindFlags = 0,
          .CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE, // TODO: figure out dynamic cpu access stuff
          .MiscFlags = 0,
          .StructureByteStride = 0,
      };
      Check(mDevice->CreateBuffer(&bufferDesc, nullptr, &mStagingBuffer));
    }
    std::vector<u8> data(mStagingBufferSize, 0);
    mContext->CopyResource(mStagingBuffer.Get(), mBuffers[handle].Get());
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    Check(mContext->Map(mStagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource));
    memcpy(data.data(), mappedResource.pData, data.size());
    mContext->Unmap(mStagingBuffer.Get(), 0);
    return data;
  }

  void Destroy(ShaderBufferHandle handle)
  {
    mBuffers.erase(handle);
    mResources.erase(handle);
    mRWResources.erase(handle);
  }

private:
  ShaderBufferHandle Create(void *data, const ShaderBufferDescriptor &descriptor, ShaderBufferHandle handle)
  {
    D3D11_BUFFER_DESC bufferDesc = {
        .ByteWidth = descriptor.size_in_bytes,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = (u32)(descriptor.accessMode == AccessMode::ReadOnly ? D3D11_BIND_SHADER_RESOURCE
                                                                         : D3D11_BIND_UNORDERED_ACCESS),
        .CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE, // TODO: figure out dynamic cpu access stuff
        .MiscFlags = 0,
        .StructureByteStride = 0,
    };

    ID3D11Buffer *buffer;
    if (data) {
      D3D11_SUBRESOURCE_DATA initData = {
          .pSysMem = data,
          .SysMemPitch = 0,
          .SysMemSlicePitch = 0,
      };

      Check(mDevice->CreateBuffer(&bufferDesc, &initData, &buffer));
    } else {
      Check(mDevice->CreateBuffer(&bufferDesc, nullptr, &buffer));
    }
    if (handle == 0) {
      mCurrentHandle++;
      mBuffers.emplace(mCurrentHandle, buffer);
      mDescriptors.emplace(mCurrentHandle, descriptor);
      mDescriptors[mCurrentHandle].size_in_bytes = descriptor.size_in_bytes;
    } else {
      mBuffers[handle] = buffer;
      mDescriptors[handle] = descriptor;
      mDescriptors[handle].size_in_bytes = descriptor.size_in_bytes;
    }
    // TODO: need to find a better way of passing in the size, should it be in the descriptor or passed as a parameter?
    if (descriptor.accessMode == AccessMode::ReadOnly) {
      // clang-format off
      D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {
          .Format = utils::FocusToDXGIFormat(descriptor.types[0]), // TODO: see if this should be specified
          .ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX,
          .BufferEx = {
              .FirstElement = 0,
              .NumElements = descriptor.size_in_bytes / utils::BytesPerVarType(descriptor.types[0]),
              .Flags = 0,
          }, // TODO: this is probably wrong
      };
      // clang-format on
      ID3D11ShaderResourceView *resourceView;
      Check(mDevice->CreateShaderResourceView(buffer, &viewDesc, &resourceView));
      if (handle == 0) {
        mResources.emplace(mCurrentHandle, resourceView);
      } else {
        mResources[handle] = resourceView;
      }
    } else if (descriptor.accessMode == AccessMode::ReadWrite || descriptor.accessMode == AccessMode::WriteOnly) {
      // clang-format off
      D3D11_UNORDERED_ACCESS_VIEW_DESC1 viewDesc = {
          .Format = utils::FocusToDXGIFormat(descriptor.types[0]),
          .ViewDimension = D3D11_UAV_DIMENSION_BUFFER,
          .Buffer = {
              .FirstElement = 0,
              .NumElements = descriptor.size_in_bytes / utils::BytesPerVarType(descriptor.types[0]),
              .Flags = 0,
          }, // TODO: this is probably wrong
      };
      // clang-format on
      ID3D11UnorderedAccessView1 *resourceView;
      Check(mDevice->CreateUnorderedAccessView1(buffer, &viewDesc, &resourceView));
      if (handle == 0) {
        mRWResources.emplace(mCurrentHandle, resourceView);
      } else {
        mRWResources[handle] = resourceView;
      }
    } else {
      assert(0 && "Illegal Access Mode");
    }
    return mCurrentHandle;
  }
};

} // namespace focus::dx11