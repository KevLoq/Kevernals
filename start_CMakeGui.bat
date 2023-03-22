rem @echo off

call config.bat

rem Launch VS Code
call "%CMAKE_DIR%\cmake-gui.exe" .
