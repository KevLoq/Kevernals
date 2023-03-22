rem @echo off

call config.bat

rem Launch VS Code
start "%VS140COMNTOOLS%\..\IDE\devenv.exe" C:\dev\personal-build\Kevernals\Kevernals.sln
