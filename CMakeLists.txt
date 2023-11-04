cmake_minimum_required(VERSION 3.19)

project(SLIPTerminal)

set(CMAKE_CXX_STANDARD 17)

# global adding include dir, shouldn't be needed, but keeping it for ref
#include_directories(include)

add_library(SLIP src/SLIP.cpp)
target_include_directories(SLIP PUBLIC include)

add_library(Listener src/Listener.cpp)
target_include_directories(Listener PUBLIC include)
# target_link_libraries(Listener SLIP)

add_library(Requestor src/Requestor.cpp)
target_include_directories(Requestor PUBLIC include)
target_link_libraries(Requestor Listener)

add_executable(SLIPTerminal src/main.cpp)
target_link_libraries(SLIPTerminal SLIPInterface DeviceImpls BaseDevice)

target_compile_features(SLIPTerminal PRIVATE cxx_std_17)
