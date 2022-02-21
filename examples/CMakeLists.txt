
set(EXAMPLE_SOURCES
        common/common.cpp
        common/tiny_obj_loader.cpp
        triangle/triangle.cpp
        simple-compute/simple_compute.cpp
        lines/Lines.cpp
        instancing/instancing.cpp
        main.cpp
)
add_executable(examples
        ${EXAMPLE_SOURCES})

target_include_directories(examples
        PUBLIC
        libs/glm
)

target_link_libraries(
        examples
        focus
        Setupapi.lib
        #            ${CMAKE_SOURCE_DIR}/libs/windows/vulkan-1.lib
        ${CMAKE_SOURCE_DIR}/libs/windows/SDL2-static.lib
        ${CMAKE_SOURCE_DIR}/libs/windows/SDL2main.lib
        msvcrtd.lib
        winmm.lib
        imm32.lib
        version.lib
        d3d11.lib
        d3d12.lib
        libcmt.lib
        dxgi.lib
        d3dcompiler.lib
        dxguid.lib
        winmm.lib
        gdi32
        opengl32
)