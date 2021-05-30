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

  Window make_window(s32 width, s32 height) override;

  ShaderHandle create_shader_from_binary(const char *vBinary, const char *fBinary) override;

  ShaderHandle create_shader_from_source(
      const char *name, const std::string &vSource, const std::string &fSource) override;

  ShaderHandle create_compute_shader_from_source(const char *name, const std::string &source) override;

  VertexBufferHandle create_vertex_buffer(void *data, VertexBufferDescriptor descriptor) override;

  IndexBufferHandle create_index_buffer(void *data, IndexBufferDescriptor descriptor) override;

  ConstantBufferHandle create_constant_buffer(void *data, ConstantBufferDescriptor descriptor) override;

  BufferHandle create_shader_buffer(void *data, ShaderBufferDescriptor descriptor) override;

  void update_vertex_buffer(VertexBufferHandle handle, void *data, u32 size) override;
  void update_index_buffer(IndexBufferHandle handle, void *data, u32 size) override;
  void update_constant_buffer(ConstantBufferHandle handle, void *data, u32 size) override;
  void update_shader_buffer(BufferHandle handle, void *data, u32 size) override;

  std::vector<u8> ReadShaderBuffer(BufferHandle handle) override;

  void *map_buffer(BufferHandle handle, AccessMode accessMode) override;

  void unmap_buffer(BufferHandle handle) override;

  void destroy_vertex_buffer(VertexBufferHandle handle) override;

  void destroy_index_buffer(IndexBufferHandle handle) override;

  void destroy_shader_buffer(BufferHandle handle) override;
  void destroy_constant_buffer(ConstantBufferHandle handle) override;

  void draw(Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState &sceneState) override;

  void dispatch_compute(
      u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, const ComputeState &computeState) override;

  void wait_for_memory(u64 flags) override;

  void clear_back_buffer(ClearState clearState = {}) override;

  void swap_buffers(const Window &window) override;


};

} // namespace focus::dx11