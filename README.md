# NGMC Toolbox

NGMC Toolbox is a toolset with the aim to aid the modding of as many file formats as possible from the Ninja Gaiden: Master Collection. Currently supported file formats include the archive `databin` and the texture containers `GT1G` and `DDS`. Work specifically on Ninja Gaiden Sigma (one) is a priority, however there is limited support for Ninja Gaiden Sigma 2 and Razor's Edge which will also continue to be improved.

## Disclaimer

The author of this repository has no affiliation with Team Ninja or Koei Tecmo and this software is intended purely for educational or entertainment purposes. The author does not claim ownership of the file formats that may be modifiable through use of the software, and no copyright infringement is intended. No game files will be distributed. All rights go to their respective owners. If you are the rights holder and would like us to remove this content, please contact us and we will comply with your request immediately.

## Usage

### NGMCToolGUI (GUI application)

NGMCToolGUI is an executable with a graphical interface powered by [Dear ImGui](https://github.com/ocornut/imgui/tree/docking) (docking-branch), running in a [GLFW](https://github.com/glfw/glfw) window on the OpenGL graphics API as exposed through [GLEW](https://github.com/nigels-com/glew), and should therefore be available on most x86/x64 Windows machines. It is the major part of the NGMC Toolbox and the recommended way of using it. All its capabilities are exposed through the UI, however it is possible to directly open files inside of it via drag-and-drop on the executable file or running `NGMCToolGUI.exe [path/to/file1] [path/to/file2] [...]` from the command line.

### NGMCTool (console application)

NGMCTool is a small command-line executable without a graphical interface, which can be used to extract the contents of a `databin` archive via the command `NGSTool.exe unpack [path/to/databin/databin] [path/to/output/directory]`. In the future, this tool may receive additional functionality.

## Building the toolset

All the latest releases of the NGMC Toolbox will be available from the Releases section here on GitHub, however if you would like to build the toolset yourself you first need to go through a few steps:

0. Make sure Visual Studio is installed on your system with the required packages (e.g. Desktop Development in C++). Visual Studio Community 2022, the latest Windows SDK, platform toolset v143 and the C++20 language standard are recommended.
1. Clone this repository recursively using `git clone -b [branch] https://github.com/Mystixor/NGMCToolbox.git --recursive`, where `[branch]` can be either `master` for the latest releases or `dev` for the latest (potentially unstable) updates of the toolset.
2. Depending on whether you would like to build NGMCTool or NGMCToolGUI, see the corresponding steps below.

### Steps for both NGMCTool and NGMCToolGUI

1. Navigate to `vendor/zlib/contrib/vstudio/vc17` and open `zlibvc.sln` in Visual Studio. Build the project `zlibstat` for the respective architecture, the binaries should end up in `vendor/zlib/contrib/vstudio/vc17` automatically (followed by either `x86` or `x64` and then `ZlibStatDebug` or `ZlibStatRelease`).

### Steps for only NGMCToolGUI

1. Go to the [GLFW repository](https://github.com/glfw/glfw) and download the `WIN32` or `WIN64` binaries (depending on the architecture you are building for) from the release which NGMCToolbox is using. At the time of writing this, that is [GLFW 3.4](https://github.com/glfw/glfw/releases/tag/3.4). NGMCToolGUI expects the library in the directories `NGMCToolGUI/vendor/GLFW/lib/x64` and `NGMCToolGUI/vendor/GLFW/lib/x86` respectively.
2. Go to the [GLEW repository](https://github.com/nigels-com/glew) and extract the contents of the latest zip or tgz release snapshot available (tested on [GLEW 2.2.0](https://github.com/nigels-com/glew/releases/tag/glew-2.2.0)) to the directory `NGMCToolGUI/vendor/GLEW`. These are the archives that come with the `include` folder necessary for building this dependency.
3. Navigate to `NGMCToolGUI/vendor/GLEW/build/vc15` and open `glew.sln` in Visual Studio. Build the project `glew_static` for the respective architecture, the binaries should end up in `NGMCToolGUI/vendor/GLEW/lib` automatically (followed by either `Debug` or `Release` and then `Win32` or `x64`).
