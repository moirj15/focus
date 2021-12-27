#include "renderer/Interface/focus.hpp"
#include "utils/FileUtils.hpp"

#include <SDL2/SDL.h>

static focus::Device *device = nullptr;

void TriangleTest(const focus::Window &window)
{
    auto handle =
        device->CreateShaderFromSource("UniformColor", utils::ReadEntireFileAsString("shaders/gl/UniformColor.vert"),
            utils::ReadEntireFileAsString("shaders/gl/UniformColor.frag"));

    focus::PipelineState pipeline_state = {
        .shader = handle,
    };

    auto pipeline = device->CreatePipeline(pipeline_state);

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

    focus::VertexBufferLayout vb_layout("INPUT");
    vb_layout.Add("aPosition", focus::VarType::Float3);

    focus::IndexBufferLayout ib_layout(focus::IndexBufferType::U32);

    focus::ConstantBufferLayout cb_layout("Constants");
    cb_layout.Add("mvp", focus::VarType::Float4x4);

    focus::SceneState sceneState = {
        .vb_handles = {device->CreateVertexBuffer(vb_layout, (uint8_t*)points, sizeof(points))},
        .cb_handles = {device->CreateConstantBuffer(cb_layout, (uint8_t*)mvp, sizeof(mvp))},
        .ib_handle = device->CreateIndexBuffer(ib_layout, (uint8_t*)indices, sizeof(indices)),
        .indexed = true,
    };

    SDL_Event e;
    while (true) {
        while (SDL_PollEvent(&e) > 0) {
            if (e.type == SDL_QUIT) {
                return;
            }
        }
        device->ClearBackBuffer({});
        device->BeginPass("Triangle pass");

        device->BindSceneState(sceneState);
        device->BindPipeline(pipeline);
        device->Draw(focus::Primitive::Triangles, 0, 3);

        device->EndPass();

        device->SwapBuffers(window);
    }
}


void TriangleTestInterleavedBuffer(const focus::Window &window)
{
    #if 0
    auto handle = device->CreateShaderFromSource("UniformColor",
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
        .types = {focus::VarType::Float3, focus::VarType::Float3},
        .size_in_bytes = sizeof(vertexData),
        .element_size_in_bytes = sizeof(vertexData) / 3,
        .usage = focus::BufferUsage::Static,
    };

    focus::IndexBufferDescriptor ibDescriptor = {
        .type = focus::IndexBufferType::U32,
        .size_in_bytes = sizeof(indices),
    };

    focus::SceneState sceneState = {
        .vb_handles = {device->CreateVertexBuffer(<#initializer #>, nullptr, 0)},
        .cb_handles = {},
        .ib_handle = device->CreateIndexBuffer(<#initializer #>, nullptr, 0),
        .indexed = true,
    };

    SDL_Event e;
    while (true) {
        while (SDL_PollEvent(&e) > 0) {
            if (e.type == SDL_QUIT) {
                return;
            }
        }
        device->ClearBackBuffer({});

//        device->Draw(focus::Primitive::Triangles, {}, handle, sceneState);
        device->Draw(focus::Primitive::Triangles);

        device->SwapBuffers(window);
    }
    #endif
}

void ComputeTest(const focus::Window &window)
{

    auto handle =
        device->CreateComputeShaderFromSource("TestCompute", utils::ReadEntireFileAsString("shaders/gl/test.comp"));

    focus::ShaderBufferLayout sDesc(0, focus::BufferUsage::Dynamic, "color_buf");
    sDesc.Add("color_buf", focus::VarType::Float);
//    focus::ShaderBufferDescriptor sDesc = {
//        .name = "color_buf",
//        .slot = 0,
//        .accessMode = focus::AccessMode::WriteOnly,
//        .types = {focus::VarType::Float},
//    };
    auto sHandle = device->CreateShaderBuffer(sDesc, nullptr, 256 * 256 * sizeof(float));
    auto *contents = (float *)device->MapBuffer(sHandle, focus::AccessMode::ReadOnly);
    for (int i = 0; i < 256 * 256; i++) {
        contents[i] = 88.0f;
    }
    device->UnmapBuffer(sHandle);

    SDL_Event e;
    while (true) {
        while (SDL_PollEvent(&e) > 0) {
            if (e.type == SDL_QUIT) {
                return;
            }
        }
        device->ClearBackBuffer({});
        device->DispatchCompute(256, 256, 1, handle, {{sHandle}, {}});
        device->WaitForMemory(0);
        auto *contents2 = (float *)device->MapBuffer(sHandle, focus::AccessMode::ReadOnly);
        device->UnmapBuffer(sHandle);

        device->SwapBuffers(window);
    }
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    setvbuf(stdout, nullptr, _IONBF, 0);
    device = focus::Device::Init(focus::RendererAPI::OpenGL);
    auto window = device->MakeWindow(1920, 1080);
    TriangleTest(window);
    TriangleTestInterleavedBuffer(window);
    ComputeTest(window);

    SDL_Quit();
    return 0;
}
