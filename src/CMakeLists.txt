cmake_minimum_required(VERSION 3.18)

add_library(navmesh
    lib/graph/Graph.cc
    lib/graph/Node.cc
    lib/graph/triangulation/Delaunay.cc
    lib/topology/Point.cc
    lib/topology/shapes/Triangle.cc
)

target_include_directories(navmesh PUBLIC
    lib/)


set(CDT_USE_AS_COMPILED_LIBRARY ON)
add_subdirectory(cdt)

target_include_directories(navmesh PUBLIC
    cdt/include/)

add_executable(app_1
    exe/app_1.cc)

target_link_libraries(app_1 PRIVATE navmesh)

add_executable(app_2
    exe/app_2.cc)

target_link_libraries(app_2 PRIVATE navmesh)


add_executable(app_3
    exe/app_3.cc)

target_link_libraries(app_3 PRIVATE navmesh)
