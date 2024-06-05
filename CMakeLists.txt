cmake_minimum_required(VERSION 3.12)
project(Zappy)

# Flags to avoid assimp compile errors
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder -Wno-array-bounds -Wno-stringop-overflow -Wno-deprecated-declarations -Wno-sign-compare -Wno-dangling-reference -Wno-maybe-uninitialized")
endif()

# Link to external libraries
add_subdirectory(external/assimp)
add_subdirectory(external/glad)
add_subdirectory(external/glfw)
add_subdirectory(external/glm)
add_subdirectory(server)

# Link to project modules
add_subdirectory(GUI)
