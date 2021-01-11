#pragma once

#include "../Interface/Context.hpp"

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include "ShaderManager.hpp"
using namespace Microsoft::WRL;

namespace focus
{

class DX11Context : public Context
{
  ComPtr<ID3D11Device> mDevice;
  ComPtr<ID3D11DeviceContext> mContext;
  ComPtr<IDXGISwapChain> mSwapChain;
  ComPtr<ID3D11Texture2D> mBackBuffer;
  ComPtr<ID3D11RenderTargetView> mBackBufferRenderTargetView;
  ComPtr<ID3D11Texture2D> mDepthStencilBuffer;
  ComPtr<ID3D11DepthStencilView> mDepthStencilView;
  D3D11_VIEWPORT mViewport;

public:
  void Init() override;

  Window MakeWindow(s32 width, s32 height) override;

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