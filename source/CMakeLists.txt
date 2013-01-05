cmake_minimum_required(VERSION 2.8)

find_package(Qt4 4.6 REQUIRED QtCore QtGui)
include(${QT_USE_FILE})

find_library(WBEMUUID_LIBRARY wbemuuid)

add_definitions(-D_WIN32_DCOM)

add_subdirectory(wmi)
add_subdirectory(gui)

