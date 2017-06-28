cmake_minimum_required(VERSION 3.6)

project(example)

add_subdirectory(cpp-curlsession)

add_executable(examples main.cpp)
target_link_libraries(examples curlsession)