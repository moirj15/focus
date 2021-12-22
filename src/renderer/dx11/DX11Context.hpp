#pragma once

#include "../Interface/focus.hpp"
#include "BufferManager.h"
#include "ShaderBufferManager.hpp"
#include "ShaderManager.hpp"

#include <d3d11.h>
#include <d3d11_3.h>
#include <dxgi.h>
#include <wrl/client.h>

namespace focus::dx11
{
using namespace Microsoft::WRL;

class DX11Context : public Device
{
    ComPtr<ID3D11Device3> mDevice;
    ComPtr<ID3D11DeviceContext3> mContext;

  private:
    ComPtr<IDXGISwapChain> mSwapChain;
    ComPtr<ID3D11Texture2D> mBackBuffer;
    ComPtr<ID3D11RenderTargetView> mBackBufferRenderTargetView;
    ComPtr<ID3D11Texture2D> mDepthStencilBuffer;
    ComPtr<ID3D11DepthStencilView> mDepthStencilView;
    ComPtr<ID3D11RasterizerState> mRasterizerState;
    D3D11_VIEWPORT mViewport;

    ShaderManager mShaderManager;
    /// Managers for read-only inputs
    BufferManager<VertexBuffer, VertexBufferLayout, D3D11_BIND_VERTEX_BUFFER> mVBManager;
    BufferManager<IndexBuffer, IndexBufferLayout, D3D11_BIND_INDEX_BUFFER> mIBManager;
    BufferManager<ConstantBuffer, ConstantBufferLayout, D3D11_BIND_CONSTANT_BUFFER> mCBManager;
    // TODO: find equivalent
    ShaderBufferManager mSBManager;

  public:
    DX11Context();

    inline ID3D11Device *GetDevice() { return mDevice.Get(); }
    inline ID3D11DeviceContext *GetContext() { return mContext.Get(); }

    // Window creation
    Window MakeWindow(int32_t width, int32_t height) override;

    // Shader creation

    // TODO: need to create a shader compile tool
    Shader CreateShaderFromBinary(const std::vector<uint8_t> &vBinary, const std::vector<uint8_t> &fBinary) override;
    Shader CreateShaderFromSource(const char *name, const std::string &vSource, const std::string &fSource) override;
    Shader CreateComputeShaderFromSource(const char *name, const std::string &source) override;

    // Buffer Creation
    VertexBuffer CreateVertexBuffer(
        const VertexBufferLayout &vertex_buffer_layout, void *data, uint32_t data_size) override;
    IndexBuffer CreateIndexBuffer(
        const IndexBufferLayout &index_buffer_descriptor, void *data, uint32_t data_size) override;
    ConstantBuffer CreateConstantBuffer(
        const ConstantBufferLayout &constant_buffer_layout, void *data, uint32_t data_size) override;
    ShaderBuffer CreateShaderBuffer(const ShaderBufferLayout &shader_buffer_layout, void *data, uint32_t data_size) override;

    Pipeline CreatePipeline(PipelineState state) override;
    /*
    // Buffer Updates
    void UpdateVertexBuffer(VertexBuffer handle, void *data, uint32_t size) override;
    void UpdateIndexBuffer(IndexBuffer handle, void *data, uint32_t size) override;
    void UpdateConstantBuffer(ConstantBuffer handle, void *data, uint32_t size) override;
    void UpdateShaderBuffer(ShaderBuffer handle, void *data, uint32_t size) override;
     */

    void *MapBuffer(ShaderBuffer handle, AccessMode accessMode) override;
    void UnmapBuffer(ShaderBuffer handle) override;

    // Buffer Destruction
    void DestroyVertexBuffer(VertexBuffer handle) override;
    void DestroyIndexBuffer(IndexBuffer handle) override;
    void DestroyShaderBuffer(ShaderBuffer handle) override;
    void DestroyConstantBuffer(ConstantBuffer handle) override;

    void BeginPass(const std::string &name) override;

    void BindSceneState(const SceneState &scene_state) override;
    void BindPipeline(Pipeline pipeline) override;
    // draw call submission
    void Draw(Primitive primitive, uint32_t starting_vertex, uint32_t point_count) override;

    void EndPass() override;
    // Compute shader dispatch
    void DispatchCompute(
        uint32_t xGroups, uint32_t yGroups, uint32_t zGroups, Shader shader, const ComputeState &computeState) override;
    void WaitForMemory(uint64_t flags) override;

    // Screen clearing
    void ClearBackBuffer(ClearState clearState = {}) override;

    void SwapBuffers(const Window &window) override;
};

} // namespace focus::dx11