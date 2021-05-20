#include "renderer/Interface/Context.hpp"
#include "utils/FileUtils.hpp"

#include <SDL2/SDL.h>
#include <cstdio>
#include <filesystem>

void TriangleTest(const focus::Window &window)
{
  auto handle = focus::gContext->CreateShaderFromSource("UniformColor",
      utils::ReadEntireFileAsString("shaders/gl/UniformColor.vert"),
      utils::ReadEntireFileAsString("shaders/gl/UniformColor.frag"));

  f32 points[] = {
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

  u32 indices[] = {0, 1, 2};

  f32 mvp[] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
  };

  focus::VertexBufferDescriptor vbDescriptor = {
      .inputDescriptorName = {"aPosition"},
      .bufferType = focus::BufferType::SingleType,
      .types = {focus::VarType::Vec3},
      .sizeInBytes = sizeof(points),
  };

  focus::IndexBufferDescriptor ibDescriptor = {
      .type = focus::IndexBufferType::U32,
      .sizeInBytes = sizeof(indices),
  };

  f32 color1[] = {
      0.0, 1.0, 0.0, 1.0,
  };
  f32 color2[] = {
      1.0, 0.0, 0.0, 1.0,
  };

  focus::ConstantBufferDescriptor vertConstDesc = {
    .name = "Constants",
      .types = {focus::VarType::Vec4},
      .slot = 0,
      .usage = focus::BufferUsage::Static,
      .sizeInBytes = sizeof(color1),
  };


  focus::ConstantBufferDescriptor fragConstDesc = {
      .name = "FragInput",
      .types = {focus::VarType::Vec4},
      .slot = 1,
      .usage = focus::BufferUsage::Static,
      .sizeInBytes = sizeof(color2),
  };

  focus::SceneState sceneState = {
      .vbHandles = {focus::gContext->CreateVertexBuffer(points, sizeof(points), vbDescriptor)},
      .cbHandles = {focus::gContext->CreateConstantBuffer(color1, sizeof(color1), vertConstDesc), focus::gContext->CreateConstantBuffer(color2, sizeof(color2), fragConstDesc)},
      .ibHandle = focus::gContext->CreateIndexBuffer(indices, sizeof(indices), ibDescriptor),
      .indexed = true,
  };


  SDL_Event e;
  while (true) {
    while (SDL_PollEvent(&e) > 0) {
      if (e.type == SDL_QUIT) {
        return;
      }
    }
    focus::gContext->Clear({});

    focus::gContext->Draw(focus::Primitive::Triangles, {}, handle, sceneState);

    focus::gContext->SwapBuffers(window);
  }
}

void TriangleTestInterleavedBuffer(const focus::Window& window)
{
  auto handle = focus::gContext->CreateShaderFromSource("UniformColor",
      utils::ReadEntireFileAsString("shaders/gl/UniformColorInterleaved.vert"),
      utils::ReadEntireFileAsString("shaders/gl/UniformColorInterleaved.frag"));

  f32 vertexData[] = {
      0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  };

  u32 indices[] = {0, 1, 2};

  f32 mvp[] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
  };

  focus::VertexBufferDescriptor vbDescriptor = {
      .inputDescriptorName = {"aPosition", "aColor"},
      .bufferType = focus::BufferType::InterLeaved,
      .types = {focus::VarType::Vec3, focus::VarType::Vec3},
      .sizeInBytes = sizeof(vertexData),
      .elementSizeInBytes = sizeof(vertexData) / 3,
      .usage = focus::BufferUsage::Static,
  };

  focus::IndexBufferDescriptor ibDescriptor = {
      .type = focus::IndexBufferType::U32,
      .sizeInBytes = sizeof(indices),
  };

  focus::SceneState sceneState = {
      .vbHandles = {focus::gContext->CreateVertexBuffer(vertexData, sizeof(vertexData), vbDescriptor)},
      .cbHandles = {},
      .ibHandle = focus::gContext->CreateIndexBuffer(indices, sizeof(indices), ibDescriptor),
      .indexed = true,
  };


  SDL_Event e;
  while (true) {
    while (SDL_PollEvent(&e) > 0) {
      if (e.type == SDL_QUIT) {
        return;
      }
    }
    focus::gContext->Clear({});

    focus::gContext->Draw(focus::Primitive::Triangles, {}, handle, sceneState);

    focus::gContext->SwapBuffers(window);
  }
}

void ComputeTest(const focus::Window &window)
{

  auto handle = focus::gContext->CreateComputeShaderFromSource(
      "TestCompute", utils::ReadEntireFileAsString("shaders/dx11/testCS.hlsl"));

  focus::ShaderBufferDescriptor sDesc = {
      .name = "color_buf",
      .slot = 0,
      .accessMode = focus::AccessMode::WriteOnly,
      .types = { focus::VarType::Float },
  };
  auto sHandle = focus::gContext->CreateShaderBuffer(nullptr, 4 * sizeof(float) * 256 * 256, sDesc);
  float *contents = (float *)focus::gContext->MapBufferPtr(sHandle, focus::AccessMode::ReadOnly);
  for (int i = 0; i < 256 * 256; i++) {
    contents[i] = 88.0f;
  }
  focus::gContext->UnmapBufferPtr(sHandle);

  bool keepWindowOpen = true;
  SDL_Event e;
  while (keepWindowOpen) {
    while (SDL_PollEvent(&e) > 0) {
      if (e.type == SDL_QUIT) {
        return;
      }
    }
    focus::gContext->Clear({});
    focus::gContext->DispatchCompute(256, 256, 1, handle, {{sHandle}, {}});
    focus::gContext->WaitForMemory(0);
    float *contents = (float *)focus::gContext->MapBufferPtr(sHandle, focus::AccessMode::ReadOnly);
    focus::gContext->UnmapBufferPtr(sHandle);

    focus::gContext->SwapBuffers(window);
  }
}

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  setvbuf(stdout, nullptr, _IONBF, 0);
  focus::Context::Init(focus::RendererAPI::OpenGL);
  auto window = focus::gContext->MakeWindow(1920, 1080);
  TriangleTest(window);
  TriangleTestInterleavedBuffer(window);
  ComputeTest(window);

  SDL_Quit();
  return 0;
}
