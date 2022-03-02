#include "instancing/instancing.hpp"
#include "simple-compute/simple_compute.hpp"
#include "triangle/triangle.hpp"

#include <SDL2/SDL.h>
#include <memory>
#include <vector>

std::vector<std::unique_ptr<Example>> examples;

/**
 * @brief test doc
 */
void Init(focus::Device *device, focus::Window window)
{
    //    examples.emplace_back(new TriangleExample(device, window));
    //    examples.emplace_back(new SimpleCompute(device, window));
    examples.emplace_back(new Instancing(device, window));
}

int main(int argc, char **argv)
{
    auto *device = focus::Device::Init(focus::RendererAPI::OpenGL);
    auto window = device->MakeWindow(1920, 1080);

    Init(device, window);

    SDL_Event e;
    int example_index = 0;
    while (true) {
        if (example_index >= examples.size()) {
            break;
        }

        examples[example_index]->DoFrame();

        while (SDL_PollEvent(&e) > 0) {
            if (e.type == SDL_QUIT) {
                example_index++;
            }
        }
    }
    return 0;
}
