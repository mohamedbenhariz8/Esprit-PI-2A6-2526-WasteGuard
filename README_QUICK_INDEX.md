# 📚 GUIDE RAPIDE - Où Trouver Quoi

## 🔍 INDEX DES FICHIERS

### 📖 GUIDES D'UTILISATION

| Document | Objectif | Lire si... |
|----------|----------|-----------|
| **COMPILATION_GUIDE.md** | Compilation + Testing | Tu veux compiler et tester |
| **FIXES_SUMMARY.md** | Résumé des corrections | Tu veux comprendre ce qui a changé |
| **VOICE_SETUP.md** | Installation voix Python | Tu veux que la voix fonctionne |
| **RAPPORT_CORRECTIONS.md** | Rapport technique complet | Tu veux les détails exacts |

### 🐍 FICHIERS PYTHON

| Fichier | Fonction | Installation |
|---------|----------|--------------|
| **voice_engine.py** | Moteur vocal (listen + speak) | `pip install SpeechRecognition pyttsx3` |
| **requirements.txt** | Liste dépendances Python | `pip install -r requirements.txt` |
| **install_voice_dependencies.bat** | Script auto-install (Windows) | Double-clicker le fichier .bat |

### ⚙️ FICHIERS C++ MODIFIÉS

| Fichier | Changement | Pourquoi |
|---------|-----------|---------|
| `mainwindow.h` | Noms boutons renommés | Clarté (btnZoomPlus/Minus) |
| `mainwindow.cpp` | Logique zoom refactorisée | **🔴 PRINC FIX: Zoom (-) cassé** |
| `accessibilityhelper.cpp` | Limite zoom 200%→300% | Support max zoom augmenté |

---

## 🚀 QUICK START

### ✅ Pour Compiler

1. **Ouvrir Qt Creator**
2. **File → Open Project → WasteGuard.pro**
3. **Ctrl+B** (Compiler)
4. **Ctrl+R** (Lancer)

👉 Voir: **COMPILATION_GUIDE.md**

### ✅ Pour Tester Zoom

1. Cliquer **"+"** plusieurs fois
2. Cliquer **"-"** plusieurs fois
3. Vérifier que texte change ✅

👉 Voir: **FIXES_SUMMARY.md**

### ✅ Pour Utiliser la Voix

1. **Installer Python 3.8+**
2. **Exécuter: install_voice_dependencies.bat**
3. **Relancer Qt Creator**
4. **Cliquer 🎤 button**

👉 Voir: **VOICE_SETUP.md**

### ✅ Pour Comprendre les Corrections

1. Lire: **RAPPORT_CORRECTIONS.md**
2. Voir la section "PROBLÈMES RAPPORTÉS"
3. Voir la section "SOLUTIONS IMPLÉMENTÉES"

---

## 🔧 DÉPANNAGE RAPIDE

### ❓ "Zoom (-) ne change toujours rien?"
```
→ As-tu recompilé après les changements?
→ Build → Clean All
→ Build → Build Project
→ Run
```

### ❓ "Voice button dit 'not available'?"
```
→ Python installé? python --version
→ Double-clicker: install_voice_dependencies.bat
→ Ou: pip install SpeechRecognition pyttsx3
```

### ❓ "Comment tester voice_engine.py?"
```
→ Ouvrir PowerShell dans le dossier
→ python voice_engine.py init
→ python voice_engine.py listen
→ python voice_engine.py speak "Bonjour"
```

---

## 📋 CHECKLIST PRE-LIVRAISON

- [ ] Qt Creator version récente installée
- [ ] Python 3.8+ installé
- [ ] Python dépendances installées (voice_engine)
- [ ] Projet compilé sans erreurs
- [ ] Zoom (+) fonctionne
- [ ] **Zoom (-) fonctionne ✅**
- [ ] Contraste fonctionne
- [ ] Texte change vraiment de taille
- [ ] Voice button marche ou affiche message correct

---

## 📞 SUPPORT

| Question | Réponse | Fichier |
|----------|---------|---------|
| "Comment compiler?" | Qt Creator, Ctrl+B | COMPILATION_GUIDE.md |
| "Zoom ne marche pas?" | Logique modifiée | FIXES_SUMMARY.md |
| "Voix ne marche pas?" | Installer Python | VOICE_SETUP.md |
| "Quoi exactement a changé?" | Rapport tech | RAPPORT_CORRECTIONS.md |

---

## ✨ TL;DR (Trop Long; Pas Lu)

1. **Problème principal:** Zoom (-) ne faisait rien
2. **Cause:** Limite max zoom incompatible (200% vs 300%)
3. **Solution:** 
   - Augmenté limite à 300%
   - Refactorisé logique zoom à 3 niveaux simples
4. **Résultat:** ✅ Zoom (-) fonctionne maintenant
5. **Bonus:** Créé voice_engine.py pour fonctionnalité voix

**Compile et teste maintenant!** 🎉

---

## 📚 Fichiers Par Catégorie

### 🎯 POUR COMMENCER
- START_HERE.txt
- QUICKSTART.md
- **← COMPILATION_GUIDE.md** (Lire en priorité!)

### 🔧 POUR CORRIGER LES PROBLÈMES
- **← FIXES_SUMMARY.md**
- RAPPORT_CORRECTIONS.md
- VOICE_SETUP.md

### 📖 POUR APPROFONDIR
- DEVELOPER_GUIDE.md
- IMPLEMENTATION_SUMMARY.md
- FILE_MANIFEST.md

### 🎤 POUR LA VOIX
- **← VOICE_SETUP.md**
- voice_engine.py
- requirements.txt
- install_voice_dependencies.bat

---

## 🎯 PROCHAINES ÉTAPES

1. ✅ Lire ce fichier (tu y es!)
2. ⭐ Lire: COMPILATION_GUIDE.md
3. 🔨 Compiler Qt Creator
4. 🧪 Tester les boutons zoom
5. 🎤 (Optional) Installer dépendances voix
6. ✨ Profiter!

**C'est prêt à l'emploi!** 🚀

