cmake_minimum_required(VERSION 2.8)

qt4_wrap_cpp(MOC_FILES
	classbrowser.hpp
	classmodel.hpp
	querymodel.hpp
	queryviewer.hpp
	window.hpp
	wqlhighlighter.hpp
)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	set(EXETYPE WIN32)
endif()

add_executable(wmiviewer ${EXETYPE}
	classbrowser.hpp
	classbrowser.cpp
	classmodel.hpp
	classmodel.cpp
	querymodel.hpp
	querymodel.cpp
	queryviewer.hpp
	queryviewer.cpp
	window.hpp
	window.cpp
	wmiviewer.cpp
	wqlhighlighter.hpp
	wqlhighlighter.cpp
	${MOC_FILES}
)

target_link_libraries(wmiviewer
	qtwmiwrapper
	${QT_LIBRARIES}
	${WBEMUUID_LIBRARY}
)

# Copy DLL into build dir to allow running from command line without setting PATH
add_custom_command(TARGET wmiviewer POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:qtwmiwrapper>" "${CMAKE_CURRENT_BINARY_DIR}"
)