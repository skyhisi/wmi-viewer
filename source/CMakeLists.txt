cmake_minimum_required(VERSION 2.8)

find_package(Qt4 4.6 REQUIRED QtCore QtGui)
set(QT_USE_QTMAIN TRUE)
include(${QT_USE_FILE})

find_library(WBEMUUID_LIBRARY wbemuuid)

set(PROJ_VERSION 0.0.7)
string(REPLACE "." "," PROJ_VERSION_COMMA "${PROJ_VERSION}")

add_definitions(-D_WIN32_DCOM -DPROJ_VERSION="${PROJ_VERSION}")
set(QT_VERSION "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")
set(DIAG_STR "Build:${CMAKE_BUILD_TYPE};Windows:${CMAKE_SYSTEM_VERSION};MSVC:${MSVC_VERSION};Qt:${QT_VERSION};CMake:${CMAKE_VERSION}")
#message(STATUS "DIAG_STR: ${DIAG_STR}")

add_subdirectory(wmi)
add_subdirectory(gui)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	add_subdirectory(package)
else()
	message(STATUS "Not building package, only built in Release mode")
endif()


