#include "DX11Context.hpp"

#include "../../utils/FileUtils.hpp"

#include <D3dcompiler.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <d3d11.h>
#include <d3d11shader.h>
#include <wrl/client.h>

#define Check(x) assert(x == S_OK)
using Microsoft::WRL::ComPtr;

namespace focus
{

#if 0
Shader *CreateShaderFromSource(const std::string &vSource, const std::string &fSource)
{
  ComPtr<ID3DBlob> vBinary;
  ComPtr<ID3DBlob> vErrors;
  HR(D3DCompile(vSource.data(), vSource.size(), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vBinary, &vErrors));

  ComPtr<ID3DBlob> fBinary;
  ComPtr<ID3DBlob> fErrors;
  HR(D3DCompile(fSource.data(), fSource.size(), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &fBinary, &fErrors));

  Shader *shader = new Shader;
  HR(sDevice->CreateVertexShader(vBinary->GetBufferPointer(), vBinary->GetBufferSize(), nullptr, &shader->mVertexShader));
  HR(sDevice->CreatePixelShader(fBinary->GetBufferPointer(), fBinary->GetBufferSize(), nullptr, &shader->mPixelShader));

  ComPtr<ID3D11ShaderReflection> vertexReflection;
  ComPtr<ID3D11ShaderReflection> pixelReflection;

//  HR(D3DReflect(vBinary->GetBufferPointer(), vBinary->GetBufferSize(), IID_ID3D11ShaderReflection, &vertexReflection));
//  HR(D3DReflect(fBinary->GetBufferPointer(), fBinary->GetBufferSize(), IID_ID3D11ShaderReflection, &pixelReflection));

  return shader;
}
#endif

void focus::DX11Context::Init()
{
}

focus::Window focus::DX11Context::MakeWindow(s32 width, s32 height)
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
  mContext->OMSetRenderTargets(1, &mBackBufferRenderTargetView, mDepthStencilView.Get());

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
  throw std::logic_error("The method or operation is not implemented.");
}

ShaderHandle DX11Context::CreateComputeShaderFromSource(const char *name, const std::string &source)
{
  throw std::logic_error("The method or operation is not implemented.");
}

VertexBufferHandle DX11Context::CreateVertexBuffer(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor)
{
  throw std::logic_error("The method or operation is not implemented.");
}

IndexBufferHandle DX11Context::CreateIndexBuffer(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor)
{
  throw std::logic_error("The method or operation is not implemented.");
}

BufferHandle DX11Context::CreateShaderBuffer(void *data, u32 sizeInBytes, ShaderBufferDescriptor descriptor)
{
  throw std::logic_error("The method or operation is not implemented.");
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
  throw std::logic_error("The method or operation is not implemented.");
}

void DX11Context::DestroyIndexBuffer(IndexBufferHandle handle)
{
  throw std::logic_error("The method or operation is not implemented.");
}

void DX11Context::Draw(Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState &sceneState)
{
  throw std::logic_error("The method or operation is not implemented.");
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
  throw std::logic_error("The method or operation is not implemented.");
}

void DX11Context::SwapBuffers(const Window &window)
{
  throw std::logic_error("The method or operation is not implemented.");
}

} // namespace focus
