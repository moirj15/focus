#include "DX11Context.hpp"

#include "../../utils/FileUtils.hpp"
#include "DX11Context.hpp"
#include "Utils.hpp"

#include <D3dcompiler.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <d3d11_3.h>
#include <d3d11shader.h>
#include <vector>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace focus::dx11
{

DX11Context::DX11Context()
{
    uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_FEATURE_LEVEL desiredLevel[] = {D3D_FEATURE_LEVEL_11_1};
    D3D_FEATURE_LEVEL featureLevel;
    // TODO: get latest dx11
    ID3D11Device *baseDevice;
    ID3D11DeviceContext *baseContext;
    Check(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, desiredLevel, 1, D3D11_SDK_VERSION,
        &baseDevice, &featureLevel, &baseContext));
    assert(featureLevel == D3D_FEATURE_LEVEL_11_1);

    baseDevice->QueryInterface(__uuidof(ID3D11Device3), &mDevice);
    baseContext->QueryInterface(__uuidof(ID3D11DeviceContext3), &mContext);

    mShaderManager = ShaderManager(mDevice.Get());
    mVBManager =
        BufferManager<VertexBuffer, VertexBufferLayout, D3D11_BIND_VERTEX_BUFFER>(mDevice.Get(), mContext.Get());
    mIBManager = BufferManager<IndexBuffer, IndexBufferLayout, D3D11_BIND_INDEX_BUFFER>(mDevice.Get(), mContext.Get());
    mCBManager =
        BufferManager<ConstantBuffer, ConstantBufferLayout, D3D11_BIND_CONSTANT_BUFFER>(mDevice.Get(), mContext.Get());
    mSBManager = ShaderBufferManager(mDevice.Get(), mContext.Get());

    // TODO: temporary rasterizer state, should create a manager for this, maybe create a handle type for this?
    // TODO: maybe an internal handle just for tracking this internaly?
    D3D11_RASTERIZER_DESC rasterizerDesc = {
        .FillMode = D3D11_FILL_SOLID,
        .CullMode = D3D11_CULL_NONE,
        .FrontCounterClockwise = true,
    };
    mDevice->CreateRasterizerState(&rasterizerDesc, &mRasterizerState);
}

Window DX11Context::MakeWindow(int32_t width, int32_t height)
{
    SDL_Window *window =
        SDL_CreateWindow("DX11", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

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
          .Width = (uint32_t)width,
          .Height = (uint32_t)height,
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
      .Width = (uint32_t)width,
      .Height = (uint32_t)height,
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
        .Width = (float)width,
        .Height = (float)height,
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f,
    };

    mContext->RSSetViewports(1, &mViewport);

    return {
        .width = width,
        .height = height,
        .sdl_window = window,
    };
}

Shader DX11Context::CreateShaderFromBinary(const std::vector<uint8_t> &vBinary, const std::vector<uint8_t> &fBinary)
{
    return {};
}

Shader DX11Context::CreateShaderFromSource(const char *name, const std::string &vSource, const std::string &fSource)
{
    return mShaderManager.AddShader(name, vSource, fSource);
}

Shader DX11Context::CreateComputeShaderFromSource(const char *name, const std::string &source)
{
    return mShaderManager.AddComputeShader(name, source);
}

VertexBuffer DX11Context::CreateVertexBuffer(
    const VertexBufferLayout &vertex_buffer_layout, void *data, uint32_t data_size)
{
    return mVBManager.Create(data, vertex_buffer_layout);
}

IndexBuffer DX11Context::CreateIndexBuffer(
    const IndexBufferLayout &index_buffer_descriptor, void *data, uint32_t data_size)
{
    return mIBManager.Create(data, index_buffer_descriptor);
}

ConstantBuffer DX11Context::CreateConstantBuffer(
    const ConstantBufferLayout &constant_buffer_layout, void *data, uint32_t data_size)
{
    return mCBManager.Create(data, constant_buffer_layout);
}

ShaderBuffer DX11Context::CreateShaderBuffer(
    const ShaderBufferLayout &shader_buffer_layout, void *data, uint32_t data_size)
{
    // TODO: redo
    //    return mSBManager.Create(data, shader_buffer_layout);
    return {};
}

Pipeline DX11Context::CreatePipeline(PipelineState state)
{
    return {};
}

#if 0
void DX11Context::UpdateVertexBuffer(VertexBuffer handle, void *data, uint32_t size)
{
    mVBManager.Update(handle, data, size);
}
void DX11Context::UpdateIndexBuffer(IndexBuffer handle, void *data, uint32_t size)
{
    mIBManager.Update(handle, data, size);
}
void DX11Context::UpdateConstantBuffer(ConstantBuffer handle, void *data, uint32_t size)
{
    mCBManager.Update(handle, data, size);
}
void DX11Context::UpdateShaderBuffer(ShaderBuffer handle, void *data, uint32_t size)
{
    mSBManager.Update(handle, data, size);
}
#endif

void *DX11Context::MapBuffer(ShaderBuffer handle, AccessMode accessMode)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Check(mContext->Map(mSBManager.mBuffers[handle].Get(), 0, D3D11_MAP_READ_WRITE, 0, &mappedResource));
    return mappedResource.pData;
}

