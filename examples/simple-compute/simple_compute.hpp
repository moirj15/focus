#pragma once

#include "../common/common.hpp"

class SimpleCompute : public Example
{
    focus::ShaderBuffer shader_buffer;
    focus::Shader compute_shader;

  public:
    explicit SimpleCompute(focus::Device *device, focus::Window window) :
            Example(device, window, "Simple Compute Example")
    {
        compute_shader =
            device->CreateComputeShaderFromSource("TestCompute", utils::ReadEntireFileAsString("shaders/gl/test.comp"));

        focus::ShaderBufferLayout sDesc(0, focus::BufferUsage::Dynamic, "color_buf");
        sDesc.Add("color_buf", focus::VarType::Float);
        shader_buffer = device->CreateShaderBuffer(sDesc, nullptr, 256 * 256 * sizeof(float));
        auto *contents = (float *)device->MapBuffer(shader_buffer, focus::AccessMode::ReadOnly);
        for (int i = 0; i < 256 * 256; i++) {
            contents[i] = 88.0f;
        }
        device->UnmapBuffer(shader_buffer);
    }

    void DoFrame() override;
};
