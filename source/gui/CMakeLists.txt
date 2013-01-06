cmake_minimum_required(VERSION 2.8)

qt4_wrap_cpp(MOC_FILES
	classbrowser.hpp
	classmodel.hpp
	objectenummodel.hpp
	querymodel.hpp
	queryviewer.hpp
	window.hpp
	wqlhighlighter.hpp
)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	set(EXETYPE WIN32)
endif()

configure_file(wmiviewer.rc wmiviewer.rc)

add_executable(wmiviewer ${EXETYPE}
	classbrowser.hpp
	classbrowser.cpp
	classmodel.hpp
	classmodel.cpp
	objectenummodel.hpp
	objectenummodel.cpp
	querymodel.hpp
	querymodel.cpp
	queryviewer.hpp
	queryviewer.cpp
	window.hpp
	window.cpp
	wmiviewer.cpp
	wqlhighlighter.hpp
	wqlhighlighter.cpp
	"${CMAKE_CURRENT_BINARY_DIR}/wmiviewer.rc"
	${MOC_FILES}
)

target_link_libraries(wmiviewer
	qtwmiwrapper
	${QT_LIBRARIES}
	${WBEMUUID_LIBRARY}
)
