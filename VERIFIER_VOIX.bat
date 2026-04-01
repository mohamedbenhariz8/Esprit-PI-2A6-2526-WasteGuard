@echo off
REM Vérifier les fichiers essentiels pour voice_engine

setlocal enabledelayedexpansion

echo.
echo ========================================
echo VERIFICATION DES FICHIERS VOICE
echo ========================================
echo.

REM Vérifier voice_engine.py
if exist "voice_engine.py" (
    echo [OK] voice_engine.py trouve dans le dossier source
) else (
    echo [ERREUR] voice_engine.py NOT FOUND!
)

REM Vérifier si Python est disponible
python --version >nul 2>&1
if !errorlevel! equ 0 (
    echo [OK] Python trouve dans PATH
    for /f "tokens=*" %%i in ('python --version') do echo      %%i
) else (
    echo [ERREUR] Python NOT found in PATH
    echo      Installer Python et l'ajouter au PATH
)

REM Vérifier les dépendances Python
echo.
echo Verification des packages Python...
python -m pip show speech_recognition >nul 2>&1
if !errorlevel! equ 0 (
    echo [OK] speech_recognition installed
) else (
    echo [MANQUE] speech_recognition - install with: pip install speech_recognition
)

python -m pip show pyttsx3 >nul 2>&1
if !errorlevel! equ 0 (
    echo [OK] pyttsx3 installed
) else (
    echo [MANQUE] pyttsx3 - install with: pip install pyttsx3
)

echo.
echo ========================================
echo Pour finir la configuration:
echo 1. Ouvrir Qt Creator
echo 2. Faire: Build ^> Clean All
echo 3. Faire: Ctrl+B pour recompiler
echo 4. Relancer l'application
echo ========================================
echo.
pause
