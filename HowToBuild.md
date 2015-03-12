# Development Environment #
To build you will need the following installed:
  * Microsoft Visual Studio 2010 Express (or other edition) ([Download](http://go.microsoft.com/?linkid=9709949))
  * Qt 4.8 for MS VS 2010 ([Download](http://releases.qt-project.org/qt4/source/qt-win-opensource-4.8.4-vs2010.exe))
  * CMake 2.8 for Windows ([Download](http://www.cmake.org/files/v2.8/cmake-2.8.10.2-win32-x86.exe))
  * Microsoft Windows SDK 7.1 ([Download](http://www.microsoft.com/en-gb/download/details.aspx?id=8279)) - Don't install the compilers, only the development libraries and tools

Other versions of the above may work, but I haven't tested them.

This should also be able to be built using Mingw, but I haven't tried that yet.

## Configuration ##
You may want to add your CMake `bin` directory (`C:\Program Files (x86)\CMake 2.8\bin`) to your system `Path` environment variable (System Properties > Advanced > Environment Variables)

# Building #

  1. Checkout the source code
  1. Start the _Visual Studio Command Prompt (2010)_
  1. Add the Qt `bin` directory to your path
    * `set PATH=C:\Qt\4.8.4\bin;%PATH%`
  1. `cd` into the checkout `build` directory
  1. Run `cmake-gui ..\source`
  1. Click _Configure_ and select _NMake Makefiles_ and _Use default native compiler_, click _Finish_
  1. If any settings appear in red, click _Configure_ again, if still red, then check the settings and click _Configure_ until all the settings are correct
  1. Click _Generate_, close the CMake GUI, and go back to the command prompt
  1. Run `nmake`, this should build the project
  1. Run `gui\wmiviewer.exe`, the application should launch !
