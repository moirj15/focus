#pragma once

#include "../Interface/Context.hpp"
#include "Utils.hpp"

#include <d3d11_3.h>
#include <unordered_map>
#include <wrl/client.h>
namespace focus::dx11
{
using Microsoft::WRL::ComPtr;

// TODO: figure out a better way to do this for different types of resource views
struct ShaderBufferManager {
  ID3D11Device3 *mDevice;
  std::unordered_map<BufferHandle, ComPtr<ID3D11Buffer>> mBuffers;
  std::unordered_map<BufferHandle, ComPtr<ID3D11ShaderResourceView>> mResources;
  std::unordered_map<BufferHandle, ComPtr<ID3D11UnorderedAccessView1>> mRWResources;
  BufferHandle mCurrentHandle = 0;

  ShaderBufferManager() = default; // TODO: temp hack for constructing D3D11Context
  explicit ShaderBufferManager(ID3D11Device3 *device) : mDevice(device) {}

  inline BufferHandle Create(void *data, u32 sizeInBytes, ShaderBufferDescriptor descriptor)
  {
    D3D11_BUFFER_DESC bufferDesc = {
        .ByteWidth = sizeInBytes,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = (u32)(
            descriptor.accessMode == AccessMode::ReadOnly ? D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_UNORDERED_ACCESS),
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
    mCurrentHandle++;
    mBuffers.emplace(mCurrentHandle, buffer);

    if (descriptor.accessMode == AccessMode::ReadOnly) {
      // clang-format off
      D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {
          .Format = utils::FocusToDXGIFormat(descriptor.types[0]), // TODO: see if this should be specified
          .ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX,
          .BufferEx = {
                  .FirstElement = 0,
                  .NumElements = sizeInBytes / utils::BytesPerVarType(descriptor.types[0]),
                  .Flags = 0,
          }, // TODO: this is probably wrong
      };
      // clang-format on
      ID3D11ShaderResourceView *resourceView;
      Check(mDevice->CreateShaderResourceView(buffer, &viewDesc, &resourceView));
      mResources.emplace(mCurrentHandle, resourceView);
    } else if (descriptor.accessMode == AccessMode::ReadWrite || descriptor.accessMode == AccessMode::WriteOnly) {
      // clang-format off
      D3D11_UNORDERED_ACCESS_VIEW_DESC1 viewDesc = {
          .Format = utils::FocusToDXGIFormat(descriptor.types[0]),
          .ViewDimension = D3D11_UAV_DIMENSION_BUFFER,
          .Buffer = {
              .FirstElement = 0,
              .NumElements = sizeInBytes / utils::BytesPerVarType(descriptor.types[0]),
              .Flags = 0,
          }, // TODO: this is probably wrong
      };
      // clang-format on
      ID3D11UnorderedAccessView1 *resourceView;
      Check(mDevice->CreateUnorderedAccessView1(buffer, &viewDesc, &resourceView));
      mRWResources.emplace(mCurrentHandle, resourceView);
    } else {
      assert(0 && "Illegal Access Mode");
    }
    return mCurrentHandle;
  }

  void Destroy(BufferHandle handle)
  {
    mBuffers.erase(handle);
    mResources.erase(handle);
    mRWResources.erase(handle);
  }
};

} // namespace focus::dx11