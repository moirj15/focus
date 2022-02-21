#pragma once

#include "../common/common.hpp"
#include "focus.hpp"

struct TriangleExample final : public Example {
    focus::Pipeline _triangle_pipeline;
    focus::SceneState _triangle_scene_state;
    explicit TriangleExample(focus::Device *device, focus::Window window) : Example(device, window, "Triangle Example")
    {
        auto triangle_shader = _device->CreateShaderFromSource("UniformColor",
            utils::ReadEntireFileAsString("shaders/gl/UniformColor.vert"),
            utils::ReadEntireFileAsString("shaders/gl/UniformColor.frag"));

        focus::PipelineState pipeline_state = {
            .shader = triangle_shader,
        };

        _triangle_pipeline = device->CreatePipeline(pipeline_state);

        // clang-format off
        float points[] = {
            0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
        };
        // clang-format on

        uint32_t indices[] = {0, 1, 2};

        // clang-format off
        float mvp[] = {
            1.0f, 0.0f, 0.0f, 0.0f, // color + padding float
            // mvp matrix
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        };
        // clang-format on

        focus::VertexBufferLayout vb_layout("Input");
        vb_layout.Add("aPosition", focus::VarType::Float3);

        focus::IndexBufferLayout ib_layout(focus::IndexBufferType::U32);

        focus::ConstantBufferLayout cb_layout("Constants");
        cb_layout.Add("color and mvp", focus::VarType::Float4x4);

        _triangle_scene_state = {
            .vb_handles = {device->CreateVertexBuffer(vb_layout, (uint8_t *)points, sizeof(points))},
            .cb_handles = {device->CreateConstantBuffer(cb_layout, (uint8_t *)mvp, sizeof(mvp))},
            .ib_handle = device->CreateIndexBuffer(ib_layout, (uint8_t *)indices, sizeof(indices)),
            .indexed = true,
        };
    }
    ~TriangleExample() override = default;
    void DoFrame() override;
};