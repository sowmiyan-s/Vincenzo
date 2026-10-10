@echo off
setlocal
set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
call "%VCVARS%" >nul
set "PATH=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;%PATH%"

cd /d "%~dp0sdk\cef"
if not exist build mkdir build
cd build
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ..
if errorlevel 1 exit /b 1
ninja libcef_dll_wrapper
if errorlevel 1 exit /b 1

copy /y libcef_dll_wrapper\libcef_dll_wrapper.lib ..\..\thirdparty\libcef_dll_wrapper.lib
echo.
echo Successfully built and copied libcef_dll_wrapper.lib!
