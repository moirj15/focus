#include "instancing.hpp"

#include <glm/ext.hpp>
#include <glm/glm.hpp>

Instancing::Instancing(focus::Device *device, const focus::Window &window) :
        Example(device, window, "Instancing Example")
{
    auto shader =
        _device->CreateShaderFromSource("UniformColor", utils::ReadEntireFileAsString("shaders/gl/UniformColor.vert"),
            utils::ReadEntireFileAsString("shaders/gl/UniformColor.frag"));

    focus::PipelineState pipeline_state = {
        .shader = shader,
    };

    _pipeline = _device->CreatePipeline(pipeline_state);

    auto mesh = utils::ReadObjectFile("data/obj-files/teapot.obj");
    _vertex_count = mesh.vertices.size() / 3;

    auto perspective = glm::infinitePerspective(90.0f, 16.0f / 9.0f, 0.01f);
    auto camera = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    auto model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    const auto mvp = perspective * camera * model;

    glm::vec4 color(1.0f, 0.0f, 0.0f, 0.0f);

    std::vector<float> cb_data;
    cb_data.insert(cb_data.end(), glm::value_ptr(color), glm::value_ptr(color) + 4);
    cb_data.insert(cb_data.end(), glm::value_ptr(mvp), glm::value_ptr(mvp) + 16);

    std::vector<glm::vec2> offsets;
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            offsets.emplace_back((float)x, (float)y);
        }
    }

    focus::VertexBufferLayout vb_layout(0, focus::BufferUsage::Default, focus::InputClassification::Normal, "INPUT");
    vb_layout.Add("aPosition", focus::VarType::Float3);

    focus::VertexBufferLayout instance_buffer_layout(
        1, focus::BufferUsage::Default, focus::InputClassification::Instanced);
    instance_buffer_layout.Add("aOffset", focus::VarType::Float2);

    focus::ConstantBufferLayout cb_layout("Constants");
    cb_layout.Add("mvp", focus::VarType::Float4x4);

    _scene_state = {
        .vb_handles = {device->CreateVertexBuffer(
                           vb_layout, mesh.vertices.data(), mesh.vertices.size() * sizeof(float)),
            device->CreateVertexBuffer(instance_buffer_layout, offsets.data(), offsets.size() * sizeof(glm::vec2))},
        .cb_handles = {device->CreateConstantBuffer(cb_layout, cb_data.data(), cb_data.size() * sizeof(float))},
        .ib_handle = {},
        .indexed = true,
    };
}

void Instancing::DoFrame()
{
    _device->ClearBackBuffer({});
    _device->BeginPass("Instanced pass");

    _device->BindSceneState(_scene_state);
    _device->BindPipeline(_pipeline);
    // TODO: need to add instanced draw call
    _device->DrawInstanced(focus::Primitive::Triangles, 0, _vertex_count, 1);

    _device->EndPass();

    _device->SwapBuffers(_window);
}
