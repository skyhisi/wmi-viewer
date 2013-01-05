cmake_minimum_required(VERSION 2.8)

qt4_wrap_cpp(MOC_FILES
	classbrowser.hpp
	classmodel.hpp
	querymodel.hpp
	queryviewer.hpp
	window.hpp
	wqlhighlighter.hpp
)

add_executable(wmiviewer
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

add_custom_command(TARGET wmiviewer POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:qtwmiwrapper>" "${CMAKE_CURRENT_BINARY_DIR}"
)