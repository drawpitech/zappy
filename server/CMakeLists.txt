# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(MyCProject C)

# Set the directories containing the source files and headers
set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

# Collect all .c files in the source directory
file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.c")

# Include directories
include_directories(${INCLUDE_DIR})

# Add the executable target
add_executable(MyCProject ${SOURCES})

# Optionally, you can set the C standard
set_target_properties(MyCProject PROPERTIES C_STANDARD 99)
