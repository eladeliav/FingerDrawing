message("$ENV{OPENCV_DIR}")
set(OpenCV_DIR $ENV{OPENCV_DIR})
set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED)

set(SOURCES
        src/main.cpp
        include/DrawingCam.hpp
        src/DrawingCam.cpp src/ForegroundExtractor.cpp include/ForegroundExtractor.hpp src/SkinDetector.cpp include/SkinDetector.hpp src/FacesRemover.cpp include/FacesRemover.hpp src/FingerCounter.cpp include/FingerCounter.hpp)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})