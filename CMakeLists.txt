cmake_minimum_required(VERSION 3.17)
project(boolalg_calc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output)

include_directories(${PROJECT_SOURCE_DIR})

add_executable(boolalg_calc main.cpp boolean_expression.cpp boolean_expression.h test/test1.cpp)
target_link_libraries(boolalg_calc -lfmt)

add_executable(test1 test/test1.cpp boolean_expression.cpp)
target_link_libraries(test1 -lfmt)
