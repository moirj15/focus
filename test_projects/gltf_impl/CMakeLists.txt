cmake_minimum_required(VERSION 3.10)
project(gltf)

include_directories(
        ${CMAKE_SOURCE_DIR}/include;
        e:/projects/focus/pub_include;
)


set(SRC
        src/main.cpp
        )


if (WIN32)
    set(CMAKE_CXX_FLAGS "/std:c++latest /MTd /MP /TP /W3 -D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -D_CRT_SECURE_NO_WARNINGS")
    set(CompilerFlags
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            )
    foreach (CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
        string(REPLACE "/MDd" "/MTd" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach ()
endif ()

add_executable(
        gltf
        ${SRC})

target_link_libraries(
        gltf
        e:/projects/focus/lib/focus.lib
        e:/projects/focus/libs/windows/SDL2-static.lib
        e:/projects/focus/libs/windows/SDL2main.lib
        ${CMAKE_SOURCE_DIR}/libs/fmt.lib

        opengl32.lib
        gdi32.lib
        user32.lib
        kernel32.lib
        msvcrtd.lib
        winmm.lib
        imm32.lib
        version.lib
        d3d11.lib
        d3d12.lib
        libcmt.lib
        dxgi.lib
        dxguid.lib
        d3dcompiler.lib
        winmm.lib
        gdi32
        opengl32
        Setupapi.lib
)
