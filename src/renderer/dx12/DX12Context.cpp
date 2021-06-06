#include "DX12Context.hpp"

#include <SDL2/SDL.h>
#include <cassert>

#define Check(x) assert(x == S_OK)

namespace focus
{

Window DX12Context::MakeWindow(s32 width, s32 height)
{
  SDL_Window *window =
      SDL_CreateWindow("DX12", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

  assert(window != nullptr);

  // Don't resize window so it doesn't mess with tiling window managers
  SDL_SetWindowResizable(window, SDL_FALSE);

  Init();

  return {
      .width = width,
      .height = height,
      .sdl_window = window,
  };
}

void DX12Context::Init()
{
#ifdef _DEBUG
  ComPtr<ID3D12Debug> debugLayer;
  Check(D3D12GetDebugInterface(__uuidof(ID3D12Debug), &debugLayer));
  debugLayer->EnableDebugLayer();
#endif
  // Create the dx12 device
  Check(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), &mDevice));

  // Create the DXGIfactory
  CreateDXGIFactory1(__uuidof(IDXGIFactory1), &mDxgiFactory);

  // Create the main fence for gpu cpu synchronization
  Check(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), &mFence));

  // Get the descriptor sizes for the various heaps
  mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  mDsvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
  mCbvSrvUavDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

  // Check for baseline of 4x msaa and set it
  D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels = {
      .Format = mBackBufferFormat,
      .SampleCount = 4,
      .Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE,
      .NumQualityLevels = 0,
  };
  Check(mDevice->CheckFeatureSupport(
      D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)));
  m4xMsaaQualityLevels = msQualityLevels.NumQualityLevels;
  assert(m4xMsaaQualityLevels > 0);

  // Create the command queue and command list
  D3D12_COMMAND_QUEUE_DESC queueDesc = {
      .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
      .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
  };
  Check(mDevice->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), &mCommandQueue));
  Check(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator),
      (void **)mDirectCommandListAllocator.GetAddressOf()));
  Check(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mDirectCommandListAllocator.Get(), nullptr,
      __uuidof(ID3D12GraphicsCommandList), (void **)mCommandList.GetAddressOf()));
  // Needs to be closed be for the initial Reset() is called
  mCommandList->Close();

  // Create the swap chain
  mSwapChain.Reset();

  // clang-format off
  DXGI_SWAP_CHAIN_DESC sd = {
      .BufferDesc = {
          .Width = 1920, // TODO: move this into a better spot
          .Height = 1080,
          .RefreshRate = {
              .Numerator = 60,
              .Denominator = 1,
          },
          .Format = mBackBufferFormat,
          .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
      },
      .SampleDesc = {
          .Count = m4xMsaaQualityLevels,
          .Quality = m4xMsaaQualityLevels - 1,
      },
      .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
      .BufferCount = mSwapChainBufferCount,
      .OutputWindow = GetActiveWindow(),
      .Windowed = true,
      .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
      .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
  };
  // clang-format on
  Check(mDxgiFactory->CreateSwapChain(mCommandQueue.Get(), &sd, mSwapChain.GetAddressOf()));

  // Create the descriptor heaps
  D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {
      .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
      .NumDescriptors = mSwapChainBufferCount,
      .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
      .NodeMask = 0,
  };
  Check(mDevice->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)mRtvHeap.GetAddressOf()));

  D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {
      .Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
      .NumDescriptors = 1,
      .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
      .NodeMask = 0,
  };
  Check(mDevice->CreateDescriptorHeap(&dsvHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)mDsvHeap.GetAddressOf()));

  D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle = {
    .ptr = mRtvHeap->GetCPUDescriptorHandleForHeapStart().ptr,
  }; 
  for (s32 i = 0; i < mSwapChainBufferCount; i++) {
    Check(mSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), &mSwapChains[i]));

    mDevice->CreateRenderTargetView(mSwapChains[i].Get(), nullptr, rtvHeapHandle);
    rtvHeapHandle.ptr += mRtvDescriptorSize;
  }
}

ShaderHandle DX12Context::CreateShaderFromSource(
    const char *name, const std::string &vSource, const std::string &fSource)
{
  return ShaderHandle{INVALID_HANDLE};
}

ShaderHandle DX12Context::CreateComputeShaderFromSource(const char *name, const std::string &source)
{
  return ShaderHandle{INVALID_HANDLE};
}

VertexBufferHandle DX12Context::CreateVertexBuffer(void *data, VertexBufferDescriptor descriptor)
{
  return VertexBufferHandle{INVALID_HANDLE};
}

IndexBufferHandle DX12Context::CreateIndexBuffer(void *data, IndexBufferDescriptor descriptor)
{
  return IndexBufferHandle{INVALID_HANDLE};
}

ShaderBufferHandle DX12Context::CreateShaderBuffer(void *data, ShaderBufferDescriptor descriptor)
{
  return ShaderBufferHandle{INVALID_HANDLE};
}

void *DX12Context::MapBuffer(ShaderBufferHandle handle, AccessMode accessMode)
{
  return nullptr;
}

void DX12Context::UnmapBuffer(ShaderBufferHandle handle)
{
}

void DX12Context::DestroyVertexBuffer(VertexBufferHandle handle)
{
}

void DX12Context::DestroyIndexBuffer(IndexBufferHandle handle)
{
}

void DX12Context::Draw(Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState &sceneState)
{
}

void DX12Context::DispatchCompute(
    u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, const ComputeState &computeState)
{
}

void DX12Context::WaitForMemory(u64 flags)
{
}

void DX12Context::ClearBackBuffer(ClearState clearState /*= */)
{
}

void DX12Context::SwapBuffers(const Window &window)
{
}

} // namespace focus
