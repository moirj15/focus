#pragma once
#include "../Interface/focus.hpp"
#if 0
#include <d3d12.h>
#include <dxgi.h>
#include <wrl/client.h>
using namespace Microsoft::WRL;
#endif
namespace focus
{
#if 0
class DX12Context : public Device
{
  ComPtr<ID3D12Device> mDevice;
  ComPtr<ID3D12Fence> mFence;
  uint32_t mRtvDescriptorSize = 0;       // render target resources
  uint32_t mDsvDescriptorSize = 0;       // depth/stencil resources
  uint32_t mCbvSrvUavDescriptorSize = 0; // constant buffer/Shade resource/unordered random access resources
  uint32_t m4xMsaaQualityLevels = 0;
  static constexpr uint32_t mSwapChainBufferCount = 2;
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

  void Init();

  ShaderHandle CreateShaderFromSource(
      const char *name, const std::string &vSource, const std::string &fSource) override;

  ShaderHandle CreateComputeShaderFromSource(const char *name, const std::string &source) override;

  VertexBufferHandle CreateVertexBuffer(void *data, VertexBufferDescriptor descriptor) override;

  IndexBufferHandle CreateIndexBuffer(void *data, IndexBufferDescriptor descriptor) override;

  ShaderBufferHandle CreateShaderBuffer(void *data, ShaderBufferDescriptor descriptor) override;

  void *MapBuffer(ShaderBufferHandle handle, AccessMode accessMode) override;

  void UnmapBuffer(ShaderBufferHandle handle) override;

  void DestroyVertexBuffer(VertexBufferHandle handle) override;

  void DestroyIndexBuffer(IndexBufferHandle handle) override;

  void Draw(Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState &sceneState) override;

  void DispatchCompute(
      u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, const ComputeState &computeState) override;

  void WaitForMemory(u64 flags) override;

  void ClearBackBuffer(ClearState clearState = {}) override;

  void SwapBuffers(const Window &window) override;
};
#endif
} // namespace focus
