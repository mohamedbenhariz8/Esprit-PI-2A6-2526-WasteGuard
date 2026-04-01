@echo off
REM ============================================================
REM SETUP VOIX AUTOMATIQUE - Tout en un!
REM ============================================================
REM Ce script:
REM 1. Verifier Python
REM 2. Installer les dependances
REM 3. Copier voice_engine.py au bon dossier
REM ============================================================

cls
echo.
echo ============================================================
echo  SETUP AUTOMATIQUE - RECONNAISSANCE VOCALE
echo ============================================================
echo.

setlocal enabledelayedexpansion

set PY_CMD=python
if exist "%~dp0.venv\Scripts\python.exe" (
    set "PY_CMD=%~dp0.venv\Scripts\python.exe"
)

REM ===== ETAPE 1: VERIFIER PYTHON =====
echo [1/3] Verification de Python...
"%PY_CMD%" --version >nul 2>&1

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERREUR: Python n'est pas installe ou pas dans PATH
    echo.
    echo SOLUTION: Telecharger Python 3.8+ (64-bit):
    echo   https://www.python.org/downloads/
    echo.
    echo IMPORTANT: Cocher "Add Python to PATH" pendant l'installation!
    echo.
    echo Apres installation de Python, relancer ce script.
    echo.
    pause
    exit /b 1
)

"%PY_CMD%" --version
echo OK - Python trouve!
echo.

REM ===== ETAPE 2: INSTALLER DEPENDANCES =====
echo [2/3] Installation des dependances Python...
echo.

echo Mise a jour de pip...
"%PY_CMD%" -m pip install --upgrade pip >nul 2>&1

echo Installation de SpeechRecognition, pyttsx3, sounddevice, numpy...
"%PY_CMD%" -m pip install SpeechRecognition pyttsx3 sounddevice numpy >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERREUR: Impossible d'installer les dependances voix
    pause
    exit /b 1
)

echo OK - Toutes les dependances installes!
echo.

REM ===== ETAPE 3: COPIER voice_engine.py =====
echo [3/3] Copie de voice_engine.py...
echo.

set SOURCE_FILE=%~dp0voice_engine.py
set BUILD_FOUND=0

for /d %%D in ("%~dp0build*") do (
    if exist "%%D\WasteGuard.exe" (
        set BUILD_DIR=%%D
        set BUILD_FOUND=1
        goto copy_file
    )
)

:copy_file
if %BUILD_FOUND% EQU 0 (
    echo ERREUR: Aucun dossier build trouve!
    echo.
    echo SOLUTION:
    echo 1. Compiler le projet Qt (Ctrl+B dans Qt Creator)
    echo 2. Relancer ce script
    echo.
    pause
    exit /b 1
)

if not exist "%SOURCE_FILE%" (
    echo ERREUR: voice_engine.py non trouve dans INTEGRATION_FINALE!
    pause
    exit /b 1
)

copy "%SOURCE_FILE%" "%BUILD_DIR%\voice_engine.py" >nul

if %ERRORLEVEL% NEQ 0 (
    echo ERREUR: Impossible de copier voice_engine.py
    pause
    exit /b 1
)

echo OK - voice_engine.py copie!
echo.

REM ===== SUCCES =====
echo ============================================================
echo SUCCESS!
echo ============================================================
echo.
echo Toutes les etapes sont terminees:
echo   ✓ Python installe
echo   ✓ Dependances installes
echo   ✓ voice_engine.py copie
echo.
echo PROCHAINES ETAPES:
echo 1. Fermer Qt Creator completement
echo 2. Relancer Qt Creator
echo 3. Compiler (Ctrl+B)
echo 4. Lancer (Ctrl+R)
echo 5. Cliquer sur le bouton VOIX (🎤)
echo.
echo La reconnaissance vocale devrait fonctionner maintenant!
echo.
pause
