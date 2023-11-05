cmake_minimum_required(VERSION 3.19)

project(SLIPTerminal)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCE_FILES src/main.cpp
    include/SmartPortHandler.h include/FakeSmartPortHandler.h src/FakeSmartPortHandler.cpp
    include/Listener.h src/Listener.cpp
    include/Requestor.h src/Requestor.cpp
    include/Responder.h src/Responder.cpp
    include/SLIP.h src/SLIP.cpp
    include/StatusRequest.h src/StatusRequest.cpp
    include/StatusResponse.h src/StatusResponse.cpp
    include/TCPConnection.h src/TCPConnection.cpp
)

add_executable(SLIPTerminal ${SOURCE_FILES})

target_compile_features(SLIPTerminal PRIVATE cxx_std_17)
