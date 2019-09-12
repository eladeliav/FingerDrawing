set(TARGET_NAME FingerDrawing)

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
        )

find_package(Qt5Widgets REQUIRED)

add_executable(${TARGET_NAME} ${SOURCES})
target_include_directories(${TARGET_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS} ${UniSockets_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} ${OpenCV_LIBS} UniSockets::UniSockets -static winpthread Qt5::Widgets)
message("-- Finished configuring Target: ${TARGET_NAME}")