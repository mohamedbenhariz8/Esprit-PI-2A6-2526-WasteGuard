@echo off
REM Simple Python Voice Setup for WasteGuard
REM =========================================

echo.
echo =========================================
echo  WasteGuard Voice Engine Setup
echo =========================================
echo.

set PY_CMD=python
if exist "%~dp0.venv\Scripts\python.exe" (
    set "PY_CMD=%~dp0.venv\Scripts\python.exe"
)

REM Copier voice_engine.py dans les dossiers de build
echo [STEP 1] Copying voice_engine.py to build folders...

if exist "debug" (
    copy /Y "voice_engine.py" "debug\voice_engine.py" >nul
    echo [OK] Copied to debug folder
)

if exist "release" (
    copy /Y "voice_engine.py" "release\voice_engine.py" >nul
    echo [OK] Copied to release folder
)

if exist "build-*" (
    for /d %%D in (build-*) do (
        copy /Y "voice_engine.py" "%%D\voice_engine.py" >nul
        echo [OK] Copied to %%D
    )
)

echo.
echo [STEP 2] Installing Python dependencies...
echo Please wait - this may take a minute...
echo.

"%PY_CMD%" -m pip install SpeechRecognition pyttsx3 sounddevice numpy 2>nul
if %errorlevel% equ 0 (
    echo [OK] All packages installed successfully!
) else (
    echo [WARNING] Installation may have partial issues
    echo You may need to retry or install manually
)

echo.
echo =========================================
echo  NEXT STEPS:
echo =========================================
echo.
echo 1. Close Qt Creator completely
echo 2. In the command line, type: cd build-release (or build-debug)
echo 3. Delete all files: del *
echo 4. Go back and rebuild: cd .. ^&^& qmake ^&^& mingw32-make
echo 5. Close and restart WasteGuard
echo 6. Test the microphone button
echo.
echo =========================================
echo.
pause
