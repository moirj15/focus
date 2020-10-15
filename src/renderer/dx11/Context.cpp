#include "Context.hpp"

#include <d3d11.h>
#include <d3d11shader.h>
#include <D3dcompiler.h>
#include <wrl/client.h>
#include "../../utils/FileUtils.hpp"

#define HR(X) assert(!FAILED(X))
using Microsoft::WRL::ComPtr;

namespace dx11::context
{

static WNDPROC sMessageHandler;
static HINSTANCE sInstanceHandle;
static ComPtr<ID3D11Device> sDevice;
static ComPtr<ID3D11DeviceContext> sContext;
static ComPtr<IDXGISwapChain> sDefaultSwapChain;
static ComPtr<ID3D11Texture2D> sDefaultDepthStencilBuffer;
static ComPtr<ID3D11DepthStencilView> sDefaultDepthStencilView;
static ComPtr<ID3D11RenderTargetView> sDefaultRenderTargetView;

void Init(WNDPROC messageHandler, HINSTANCE instanceHandle)
{
  sMessageHandler = messageHandler;
  sInstanceHandle = instanceHandle;

  u32 createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG /*| D3D11_CREATE_DEVICE_DEBUGGABLE*/;
#endif
  D3D_FEATURE_LEVEL desiredLevel[] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};
  D3D_FEATURE_LEVEL featureLevel;
  ID3D11Device *device;
  ID3D11DeviceContext *context;
  // TODO: get latest dx11
  HR(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION,
      (ID3D11Device **)&device, &featureLevel, (ID3D11DeviceContext **)&context));
  assert(featureLevel == D3D_FEATURE_LEVEL_11_0);
  sDevice = device;
  sContext = context;
}

renderer::Window MakeWindow(s32 width, s32 height)
{
  WNDCLASS windowClass = {
      .style = CS_HREDRAW | CS_VREDRAW,
      .lpfnWndProc = sMessageHandler,
      .hInstance = sInstanceHandle,
      .hIcon = LoadIcon(0, IDI_APPLICATION),
      .hCursor = LoadCursor(0, IDC_ARROW),
      .hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH),
      .lpszClassName = "W",
  };
  assert(RegisterClass(&windowClass));
  HWND windowHandle = CreateWindow(
      "W", "W", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, sInstanceHandle, 0);
  assert(windowHandle);
  ShowWindow(windowHandle, true);
  UpdateWindow(windowHandle);

  DXGI_SWAP_CHAIN_DESC swapChainDesc = {
      .BufferDesc =
          {
              .Width = (u32)width,
              .Height = (u32)height,
              .RefreshRate =
                  {
                      .Numerator = 60,
                      .Denominator = 1,
                  },
              .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
              .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
              .Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
          },
      // Multi sampling would be initialized here
      .SampleDesc =
          {
              .Count = 1,
              .Quality = 0,
          },
      .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
      .BufferCount = 1,
      .OutputWindow = windowHandle,
      .Windowed = true,
      .SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
      .Flags = 0,
  };

  ComPtr<IDXGIDevice> dxgiDevice;
  HR(sDevice->QueryInterface(__uuidof(IDXGIDevice), &dxgiDevice));
  ComPtr<IDXGIAdapter> adapter;
  HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), &adapter));
  ComPtr<IDXGIFactory> factory;
  HR(adapter->GetParent(__uuidof(IDXGIFactory), &factory));

  HR(factory->CreateSwapChain(sDevice.Get(), &swapChainDesc, &sDefaultSwapChain));

  ComPtr<ID3D11Texture2D> backBuffer;
  sDefaultSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
  sDevice->CreateRenderTargetView(backBuffer.Get(), 0, &sDefaultRenderTargetView);

  D3D11_TEXTURE2D_DESC depthStencilDesc = {
      .Width = (u32)width,
      .Height = (u32)height,
      .MipLevels = 1,
      .ArraySize = 1,
      .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
      // Multi sampling here
      .SampleDesc =
          {
              .Count = 1,
              .Quality = 0,
          },
      .Usage = D3D11_USAGE_DEFAULT,
      .BindFlags = D3D11_BIND_DEPTH_STENCIL,
      .CPUAccessFlags = 0,
      .MiscFlags = 0,
  };

  HR(sDevice->CreateTexture2D(&depthStencilDesc, 0, &sDefaultDepthStencilBuffer));
  HR(sDevice->CreateDepthStencilView(sDefaultDepthStencilBuffer.Get(), 0, &sDefaultDepthStencilView));
  sContext->OMSetRenderTargets(1, &sDefaultRenderTargetView, sDefaultDepthStencilView.Get());

  D3D11_VIEWPORT viewPort = {
      .TopLeftX = 0.0f,
      .TopLeftY = 0.0f,
      .Width = (f32)width,
      .Height = (f32)height,
      .MinDepth = 0.0f,
      .MaxDepth = 1.0f,
  };

  sContext->RSSetViewports(1, &viewPort);

  return {
      .mWidth = width,
      .mHeight = height,
      .mWindowHandle = windowHandle,
  };
}

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

} // namespace renderer::dx11
