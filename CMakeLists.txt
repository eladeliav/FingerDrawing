cmake_minimum_required(VERSION 3.0.0)
project(FingerDrawing VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

message("$ENV{OPENCV_DIR}")
set(OpenCV_DIR $ENV{OPENCV_DIR})
set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED)

set(SOURCES
        src/src/main.cpp
        src/include/DrawingCam.hpp
        src/src/DrawingCam.cpp
        src/src/ForegroundExtractor.cpp
        src/include/ForegroundExtractor.hpp
        src/src/SkinDetector.cpp src/include/SkinDetector.hpp
        src/src/FacesRemover.cpp src/include/FacesRemover.hpp
        src/src/FingerCounter.cpp
        src/include/FingerCounter.hpp)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})