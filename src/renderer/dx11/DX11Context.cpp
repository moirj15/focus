#include "DX11Context.hpp"

#include "../../utils/FileUtils.hpp"
#include "DX11Context.hpp"
#include "Utils.hpp"

#include <D3dcompiler.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <d3d11.h>
#include <d3d11shader.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace focus::dx11
{

DX11Context::DX11Context()
{
  u32 createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
  D3D_FEATURE_LEVEL desiredLevel[] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};
  D3D_FEATURE_LEVEL featureLevel;
  // TODO: get latest dx11
  Check(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &mDevice,
      &featureLevel, &mContext));
  assert(featureLevel == D3D_FEATURE_LEVEL_11_0);

  mShaderManager = ShaderManager(mDevice.Get());
  mVBManager = BufferManager<VertexBufferHandle, VertexBufferDescriptor, D3D11_BIND_VERTEX_BUFFER>(mDevice.Get());
  mIBManager = BufferManager<IndexBufferHandle, IndexBufferDescriptor, D3D11_BIND_INDEX_BUFFER>(mDevice.Get());
  mCBManager = BufferManager<ConstantBufferHandle, ConstantBufferDescriptor, D3D11_BIND_CONSTANT_BUFFER>(mDevice.Get());
  mSBManager = ShaderBufferManager(mDevice.Get());

  // TODO: temporary rasterizer state, should create a manager for this, maybe create a handle type for this?
  // TODO: maybe an internal handle just for tracking this internaly?
  D3D11_RASTERIZER_DESC rasterizerDesc = {
    .FillMode = D3D11_FILL_SOLID,
    .CullMode = D3D11_CULL_NONE,
    .FrontCounterClockwise = true,
  };
  mDevice->CreateRasterizerState(&rasterizerDesc, &mRasterizerState);


}

void DX11Context::Init()
{
}

Window DX11Context::MakeWindow(s32 width, s32 height)
{
  SDL_Window *window =
      SDL_CreateWindow("DX12", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

  assert(window != nullptr);

  // Don't resize window so it doesn't mess with tiling window managers
  SDL_SetWindowResizable(window, SDL_FALSE);

  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(window, &wmInfo);
  HWND hwnd = wmInfo.info.win.window;

  // clang-format off
  DXGI_SWAP_CHAIN_DESC swapChainDesc = {
      .BufferDesc = {
          .Width = (u32)width,
          .Height = (u32)height,
          .RefreshRate = {
              .Numerator = 60,
              .Denominator = 1,
          },
          .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
          .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
          .Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
      },
      // Multi sampling would be initialized here
      .SampleDesc = {
          .Count = 1,
          .Quality = 0,
      },
      .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
      .BufferCount = 1,
      .OutputWindow = hwnd,
      .Windowed = true,
      .SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
      .Flags = 0,
  };
  // clang-format on

  ComPtr<IDXGIDevice> dxgiDevice;
  Check(mDevice->QueryInterface(__uuidof(IDXGIDevice), &dxgiDevice));
  ComPtr<IDXGIAdapter> adapter;
  Check(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), &adapter));
  ComPtr<IDXGIFactory> factory;
  Check(adapter->GetParent(__uuidof(IDXGIFactory), &factory));

  Check(factory->CreateSwapChain(mDevice.Get(), &swapChainDesc, &mSwapChain));

  mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &mBackBuffer);
  mDevice->CreateRenderTargetView(mBackBuffer.Get(), 0, &mBackBufferRenderTargetView);

  // clang-format off
  D3D11_TEXTURE2D_DESC depthStencilDesc = {
      .Width = (u32)width,
      .Height = (u32)height,
      .MipLevels = 1,
      .ArraySize = 1,
      .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
      // Multi sampling here
      .SampleDesc = {
          .Count = 1,
          .Quality = 0,
      },
      .Usage = D3D11_USAGE_DEFAULT,
      .BindFlags = D3D11_BIND_DEPTH_STENCIL,
      .CPUAccessFlags = 0,
      .MiscFlags = 0,
  };
  // clang-format on

  Check(mDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));
  Check(mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), 0, &mDepthStencilView));
  mContext->OMSetRenderTargets(1, mBackBufferRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

  // TODO: be careful about setting this to something else down the road. Currently not checking for changes
  mViewport = {
      .TopLeftX = 0.0f,
      .TopLeftY = 0.0f,
      .Width = (f32)width,
      .Height = (f32)height,
      .MinDepth = 0.0f,
      .MaxDepth = 1.0f,
  };

  mContext->RSSetViewports(1, &mViewport);

  return {
      .mWidth = width,
      .mHeight = height,
      .mSDLWindow = window,
  };
}

