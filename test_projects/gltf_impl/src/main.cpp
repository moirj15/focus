#include <fmt/core.h>
#include <fmt/format.h>
#include <focus.hpp>

int main(int argc, char **argv)
{
  focus::Init(focus::RendererAPI::OpenGL);
  fmt::print("hello {}\n", "test");
  return 0;
}
