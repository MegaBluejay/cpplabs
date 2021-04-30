cmake_minimum_required(VERSION 3.19)
project(lab3)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(pugixml-1.11)

add_executable(lab3 main.cpp)
target_link_libraries(lab3 pugixml)
