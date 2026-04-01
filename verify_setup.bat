@echo off
REM ============================================================
REM Script de vérification - Système d'accessibilité WasteGuard
REM ============================================================

echo.
echo ╔═══════════════════════════════════════════════════════════╗
echo ║  VÉRIFICATION SYSTÈME ACCESSIBILITÉ WASTEGUARD            ║
echo ║  Module: Maintenance                                      ║
echo ║  Date: 24 mars 2026                                       ║
echo ╚═══════════════════════════════════════════════════════════╝
echo.

REM Vérifier les fichiers C++
echo [1/5] Vérification des fichiers C++...
if exist "accessibilityhelper.h" (
    echo   ✓ accessibilityhelper.h
) else (
    echo   ✗ accessibilityhelper.h MANQUANT
    exit /b 1
)

if exist "accessibilityhelper.cpp" (
    echo   ✓ accessibilityhelper.cpp
) else (
    echo   ✗ accessibilityhelper.cpp MANQUANT
    exit /b 1
)

if exist "voiceassistant.h" (
    echo   ✓ voiceassistant.h
) else (
    echo   ✗ voiceassistant.h MANQUANT
    exit /b 1
)

if exist "voiceassistant.cpp" (
    echo   ✓ voiceassistant.cpp
) else (
    echo   ✗ voiceassistant.cpp MANQUANT
    exit /b 1
)

REM Vérifier le fichier Python
echo.
echo [2/5] Vérification du moteur Python...
if exist "voice_engine.py" (
    echo   ✓ voice_engine.py
) else (
    echo   ✗ voice_engine.py MANQUANT
    exit /b 1
)

REM Vérifier la documentation
echo.
echo [3/5] Vérification de la documentation...
if exist "ACCESSIBILITY_GUIDE.md" (
    echo   ✓ ACCESSIBILITY_GUIDE.md
) else (
    echo   ✗ ACCESSIBILITY_GUIDE.md MANQUANT (optionnel)
)

if exist "DEVELOPER_GUIDE.md" (
    echo   ✓ DEVELOPER_GUIDE.md
) else (
    echo   ✗ DEVELOPER_GUIDE.md MANQUANT (optionnel)
)

if exist "IMPLEMENTATION_SUMMARY.md" (
    echo   ✓ IMPLEMENTATION_SUMMARY.md
) else (
    echo   ✗ IMPLEMENTATION_SUMMARY.md MANQUANT (optionnel)
)

REM Vérifier les modifications à mainwindow
echo.
echo [4/5] Vérification des modifications Qt...
findstr /c:"#include \"accessibilityhelper.h\"" mainwindow.h >nul
if %errorlevel% equ 0 (
    echo   ✓ mainwindow.h modifié (AccessibilityHelper)
) else (
    echo   ✗ mainwindow.h non modifié correctement
    exit /b 1
)

findstr /c:"#include \"voiceassistant.h\"" mainwindow.h >nul
if %errorlevel% equ 0 (
    echo   ✓ mainwindow.h inclut VoiceAssistant
) else (
    echo   ✗ mainwindow.h manque VoiceAssistant
    exit /b 1
)

findstr /c:"setupAccessibilityModule" mainwindow.cpp >nul
if %errorlevel% equ 0 (
    echo   ✓ mainwindow.cpp a setupAccessibilityModule
) else (
    echo   ✗ mainwindow.cpp manque setupAccessibilityModule
    exit /b 1
)

REM Vérifier le fichier projet
echo.
echo [5/5] Vérification du fichier projet...
findstr /c:"accessibilityhelper.cpp" projettcpp.pro >nul
if %errorlevel% equ 0 (
    echo   ✓ projettcpp.pro inclut accessibilityhelper.cpp
) else (
    echo   ✗ projettcpp.pro manque accessibilityhelper.cpp
    exit /b 1
)

findstr /c:"voiceassistant.cpp" projettcpp.pro >nul
if %errorlevel% equ 0 (
    echo   ✓ projettcpp.pro inclut voiceassistant.cpp
) else (
    echo   ✗ projettcpp.pro manque voiceassistant.cpp
    exit /b 1
)

REM Déterminer les dépendances Python
echo.
echo ═══════════════════════════════════════════════════════════
echo Vérification des dépendances Python...
echo ═══════════════════════════════════════════════════════════

python --version >nul 2>&1
if %errorlevel% equ 0 (
    echo   ✓ Python installé
    python --version
) else (
    echo   ✗ Python non trouvé
    echo     Solution: Installer Python 3.7+ depuis python.org
    exit /b 1
)

python -c "import speech_recognition; print('   ✓ speechrecognition');" 2>nul
if %errorlevel% neq 0 (
    echo   ✗ speechrecognition manquant
    echo     Solution: pip install speechrecognition
)

python -c "import pyttsx3; print('   ✓ pyttsx3');" 2>nul
if %errorlevel% neq 0 (
    echo   ✗ pyttsx3 manquant
    echo     Solution: pip install pyttsx3
)

python -c "import sounddevice; print('   ✓ sounddevice');" 2>nul
if %errorlevel% neq 0 (
    echo   ✗ sounddevice manquant
    echo     Solution: pip install sounddevice
)

python -c "import numpy; print('   ✓ numpy');" 2>nul
if %errorlevel% neq 0 (
    echo   ✗ numpy manquant
    echo     Solution: pip install numpy
)

REM Résultat final
echo.
echo ═══════════════════════════════════════════════════════════
echo ✅ VÉRIFICATION COMPLÈTE - TOUS LES FICHIERS PRÉSENTS
echo ═══════════════════════════════════════════════════════════
echo.
echo Prochaine étape:
echo   1. qmake -r
echo   2. nmake clean && nmake
echo   3. Lancer WasteGuard.exe
echo.
pause
