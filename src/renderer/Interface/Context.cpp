#include "Context.hpp"

#ifdef _WIN32
#include "../dx11/context.hpp"
#endif
#include "../gl/GLContext.hpp"
namespace focus
{

Context *gContext = nullptr;

#ifdef _WIN32
void Context::Init(RendererAPI api, WNDPROC messageHandler, HINSTANCE instanceHandle)
{
  assert(gContext == nullptr);
  assert(api != RendererAPI::Invalid);
  if (api == RendererAPI::OpenGL) {
    gContext = new GLContext(messageHandler, instanceHandle);
  } else if (api == RendererAPI::Vulkan) {
    assert(0);
  } else if (api == RendererAPI::DX11) {
    assert(0);
  } else if (api == RendererAPI::DX12) {
    assert(0);
  }
}
#else

void Context::Init(RendererAPI api)
{
  assert(gContext == nullptr);
  assert(api != RendererAPI::Invalid);
  if (api == RendererAPI::OpenGL) {
    gContext = new GLContext();
  } else if (api == RendererAPI::Vulkan) {
    assert(0);
  }
}
#endif

} // namespace focus
