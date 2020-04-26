set(TARGET_NAME FingerDrawing)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_PREFIX_PATH $ENV{QT_DIR})

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
message("-- Found Qt5Core: ${Qt5Core_FOUND}")
message("-- Found Qt5Widgets: ${Qt5Widgets_FOUND}")

set(SOURCES
        src/main.cpp
        include/DrawingCam.hpp
        src/DrawingCam.cpp
        src/FingersDetector.cpp
        include/FingersDetector.hpp
        src/Helpers.cpp
        include/Helpers.hpp
        include/ForegroundExtractor.hpp
        src/ForegroundExtractor.cpp
        include/FacesRemover.hpp
        src/FacesRemover.cpp
        include/SkinDetector.hpp
        src/SkinDetector.cpp
        ui/MainWindow.hpp
        ui/MainWindow.cpp
        ui/mainwindow.ui
        include/timer.hpp
        include/ConnectionManager.hpp
        src/ConnectionManager.cpp
)

configure_file("../../Resources/haarcascade_frontalface_alt.xml" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/haarcascade_frontalface_alt.xml" COPYONLY)

add_executable(${TARGET_NAME} ${SOURCES})
target_include_directories(${TARGET_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS} ${UniSockets_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} ${OpenCV_LIBS} UniSockets::UniSockets Qt5::Widgets)
if(WIN32)
    target_link_libraries(${TARGET_NAME} -static winpthread)
    endif()
message("-- Finished configuring Target: ${TARGET_NAME}")
