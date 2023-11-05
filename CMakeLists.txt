cmake_minimum_required(VERSION 3.4...3.22)
project(SLIPTerminal)

set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(include)

set(SOURCE_FILES src/main.cpp
    include/SmartPortHandler.h include/FakeSmartPortHandler.h src/FakeSmartPortHandler.cpp
    include/Listener.h src/Listener.cpp
    include/Requestor.h src/Requestor.cpp
    include/Responder.h src/Responder.cpp
    include/SLIP.h src/SLIP.cpp
    include/StatusRequest.h src/StatusRequest.cpp
    include/StatusResponse.h src/StatusResponse.cpp
    include/TCPConnection.h src/TCPConnection.cpp
    include/Util.h src/Util.cpp
)

add_executable(SLIPTerminal ${SOURCE_FILES})

message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
if(${CMAKE_SYSTEM_NAME} MATCHES "MINGW64_NT.*" OR ${CMAKE_SYSTEM_NAME} MATCHES "MINGW32_NT.*")
    # this one is working on MSYS2 MINGW64 for me
    message(STATUS "Adding ws2_32 (1)")
    target_link_libraries(SLIPTerminal ws2_32)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Adding ws2_32 (2)")
    target_link_libraries(SLIPTerminal ws2_32)
endif()

if(WIN32)
    message(STATUS "Adding ws2_32 (3)")
    target_link_libraries(SLIPTerminal ws2_32)
endif()

# should be add automatically! My config must suck
target_link_libraries(SLIPTerminal stdc++)

target_compile_features(SLIPTerminal PRIVATE cxx_std_17)
