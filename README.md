


# ImStudio

Real-time GUI layout creator/editor for [Dear ImGui](https://github.com/ocornut/imgui)

### [Try Online](https://raais.github.io/ImStudio) (only the official version for now)

![Screenshot](https://user-images.githubusercontent.com/64605172/166310444-d7520e69-0d74-4dd8-a84e-2144504ab466.png)

Inspired by [Code-Building/ImGuiBuilder](https://github.com/Code-Building/ImGuiBuilder)

## Features

 - Drag edit
 - Property edit
 - Covers most of the commonly used default widgets (primitives, data inputs, and other miscellaneous)
 - Child windows
 - Real-time generation
 - Export to clipboard
 - Useful tools (Style & Color export, Demo Window, etc.)
 - Helpful resources (external)
 - Project import
 - Project management

## ToDo
 - ~~import feature~~ **Implemented!**
 - Adding objects (lines, circles, etc.)
 - ~~Saving/Loading as a Project~~ **Implemented!**
 - Multilingual support

 # Notes
 - On Windows, projects are located in "C:/Users/*Current user name*/ImStudio-Reworked"
 - On Linux, projects are located in "/home/*Current user name*/.ImStudio-Reworked" (or the shortened path "~/.ImStudio-Reworked")
 
## Installation

### Build Dependencies

 - [CMake](https://cmake.org/download)
 - [GLFW](https://www.glfw.org/download) \
 <sup><sub> \*Downloaded during build on Windows and MacOS </sub></sup>\
 <sub> **Debian:** sudo apt-get install libglfw3 libglfw3-dev </sub>\
 <sub> **Arch:** sudo pacman -S glfw </sub>\
 <sub> **Fedora:** sudo dnf install glfw glfw-devel </sub>

### Instructions

### Linux

#### Build
```bash
git clone --depth 1 https://github.com/ZailoxTT/ImStudio-Reworked.git
cd ImStudio-Reworked
./build.sh
```

### Windows

#### Pre-Built

[Installer x64](https://github.com/ZailoxTT/ImStudio-Reworked/releases)

#### Build
Make sure MSVC is in your environment (eg. x64 Native Tools Command Prompt)
```cmd
"C:\Program Files\Git\bin\git.exe" clone --depth 1 https://github.com/ZailoxTT/ImStudio-Reworked.git
cd ImStudio-Reworked
md build
cd build
"C:\Program Files\CMake\bin\cmake.exe" .. -DCMAKE_BUILD_TYPE=Release
"C:\Program Files\CMake\bin\cmake.exe" --build . --config Release
```

## Credits
Thanks to [Omar](https://github.com/ocornut) for [Dear ImGui](https://github.com/ocornut/imgui).\
Thanks to [Code-Building](https://github.com/Code-Building) for the inspiration.\
Thanks to [Raais](https://github.com/Raais/ImStudio) for the original project.
