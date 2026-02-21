cmake_minimum_required(VERSION 4.1)
project(devenv)

set(CMAKE_CXX_STANDARD 20)

find_package(nlohmann_json REQUIRED)

add_executable(devenv main.cpp)
target_link_libraries(devenv PRIVATE nlohmann_json::nlohmann_json)
