#include "ShaderManager.hpp"

namespace focus::dx11
{

ShaderHandle ShaderManager::AddShader(const char *name, const std::string &vSource, const std::string &pSource)
{
  ComPtr<ID3DBlob> vBinary;
  ComPtr<ID3DBlob> vErrors;
  Check(D3DCompile(vSource.data(), vSource.size(), nullptr, nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vBinary, &vErrors));

  ComPtr<ID3DBlob> fBinary;
  ComPtr<ID3DBlob> fErrors;
  Check(D3DCompile(pSource.data(), pSource.size(), nullptr, nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &fBinary, &fErrors));

  Shaders shaders;

  Check(mDevice->CreateVertexShader(vBinary->GetBufferPointer(), vBinary->GetBufferSize(), nullptr, &shaders.vertexShader));
  Check(mDevice->CreatePixelShader(fBinary->GetBufferPointer(), fBinary->GetBufferSize(), nullptr, &shaders.pixelShader));

  ComPtr<ID3D11ShaderReflection> vertexReflection;
  ComPtr<ID3D11ShaderReflection> pixelReflection;
  mCurrHandle++;
  mShaderNames.emplace(mCurrHandle, name);
  mGfxShaders.emplace(mCurrHandle, shaders);

  return mCurrHandle;
}

ShaderHandle ShaderManager::AddComputeShader(const char *name, const std::string &source)
{
  ComPtr<ID3DBlob> cBinary;
  ComPtr<ID3DBlob> cErrors;
  Check(D3DCompile(source.data(), source.size(), nullptr, nullptr, nullptr, "CSMain", "cs_5_0", 0, 0, &cBinary, &cErrors));
  ID3D11ComputeShader *cShader;
  Check(mDevice->CreateComputeShader(cBinary->GetBufferPointer(), cBinary->GetBufferSize(), nullptr, &cShader));
  mCurrHandle++;
  mShaderNames.emplace(mCurrHandle, name);
  mComputeShaders.emplace(mCurrHandle, cShader);

  return mCurrHandle;
}

Shaders ShaderManager::GetProgram(ShaderHandle handle)
{
  return mGfxShaders[handle];
}

void ShaderManager::DeleteShader(ShaderHandle handle)
{
  mGfxShaders.erase(handle);
  mComputeShaders.erase(handle);
}
} // namespace focus