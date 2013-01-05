cmake_minimum_required(VERSION 2.8)

find_program(WIX_CANDLE_EXE candle.exe
	HINTS "C:/Program Files (x86)/WiX Toolset v3.7/bin"
	DOC "WIX Candle (Compiler)")
find_program(WIX_LIGHT_EXE light.exe
	HINTS "C:/Program Files (x86)/WiX Toolset v3.7/bin"
	DOC "WIX Light (Linker)")
	
# execute_process(COMMAND "${QT_QMAKE_EXECUTABLE}" -query QT_INSTALL_BINS
	# OUTPUT_VARIABLE QT_BIN_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}" WIXBINDIR)
file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}" WIXSRCDIR)
file(TO_NATIVE_PATH "${QT_BINARY_DIR}" WIXQTDIR)


add_custom_command(
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/wmiviewer.wixobj"
	COMMAND "${WIX_CANDLE_EXE}" -nologo
		"-dQTDIR=${WIXQTDIR}"
		"-dBINDIR=${WIXBINDIR}"
		"-dSRCDIR=${WIXSRCDIR}"
		-ext WixUIExtension
		-out "${CMAKE_CURRENT_BINARY_DIR}/wmiviewer.wixobj"
		"${CMAKE_CURRENT_SOURCE_DIR}/wmiviewer.wxs"
	MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/wmiviewer.wxs"
	COMMENT "Wix Candle" VERBATIM
)

add_custom_command(
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/wmiviewer.msi"
	COMMAND "${WIX_LIGHT_EXE}" -nologo
		-ext WixUIExtension
		-out "${CMAKE_CURRENT_BINARY_DIR}/wmiviewer.msi"
		"${CMAKE_CURRENT_BINARY_DIR}/wmiviewer.wixobj"
	MAIN_DEPENDENCY "${CMAKE_CURRENT_BINARY_DIR}/wmiviewer.wixobj"
	DEPENDS qtwmiwrapper wmiviewer
	COMMENT "Wix Light" VERBATIM
)

add_custom_target(package ALL
	DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/wmiviewer.msi")