void DX11Context::UnmapBuffer(ShaderBuffer handle)
{
    mContext->Unmap(mSBManager.mBuffers[handle].Get(), 0);
}

void DX11Context::DestroyVertexBuffer(VertexBuffer handle)
{
    mVBManager.Destroy(handle);
}

void DX11Context::DestroyIndexBuffer(IndexBuffer handle)
{
    mIBManager.Destroy(handle);
}

void DX11Context::DestroyShaderBuffer(ShaderBuffer handle)
{
    mSBManager.Destroy(handle);
}

void DX11Context::DestroyConstantBuffer(ConstantBuffer handle)
{
    mCBManager.Destroy(handle);
}

void DX11Context::BeginPass(const std::string &name)
{
}

void DX11Context::BindSceneState(const SceneState &scene_state)
{
}
void DX11Context::BindPipeline(Pipeline pipeline)
{
}
void DX11Context::Draw(Primitive primitive)
{
}

void DX11Context::EndPass()
{
}

/*
void DX11Context::Draw(Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState &sceneState)
{
    // TODO: some kinda state tracking
    mContext->OMSetRenderTargets(1, mBackBufferRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

    auto programs = mShaderManager.GetProgram(shader);

    // Setup the shader state
    mContext->VSSetShader(programs.vertexShader.Get(), nullptr, 0);
    mContext->PSSetShader(programs.pixelShader.Get(), nullptr, 0);

    mContext->IASetInputLayout(programs.inputLayout.Get());
    mContext->IASetPrimitiveTopology(utils::PrimitiveToD3D11(primitive));

    mContext->RSSetState(mRasterizerState.Get());

    for (auto vbHandle : sceneState.vb_handles) {
        // TODO: need to start storing the stride in the descriptor or someplace else
        auto *vBuffer = mVBManager.Get(vbHandle);
        auto vbDesc = mVBManager.mDescriptors[vbHandle];
        u32 offset = 0;
        mContext->IASetVertexBuffers(0, 1, &vBuffer, &programs.inputStride, &offset);
    }
    for (auto cbHandle : sceneState.cb_handles) {
        // TODO: figure out a good way to do this for different shader stages
        // TODO: also need to handle when a shader stage takes multible constant buffers
        auto *cBuffer = mCBManager.Get(cbHandle);
        mContext->VSSetConstantBuffers(0, 1, &cBuffer);
    }
    auto ibDesc = mIBManager.mDescriptors[sceneState.ib_handle];
    auto *iBuffer = mIBManager.Get(sceneState.ib_handle);
    mContext->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);
    mContext->Draw(ibDesc.size_in_bytes / 4, 0);
}
*/

void DX11Context::DispatchCompute(
    uint32_t xGroups, uint32_t yGroups, uint32_t zGroups, Shader shader, const ComputeState &computeState)
{
    auto cs = mShaderManager.GetComputeShader(shader);
    mContext->CSSetShader(cs, nullptr, 0);
    std::vector<ID3D11ShaderResourceView *> readResources;
    std::vector<ID3D11UnorderedAccessView *> writeResources;
    for (auto bHandle : computeState.buffer_handles) {
        if (mSBManager.mResources.contains(bHandle)) {
            readResources.push_back(mSBManager.mResources[bHandle].Get());
        } else if (mSBManager.mRWResources.contains(bHandle)) {
            writeResources.push_back(mSBManager.mRWResources[bHandle].Get());
        }
    }
    mContext->CSSetShaderResources(0, readResources.size(), readResources.data());
    uint32_t uavInitialCounts = -1;
    mContext->CSSetUnorderedAccessViews(0, writeResources.size(), writeResources.data(), &uavInitialCounts);
    for (auto cbHandle : computeState.cb_handles) {
        // TODO: figure out a good way to do this for different shader stages
        // TODO: also need to handle when a shader stage takes multible constant buffers
        auto *cBuffer = mCBManager.Get(cbHandle);
        mContext->CSSetConstantBuffers(0, 1, &cBuffer);
    }
    mContext->Dispatch(xGroups, yGroups, zGroups);
}

void DX11Context::WaitForMemory(uint64_t flags)
{
    // Not needed for dx11?
}

void DX11Context::ClearBackBuffer(ClearState clearState)
{
    if ((uint32_t)clearState.to_clear & (uint32_t)ClearBuffer::Color) {
        mContext->ClearRenderTargetView(mBackBufferRenderTargetView.Get(), (float *)&clearState.clear_color);
    }
    if ((uint32_t)clearState.to_clear & (uint32_t)ClearBuffer::DepthStencil) {
        // TODO: need to add a depth to ClearState
        mContext->ClearDepthStencilView(
            mDepthStencilView.Get(), utils::ClearBufferToD3D11(clearState.to_clear), 1.0f, 0);
    }
}

void DX11Context::SwapBuffers(const Window &window)
{
    mSwapChain->Present(1, 0);
    // SDL_GL_SwapWindow(window.mSDLWindow);
}

} // namespace focus::dx11
