cmake_minimum_required(VERSION 3.19)

project(SLIPTerminal)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_library(BaseDevice INTERFACE)
target_include_directories(BaseDevice INTERFACE include)

add_library(DeviceImpls INTERFACE)
target_include_directories(DeviceImpls INTERFACE include)

add_library(SLIPInterface INTERFACE)
target_include_directories(SLIPInterface INTERFACE include)
target_link_libraries(SLIPInterface INTERFACE BaseDevice)

add_executable(SLIPTerminal src/main.cpp)
target_link_libraries(SLIPTerminal SLIPInterface DeviceImpls BaseDevice)

target_compile_features(SLIPTerminal PRIVATE cxx_std_17)
