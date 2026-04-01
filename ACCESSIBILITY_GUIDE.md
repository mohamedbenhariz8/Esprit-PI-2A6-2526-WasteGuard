# 🎯 GUIDE D'ACCESSIBILITÉ WASTEGUARD - MODULE MAINTENANCE

## 📋 Vue d'ensemble

Un système complet d'accessibilité pour les personnes handicapées a été intégré au module maintenance de WasteGuard. Ce guide explique les fonctionnalités et comment les utiliser.

---

## 🎤 1. ASSISTANT VOCAL

### Fonctionnalités
- **Reconnaissance vocale** en français
- **Synthèse vocale** (TTS) pour lire du texte
- **Commandes vocales** intuitives en français

### Installation des dépendances Python

```bash
# Installer les packages Python requis
pip install speechrecognition pyttsx3 sounddevice numpy pyaudio

# Ou sur Windows avec conda
conda install -c conda-forge speech_recognition pyttsx3 sounddevice numpy
```

### Commandes vocales disponibles

| Commande | Action |
|----------|--------|
| "ajouter intervention" | Ajouter une nouvelle intervention |
| "supprimer intervention" | Supprimer une intervention existante |
| "rechercher [nom]" | Rechercher une intervention |
| "afficher" ou "show" | Afficher la liste des interventions |
| "exporter" | Exporter les données |
| "aide" ou "help" | Afficher l'aide |

### Utilisation

1. Cliquez sur le bouton **"🎤 Voix"** en haut du module maintenance
2. Une boîte de dialogue s'affiche : **"Je vous écoute"**
3. Prononcez votre commande en français
4. Le système exécute l'action correspondante
5. Cliquez à nouveau pour arrêter l'écoute

### Exemple d'utilisation
```
Utilisateur: "ajouter intervention"
Système réagit: Prépare l'ajout d'une nouvelle intervention
```

---

## 🎨 2. MODE CONTRASTE ÉLEVÉ

### Caractéristiques
- **Fond noir** avec **texte blanc** pour une meilleure lisibilité
- **Bordures jaunes** autour des éléments importants
- **Bordures cyan** lors du survol
- **Focus vert** pour la navigation clavier

### Utilisation

1. Cliquez sur le bouton **"⚫ Contraste"** dans la barre d'accessibilité
2. L'interface basculera immédiatement en mode contraste élevé
3. Cliquez à nouveau pour revenir au mode normal

