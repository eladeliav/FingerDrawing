set(TARGET_NAME FingerDrawing)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

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
        include/MainWindow.hpp
        src/MainWindow.cpp
        mainwindow.ui
        )

add_executable(${TARGET_NAME} ${SOURCES})
target_include_directories(${TARGET_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS} ${UniSockets_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} ${OpenCV_LIBS} UniSockets::UniSockets -static winpthread Qt5::Widgets)
message("-- Finished configuring Target: ${TARGET_NAME}")
