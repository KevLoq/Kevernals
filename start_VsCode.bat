@echo off

if not defined ADAM_EXTERNALS_DIR (
    set ADAM_EXTERNALS_DIR=C:\dev\ADAM-externals
) 

if not defined EXTERNALS_DIR (
    set EXTERNALS_DIR=C:\Users\loquin.k\Documents\libraries
)

if not defined VTK_DIR (
	set VTK_DIR=%EXTERNALS_DIR%\binaries\VTK-9.0.1
)
	
if not defined GDCM_DIR (
	set GDCM_DIR=%EXTERNALS_DIR%\binaries\gdcm-3.0.8
)
 
if not defined Qt5_DIR (
	set Qt5_DIR=%ADAM_EXTERNALS_DIR%\Qt-5.15.8\5.15.8\msvc2019_64
)

if not defined Torch_DIR (
	set Torch_DIR=%EXTERNALS_DIR%\binaries\torch\libtorch-win-shared-with-deps-1.11.0+cu113\libtorch
)

set PATH=%Qt5_DIR%\bin;%PATH%
set PATH=%VTK_DIR%\bin\Debug;%PATH%
set PATH=%VTK_DIR%\bin\RelWithDebInfo;%PATH%
set PATH=%GDCM_DIR%\bin\Debug;%PATH%
set PATH=%GDCM_DIR%\bin\RelWithDebInfo;%PATH%


rem Launch VS Code
call "%LOCALAPPDATA%\Programs\Microsoft VS Code\Code.exe" .
