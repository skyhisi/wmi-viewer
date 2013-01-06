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

set(VC_REDIST_URL "http://download.microsoft.com/download/C/6/D/C6D0FD4E-9E53-4897-9B91-836EBA2AACD3/vcredist_x86.exe")

file(DOWNLOAD "${VC_REDIST_URL}" "${CMAKE_CURRENT_BINARY_DIR}/vcredist_x86.exe"
	SHOW_PROGRESS EXPECTED_MD5 "cede02d7af62449a2c38c49abecc0cd3")

add_custom_command(
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/wmiviewer.wixobj"
	COMMAND "${WIX_CANDLE_EXE}" -nologo
		"-dQTDIR=${WIXQTDIR}"
		"-dBINDIR=${WIXBINDIR}"
		"-dSRCDIR=${WIXSRCDIR}"
		"-dthisVersion=${PROJ_VERSION}"
		-ext WixUIExtension
		-out "${CMAKE_CURRENT_BINARY_DIR}/wmiviewer.wixobj"
		"${CMAKE_CURRENT_SOURCE_DIR}/wmiviewer.wxs"
	MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/wmiviewer.wxs"
	COMMENT "MSI Candle" VERBATIM
)

add_custom_command(
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/wmiviewer.msi"
	COMMAND "${WIX_LIGHT_EXE}" -nologo
		-ext WixUIExtension
		-out "${CMAKE_CURRENT_BINARY_DIR}/wmiviewer.msi"
		"${CMAKE_CURRENT_BINARY_DIR}/wmiviewer.wixobj"
	MAIN_DEPENDENCY "${CMAKE_CURRENT_BINARY_DIR}/wmiviewer.wixobj"
	DEPENDS qtwmiwrapper wmiviewer
	COMMENT "MSI Light" VERBATIM
)

add_custom_command(
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/bootstrapper.wixobj"
	COMMAND "${WIX_CANDLE_EXE}" -nologo
		"-dQTDIR=${WIXQTDIR}"
		"-dBINDIR=${WIXBINDIR}"
		"-dSRCDIR=${WIXSRCDIR}"
		"-dthisVersion=${PROJ_VERSION}"
		"-dREDISTURL=${VC_REDIST_URL}"
		-ext WixBalExtension
		-ext WixUtilExtension
		-out "${CMAKE_CURRENT_BINARY_DIR}/bootstrapper.wixobj"
		"${CMAKE_CURRENT_SOURCE_DIR}/bootstrapper.wxs"
	MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/bootstrapper.wxs"
	COMMENT "Bootstrapper Candle" VERBATIM
)

add_custom_command(
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/wmiviewer-${PROJ_VERSION}.exe"
	COMMAND "${WIX_LIGHT_EXE}" -nologo
		-ext WixBalExtension
		-ext WixUtilExtension
		-out "${CMAKE_CURRENT_BINARY_DIR}/wmiviewer-${PROJ_VERSION}.exe"
		"${CMAKE_CURRENT_BINARY_DIR}/bootstrapper.wixobj"
	MAIN_DEPENDENCY "${CMAKE_CURRENT_BINARY_DIR}/bootstrapper.wixobj"
	DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/wmiviewer.msi"
	COMMENT "Bootstrapper Light" VERBATIM
)

add_custom_target(package ALL
	DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/wmiviewer-${PROJ_VERSION}.exe")

