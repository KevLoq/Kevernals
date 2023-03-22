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

if not defined GTEST_ROOT (
	set GTEST_ROOT=%ADAM_EXTERNALS_DIR%\GoogleTest-1.13.0
)
 
set PATH=%VTK_DIR%\bin\Debug;%PATH%
set PATH=%VTK_DIR%\bin\RelWithDebInfo;%PATH%
set PATH=%GDCM_DIR%\bin\Debug;%PATH%
set PATH=%GDCM_DIR%\bin\RelWithDebInfo;%PATH%
set PATH=%GTEST_ROOT%\lib;%PATH%


if not defined CMAKE_DIR (
	set CMAKE_DIR=C:\dev\ADAM-externals\cmake-3.22.2\bin
)