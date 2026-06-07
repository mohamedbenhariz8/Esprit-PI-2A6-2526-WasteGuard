# 🚀 DÉMARRAGE RAPIDE

## Pour les utilisateurs finals

### Installation et utilisation

1. **Téléchargez le dernier WasteGuard**

2. **Lancez l'application**
   - Les 3 boutons d'accessibilité apparaissent automatiquement

3. **Utilisez les fonctionnalités**
   - 🎤 Cliquez pour la reconnaissance vocale
   - ⚫ Cliquez pour activer le contraste élevé
   - 🔍 Cliquez pour augmenter le zoom texte

### Premier démarrage

La première fois, Python doit être installé :

```bash
# 1. Télécharger Python depuis python.org
# 2. Installer: python-3.x.x-amd64.exe

# 3. Ouvrir PowerShell/CMD et exécuter:
pip install speechrecognition pyttsx3 sounddevice numpy pyaudio
```

### Tester rapidement

```bash
# Vérifier que tout est OK
.\verify_setup.bat
```

---

## Pour les développeurs

### Compilation rapide

```bash
# 1. Ouvrir le projet dans Qt Creator
# Fichier -> Ouvrir -> projettcpp.pro

# 2. Compiler
# Build -> Rebuild All

# 3. Lancer
# Build -> Run
```

### Structure du projet

```
├── accessibilityhelper.h/cpp      ← Accessibilité UI
├── voiceassistant.h/cpp           ← Gestion vocal
├── voice_engine.py                ← Moteur Python
├── mainwindow.h/cpp               ← Interface principale
├── projettcpp.pro                 ← Configuration projet
└── Documentation/
    ├── ACCESSIBILITY_GUIDE.md     ← Guide complet
    ├── DEVELOPER_GUIDE.md         ← Pour contributeurs
    └── IMPLEMENTATION_SUMMARY.md  ← Résumé technique
```

### Commandes utiles

```bash
# Nettoyer et reconstruire
qmake -r
nmake clean
nmake

# Basique Python
python voice_engine.py           # Test du moteur
python -m pip install --upgrade  # Mettre à jour pip

# Déboguer
# Qt Creator -> Debug (F5)
# Fenêtre: Application Output pour les logs
```

### Ajouter une nouvelle commande vocale

1. Éditer `voiceassistant.cpp`, fonction `parseVoiceCommand()`
2. Ajouter la condition : `if (lowerText.contains("mot-clé")) commands.append("COMMAND");`
3. Éditer `mainwindow.cpp`, ajouter un handler : `void MainWindow::handleVoiceCommand()`
4. Compiler et tester

---

## Fichiers importants

### À connaître

```
✓ accessibilityhelper.cpp    - Mode contraste + zoom
✓ voiceassistant.cpp         - Communication avec Python
✓ voice_engine.py            - Reconnaissance vocale
✓ mainwindow.cpp             - Intégration UI
✓ projettcpp.pro             - Configuration de build
```

### Documentation

```
📖 ACCESSIBILITY_GUIDE.md     - Guide utilisateur (français)
📖 DEVELOPER_GUIDE.md         - Pour étendre le système
📖 IMPLEMENTATION_SUMMARY.md  - Résumé technique complet
```

---

## Dépannage rapide

### "Erreur: module vocal non disponible"

```bash
# Installer les dépendances
pip install speechrecognition pyttsx3 sounddevice numpy

# Vérifier
python -c "import speech_recognition; print('OK')"
```

### "Les boutons n'apparaissent pas"

```bash
# Reconstruire le projet
qmake -r && nmake clean && nmake

# Vérifier mainwindow.h/cpp sont modifiés correctement
```

### "La reconnaissance vocale ne marche pas"

```bash
# Vérifier le microphone Windows
# Paramètres -> Son -> Entrée -> Tester le micro

# Tester directement en Python
python voice_engine.py
# Dire quelque chose...
# Vous devriez voir le texte reconnu
```

---

## Support

| Question | Réponse |
|----------|---------|
| Où installer Python? | www.python.org (version 3.7+) |
| Quelle version Qt? | Qt 5.15+ ou Qt 6.0+ |
| Ça marche sur Mac? | Oui, probablement (non testé) |
| Ça marche sur Linux? | Oui, avec les mêmes dépendances |
| Comment contribuer? | Voir DEVELOPER_GUIDE.md |

---

## Prochaines étapes

1. ✅ Compiler le projet (`qmake && nmake`)
2. ✅ Installer les dépendances Python
3. ✅ Lancer l'application (`WasteGuard.exe`)
4. ✅ Tester les 3 boutons
5. ✅ Lire la documentation complète

---

**Bon développement! 🚀**

Pour plus d'informations, consultez:
- [ACCESSIBILITY_GUIDE.md](./ACCESSIBILITY_GUIDE.md) - Guide complet
- [DEVELOPER_GUIDE.md](./DEVELOPER_GUIDE.md) - Pour développeurs
- [IMPLEMENTATION_SUMMARY.md](./IMPLEMENTATION_SUMMARY.md) - Résumé technique
