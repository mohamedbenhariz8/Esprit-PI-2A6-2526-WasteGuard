# 🎤 ACTIVATION SIMPLE DE LA VOIX

## ⚠️ Le Problème

Qt compilé cherche `voice_engine.py` **dans le dossier buildé**, pas dans `INTEGRATION_FINALE`.

Exemple:
```
C:\Users\Lightning Bacqueenie\Desktop\final\INTEGRATION_FINALE\build-WasteGuard-Qt_6_7_3_MinGW_64bit-Debug\
  → WasteGuard.exe  (L'exécutable)
  → voice_engine.py  (DOIT ÊTRE ICI!)
```

---

## ✅ Solution 3 Étapes (Simple et Facile)

### ÉTAPE 1: Installer Python

**Vérifier:**
```bash
python --version
```

Doit afficher `Python 3.8` ou plus récent.

**Si pas installé:** Télécharger de https://www.python.org

---

### ÉTAPE 2: Installer dépendances Python

**PowerShell Admin:**
```powershell
pip install SpeechRecognition pyttsx3
```

**Vérifier:**
```powershell
python -c "import speech_recognition; import pyttsx3; print('OK')"
```

Doit afficher `OK`.

---

### ÉTAPE 3: Copier voice_engine.py au bon endroit

**PLAN A (Recommandé):**
1. Ouvrir `INTEGRATION_FINALE` dans l'explorateur Windows
2. Copier le fichier `voice_engine.py` (Ctrl+C)
3. Naviguer au dossier build Qt (exemple):
   - `build-WasteGuard-Qt_6_7_3_MinGW_64bit-Debug\`
4. **Coller** voice_engine.py dans ce dossier (Ctrl+V)

**PLAN B (Alternative):**
```powershell
# PowerShell (Execute en Admin)
$buildDir = "C:\path\to\build-WasteGuard-Qt_6_7_3_MinGW_64bit-Debug"
$srcFile = "C:\Users\Lightning Bacqueenie\Desktop\final\INTEGRATION_FINALE\voice_engine.py"
Copy-Item $srcFile $buildDir
```

---

## 🧪 Test Rapide

### Test 1: Voice Engine Seul
```bash
cd C:\Users\Lightning Bacqueenie\Desktop\final\INTEGRATION_FINALE
python voice_engine.py listen       # Doit écouter 10 sec
```

### Test 2: Application Qt
1. Compiler Qt: `Ctrl+B`
2. Copier voice_engine.py au BUILD FOLDER
3. Lancer Qt: `Ctrl+R`
4. Cliquer 🎤 button
5. Si voir "Moteur vocal disponible" → ✅ OK!
6. Si voir message erreur → Relire Solution 3 Étapes

---

## 💡 Troubleshooting Rapide

| Problème | Solution |
|----------|----------|
| "Python not found" | Installer Python et ajouter à PATH |
| "ImportError speech_recognition" | `pip install SpeechRecognition` |
| "ImportError pyttsx3" | `pip install pyttsx3` |
| "voice_engine.py not found" | Copier à bon dossier (build folder) |
| "Microphone not available" | Vérifier microphone Windows (Settings → Sound) |

---

## 📁 Arborescence Finale

```
INTEGRATION_FINALE/
  ├─ voice_engine.py         ← Source Python
  ├─ WasteGuard.pro
  └─ mainwindow.cpp
  
build-WasteGuard-Qt_6_7_3_MinGW_64bit-Debug/
  ├─ WasteGuard.exe          ← L'app compilée
  ├─ voice_engine.py         ← ✅ COPIER ICI!
  └─ ...
```

---

## ✨ C'est Tout!

Une fois voice_engine.py dans le bon dossier + dépendances installées = Voix fonctionnelle ✅

