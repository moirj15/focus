#include "Context.hpp"

#ifdef _WIN32
#include "../dx11/context.hpp"
#endif
#include "../gl/GLContext.hpp"
namespace focus
{
#if defined(RENDERER_OPENGL)
static constexpr RendererAPI sAPI = RendererAPI::OpenGL;
#elif defined(RENDERER_VULKAN)
static constexpr RendererAPI sAPI = RendererAPI::Vulkan;
#elif defined(RENDERER_DX11)
static constexpr RendererAPI sAPI = RendererAPI::DX11;
#elif defined(RENDERER_DX12)
static constexpr RendererAPI sAPI = RendererAPI::DX12;
#else
#error MUST SELECT RENDERER API AT COMPILE TIME
#endif

Context *gContext = nullptr;

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

} // namespace focus
