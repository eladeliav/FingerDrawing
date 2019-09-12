set(TARGET_NAME FingerDrawingServer)

set(SOURCES
        src/main.cpp
)

add_executable(${TARGET_NAME} ${SOURCES})
target_include_directories(${TARGET_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS} ${UniSockets_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} ${OpenCV_LIBS} UniSockets::UniSockets)
message("-- Finished configuring Target: ${TARGET_NAME}")