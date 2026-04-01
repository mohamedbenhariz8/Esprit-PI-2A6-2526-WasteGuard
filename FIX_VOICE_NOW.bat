@echo off
REM Solution automatique pour fixer le moteur vocal

setlocal enabledelayedexpansion

echo.
echo ========================================
echo  REPAIR VOICE ENGINE - WASTEGUARD
echo ========================================
echo.

REM 1. Copier voice_engine.py au dossier de build
echo [1] Searching for build directory...

REM Chercher le dossier build (debug ou release)
if exist "debug\voice_engine.py" (
    echo [OK] voice_engine.py already in debug folder
) else (
    if exist "debug\" (
        echo [COPY] Copying voice_engine.py to debug folder...
        copy "voice_engine.py" "debug\voice_engine.py" 
        if !errorlevel! equ 0 (
            echo [SUCCESS] Copied to debug folder
        ) else (
            echo [FAILED] Could not copy to debug folder
        )
    )
)

if exist "release\voice_engine.py" (
    echo [OK] voice_engine.py already in release folder
) else (
    if exist "release\" (
        echo [COPY] Copying voice_engine.py to release folder...
        copy "voice_engine.py" "release\voice_engine.py"
        if !errorlevel! equ 0 (
            echo [SUCCESS] Copied to release folder
        ) else (
            echo [FAILED] Could not copy to release folder
        )
    )
)

REM 2. Installer les dépendances Python
echo.
echo [2] Installing Python dependencies...
echo.

REM Essayer avec python normal
python --version >nul 2>&1
if !errorlevel! equ 0 (
    echo [OK] Python found
    echo Installing speech_recognition...
    python -m pip install speech_recognition --break-system-packages --quiet 2>nul
    if !errorlevel! equ 0 (
        echo [OK] speech_recognition installed
    ) else (
        echo [RETRY] Trying without flags...
        python -m pip install speech_recognition --quiet 2>nul
    )
    
    echo Installing pyttsx3...
    python -m pip install pyttsx3 --break-system-packages --quiet 2>nul
    if !errorlevel! equ 0 (
        echo [OK] pyttsx3 installed
    ) else (
        echo [RETRY] Trying without flags...
        python -m pip install pyttsx3 --quiet 2>nul
    )
    
    echo Installing sounddevice...
    python -m pip install sounddevice --break-system-packages --quiet 2>nul
    if !errorlevel! equ 0 (
        echo [OK] sounddevice installed
    )
) else (
    echo [WARNING] Python not found in PATH
    echo Please install Python 3.8+ and add it to PATH
)

echo.
echo ========================================
echo  NEXT STEPS:
echo ========================================
echo 1. In Qt Creator: Build ^> Clean All
echo 2. Rebuild the project (Ctrl+B)
echo 3. Close and restart the application
echo 4. Click the microphone button to test voice
echo.
echo If voice still doesn't work:
echo - Install Python from python.org
echo - Run this script again
echo ========================================
echo.
pause