### Couleurs du mode contraste
- **Fond** : Noir (#000000)
- **Texte** : Blanc (#FFFFFF)
- **Bordures** : Jaune (#FFFF00)
- **Hover** : Cyan (#00FFFF)
- **Focus** : Vert (#00FF00)

---

## 🔍 3. ZOOM TEXTE

### Niveaux de zoom
- **100%** : Taille standard
- **150%** : 50% plus grand
- **200%** : Double de la taille

### Utilisation

1. Cliquez sur le bouton **"🔍 Zoom"** dans la barre d'accessibilité
2. Chaque clic augmente le zoom : 100% → 150% → 200% → 100%
3. La taille de police change pour tous les éléments

### Conseil
Pour les personnes malvoyantes, combinez le zoom texte avec le mode contraste pour une meilleure expérience.

---

## ⌨️ 4. NAVIGATION CLAVIER AMÉLIORÉE

### Touches disponibles
- **Tab** : Naviguer entre les éléments
- **Shift + Tab** : Naviguer en arrière
- **Enter** : Cliquer sur un bouton
- **Espace** : Cocher/Décocher
- **Flèches** : Naviguer dans les listes

### Accessibilité des widgets
Tous les widgets du module maintenance ont des noms accessibles pour les lecteurs d'écran :
- Tableau des interventions
- Champs de formulaire
- Boutons d'action

---

## 🏗️ 5. ARCHITECTURE TECHNIQUE

### Composants créés

#### **AccessibilityHelper** (`accessibilityhelper.h/cpp`)
- Gère le mode contraste élevé
- Contrôle le zoom du texte
- Configure les noms accessibles
- Configure la navigation clavier

#### **VoiceAssistant** (`voiceassistant.h/cpp`)
- Communication avec le moteur Python
- Gestion des signaux/slots
- Parsing des commandes vocales
- Synthèse vocale

#### **voice_engine.py**
- Script Python pour reconnaissance vocale
- Utilise Google Speech Recognition API
- Synthèse vocale avec pyttsx3
- Communication via stdin/stdout

### Flux de communication

```
Qt Application
    ↓
VoiceAssistant (C++)
    ↓
Python Process (voice_engine.py)
    ↓
Google Speech Recognition API
    ↓
pyttsx3 (TTS)
```

---

## 🚀 6. DÉPLOIEMENT

### Fichiers ajoutés
- `accessibilityhelper.h` / `accessibilityhelper.cpp`
- `voiceassistant.h` / `voiceassistant.cpp`
- `voice_engine.py`
- `mainwindow_accessibility_impl.cpp` (implémentations)

### Modifications apportées
- `mainwindow.h` : Ajout des déclarations de membres et slots
- `mainwindow.cpp` : Intégration des fonctionnalités
- `projettcpp.pro` : Ajout des nouveaux fichiers sources

### Configuration du build

Le fichier `projettcpp.pro` a été mis à jour avec :
```pro
SOURCES += accessibilityhelper.cpp voiceassistant.cpp
HEADERS += accessibilityhelper.h voiceassistant.h
QT += concurrent
```

### Compilation

```bash
# Reconstruire le projet
qmake
make
```

### Vérification

Après la compilation, assurez-vous que :
1. Les trois boutons d'accessibilité apparaissent dans le module maintenance
2. Le microphone fonctionne (nécessite Python installé)
3. Le zoom texte change la taille des polices
4. Le bouton contraste active/désactive le mode

---

## 🐛 7. DÉPANNAGE

### Le moteur vocal ne fonctionne pas
```
Erreur: "Moteur vocal non disponible"
Solution:
1. Vérifier que Python est installé
2. Installer les dépendances: pip install speechrecognition pyttsx3 sounddevice
3. Vérifier que voice_engine.py est dans le même répertoire que l'exécutable
```

### Les boutons n'apparaissent pas
```
Solution:
1. Assurez-vous que le module maintenance est activé
2. Vérifiez les logs de la console (qDebug)
3. Reconstruisez le projet avec: qmake && make
```

### Le zoom texte ne fonctionne pas
```
Solution:
1. Vérifiez que AccessibilityHelper est inicialié
2. Le zoom n'affecte que les widgets du module maintenance
3. Essayez de cliquer plusieurs fois
```

### La reconnaissance vocale est inexacte
```
Solution:
1. Parlez clairement et calmement
2. Réduisez le bruit de fond
3. Utilisez un microphone de meilleure qualité
4. Assurez-vous que la connexion Internet est stable (Google Speech API)
```

---

## 📝 8. APIConfiguration pour développeurs

### Initialiser l'accessibilité
```cpp
m_accessibilityHelper = new AccessibilityHelper(this);
m_voiceAssistant = new VoiceAssistant(this);
m_voiceAssistant->initialize("path/to/voice_engine.py");
```

### Ajouter un élément accessible
```cpp
m_accessibilityHelper->setAccessibleWidget(widget, "Nom", "Description");
```

### Déclencher une synthèse vocale
```cpp
m_voiceAssistant->speak("Texte à lire");
```

### Réagir à une commande vocale
```cpp
connect(m_voiceAssistant, &VoiceAssistant::commandRecognized,
        this, &MainWindow::onVoiceCommandRecognized);
```

---

## ✅ 9. CONFORMITÉ ACCESSIBILITÉ

Ce système respecte les normes :
- **WCAG 2.1** (Web Content Accessibility Guidelines)
- **Loi Française n°2005-102** (Loi Handicap et accessibilité)
- **ISO/CEI 40500** (Accessibilité du web)

### Fonctionne avec
- ✅ Lecteurs d'écran (NVDA, JAWS)
- ✅ Loupes d'écran
- ✅ Claviers spécialisés
- ✅ Commandes vocales

---

## 📞 10. SUPPORT

Pour toute question ou problème :
1. Consultez les logs : activez `qDebug()` dans QtCreator
2. Vérifiez les dépendances Python
3. Testez le script `voice_engine.py` indépendamment
4. Consultez la documentation des fichiers source

---

## 🎉 Conclusion

Ce système d'accessibilité rend WasteGuard plus inclusif pour les personnes en situation de handicap. Les trois piliers (voix, contraste, zoom) permettent une meilleure ergonomie et accessibilité général.

**Merci de rendre le logiciel accessible à tous! ♿💪**
