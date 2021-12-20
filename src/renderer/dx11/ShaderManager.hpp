#pragma once
#include "../Interface/FocusBackend.hpp"

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
  uint32_t inputStride = 0;
};


class ShaderManager
{

  std::unordered_map<Shader, const char *> mShaderNames;
  std::unordered_map<Shader, Shaders> mGfxShaders;
  std::unordered_map<Shader, ComPtr<ID3D11ComputeShader>> mComputeShaders;
  // TODO: not thread safe, but not anticipating multithreaded shader compilation/creation now or in the future
  Shader mCurrHandle{0};
  ID3D11Device *mDevice;

public:
  ShaderManager() = default; // TODO: temp hack for constructing D3D11Context
  ShaderManager(ID3D11Device *device) : mDevice(device) {}
  Shader AddShader(const char *name, const std::string &vSource, const std::string &pSource);
  Shader AddComputeShader(const char *name, const std::string &source);
  Shaders GetProgram(Shader handle);
  ID3D11ComputeShader *GetComputeShader(Shader handle);

  void DeleteShader(Shader handle);
private:
  DXGI_FORMAT GetFormat(D3D11_SIGNATURE_PARAMETER_DESC desc);
  uint32_t GetInputSize(D3D11_SIGNATURE_PARAMETER_DESC desc);

  // TODO: precompiled shaders
};

} // namespace focus::dx11