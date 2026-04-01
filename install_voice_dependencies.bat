@echo off
REM ============================================================
REM WasteGuard Voice Engine - Installation des dependances Python
REM ============================================================

echo.
echo Verification de Python...
python --version
if %ERRORLEVEL% NEQ 0 (
    echo ERREUR: Python n'est pas installe ou pas dans PATH
    echo Telecharger Python 3.8+ depuis: https://www.python.org
    pause
    exit /b 1
)

echo.
echo Installation des dependances Python...
echo.

pip install --upgrade pip

echo.
echo [1/3] Installation: SpeechRecognition...
pip install SpeechRecognition

echo.
echo [2/3] Installation: pyttsx3...
pip install pyttsx3

echo.
echo [3/3] Installation: pyaudio (optionnel pour le microphone)...
pip install pyaudio

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo WARNING: pyaudio a echoue (c'est normal sous Windows)
    echo Essayer: pip install pipwin
    echo Puis: pipwin install pyaudio
    pause
)

echo.
echo Verification...
python -c "import speech_recognition; import pyttsx3; print('SUCCESS: Toutes les dependances sont installes!')"

if %ERRORLEVEL% EQ 0 (
    echo.
    echo ============================================================
    echo SUCCES! Les dependances sont installes.
    echo Tu peux maintenant compiler le projet Qt.
    echo ============================================================
    echo.
    echo Test rapide:
    echo   python voice_engine.py listen
    echo   python voice_engine.py speak "Bonjour"
    echo.
) else (
    echo.
    echo ERREUR: Verifier l'installation des modules Python
    echo.
)

pause
