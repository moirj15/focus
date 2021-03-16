#include "ShaderManager.hpp"

#include <cassert>

namespace focus::dx11
{

ShaderHandle ShaderManager::AddShader(const char *name, const std::string &vSource, const std::string &pSource)
{
  ComPtr<ID3DBlob> vBinary;
  ComPtr<ID3DBlob> vErrors;
  u32 flags = 0;
#ifdef _DEBUG
  flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
  // TODO: better compile error checking
  if (FAILED(D3DCompile(
          vSource.data(), vSource.size(), nullptr, nullptr, nullptr, "VSMain", "vs_5_0", flags, 0, &vBinary, &vErrors))) {
    printf("VERT ERROR: %s\n", (char *)vErrors->GetBufferPointer());
    assert(0);
  }

  ComPtr<ID3DBlob> fBinary;
  ComPtr<ID3DBlob> fErrors;
  if (FAILED(D3DCompile(
          pSource.data(), pSource.size(), nullptr, nullptr, nullptr, "PSMain", "ps_5_0", flags, 0, &fBinary, &fErrors))) {
    printf("VERT ERROR: %s\n", (char *)vErrors->GetBufferPointer());
    assert(0);
  }

  Shaders shaders;
  Check(mDevice->CreateVertexShader(
      vBinary->GetBufferPointer(), vBinary->GetBufferSize(), nullptr, &shaders.vertexShader));
  Check(
      mDevice->CreatePixelShader(fBinary->GetBufferPointer(), fBinary->GetBufferSize(), nullptr, &shaders.pixelShader));

  ComPtr<ID3D11ShaderReflection> vertexReflection;
  Check(D3DReflect(
      vBinary->GetBufferPointer(), vBinary->GetBufferSize(), IID_ID3D11ShaderReflection, (void **)&vertexReflection));

  // Use the reflection api to determine the format of the INPUT_ELEMENT_DESC
  // TODO: this will go in the shader tool eventually
  D3D11_SHADER_DESC vDesc;
  Check(vertexReflection->GetDesc(&vDesc));
  std::vector<D3D11_INPUT_ELEMENT_DESC> descriptors(vDesc.InputParameters);
  for (s32 i = 0; i < descriptors.size(); i++) {
    D3D11_SIGNATURE_PARAMETER_DESC inputDesc;
    Check(vertexReflection->GetInputParameterDesc(i, &inputDesc));
    descriptors[i] = {
        .SemanticName = inputDesc.SemanticName,
        .SemanticIndex = inputDesc.SemanticIndex,
        .Format = GetFormat(inputDesc),
        .InputSlot = 0,
        .AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT, // TODO: make sure this is correct
        .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
        .InstanceDataStepRate = 0,
    };
    shaders.inputStride += GetInputSize(inputDesc);
  }
  mDevice->CreateInputLayout(descriptors.data(), descriptors.size(), vBinary->GetBufferPointer(),
      vBinary->GetBufferSize(), &shaders.inputLayout);
  mCurrHandle++;
  mShaderNames.emplace(mCurrHandle, name);
  mGfxShaders.emplace(mCurrHandle, shaders);

  return mCurrHandle;
}

ShaderHandle ShaderManager::AddComputeShader(const char *name, const std::string &source)
{
  ComPtr<ID3DBlob> cBinary;
  ComPtr<ID3DBlob> cErrors;
  u32 flags = 0;
#ifdef _DEBUG
  flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
  if (FAILED(D3DCompile(
          source.data(), source.size(), nullptr, nullptr, nullptr, "CSMain", "cs_5_0", flags, 0, &cBinary, &cErrors))) {
    printf("Compute ERROR: %s\n", (char *)cErrors->GetBufferPointer());
    assert(0);
  }
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

ID3D11ComputeShader *ShaderManager::GetComputeShader(ShaderHandle handle)
{
  return mComputeShaders[handle].Get();
}

void ShaderManager::DeleteShader(ShaderHandle handle)
{
  mGfxShaders.erase(handle);
  mComputeShaders.erase(handle);
}

DXGI_FORMAT ShaderManager::GetFormat(D3D11_SIGNATURE_PARAMETER_DESC desc)
{
  u32 componentCount = 0;
  if (desc.Mask & (1 << 3)) {
    componentCount = 4;
  } else if (desc.Mask & (1 << 2)) {
    componentCount = 3;
  } else if (desc.Mask & (1 << 1)) {
    componentCount = 2;
  } else if (desc.Mask & (1 << 0)) {
    componentCount = 1;
  }
  if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
    if (componentCount == 1) {
      return DXGI_FORMAT_R32_UINT;
    } else if (componentCount == 2) {
      return DXGI_FORMAT_R32G32_UINT;
    } else if (componentCount == 3) {
      return DXGI_FORMAT_R32G32B32_UINT;
    } else /* componentCount == 4 */ {
      return DXGI_FORMAT_R32G32B32A32_UINT;
    }
  } else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
    if (componentCount == 1) {
      return DXGI_FORMAT_R32_SINT;
    } else if (componentCount == 2) {
      return DXGI_FORMAT_R32G32_SINT;
    } else if (componentCount == 3) {
      return DXGI_FORMAT_R32G32B32_SINT;
    } else /* componentCount == 4 */ {
      return DXGI_FORMAT_R32G32B32A32_SINT;
    }
  } else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
    if (componentCount == 1) {
      return DXGI_FORMAT_R32_FLOAT;
    } else if (componentCount == 2) {
      return DXGI_FORMAT_R32G32_FLOAT;
    } else if (componentCount == 3) {
      return DXGI_FORMAT_R32G32B32_FLOAT;
    } else /* componentCount == 4 */ {
      return DXGI_FORMAT_R32G32B32A32_FLOAT;
    }
  } else {
    assert(0 && "unknown format type");
    return DXGI_FORMAT_UNKNOWN;
  }
}

u32 ShaderManager::GetInputSize(D3D11_SIGNATURE_PARAMETER_DESC desc)
{
  if (desc.Mask & (1 << 3)) {
    return 4 * sizeof(f32);
  } else if (desc.Mask & (1 << 2)) {
    return 3 * sizeof(f32);
  } else if (desc.Mask & (1 << 1)) {
    return 2 * sizeof(f32);
  } else if (desc.Mask & (1 << 0)) {
    return 1 * sizeof(f32);
  } else {
    assert(0 && "unknown format type");
    return DXGI_FORMAT_UNKNOWN;
  }
}

} // namespace focus::dx11