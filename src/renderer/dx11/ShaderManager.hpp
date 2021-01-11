#pragma once
#include "../Interface/Handles.hpp"

#include <d3d11.h>
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
};

class ShaderManager
{

  std::unordered_map<ShaderHandle, const char *> mShaderNames;
  std::unordered_map<ShaderHandle, Shaders> mGfxShaders;
  std::unordered_map<ShaderHandle, ComPtr<ID3D11ComputeShader>> mComputeShaders;
  // TODO: not thread safe, but not anticipating multithreaded shader compilation/creation now or in the future
  ShaderHandle mCurrHandle = 0;
  ID3D11Device *mDevice;

public:
  ShaderManager(ID3D11Device *device) : mDevice(device) {}
  ShaderHandle AddShader(const char *name, const std::string &vSource, const std::string &pSource);
  ShaderHandle AddComputeShader(const char *name, const std::string &source);
  Shaders GetProgram(ShaderHandle handle);
  void DeleteShader(ShaderHandle handle);

  // TODO: precompiled shaders
};

} // namespace focus::dx11