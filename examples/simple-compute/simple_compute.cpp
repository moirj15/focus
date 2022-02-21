#include "simple_compute.hpp"

void SimpleCompute::DoFrame()
{
    _device->ClearBackBuffer({});
    _device->DispatchCompute(256, 256, 1, compute_shader, {{shader_buffer}, {}});
    _device->WaitForMemory(0);
    auto *contents2 = (float *)_device->MapBuffer(shader_buffer, focus::AccessMode::ReadOnly);
    _device->UnmapBuffer(shader_buffer);

    _device->SwapBuffers(_window);
}
