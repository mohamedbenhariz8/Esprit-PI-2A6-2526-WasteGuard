@echo off
echo ========================================================
echo Lancement de l'application WasteGuard avec IA Ollama
echo ========================================================

rem Configuration du fournisseur en local
set WASTEGUARD_AI_PROVIDER=ollama
set OLLAMA_BASE_URL=http://127.0.0.1:11434/v1

rem Modifiez cette ligne si vous utilisez un autre modele (ex: llama3.1, mistral)
set OLLAMA_MODEL=llama3.1

echo.
echo === Verifiez que Ollama est en cours d'execution ===
echo (Ouvrez un terminal et tapez: ollama run %OLLAMA_MODEL%)
echo.
echo Lancement de l'application...
echo.

cd debug 2>nul || cd release 2>nul
start WasteGuard.exe
pause
