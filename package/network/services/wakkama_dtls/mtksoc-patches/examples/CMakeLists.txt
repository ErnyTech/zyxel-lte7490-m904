cmake_minimum_required(VERSION 2.8.9)

add_definitions(-D_POSIX_C_SOURCE=200809)
add_definitions(-pedantic)
#add_definitions(-DDTLS=1)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/client)
