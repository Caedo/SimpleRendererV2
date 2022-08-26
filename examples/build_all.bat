@echo off

set projects=01_HelloTriangle
set projects=%projects%;02_HelloQuad
set projects=%projects%;03_szescian_cpp

for %%p in (%projects%) do (
    pushd %%p
    call build.bat dontrun
    popd
)