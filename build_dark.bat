@echo off
set PATH=C:\Qt\Tools\mingw1120_64\bin;C:\Qt\Tools\mingw1120_64\libexec\gcc\x86_64-w64-mingw32\11.2.0;C:\Qt\6.7.3\mingw_64\bin;%PATH%
cd /d "C:\Users\moham\Music\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\build_theme_check"
mingw32-make.exe -j4 2>&1
