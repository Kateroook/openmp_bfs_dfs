cmake_minimum_required(VERSION 3.29)
project(bfs_dfs_parallel)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)

add_executable(bfs_dfs_parallel
        main.cpp
        Graph.cpp
        Graph.h
        GraphGenerator.cpp
        GraphGenerator.h
        TestResult.h
        ParallelGraphTraversal.cpp
        ParallelGraphTraversal.h
        tester.h
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(bfs_dfs_parallel PUBLIC OpenMP::OpenMP_CXX)
endif()