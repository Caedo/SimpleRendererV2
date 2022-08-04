@echo off

if NOT "%Platform%" == "X64" IF NOT "%Platform%" == "x64" (call vcvarsall x64)

set exe_name=SimpleRenderer
set compile_flags= -nologo /Zi /FC /I ../include/ /W3
set linker_flags= glfw3dll.lib gdi32.lib user32.lib kernel32.lib opengl32.lib /INCREMENTAL:NO
set linker_path="../lib/"

if not exist build mkdir build
pushd build

del %exe_name%.exe

start /b /wait "" "cl.exe" %compile_flags% ../src/unity.cpp /link %linker_flags% /libpath:%linker_path% /out:%exe_name%.exe
copy ..\bin\* . >NUL

if not exist shaders mkdir shaders
copy ..\src\shaders\* .\shaders >NUL
copy ..\lib\glfw3.dll . >NUL

%exe_name%.exe

popd
