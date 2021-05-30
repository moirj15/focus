#include "Context.hpp"

#ifdef _WIN32
#include "../dx11/DX11Context.hpp"
#include "../dx12/DX12Context.hpp"
#endif
#include "../gl/GLContext.hpp"
namespace focus
{

std::pair<ID3D11Device *, ID3D11DeviceContext *> GetDeviceAndContext()
{
#if _WIN32
  assert(sAPI == RendererAPI::DX11);
  auto *dx11 = (dx11::DX11Context *)gContext;
  return {dx11->GetDevice(), dx11->GetContext()};
#else
  return {nullptr, nullptr};
#endif
}

} // namespace focus
