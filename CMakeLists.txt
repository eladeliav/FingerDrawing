cmake_minimum_required(VERSION 3.0.0)
project(FingerDrawing VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -std=c++11")
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -static-libgcc -static-libstdc++")
    message(${CMAKE_CXX_FLAGS})
    endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)


set(OpenCV_DIR $ENV{OPENCV_DIR})
message(${OpenCV_DIR})
set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED)
message("-- Found OpenCV: ${OpenCV_FOUND}")
set(UniSockets_DIR "C:\\Users\\elade\\Programming\\UniSockets\\build")

find_package(UniSockets REQUIRED)
message("-- Found UniSockets: ${UniSockets_FOUND}")

add_subdirectory(src)
message("about to path")
foreach(path ${CMAKE_PREFIX_PATH})
    message("Path: " ${path})
endforeach(path)