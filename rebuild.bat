@echo off
cd /d "%~dp0"
qmake WasteGuard.pro
mingw32-make clean
mingw32-make
