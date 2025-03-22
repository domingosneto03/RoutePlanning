cmake_minimum_required(VERSION 3.30)
project(da_project1)

set(CMAKE_CXX_STANDARD 20)

add_executable(da_project1 main.cpp
        data_structures/Graph.h
        data_structures/MutablePriorityQueue.h
        menu.cpp
        menu.h
        reader.h
        RestrictedRoute.cpp
        RestrictedRoute.h
        BestRoute.cpp
        BestRoute.h
)
