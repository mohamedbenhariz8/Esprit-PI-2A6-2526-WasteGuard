# 🎤 Installation Python pour WasteGuard Voice Engine

## ⚡ Quick Fix (5 minutes)

### 1. Installer Python

**Windows:**
1. Aller sur: https://www.python.org/downloads/
2. Cliquer "Download Python 3.11" (ou plus récent)
3. **IMPORTANT** ✓ Cocher "Add Python to PATH"
4. Cliquer "Install Now"

**Vérifier l'installation:**
```
Ouvrir PowerShell ou CMD et taper:
python --version
```

### 2. Installer les dépendances

Ouvrir PowerShell ou CMD dans le dossier `INTEGRATION_FINALE` et exécuter:

```
python -m pip install speech_recognition pyttsx3 sounddevice
```

Attendre que l'installation se termine.

### 3. Redémarrer l'application

1. Fermer complètement WasteGuard
2. Relancer l'application
3. Cliquer sur le bouton 🎤 (microphone)

---

## ✅ Vérifier que tout fonctionne

### Tester Python:
```powershell
python -c "import speech_recognition; print('OK - speech_recognition works')"
python -c "import pyttsx3; print('OK - pyttsx3 works')"
python -c "import sounddevice; print('OK - sounddevice works')"
```

### Tester la voix dans WasteGuard:
- Cliquer sur le bouton 🎤
- Parler clairement: "Bonjour"
- Le texte devrait apparaître

---

## ❌ Si cela ne fonctionne toujours pas

### Option 1: Réinstaller Python
```
1. Désinstaller Python (Paramètres > Applications)
2. Redémarrer l'ordinateur
3. Télécharger Python 3.11 depuis python.org
4. Installer avec "Add Python to PATH" cochée
5. Redémarrer l'ordinateur
```

### Option 2: Utiliser le script automatisé
Exécuter le fichier batch dans INTEGRATION_FINALE:
```
FIX_VOICE_ADVANCED.bat
```

### Option 3: Voice Engine optionnel
Si Python ne fonctionne pas:
- L'application continue à fonctionner normalement
- Utiliser les autres fonctionnalités (gestion stock, produits, etc)
- La reconnaissance vocale reste disponible quand Python est installé

---

## 🛠️ Configuration alternative (développeurs)

Si vous utilisez un environnement virtuel Python:

```bash
# Créer un venv
python -m venv venv

# Activer le venv
# Windows:
venv\Scripts\activate

# Installer les packages
pip install speech_recognition pyttsx3 sounddevice
```

Puis modifier le chemin dans `mainwindow.cpp` ligne ~18420:
```cpp
QString pythonScriptPath = QCoreApplication::applicationDirPath() + "/voice_engine.py";
```

Pour pointer vers votre Python venv.

---

## 📞 Support

Si le problème persiste:
1. Vérifier que `voice_engine.py` existe dans le dossier de build
2. Vérifier que Python est dans le PATH: `python --version`
3. Vérifier les logs: Ouvrir la Console Qt Creator (Alt+3)

