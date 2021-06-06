#include "renderer/Interface/focus.hpp"
#include "utils/FileUtils.hpp"

#include <SDL2/SDL.h>

void TriangleTest(const focus::Window &window)
{
  auto handle =
      focus::CreateShaderFromSource("UniformColor", utils::ReadEntireFileAsString("shaders/gl/UniformColor.vert"),
          utils::ReadEntireFileAsString("shaders/gl/UniformColor.frag"));

  float points[] = {
      0.0f,
      1.0f,
      0.0f,
      -1.0f,
      -1.0f,
      0.0f,
      1.0f,
      -1.0f,
      0.0f,
  };

  uint32_t indices[] = {0, 1, 2};

  float mvp[] = {
      1.0f,
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      1.0f,
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      1.0f,
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      1.0f,
  };

  focus::VertexBufferDescriptor vbDescriptor = {
      .input_descriptor_name = {"aPosition"},
      .buffer_type = focus::BufferType::SingleType,
      .types = {focus::VarType::Vec3},
      .size_in_bytes = sizeof(points),
  };

  focus::IndexBufferDescriptor ibDescriptor = {
      .type = focus::IndexBufferType::U32,
      .size_in_bytes = sizeof(indices),
  };

  focus::ConstantBufferDescriptor mvpDesc = {
      .name = "Constants",
      .types = {focus::VarType::Mat4},
      .slot = 0,
      .usage = focus::BufferUsage::Static,
      .size_in_bytes = sizeof(mvp),
  };

  focus::SceneState sceneState = {
      .vb_handles = {focus::CreateVertexBuffer(points, vbDescriptor)},
      .cb_handles = {focus::CreateConstantBuffer(mvp, mvpDesc)},
      .ib_handle = focus::CreateIndexBuffer(indices, ibDescriptor),
      .indexed = true,
  };

  SDL_Event e;
  while (true) {
    while (SDL_PollEvent(&e) > 0) {
      if (e.type == SDL_QUIT) {
        return;
      }
    }
    focus::ClearBackBuffer({});

    focus::Draw(focus::Primitive::Triangles, {}, handle, sceneState);

    focus::swap_buffers(window);
  }
}

void TriangleTestInterleavedBuffer(const focus::Window &window)
{
  auto handle = focus::CreateShaderFromSource("UniformColor",
      utils::ReadEntireFileAsString("shaders/gl/UniformColorInterleaved.vert"),
      utils::ReadEntireFileAsString("shaders/gl/UniformColorInterleaved.frag"));

  float vertexData[] = {
      0.0f,
      1.0f,
      0.0f,
      1.0f,
      0.0f,
      0.0f,
      -1.0f,
      -1.0f,
      0.0f,
      0.0f,
      1.0f,
      0.0f,
      1.0f,
      -1.0f,
      0.0f,
      0.0f,
      0.0f,
      1.0f,
  };

  uint32_t indices[] = {0, 1, 2};

  float mvp[] = {
      1.0f,
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      1.0f,
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      1.0f,
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      1.0f,
  };

  focus::VertexBufferDescriptor vbDescriptor = {
      .input_descriptor_name = {"aPosition", "aColor"},
      .buffer_type = focus::BufferType::InterLeaved,
      .types = {focus::VarType::Vec3, focus::VarType::Vec3},
      .size_in_bytes = sizeof(vertexData),
      .element_size_in_bytes = sizeof(vertexData) / 3,
      .usage = focus::BufferUsage::Static,
  };

  focus::IndexBufferDescriptor ibDescriptor = {
      .type = focus::IndexBufferType::U32,
      .size_in_bytes = sizeof(indices),
  };

  focus::SceneState sceneState = {
      .vb_handles = {focus::CreateVertexBuffer(vertexData, vbDescriptor)},
      .cb_handles = {},
      .ib_handle = focus::CreateIndexBuffer(indices, ibDescriptor),
      .indexed = true,
  };

  SDL_Event e;
  while (true) {
    while (SDL_PollEvent(&e) > 0) {
      if (e.type == SDL_QUIT) {
        return;
      }
    }
    focus::ClearBackBuffer({});

    focus::Draw(focus::Primitive::Triangles, {}, handle, sceneState);

    focus::swap_buffers(window);
  }
}

void ComputeTest(const focus::Window &window)
{

  auto handle =
      focus::CreateComputeShaderFromSource("TestCompute", utils::ReadEntireFileAsString("shaders/dx11/testCS.hlsl"));

  focus::ShaderBufferDescriptor sDesc = {
      .name = "color_buf",
      .slot = 0,
      .accessMode = focus::AccessMode::WriteOnly,
      .types = {focus::VarType::Float},
  };
  auto sHandle = focus::CreateShaderBuffer(nullptr, focus::ShaderBufferDescriptor());
  auto *contents = (float *)focus::MapBuffer(sHandle, focus::AccessMode::ReadOnly);
  for (int i = 0; i < 256 * 256; i++) {
    contents[i] = 88.0f;
  }
  focus::UnmapBuffer(sHandle);

  SDL_Event e;
  while (true) {
    while (SDL_PollEvent(&e) > 0) {
      if (e.type == SDL_QUIT) {
        return;
      }
    }
    focus::ClearBackBuffer({});
    focus::DispatchCompute(256, 256, 1, handle, {{sHandle}, {}});
    focus::WaitForMemory(0);
    auto *contents2 = (float *)focus::MapBuffer(sHandle, focus::AccessMode::ReadOnly);
    focus::UnmapBuffer(sHandle);

    focus::swap_buffers(window);
  }
}

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  setvbuf(stdout, nullptr, _IONBF, 0);
  focus::Init(focus::RendererAPI::OpenGL);
  auto window = focus::MakeWindow(1920, 1080);
  TriangleTest(window);
  TriangleTestInterleavedBuffer(window);
  ComputeTest(window);

  SDL_Quit();
  return 0;
}
