cmake_minimum_required(VERSION 3.10)
project(focus)
#set(CMAKE_UNITY_BUILD ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#TODO put the defines up here

set(SRC
        src/main.cpp
        src/common.h
        )
file(GLOB RENDERER_SRC CONFIGURE_DEPENDS "src/renderer/*")

# Opengl Backend
set(OPENGL_BACKEND_DIR ${CMAKE_SOURCE_DIR}/src/renderer/gl)
set(OPENGL_BACKEND_SRC
        ${OPENGL_BACKEND_DIR}/BufferManager.cpp
        ${OPENGL_BACKEND_DIR}/GLContext.cpp
        ${OPENGL_BACKEND_DIR}/ShaderManager.cpp
        ${OPENGL_BACKEND_DIR}/glad.c
        )
set(OPENGL_BACKEND_HEADERS
        ${OPENGL_BACKEND_DIR}/BufferManager.hpp
        ${OPENGL_BACKEND_DIR}/GLContext.hpp
        ${OPENGL_BACKEND_DIR}/ShaderManager.hpp
        ${OPENGL_BACKEND_DIR}/Utils.hpp
        ${OPENGL_BACKEND_DIR}/Types.hpp
        ${OPENGL_BACKEND_DIR}/glad.h
        )

# vulkan backend
file(GLOB RENDERER_VULKAN_SRC CONFIGURE_DEPENDS "src/renderer/vk/*")

# DX11 backend

set(DX11_BACKEND_DIR ${CMAKE_SOURCE_DIR}/src/renderer/dx11)
set(DX11_BACKEND_SRC
        ${DX11_BACKEND_DIR}/DX11Context.cpp
        ${DX11_BACKEND_DIR}/ShaderManager.cpp
        )
set(DX11_BACKEND_HEADERS
        ${DX11_BACKEND_DIR}/DX11Context.hpp
        ${DX11_BACKEND_DIR}/ShaderManager.hpp
        ${DX11_BACKEND_DIR}/BufferManager.hpp
        ${DX11_BACKEND_DIR}/ShaderBufferManager.hpp
        ${DX11_BACKEND_DIR}/Types.hpp
        ${DX11_BACKEND_DIR}/Utils.hpp
        )

# DX12 backend
file(GLOB RENDERER_DX12_SRC CONFIGURE_DEPENDS "src/renderer/dx12/*")

# Interface
set(INTERFACE_DIR ${CMAKE_SOURCE_DIR}/src/renderer/Interface)
set(INTERFACE_SRC
        ${INTERFACE_DIR}/Context.cpp)
set(INTERFACE_HEADERS
        ${INTERFACE_DIR}/Context.hpp)

include_directories(
        ${CMAKE_SOURCE_DIR}/include;
)

add_library(focus STATIC
        ${INTERFACE_SRC}
        ${INTERFACE_HEADERS}
        ${OPENGL_BACKEND_SRC}
        ${OPENGL_BACKEND_HEADERS}
        #${DX11_BACKEND_SRC}
        #${DX11_BACKEND_SRC}
        )
install(TARGETS focus DESTINATION ${CMAKE_SOURCE_DIR}/lib)
install(FILES ${INTERFACE_HEADERS} DESTINATION ${CMAKE_SOURCE_DIR}/pub_include)
if (WIN32)
    add_executable(
            focus_main
            ${SRC}
            ${RENDERER_SRC}
            ${OPENGL_BACKEND_SRC}
            ${OPENGL_BACKEND_HEADERS}
            ${DX11_BACKEND_SRC}
            ${DX11_BACKEND_SRC}
            ${RENDERER_DX12_SRC}
            ${INTERFACE_SRC}
            ${INTERFACE_HEADERS}
    )
    target_link_directories(focus_main PUBLIC ${CMAKE_SOURCE_DIR}/libs/windows)
    set_target_properties(
            focus_main PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    target_include_directories(focus PUBLIC ${RENDERER_INTERFACE_SRC})
    message("WINDOWS BUILD")
    message("${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
    set(CMAKE_CXX_FLAGS "/std:c++latest /MTd /MP /TP /W3 -D_CRT_SECURE_NO_WARNINGS -DGLEW_STATIC -DGLFW_INCLUDE_NONE -DRENDERER_OPENGL")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DGLFW_INCLUDE_NONE -D_DEBUG")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE /ENTRY:WinMainCRTStartup")
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

    find_package(OpenGL REQUIRED)
    target_link_libraries(
            focus_main
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
else ()
    add_executable(
            focus_main
            ${SRC}
            ${RENDERER_SRC}
            ${OPENGL_BACKEND_SRC}
            ${OPENGL_BACKEND_HEADERS}
            ${INTERFACE_SRC}
            ${INTERFACE_HEADERS}
    )
    message("LINUX BUILD")


    set(CMAKE_CXX_FLAGS "-std=c++2a -g -Wall -Wextra -pedantic -D_DEBUG -DGLFW_INCLUDE_NONE -DRENDERER_OPENGL -DGLFW_INCLUDE_VULKAN")
    set(LINUX_LIBS "${CMAKE_SOURCE_DIR}/libs/linux")
    target_link_libraries(
            focus_main
            X11
            dl
            pthread
            /usr/local/lib/libSDL2.so
    )

endif ()

