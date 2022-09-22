# SimpleRendererV2

Graphics library made with simplicity in mind.

## Library includes:
- simple window and GL context creation using GLFW and GLAD,
- mesh creation,
- assets loading (shaders, textures, fonts...), 
- basic text rendering,
- Utility libraries (Dear ImGui, Raymath, stb),
- memory management utility.

SimpleRenderer is written in C-style C++, without using STL - all needed data structures and functions have basic implementation, so you can see how things works under the hood. It is not optimized for high performance.

## Building

For now Unity builds are only supported. To build sample program, make sure that [vcvarsall](https://learn.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=msvc-170#developer_command_file_locations) is in your PATH, or use Developer Command Prompt x64, and just call build.bat. Other build options (CMake/Make/Visual Studio) will be added in the future.