#pragma once

#include <vector>
#include <d3d11.h>
#include <d3d11shader.h>
#include <wrl/client.h>

#include "../Interface/IShader.hpp"
using Microsoft::WRL::ComPtr;

namespace dx11
{

struct Shader : public renderer::IShader {
  ComPtr<ID3D11VertexShader> mVertexShader;
  ComPtr<ID3D11PixelShader> mPixelShader;
};

} // namespace dx11