cmake_minimum_required(VERSION 3.12)
project(zappy_gui)

set(CMAKE_CXX_STANDARD 20)
file(GLOB_RECURSE SOURCES src/**.cpp)

# ImGui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
)
include_directories(${IMGUI_DIR})

# Linux specific flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Werror -Wall -Wextra -Wpedantic")
endif()

# Executable
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../external/stb)
add_executable(zappy_gui ${SOURCES} ${IMGUI_SOURCES})
target_link_libraries(zappy_gui glfw glm glad assimp)

# Copy zappy_gui to build
add_custom_command(TARGET zappy_gui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:zappy_gui> ..)
