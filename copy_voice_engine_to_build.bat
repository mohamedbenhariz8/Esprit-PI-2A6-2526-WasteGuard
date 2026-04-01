@echo off
REM ============================================================
REM Script pour copier voice_engine.py au dossier build Qt
REM Necessite: Qt Creator a ete utilise pour compiler
REM ============================================================

setlocal enabledelayedexpansion

echo.
echo ============================================================
echo  COPY voice_engine.py to Qt Build Folder
echo ============================================================
echo.

REM Dossier source
set SOURCE_FILE=%~dp0voice_engine.py
echo Source: %SOURCE_FILE%

REM Verifier que le fichier existe
if not exist "%SOURCE_FILE%" (
    echo ERREUR: voice_engine.py non trouve!
    echo         Verifier que ce script est dans: INTEGRATION_FINALE
    pause
    exit /b 1
)

REM Dossier build - chercher les build folders possibles
set BUILD_FOUND=0
for /d %%D in ("%~dp0build*") do (
    if exist "%%D\WasteGuard.exe" (
        set BUILD_DIR=%%D
        set BUILD_FOUND=1
        echo Destination: %%D
        goto found_build
    )
)

:found_build
if %BUILD_FOUND% EQU 0 (
    echo.
    echo ERREUR: Aucun dossier build trouve!
    echo.
    echo Solution:
    echo 1. Compiler le projet Qt (Ctrl+B dans Qt Creator)
    echo 2. Attendre que la compilation finisse
    echo 3. Relancer ce script
    echo.
    pause
    exit /b 1
)

REM Copier le fichier
echo.
echo Copie en cours...
copy "%SOURCE_FILE%" "%BUILD_DIR%\voice_engine.py" >nul

if !ERRORLEVEL! EQU 0 (
    echo.
    echo ============================================================
    echo SUCCESS! voice_engine.py copie au dossier build
    echo.
    echo Dossier: %BUILD_DIR%
    echo Fichier: voice_engine.py
    echo ============================================================
    echo.
    echo Prochaines etapes:
    echo 1. Installer Python: python --version
    echo 2. Installer dependances: pip install SpeechRecognition pyttsx3
    echo 3. Lancer l'app Qt: Ctrl+R dans Qt Creator
    echo 4. Cliquer sur le bouton 🎤
    echo.
) else (
    echo.
    echo ERREUR: Echec de la copie!
    echo Verifier les permissions ou relancer en Admin
    echo.
)

pause
