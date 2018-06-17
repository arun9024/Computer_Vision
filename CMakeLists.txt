cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(measure_focus)
find_package( OpenCV REQUIRED )

include_directories(include)

set(SOURCE_MEASURE_FOCUS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

add_executable(measure_focus ${SOURCE_MEASURE_FOCUS})

SET(COMPILE_FLAGS "-g -std=c++11")
SET(CMAKE_CXX_FLAGS "-Wall -O2")
add_definitions(${COMPILE_FLAGS})

TARGET_LINK_LIBRARIES(measure_focus
    ${OpenCV_LIBS}
)
