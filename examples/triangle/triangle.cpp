#include "triangle.hpp"

void TriangleExample::DoFrame()
{
    _device->ClearBackBuffer({});
    _device->BeginPass("Triangle pass");

    _device->BindSceneState(_triangle_scene_state);
    _device->BindPipeline(_triangle_pipeline);
    _device->Draw(focus::Primitive::Triangles, 0, 3);

    _device->EndPass();

    _device->SwapBuffers(_window);
}
