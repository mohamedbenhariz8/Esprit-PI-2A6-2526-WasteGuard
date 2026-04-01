@echo off
REM Solution avancée pour fixer le moteur vocal - Utilise le chemin complet de Python

setlocal enabledelayedexpansion

echo.
echo ========================================
echo  VOICE ENGINE REPAIR - WASTEGUARD
echo ========================================
echo.

REM 1. Chercher les dossiers de build
echo [1] Searching for build directories...

set "FOUND_BUILD=0"
for /d %%D in (debug release build *build* *Debug* *Release*) do (
    if exist "%%D\" (
        echo [FOUND BUILD] %%D
        set "FOUND_BUILD=1"
        if not exist "%%D\voice_engine.py" (
            echo [COPY] Copying voice_engine.py to %%D...
            copy "voice_engine.py" "%%D\voice_engine.py" >nul 2>&1
            if !errorlevel! equ 0 (
                echo [SUCCESS] ✓ Copied to %%D
            )
        ) else (
            echo [OK] voice_engine.py already in %%D
        )
    )
)

if !FOUND_BUILD! equ 0 (
    echo [WARNING] No build directory found yet - it will be created during compilation
)

REM 2. Installer Python dependencies avec le chemin complet
echo.
echo [2] Checking Python installation...

set "PYTHON_PATH=C:\Users\%USERNAME%\.local\bin\python3.14.exe"
set "PYTHON_ALT=C:\Users\%USERNAME%\AppData\Local\Programs\Python*\python.exe"

if exist "!PYTHON_PATH!" (
    echo [OK] Found Python at: %PYTHON_PATH%
    
    echo Installing voice dependencies...
    "!PYTHON_PATH!" -m pip install speech_recognition pyttsx3 sounddevice --break-system-packages --quiet
    if !errorlevel! equ 0 (
        echo [SUCCESS] ✓ All packages installed
    ) else (
        echo [WARNING] Package installation may need retry
    )
) else (
    echo [INFO] Python 3.14 not found in standard location
    echo [INFO] Checking for other Python installations...
    
    setlocal enabledelayedexpansion
    for /f "delims=" %%A in ('dir /s /b "C:\Users\%USERNAME%\AppData\Local\Programs\Python*\python.exe" 2^>nul ^| findstr /r "python.exe" ^| head -1') do (
        set "PYTHON_PATH=%%A"
    )
    
    if defined PYTHON_PATH (
        echo [OK] Found Python at: !PYTHON_PATH!
        "!PYTHON_PATH!" -m pip install speech_recognition pyttsx3 sounddevice --quiet
    ) else (
        echo [ERROR] Python not found!
        echo Please download Python from: https://www.python.org/downloads/
        echo Then run this script again.
        echo.
        pause
        exit /b 1
    )
)

REM 3. Vérifier les packages
echo.
echo [3] Verifying packages...
"!PYTHON_PATH!" -c "import speech_recognition; print('[OK] speech_recognition imported')" >nul 2>&1
if !errorlevel! equ 0 echo [OK] speech_recognition ✓
"!PYTHON_PATH!" -c "import pyttsx3; print('[OK] pyttsx3 imported')" >nul 2>&1
if !errorlevel! equ 0 echo [OK] pyttsx3 ✓
"!PYTHON_PATH!" -c "import sounddevice; print('[OK] sounddevice imported')" >nul 2>&1
if !errorlevel! equ 0 echo [OK] sounddevice ✓

echo.
echo ========================================
echo  ✓ VOICE ENGINE REPAIRED!
echo ========================================
echo.
echo NEXT STEPS:
echo 1. Open Qt Creator
echo 2. Build ^> Clean All
echo 3. Press Ctrl+B to rebuild
echo 4. Close and restart WasteGuard
echo 5. Test the microphone button 🎤
echo.
echo ========================================
echo.
pause
