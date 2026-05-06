@echo off
echo ========================================================
echo Lancement de l'application avec IA Ollama locale
echo ========================================================

rem Configuration du fournisseur en local
set WASTEGUARD_AI_PROVIDER=ollama
set OLLAMA_MODEL=qwen2.5:0.5b
rem (changez le modele selon ce que vous avez telecharge : llama3, mistral, etc)
set OLLAMA_BASE_URL=http://127.0.0.1:11434/v1

echo.
echo 1. Assurez-vous qu'Ollama est bien demarre ("ollama run %OLLAMA_MODEL%" au moins une fois)
echo 2. Lancement...
echo.

debug\INTEGRATION_FINALE1111.exe
pause
