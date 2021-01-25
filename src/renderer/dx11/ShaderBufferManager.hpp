#pragma once

#include "../Interface/Context.hpp"
#include "Utils.hpp"

#include <d3d11.h>
#include <unordered_map>
#include <wrl/client.h>
namespace focus::dx11
{
using Microsoft::WRL::ComPtr;

// TODO: figure out a better way to do this for different types of resource views
class ShaderBufferManager
{
  ID3D11Device *mDevice;
  std::unordered_map<BufferHandle, ComPtr<ID3D11Buffer>> mBuffers;
  std::unordered_map<BufferHandle, ComPtr<ID3D11ShaderResourceView>> mResources;
  std::unordered_map<BufferHandle, ComPtr<ID3D11UnorderedAccessView>> mRWResources;
  BufferHandle mCurrentHandle = 0;

public:
  ShaderBufferManager() = default; // TODO: temp hack for constructing D3D11Context
  explicit ShaderBufferManager(ID3D11Device *device) : mDevice(device) {}

  inline BufferHandle Create(void *data, u32 sizeInBytes, ShaderBufferDescriptor descriptor)
  {
    D3D11_BUFFER_DESC bufferDesc = {
        .ByteWidth = sizeInBytes,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = (u32)(
            descriptor.accessMode == AccessMode::ReadOnly ? D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_UNORDERED_ACCESS),
        .CPUAccessFlags = 0, // TODO: figure out dynamic cpu access stuff
        .MiscFlags = 0,
    };

    D3D11_SUBRESOURCE_DATA initData = {
        .pSysMem = data,
        .SysMemPitch = 0,
        .SysMemSlicePitch = 0,
    };

    ID3D11Buffer *buffer;
    Check(mDevice->CreateBuffer(&bufferDesc, &initData, &buffer));
    mCurrentHandle++;
    mBuffers.emplace(mCurrentHandle, buffer);

    if (descriptor.accessMode == AccessMode::ReadOnly) {
      // clang-format off
      D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {
          .Format = DXGI_FORMAT_UNKNOWN, // TODO: see if this should be specified
          .ViewDimension = D3D11_SRV_DIMENSION_BUFFER,
          .BufferEx = {
                  .FirstElement = 0,
                  .NumElements = sizeInBytes,
                  .Flags = 0,
          }, // TODO: this is probably wrong
      };
      // clang-format on
      ComPtr<ID3D11ShaderResourceView> resourceView;
      Check(mDevice->CreateShaderResourceView(buffer, &viewDesc, &resourceView));
      mResources[mCurrentHandle] = resourceView;
    } else if (descriptor.accessMode == AccessMode::ReadWrite) {
      // clang-format off
      D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc = {
          .Format = DXGI_FORMAT_UNKNOWN,
          .ViewDimension = D3D11_UAV_DIMENSION_BUFFER,
          .Buffer = {
              .FirstElement = 0,
              .NumElements = sizeInBytes,
              .Flags = 0,
          }, // TODO: this is probably wrong
      };
      // clang-format on
      ComPtr<ID3D11UnorderedAccessView> resourceView;
      Check(mDevice->CreateUnorderedAccessView(buffer, &viewDesc, &resourceView));
      mRWResources[mCurrentHandle] = resourceView;
    } else {
      assert(0 && "Illegal Access Mode");
    }
    return mCurrentHandle;
  }

  void Destroy(BufferHandle handle) {
    mBuffers.erase(handle);
    mResources.erase(handle);
    mRWResources.erase(handle);
  }
};

} // namespace focus::dx11