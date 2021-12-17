cmake_minimum_required(VERSION 3.0.0)
project(gstreamer_cxx VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(PkgConfig) #finding pkg-config is a helper tool

#using pkg-config to getting Gstreamer
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

#including GStreamer header files directory
include_directories(
        ${GLIB_INCLUDE_DIRS}
        ${GSTREAMER_INCLUDE_DIRS}
)

#linking GStreamer library directory
link_directories(
        ${GLIB_LIBRARY_DIRS}
        ${GSTREAMER_LIBRARY_DIRS}
)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(gstreamer_cxx main.cpp GstVideoCap/GstVideoCap.h)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(${PROJECT_NAME} ${GSTREAMER_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
