cmake_minimum_required(VERSION 3.18)

project(navmesh)

add_library(navmesh
    src/lib/graph/Graph.cc
    src/lib/graph/Node.cc
    src/lib/graph/triangulation/Delaunay.cc
    src/lib/topology/Point.cc
    src/lib/topology/shapes/Triangle.cc
)

target_include_directories(navmesh PUBLIC
    src/lib/)

add_executable(app_1
    src/exe/app_1.cc)

target_link_libraries(app_1 PRIVATE navmesh)

add_executable(app_2
    src/exe/app_2.cc)

target_link_libraries(app_2 PRIVATE navmesh)


add_executable(app_3
    src/exe/app_3.cc)

target_link_libraries(app_3 PRIVATE navmesh)
