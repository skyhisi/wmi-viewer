cmake_minimum_required(VERSION 2.8)

find_package(Qt4 4.6 REQUIRED QtCore QtGui)
set(QT_USE_QTMAIN TRUE)
include(${QT_USE_FILE})

find_library(WBEMUUID_LIBRARY wbemuuid)

add_definitions(-D_WIN32_DCOM)

add_subdirectory(wmi)
add_subdirectory(gui)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	add_subdirectory(package)
else()
	message(STATUS "Not building package, only built in Release mode")
endif()


