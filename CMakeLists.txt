cmake_minimum_required(VERSION 3.10)
project(focus)
#set(CMAKE_UNITY_BUILD ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Opengl Backend
set(OPENGL_BACKEND_DIR ${PROJECT_SOURCE_DIR}/src/gl)
set(OPENGL_BACKEND_SRC
        ${OPENGL_BACKEND_DIR}/GLDevice.cpp
        ${OPENGL_BACKEND_DIR}/ShaderManager.cpp
        ${OPENGL_BACKEND_DIR}/glad.c
        )

# Interface
set(INTERFACE_DIR ${PROJECT_SOURCE_DIR}/src/Interface)
set(INTERFACE_SRC
        ${INTERFACE_DIR}/focus.cpp
        )

set(INTERFACE_HEADERS
        ${INTERFACE_DIR}/focus.hpp
        )

include_directories(
        ${PROJECT_SOURCE_DIR}/include;
)

add_library(focus STATIC
        ${INTERFACE_SRC}
        ${INTERFACE_HEADERS}
        ${OPENGL_BACKEND_SRC}
        ${OPENGL_BACKEND_HEADERS}
        )
target_include_directories(focus PUBLIC ${INTERFACE_DIR})

add_subdirectory(examples)