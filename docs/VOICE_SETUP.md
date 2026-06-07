# WasteGuard Voice Engine Setup Guide

## 🎤 Installation des Dépendances Python

### Windows - Étapes rapides

1. **Ouvrir PowerShell en tant qu'administrateur**

2. **Installer les dépendances requises:**
```powershell
pip install SpeechRecognition pyttsx3 pyaudio
```

Si `pyaudio` pose problème, essayer:
```powershell
pip install pipwin
pipwin install pyaudio
```

### Liste complète des dépendances

| Paquet | Version | Utilité |
|--------|---------|---------|
| `SpeechRecognition` | 3.10+ | Reconnaissance vocale via Google |
| `pyttsx3` | 2.90+ | Synthèse vocale (texte → parole) |
| `pyaudio` | 0.2.13+ | Interface audio (optionnel si pas de micro) |

### Vérifier l'installation

```powershell
python -c "import speech_recognition as sr; import pyttsx3; print('OK')"
```

Doit afficher: `OK`

---

## 🔧 Test du Moteur Vocal

### 1. Test basique (sans Qt)

```powershell
cd C:\Users\Lightning Bacqueenie\Desktop\final\INTEGRATION_FINALE
python voice_engine.py
```

Doit afficher:
```
[HH:MM:SS] SUCCESS: Voice engine ready!
```

### 2. Test de synthèse vocale

```powershell
python voice_engine.py speak "Bonjour"
```

→ L'ordinat eur parlera "Bonjour"

### 3. Test de reconnaissance vocale

```powershell
python voice_engine.py listen
```

→ Le script écoute le microphone pendant 10 secondes
→ Parlez, le texte s'affichera en JSON

---

## 🚀 Intégration Qt C++

Le code Qt utilise `QProcess` pour lancer `voice_engine.py`:

### Comment ça fonctionne:

1. **Qt lance le script Python** avec `QProcess`
2. **Python écoute et parle** via `speech_recognition` + `pyttsx3`
3. **Qt lit la sortie JSON** de Python
4. **Qt affiche les résultats** dans l'interface

### Code C++ dans `voiceassistant.cpp`:

```cpp
// Lancer le moteur vocal
m_process = new QProcess(this);
m_process->start("python", QStringList() << "voice_engine.py");

// Écouter
m_process->write("listen\n");

// Parler
m_process->write("speak\nBonjour le monde\n");
```

---

## ❌ Dépannage

### Erreur: "No microphone found"

**Solution:**
- Vérifier que le microphone est connecté et fonctionnel
- Aller dans Paramètres Windows → Son → Entrée
- S'assurer que le microphone par défaut est sélectionné

### Erreur: "Missing module speech_recognition"

**Solution:**
```powershell
pip install --upgrade SpeechRecognition
```

### Erreur: "API error"

**Causes possibles:**
- Pas de connexion Internet (Google Speech Recognition requis)
- Trop de requêtes (limites Google)

**Solution:**
- Vérifier la connexion Internet
- Attendre quelques minutes avant d'essayer à nouveau

### Le microphone ne capte rien

**Solution:**
- Augmenter le gain du microphone dans Windows
- Réduire le bruit de fond
- Parler plus fort et plus lentement

### pyttsx3 ne parle pas

**Windows uniquement:**
```powershell
pip install pypiwin32
pypiwin32_postinstall -install
```

---

## 📋 Vérification Finale

Avant de compiler Qt, vérifier:

✅ Python installé version 3.8+
```powershell
python --version
```

✅ SpeechRecognition installé
```powershell
pip list | findstr "SpeechRecognition"
```

✅ pyttsx3 installé
```powershell
pip list | findstr "pyttsx3"
```

✅ Microphone fonctionne
```powershell
python voice_engine.py listen
```

✅ Haut-parleurs fonctionnent
```powershell
python voice_engine.py speak "Test audio"
```

Si tout ✅, tu es prêt à compiler Qt!

---

## 🎯 Commandes Principales

| Commande | Effet |
|----------|--------|
| `python voice_engine.py` | Test complet (init engine + recognizer) |
| `python voice_engine.py init` | Vérifier l'initialisation JSON |
| `python voice_engine.py listen` | Écouter 10 secondes et retourner le texte JSON |
| `python voice_engine.py speak "texte"` | Prononcer le texte |

---

## Notes Importantes

- ⚠️ **La reconnaissance vocale nécessite Internet** - elle utilise Google Speech Recognition
- 🔒 **Les données audio ne sont pas enregistrées** - Google API utile seulement pour la reconnaissance
- 🎙️ **Le microphone doit être de bonne qualité** - sinon la reconnaissance sera mauvaise
- 🌍 **Supporte le français, anglais, espagnol** avec `LANGUAGE = 'fr-FR'` dans voice_engine.py

