cmake_minimum_required(VERSION 3.10)
project(vkRenderer)
set(CMAKE_UNITY_BUILD ON)

file(GLOB SRC CONFIGURE_DEPENDS "src/*")

add_executable(
  vkRenderer
  ${SRC}
)


if (WIN32)
  message("WINDOWS")
  include_directories(
    G:/libraries/glm;
    G:/libraries/glfw-3.3.2.bin.WIN64/include;
    G:/VulkanSDK/1.2.131.2/include;
  )
  set(CMAKE_CXX_FLAGS "/std:c++latest /MDd /MP /TP /W3 -D_CRT_SECURE_NO_WARNINGS -DGLEW_STATIC")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
  target_link_libraries(
    vkRenderer
    G:/VulkanSDK/1.2.131.2/Lib/vulkan-1.lib

    G:/libraries/glfw-3.3.2.bin.WIN64/lib-vc2019/glfw3.lib
  )
else()

endif ()

