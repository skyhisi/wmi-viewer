cmake_minimum_required(VERSION 2.8)

configure_file(qtwmiwrapper.rc qtwmiwrapper.rc)

add_library(qtwmiwrapper SHARED
	comscope.hpp
	comscope.cpp
	qtcominterop.hpp
	qtcominterop.cpp
	wmibase.hpp
	wmiclassobjectenum.hpp
	wmiclassobjectenum.cpp
	wmiclassobject.hpp
	wmiclassobject.cpp
	wmiqualifierset.hpp
	wmiqualifierset.cpp
	wmilocator.hpp
	wmilocator.cpp
	wmiservice.hpp
	wmiservice.cpp
	"${CMAKE_CURRENT_BINARY_DIR}/qtwmiwrapper.rc"
)

target_link_libraries(qtwmiwrapper
	${QT_LIBRARIES}
	${WBEMUUID_LIBRARY}
)

