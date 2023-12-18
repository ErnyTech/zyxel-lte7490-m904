cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_BUILD_TYPE "Debug")
add_definitions(-DLWM2M_SUPPORT_TLV)
add_definitions(-DDTLS=1)

project(wakaama_dtls C)

# Enable "test" target
enable_testing()

add_subdirectory(examples)
