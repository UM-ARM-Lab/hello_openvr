cmake_minimum_required(VERSION 2.8)
project(hello_openvr)

set(openvr_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/openvr/headers")
#set(openvr_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/openvr/lib/linux64")
set(openvr_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/openvr/lib/win64")
set(openvr_BINARY_DIR "${CMAKE_SOURCE_DIR}/openvr/bin/win64")

include_directories(SYSTEM ${openvr_INCLUDE_DIR})
link_directories(${openvr_LIBRARY_DIR})

add_executable(main main.cpp)
target_link_libraries(main openvr_api)

file(COPY ${openvr_BINARY_DIR}/openvr_api.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
