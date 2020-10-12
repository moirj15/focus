#include "Context.hpp"

#ifdef _WIN32
#include "../dx11/context.hpp"
#endif
#include "../gl/Context.hpp"
namespace renderer
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
namespace context
{

#ifdef _WIN32
void Init(WNDPROC messageHandler, HINSTANCE instanceHandle)
{
  if constexpr (sAPI == RendererAPI::OpenGL) {
    gl::context::Init(messageHandler, instanceHandle);
  } else if constexpr (sAPI == RendererAPI::Vulkan) {
  } else if constexpr (sAPI == RendererAPI::DX11) {
    dx11::context::Init(messageHandler, instanceHandle);
  } else if constexpr (sAPI == RendererAPI::DX12) {
  }
  static_assert(sAPI != RendererAPI::Invalid);
}
Window MakeWindow(s32 width, s32 height)
{
  if constexpr (sAPI == RendererAPI::OpenGL) {
    return gl::context::MakeWindow(width, height);
  } else if constexpr (sAPI == RendererAPI::Vulkan) {
  } else if constexpr (sAPI == RendererAPI::DX11) {
    return dx11::context::MakeWindow(width, height);
  } else if constexpr (sAPI == RendererAPI::DX12) {
  }
}
#else
void Init()
{
  if constexpr (sAPI == RendererAPI::OpenGL) {
    gl::context::Init();
  } else if constexpr (sAPI == RendererAPI::Vulkan) {
  } else if constexpr (sAPI == RendererAPI::DX12) {
  }
  static_assert(sAPI != RendererAPI::Invalid);
}

Window MakeWindow(s32 width, s32 height)
{
  if constexpr (sAPI == RendererAPI::OpenGL) {
    return gl::context::MakeWindow(width, height);
  } else if constexpr (sAPI == RendererAPI::Vulkan) {
  } 
}
#endif
ShaderHandle CreateShaderFromBinary(const char *vBinary, const char *fBinary)
{
  if constexpr (sAPI == RendererAPI::OpenGL) {
    return INVALID_HANDLE;
  } else if constexpr (sAPI == RendererAPI::Vulkan) {
    return INVALID_HANDLE;
  } else if constexpr (sAPI == RendererAPI::DX11) {
    return INVALID_HANDLE;
  } else if constexpr (sAPI == RendererAPI::DX12) {
    return INVALID_HANDLE;
  }
}

ShaderHandle CreateShaderFromSource(const char *vSource, const char *fSource)
{
  if constexpr (sAPI == RendererAPI::OpenGL) {
    return INVALID_HANDLE;
  } else if constexpr (sAPI == RendererAPI::Vulkan) {
    return INVALID_HANDLE;
  } else if constexpr (sAPI == RendererAPI::DX11) {
    return INVALID_HANDLE;
  } else if constexpr (sAPI == RendererAPI::DX12) {
    return INVALID_HANDLE;
  }
}

VertexBufferHandle CreateVertexBuffer(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor)
{
  if constexpr (sAPI == RendererAPI::OpenGL) {
    return gl::context::CreateVertexBuffer(data, sizeInBytes, descriptor);
  } else if constexpr (sAPI == RendererAPI::Vulkan) {
    return INVALID_HANDLE;
  } else if constexpr (sAPI == RendererAPI::DX11) {
    return INVALID_HANDLE;
  } else if constexpr (sAPI == RendererAPI::DX12) {
    return INVALID_HANDLE;
  }
}

IndexBufferHandle CreateIndexBuffer(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor)
{
  if constexpr (sAPI == RendererAPI::OpenGL) {
    return gl::context::CreateIndexBuffer(data, sizeInBytes, descriptor);
  } else if constexpr (sAPI == RendererAPI::Vulkan) {
    return INVALID_HANDLE;
  } else if constexpr (sAPI == RendererAPI::DX11) {
    return INVALID_HANDLE;
  } else if constexpr (sAPI == RendererAPI::DX12) {
    return INVALID_HANDLE;
  }
}

void DestroyIndexBuffer(IndexBufferHandle handle)
{
}

} // namespace context
} // namespace renderer
