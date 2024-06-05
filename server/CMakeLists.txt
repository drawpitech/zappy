cmake_minimum_required(VERSION 3.27)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(zappy_server C)

set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.c")

add_executable(zappy_server ${SOURCES})
set_target_properties(zappy_server PROPERTIES C_STANDARD 99)

target_include_directories(zappy_server PRIVATE ${INCLUDE_DIR})

if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  # Sanitizers
  set(SANITIZER
      "NONE"
      CACHE STRING "Enable sanitizers")
  if(SANITIZER STREQUAL "MEMORY")
    target_link_options(zappy_server PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_compile_options(zappy_server PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
  elseif(SANITIZER STREQUAL "THREAD")
    target_link_options(zappy_server PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_compile_options(zappy_server PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
  endif()

  # Debug mode
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(zappy_server PRIVATE -DIS_DEBUG -g3)
  endif()

  # Warnings go brrr
  target_compile_options(zappy_server PRIVATE
    -W -Wall -Wextra -Wunused -Wpedantic
    -Wundef -Wshadow -Wcast-align -Wunused
    -Wunreachable-code
    -Wstrict-prototypes -Wmissing-prototypes
    -Waggregate-return -Wcast-qual
  )
endif()

# Optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set_target_properties(zappy_server PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

install(TARGETS zappy_server DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

