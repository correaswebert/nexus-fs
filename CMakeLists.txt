cmake_minimum_required(VERSION 3.10)

project(NexusFuse VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

# create the mount point
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/fuse_mount)
