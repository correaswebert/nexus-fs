add_executable(nexus-fuse main.cpp fuse/fuse_ops.cpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE3 REQUIRED fuse3)

target_include_directories(nexus-fuse PRIVATE ${FUSE3_INCLUDE_DIRS})
target_link_libraries(nexus-fuse PRIVATE ${FUSE3_LIBRARIES})
