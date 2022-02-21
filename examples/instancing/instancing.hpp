#pragma once

#include "../common/common.hpp"

class Instancing final : public Example
{
    focus::Pipeline _pipeline;
    focus::SceneState _scene_state;
    uint32_t _vertex_count = 0;
  public:
    Instancing(focus::Device *device, const focus::Window &window);
    void DoFrame() override;
};

