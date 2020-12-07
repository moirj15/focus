cmake_minimum_required(VERSION 3.10)
project(focus)
set(CMAKE_UNITY_BUILD ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#TODO put the defines up here

#file(GLOB SRC CONFIGURE_DEPENDS "src/*")
set(SRC
        src/main.cpp
        #src/stb_image.h
        #src/stb_image.cpp
        src/common.h
        )
file(GLOB RENDERER_SRC CONFIGURE_DEPENDS "src/renderer/*")
file(GLOB RENDERER_VULKAN_SRC CONFIGURE_DEPENDS "src/renderer/vk/*")
file(GLOB RENDERER_OPENGL_SRC CONFIGURE_DEPENDS "src/renderer/gl/*")
file(GLOB RENDERER_DX11_SRC CONFIGURE_DEPENDS "src/renderer/dx11/*")
file(GLOB RENDERER_INTERFACE_SRC CONFIGURE_DEPENDS "src/renderer/Interface/*")
file(GLOB RENDERER_INTERFACE_HEADERS CONFIGURE_DEPENDS "src/renderer/Interface/*.hpp")
#file(GLOB SHADER_HLSL_VERTEX_SRC CONFIGURE_DEPENDS "shaders/dx11/*")

#set_source_files_properties(${SHADER_HLSL_VERTEX_SRC} PROPERTIES VS_SHADER_TYPE Vertex VS_SHADER_MODEL 5.0 VS_SHADER_ENTRY_POINT main)

include_directories(
        ${CMAKE_SOURCE_DIR}/include;
)

if (WIN32)
    add_executable(
            focus_main
            ${SRC}
            ${RENDERER_SRC}
#            ${RENDERER_VULKAN_SRC}
            ${RENDERER_OPENGL_SRC}
#            ${RENDERER_DX11_SRC}
            ${RENDERER_INTERFACE_SRC}
    )
    add_library(focus STATIC ${RENDERER_INTERFACE_SRC} ${RENDERER_OPENGL_SRC})
    target_include_directories(focus PUBLIC ${RENDERER_INTERFACE_SRC})
    message("WINDOWS BUILD")
    message("${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
    #    set(CMAKE_CXX_FLAGS "/std:c++latest /MDd /MP /TP /W3 -D_CRT_SECURE_NO_WARNINGS -DGLEW_STATIC -DGLFW_INCLUDE_NONE -DRENDERER_DX11")
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
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
        string(REPLACE "/MDd" "/MTd" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()

    find_package(OpenGL REQUIRED)
    target_link_libraries(
            focus_main
#            ${CMAKE_SOURCE_DIR}/libs/windows/vulkan-1.lib
            ${CMAKE_SOURCE_DIR}/libs/windows/glfw3.lib
            d3d11.lib
            dxgi.lib
            d3dcompiler.lib
            winmm.lib
            gdi32
            opengl32
            #D3D11_1SDKLayers.dll
    )
    install(TARGETS focus DESTINATION ${CMAKE_SOURCE_DIR}/lib)
    install(FILES ${RENDERER_INTERFACE_HEADERS} DESTINATION ${CMAKE_SOURCE_DIR}/pub_include)
else ()
    add_executable(
            vkRenderer
            ${SRC}
            ${RENDERER_SRC}
            # comment out while working on opengl stuff
            #${RENDERER_VULKAN_SRC}
            ${RENDERER_OPENGL_SRC}
            ${RENDERER_INTERFACE_SRC}
    )
    message("LINUX BUILD")


    set(CMAKE_CXX_FLAGS "-std=c++2a -g -Wall -Wextra -pedantic -D_DEBUG -DGLFW_INCLUDE_NONE -DRENDERER_OPENGL -DGLFW_INCLUDE_VULKAN")
    set(LINUX_LIBS "${CMAKE_SOURCE_DIR}/libs/linux")
    target_link_libraries(
            vkRenderer
            ${LINUX_LIBS}/libvulkan.so
            ${LINUX_LIBS}/libglfw3.a
            ${LINUX_LIBS}/libfmt.a
            X11
            #Xxf86vm
            #Xrandr
            #pthread
            #Xi
            dl
            #Xinerama
            #Xcursor
            pthread


    )

endif ()

