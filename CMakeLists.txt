cmake_minimum_required(VERSION 3.17)
project(boolalg_calc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output)

include_directories(${PROJECT_SOURCE_DIR})
link_libraries(-lfmt)

add_executable(boolalg_calc main.cpp boolexpr.cpp kmap.cpp)

add_executable(test1 test/test1.cpp boolexpr.cpp)

add_executable(test2 test/test2.cpp boolexpr.cpp)

add_executable(test3 test/test3.cpp boolexpr.cpp kmap.cpp)
