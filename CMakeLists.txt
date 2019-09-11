cmake_minimum_required(VERSION 3.0.0)
project(FingerDrawing VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

set(OpenCV_DIR $ENV{OPENCV_DIR})
set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED)

set(UniSockets_DIR $ENV{UNISOCKETS_DIR})
find_package(UniSockets REQUIRED)
message("-- Found UniSockets: ${UniSockets_FOUND}")

add_subdirectory(src)