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
  Window make_window(s32 width, s32 height) override;

  void Init() override;

  ShaderHandle create_shader_from_binary(const char *vBinary, const char *fBinary) override;

  ShaderHandle create_shader_from_source(
      const char *name, const std::string &vSource, const std::string &fSource) override;

  ShaderHandle create_compute_shader_from_source(const char *name, const std::string &source) override;

  VertexBufferHandle create_vertex_buffer(void *data, VertexBufferDescriptor descriptor) override;

  IndexBufferHandle create_index_buffer(void *data, IndexBufferDescriptor descriptor) override;

  BufferHandle create_shader_buffer(void *data, ShaderBufferDescriptor descriptor) override;

  void *map_buffer(BufferHandle handle, AccessMode accessMode) override;

  void unmap_buffer(BufferHandle handle) override;

  void destroy_vertex_buffer(VertexBufferHandle handle) override;

  void destroy_index_buffer(IndexBufferHandle handle) override;

  void draw(Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState &sceneState) override;

  void dispatch_compute(
      u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, const ComputeState &computeState) override;

  void wait_for_memory(u64 flags) override;

  void clear_back_buffer(ClearState clearState = {}) override;

  void swap_buffers(const Window &window) override;
};
} // namespace focus
