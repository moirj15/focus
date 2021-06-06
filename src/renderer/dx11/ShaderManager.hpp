#pragma once
#include "../Interface/Context.hpp"

#include <d3d11.h>
#include <d3d11shader.h>
#include <optional>
#include <unordered_map>
#include <wrl/client.h>

namespace focus::dx11
{

using Microsoft::WRL::ComPtr;



struct Shaders {
  ComPtr<ID3D11VertexShader> vertexShader;
  ComPtr<ID3D11HullShader> hullShader;
  ComPtr<ID3D11DomainShader> domainShader;
  ComPtr<ID3D11GeometryShader> geometryShader;
  ComPtr<ID3D11PixelShader> pixelShader;

  ComPtr<ID3D11InputLayout> inputLayout;

  // TODO: temporary until a better way of storing this information is thought of
  u32 inputStride = 0;
};


class ShaderManager
{

  std::unordered_map<ShaderHandle, const char *> mShaderNames;
  std::unordered_map<ShaderHandle, Shaders> mGfxShaders;
  std::unordered_map<ShaderHandle, ComPtr<ID3D11ComputeShader>> mComputeShaders;
  // TODO: not thread safe, but not anticipating multithreaded shader compilation/creation now or in the future
  ShaderHandle mCurrHandle{0};
  ID3D11Device *mDevice;

public:
  ShaderManager() = default; // TODO: temp hack for constructing D3D11Context
  ShaderManager(ID3D11Device *device) : mDevice(device) {}
  ShaderHandle AddShader(const char *name, const std::string &vSource, const std::string &pSource);
  ShaderHandle AddComputeShader(const char *name, const std::string &source);
  Shaders GetProgram(ShaderHandle handle);
  ID3D11ComputeShader *GetComputeShader(ShaderHandle handle);

  void DeleteShader(ShaderHandle handle);
private:
  DXGI_FORMAT GetFormat(D3D11_SIGNATURE_PARAMETER_DESC desc);
  u32 GetInputSize(D3D11_SIGNATURE_PARAMETER_DESC desc);

  // TODO: precompiled shaders
};

} // namespace focus::dx11