ShaderHandle DX11Context::CreateShaderFromBinary(const char *vBinary, const char *fBinary)
{
  throw std::logic_error("The method or operation is not implemented.");
}

ShaderHandle DX11Context::CreateShaderFromSource(
    const char *name, const std::string &vSource, const std::string &fSource)
{
  return mShaderManager.AddShader(name, vSource, fSource);
}

ShaderHandle DX11Context::CreateComputeShaderFromSource(const char *name, const std::string &source)
{
  return mShaderManager.AddComputeShader(name, source);
}

VertexBufferHandle DX11Context::CreateVertexBuffer(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor)
{
  return mVBManager.Create(data, sizeInBytes, descriptor);
}

IndexBufferHandle DX11Context::CreateIndexBuffer(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor)
{
  return mIBManager.Create(data, sizeInBytes, descriptor);
}

BufferHandle DX11Context::CreateShaderBuffer(void *data, u32 sizeInBytes, ShaderBufferDescriptor descriptor)
{
  return mSBManager.Create(data, sizeInBytes, descriptor);
}

ConstantBufferHandle DX11Context::CreateConstantBuffer(void *data, u32 sizeInBytes, ConstantBufferDescriptor descriptor) 
{
  return mCBManager.Create(data, sizeInBytes, descriptor);
}

void *DX11Context::MapBufferPtr(BufferHandle handle, AccessMode accessMode)
{
  throw std::logic_error("The method or operation is not implemented.");
}

void DX11Context::UnmapBufferPtr(BufferHandle handle)
{
  throw std::logic_error("The method or operation is not implemented.");
}

void DX11Context::DestroyVertexBuffer(VertexBufferHandle handle)
{
  mVBManager.Destroy(handle);
}

void DX11Context::DestroyIndexBuffer(IndexBufferHandle handle)
{
  mIBManager.Destroy(handle);
}

void DX11Context::DestroyShaderBuffer(BufferHandle handle)
{
  mSBManager.Destroy(handle);
}

void DX11Context::Draw(Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState &sceneState)
{
  // TODO: some kinda state tracking
  mContext->OMSetRenderTargets(1, mBackBufferRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

  auto programs = mShaderManager.GetProgram(shader);

  // Setup the shader state
  mContext->VSSetShader(programs.vertexShader.Get(), nullptr, 0);
  mContext->PSSetShader(programs.pixelShader.Get(), nullptr, 0);

  mContext->IASetInputLayout(programs.inputLayout.Get());
  mContext->IASetPrimitiveTopology(PrimitiveToD3D11(primitive));

  mContext->RSSetState(mRasterizerState.Get());


  for (auto vbHandle : sceneState.vbHandles) {
    // TODO: need to start storing the stride in the descriptor or someplace else
    auto *vBuffer = mVBManager.Get(vbHandle);
    auto vbDesc = mVBManager.mDescriptors[vbHandle];
    u32 offset = 0;
    mContext->IASetVertexBuffers(0, 1, &vBuffer, &programs.inputStride, &offset);
  }
  for (auto cbHandle : sceneState.cbHandles) {
    // TODO: figure out a good way to do this for different shader stages
    // TODO: also need to handle when a shader stage takes multible constant buffers
    auto *cBuffer = mCBManager.Get(cbHandle);
    mContext->VSSetConstantBuffers(0, 1, &cBuffer);
  }
  auto ibDesc = mIBManager.mDescriptors[sceneState.ibHandle];
  auto *iBuffer = mIBManager.Get(sceneState.ibHandle);
  mContext->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);
  mContext->Draw(ibDesc.sizeInBytes / 4, 0);
}

void DX11Context::DispatchCompute(
    u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, const ComputeState &computeState)
{
  throw std::logic_error("The method or operation is not implemented.");
}

void DX11Context::WaitForMemory(u64 flags)
{
  throw std::logic_error("The method or operation is not implemented.");
}

void DX11Context::Clear(ClearState clearState)
{
  if ((u32)clearState.toClear & (u32)ClearBuffer::Color) {
    mContext->ClearRenderTargetView(mBackBufferRenderTargetView.Get(), (float *)&clearState.clearColor);
  }
  if ((u32)clearState.toClear & (u32)ClearBuffer::DepthStencil) {
    // TODO: need to add a depth to ClearState
    mContext->ClearDepthStencilView(mDepthStencilView.Get(), ClearBufferToD3D11(clearState.toClear), 1.0f, 0);
  }
}

void DX11Context::SwapBuffers(const Window &window)
{
  mSwapChain->Present(1, 0);
  //SDL_GL_SwapWindow(window.mSDLWindow);
}

} // namespace focus::dx11
