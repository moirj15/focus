#pragma once
#include "../Interface/Context.hpp"

#include <d3d12.h>
#include <dxgi.h>
#include <wrl/client.h>
using namespace Microsoft::WRL;

namespace focus
{
class DX12Context : public Context
{
  ComPtr<ID3D12Device> mDevice;
  ComPtr<ID3D12Fence> mFence;
  u32 mRtvDescriptorSize = 0;       // render target resources
  u32 mDsvDescriptorSize = 0;       // depth/stencil resources
  u32 mCbvSrvUavDescriptorSize = 0; // constant buffer/Shade resource/unordered random access resources
  u32 m4xMsaaQualityLevels = 0;
  static constexpr u32 mSwapChainBufferCount = 2;
  int mCurrBackBuffer = 0;
  DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
  ComPtr<ID3D12CommandQueue> mCommandQueue;
  ComPtr<ID3D12CommandAllocator> mDirectCommandListAllocator;
  ComPtr<ID3D12GraphicsCommandList> mCommandList;
  ComPtr<IDXGIFactory1> mDxgiFactory;
  ComPtr<IDXGISwapChain> mSwapChain;
  ComPtr<ID3D12DescriptorHeap> mRtvHeap;
  ComPtr<ID3D12DescriptorHeap> mDsvHeap;
  ComPtr<ID3D12Resource> mSwapChains[mSwapChainBufferCount];

public:
  Window MakeWindow(s32 width, s32 height) override;

  void Init() override;

  ShaderHandle CreateShaderFromBinary(const char *vBinary, const char *fBinary) override;

  ShaderHandle CreateShaderFromSource(
      const char *name, const std::string &vSource, const std::string &fSource) override;

  ShaderHandle CreateComputeShaderFromSource(const char *name, const std::string &source) override;

  VertexBufferHandle CreateVertexBuffer(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor) override;

  IndexBufferHandle CreateIndexBuffer(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor) override;

  BufferHandle CreateShaderBuffer(void *data, u32 sizeInBytes, ShaderBufferDescriptor descriptor) override;

  void *MapBufferPtr(BufferHandle handle, AccessMode accessMode) override;

  void UnmapBufferPtr(BufferHandle handle) override;

  void DestroyVertexBuffer(VertexBufferHandle handle) override;

  void DestroyIndexBuffer(IndexBufferHandle handle) override;

  void Draw(Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState &sceneState) override;

  void DispatchCompute(
      u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, const ComputeState &computeState) override;

  void WaitForMemory(u64 flags) override;

  void Clear(ClearState clearState = {}) override;

  void SwapBuffers(const Window &window) override;
};
} // namespace focus
