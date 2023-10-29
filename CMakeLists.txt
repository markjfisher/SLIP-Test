cmake_minimum_required(VERSION 3.10)

project(SLIPTerminal)

set(CMAKE_CXX_STANDARD 17)

add_library(SLIPInterface INTERFACE)
target_include_directories(SLIPInterface INTERFACE include/SLIPInterface)

add_library(BaseDevice INTERFACE)
target_include_directories(BaseDevice INTERFACE include/BaseDevice)

add_library(DeviceImpls INTERFACE)
target_include_directories(DeviceImpls INTERFACE include/DeviceImpls)

add_executable(SLIPTerminal src/main.cpp)
target_link_libraries(SLIPTerminal SLIPInterface DeviceImpls)

target_compile_features(SLIPTerminal PRIVATE cxx_std_17)
