cmake_minimum_required(VERSION 3.12)
project(zappy_gui)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
file(GLOB_RECURSE SOURCES src/**.cpp)

# Flags to avoid assimp compile errors
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder -Wno-array-bounds -Wno-stringop-overflow -Wno-deprecated-declarations -Wno-sign-compare -Wno-dangling-reference -Wno-maybe-uninitialized")
endif()

# Link to external libraries
add_subdirectory(external/assimp)
add_subdirectory(external/glad)
add_subdirectory(external/glfw)
add_subdirectory(external/glm)

# ImGui
set(IMGUI_DIR external/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
include_directories(${IMGUI_DIR})

# Linux specific flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Werror -Wall -Wextra -Wpedantic")
endif()

# Executable
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/external/stb)
add_executable(zappy_gui ${SOURCES} ${IMGUI_SOURCES})
target_link_libraries(zappy_gui glfw glm glad assimp)

# Copy zappy_gui to build
install(TARGETS zappy_gui DESTINATION ${CMAKE_INSTALL_BINDIR})
