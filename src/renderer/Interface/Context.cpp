#include "Context.hpp"

#ifdef _WIN32
#include "../dx11/DX11Context.hpp"
#include "../dx12/DX12Context.hpp"
#endif
#include "../gl/GLContext.hpp"
namespace focus
{

Context *gContext = nullptr;

void Context::Init(RendererAPI api)
{
  assert(gContext == nullptr);
  assert(api != RendererAPI::Invalid);
  if (api == RendererAPI::OpenGL) {
    gContext = new GLContext();
  } else if (api == RendererAPI::Vulkan) {
    assert(0);
  }
#if _WIN32
  else if (api == RendererAPI::DX11) {
    gContext = new dx11::DX11Context();
  } else if (api == RendererAPI::DX12) {
    assert(0);
    //gContext = new DX12Context();
  }
#else
  assert(api != RendererAPI::DX11 || api != RendererAPI::DX12);
#endif
}

} // namespace focus
