@echo off

if NOT "%Platform%" == "X64" IF NOT "%Platform%" == "x64" (call vcvarsall x64)

set exe_name=HelloQuad
set compile_flags= -nologo /Zi /FC /I /W3 /D UNITY_BUILD
set linker_flags= glfw3dll.lib gdi32.lib user32.lib kernel32.lib opengl32.lib /INCREMENTAL:NO
set linker_path="../../lib/"

del %exe_name%.exe

start /b /wait "" "cl.exe" %compile_flags% ./%exe_name%.cpp /link %linker_flags% /libpath:%linker_path% /out:%exe_name%.exe
copy ..\..\lib\* . >NUL

if NOT "%1" == "dontrun" ( %exe_name%.exe )