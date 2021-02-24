#pragma once

#include "../Interface/Context.hpp"
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

class DX11Context : public Context
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
  BufferManager<VertexBufferHandle, VertexBufferDescriptor, D3D11_BIND_VERTEX_BUFFER> mVBManager;
  BufferManager<IndexBufferHandle, IndexBufferDescriptor, D3D11_BIND_INDEX_BUFFER> mIBManager;
  BufferManager<ConstantBufferHandle, ConstantBufferDescriptor, D3D11_BIND_CONSTANT_BUFFER> mCBManager;
  // TODO: find equivalent
  ShaderBufferManager mSBManager;

public:
  DX11Context();

  inline ID3D11Device *GetDevice() { return mDevice.Get(); }
  inline ID3D11DeviceContext *GetContext() { return mContext.Get(); }

  void Init() override;

  Window MakeWindow(s32 width, s32 height) override;

  ShaderHandle CreateShaderFromBinary(const char *vBinary, const char *fBinary) override;

  ShaderHandle CreateShaderFromSource(
      const char *name, const std::string &vSource, const std::string &fSource) override;

  ShaderHandle CreateComputeShaderFromSource(const char *name, const std::string &source) override;

  VertexBufferHandle CreateVertexBuffer(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor) override;

  IndexBufferHandle CreateIndexBuffer(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor) override;

  ConstantBufferHandle CreateConstantBuffer(void *data, u32 sizeInBytes, ConstantBufferDescriptor descriptor) override;

  BufferHandle CreateShaderBuffer(void *data, u32 sizeInBytes, ShaderBufferDescriptor descriptor) override;

  void UpdateVertexBuffer(VertexBufferHandle handle, void *data, u32 size) override;
  void UpdateIndexBuffer(IndexBufferHandle handle, void *data, u32 size) override;
  void UpdateConstantBuffer(ConstantBufferHandle handle, void *data, u32 size) override;
  void UpdateShaderBuffer(BufferHandle handle, void *data, u32 size) override;

  void *MapBufferPtr(BufferHandle handle, AccessMode accessMode) override;

  void UnmapBufferPtr(BufferHandle handle) override;

  void DestroyVertexBuffer(VertexBufferHandle handle) override;

  void DestroyIndexBuffer(IndexBufferHandle handle) override;

  void DestroyShaderBuffer(BufferHandle handle) override;
  void DestroyConstantBuffer(ConstantBufferHandle handle) override;

  void Draw(Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState &sceneState) override;

  void DispatchCompute(
      u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, const ComputeState &computeState) override;

  void WaitForMemory(u64 flags) override;

  void Clear(ClearState clearState = {}) override;

  void SwapBuffers(const Window &window) override;
};

} // namespace focus::dx11