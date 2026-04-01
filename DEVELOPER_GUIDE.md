# 🛠️ GUIDE DÉVELOPPEUR - EXTENSION DU SYSTÈME D'ACCESSIBILITÉ

## 📚 Table des matières

1. [Architecture système](#architecture-système)
2. [Classes principales](#classes-principales)
3. [Ajouter des fonctionnalités](#ajouter-des-fonctionnalités)
4. [Exemple pratique](#exemple-pratique)
5. [Bonnes pratiques](#bonnes-pratiques)

---

## Architecture système

### Diagramme de flux

```
Utilisateur finale
       ↓
   Boutons UI (Qt)
    ↙  ↓  ↘
🎤  ⚫  🔍
(voix)(contraste)(zoom)
    ↓    ↓    ↓
VoiceAssistant
AccessibilityHelper
    ↓
Moteur Python (voice_engine.py)
    ↓
Google Speech API / pyttsx3
```

### Communication

**Qt ↔ Python** :
- `QProcess` pour lancer le script Python
- `stdin/stdout` pour la communication
- Protocole simple : `COMMAND:data` et `RESPONSE:result`

**Signaux/Slots** :
- `VoiceAssistant::recognizedText` → `MainWindow::onVoiceRecognized`
- `VoiceAssistant::commandRecognized` → `MainWindow::onVoiceCommandRecognized`
- `VoiceAssistant::error` → `MainWindow::onVoiceError`

---

## Classes principales

### 1. Classes principales : `AccessibilityHelper`

**Fichier** : `accessibilityhelper.h/cpp`

**Responsabilités** :
- Gérer le mode contraste élevé
- Gérer le zoom du texte
- Configurer la navigat ion clavier
- Définir les noms accessibles pour les widgets

**Méthodes publiques** :

```cpp
// Mode contraste
void enableHighContrast(QWidget *widget, bool enable = true);
bool isHighContrastEnabled() const;

// Zoom
void setTextZoom(int zoomPercentage);  // 50-200%
int getTextZoom() const;
int adjustFontSize(int baseSize) const;
QFont getScaledFont(const QFont &baseFont) const;

// Accessibilité
void setAccessibleWidget(QWidget *widget, const QString &name, 
                        const QString &description, const QString &role = "");
void setAccessibleName(QWidget *widget, const QString &name);
void setAccessibleDescription(QWidget *widget, const QString &description);

// Navigation
void setupKeyboardNavigation(QWidget *widget);
void enableTabOrder(QWidget *widget, const QList<QWidget*> &order);

// Thèmes
void applyHighContrastTheme(QWidget *widget);
void applyDefaultTheme(QWidget *widget);
```

### 2. `VoiceAssistant`

**Fichier** : `voiceassistant.h/cpp`

**Responsabilités** :
- Gérer le processus Python
- Gérer la reconnaissance vocale
- Gérer la synthèse vocale
- Parser les commandes reconnues

**Méthodes publiques** :

```cpp
bool initialize(const QString &pythonScriptPath);
bool isInitialized() const;

// Reconnaissance vocale
void startListening();
void stopListening();
bool isListening() const;

// Synthèse vocale
void speak(const QString &text);

// Envoi de commande
void sendCommand(const QString &command);
```

**Signaux** :

```cpp
signals:
    // Reconnaissance vocale
    void listeningStarted();
    void listeningFinished();
    void recognizedText(const QString &text);
    void commandRecognized(const QString &command);
    void error(const QString &errorMsg);

    // Synthèse vocale
    void speechStarted();
    void speechFinished();
```

---

## Ajouter des fonctionnalités

### Ajouter une nouvelle commande vocale

**Étape 1** : Parser dans `VoiceAssistant::parseVoiceCommand()`

```cpp
// voiceassistant.cpp
QStringList VoiceAssistant::parseVoiceCommand(const QString &text)
{
    QStringList commands;
    QString lowerText = text.toLower();

    // Nouvelle commande
    if (lowerText.contains("imprimer")) {
        commands.append("PRINT");
    }

    return commands;
}
```

**Étape 2** : Implémenter le handler dans `MainWindow`

```cpp
// mainwindow.h
private slots:
    void handleVoicePrint();

// mainwindow.cpp
void MainWindow::onVoiceCommandRecognized(const QString &command)
{
    // ...
    else if (command == "PRINT") {
        handleVoicePrint();
    }
}

void MainWindow::handleVoicePrint()
{
    m_voiceAssistant->speak("Impression des données");
    qDebug() << "Voice command: Print";
    // Votre logique d'impression ici
}
```

---

### Ajouter une nouvelle amélioration d'accessibilité

**Exemple** : Ajouter le support des polices dyslexie

```cpp
// accessibilityhelper.h
class AccessibilityHelper : public QObject
{
    // ...
public:
    void enableDyslexiaFont(QWidget *widget, bool enable);
    
private:
    bool m_dyslexiaFontEnabled;
};

// accessibilityhelper.cpp
void AccessibilityHelper::enableDyslexiaFont(QWidget *widget, bool enable)
{
    m_dyslexiaFontEnabled = enable;
    
    if (!widget) return;

    QFont dyslexiaFont;
    if (enable) {
        dyslexiaFont.setFamily("OpenDyslexic");  // Ou autre police
        dyslexiaFont.setPointSize(adjustFontSize(12));
    }

    // Appliquer à tous les widgets
    QList<QWidget*> allWidgets = widget->findChildren<QWidget*>();
    for (QWidget *w : allWidgets) {
        w->setFont(dyslexiaFont);
    }
}
```

---

### Ajouter du support multilingue

**Modif ier** `voice_engine.py` :

```python
# Configuration des langues
LANGUAGE_CONFIG = {
    'FR': 'fr-FR',
    'EN': 'en-US',
    'ES': 'es-ES',
    'DE': 'de-DE'
}

def set_language(lang_code):
    global LANGUAGE
    LANGUAGE = LANGUAGE_CONFIG.get(lang_code, 'fr-FR')
    engine.setProperty('voice', get_voice_for_language(lang_code))
```

Et connecter dans `mainwindow.h/cpp` pour permettre la sélection de la langue.

---

## Exemple pratique

### Scénario : Ajouter un bouton pour réduire l'opacité

**Étape 1** : Ajouter le widget dans `MainWindow::addAccessibilityButtonsToMaintenance()`

```cpp
// Bouton opacité
QPushButton *btnOpacity = new QPushButton("🌫️ Opacité", this);
btnOpacity->setCheckable(true);
connect(btnOpacity, &QPushButton::clicked, this, &MainWindow::on_btnOpacity_clicked);
accessibilityLayout->addWidget(btnOpacity);
```

**Étape 2** : Implémenter le slot

```cpp
// mainwindow.h
private slots:
    void on_btnOpacity_clicked();

// mainwindow.cpp
void MainWindow::on_btnOpacity_clicked()
{
    static float opacity = 1.0;
    
    if (m_btnOpacity->isChecked()) {
        opacity = 0.8;  // 80% opaque
        m_voiceAssistant->speak("Opacité réduite");
    } else {
        opacity = 1.0;  // 100% opaque
        m_voiceAssistant->speak("Opacité normale");
    }

    this->setWindowOpacity(opacity);
}
```

---

## Bonnes pratiques

### 1. Pas de dépendances d'interface

```cpp
// ❌ Mauvais : Classe d'accessibilité dépend de l'interface
class BadAccessibility {
    void showMessageBox() { /* ... */ }
};

// ✅ Bon : Émettre un signal
class GoodAccessibility : public QObject {
signals:
    void errorOccurred(const QString &msg);
};
```

### 2. Utiliser les signaux pour la communication

```cpp
// ❌ Mauvais : Appel direct
m_voiceAssistant->executeCommand();
mainWindow->updateUI();

// ✅ Bon : Via signaux
connect(m_voiceAssistant, &VoiceAssistant::commandRecognized,
        this, &MainWindow::onCommandRecognized);
```

### 3. Gérer les ressources correctement

```cpp
// ✅ Bon : Utiliser new avec this comme parent
m_voiceAssistant = new VoiceAssistant(this);  // Sera supprimé avec le parent

// ❌ Mauvais : Pas de parent
VoiceAssistant *assistant = new VoiceAssistant();  // Fuite mémoire potentielle
```

### 4. Documenter les fonctions pour les développeurs

```cpp
/**
 * @brief Analyse une commande vocale reconnue
 * @param text Le texte reconnu de l'utilisateur
 * @return Liste des commandes identifiées
 * 
 * @example
 * QStringList cmds = parseVoiceCommand("ajouter une intervention");
 * // Retourne: {"ADD_INTERVENTION"}
 */
QStringList VoiceAssistant::parseVoiceCommand(const QString &text)
```

### 5. Tester indépendamment

```cpp
// Créer une classe de test pour chaque composant
class AccessibilityHelperTest : public QObject {
    Q_OBJECT
private slots:
    void testZoom();
    void testContrast();
    void testAccessibleNames();
};
```

---

## Intégration avec d'autres modules

### Ajouter l'accessibilité au module client

```cpp
// mainwindow.cpp
void MainWindow::setupClientAccessibility()
{
    QTableWidget *clientTable = /*... obtenir la table client ...*/;
    
    if (clientTable) {
        m_accessibilityHelper->setAccessibleName(
            clientTable, "Tableau des clients"
        );
        m_accessibilityHelper->setAccessibleDescription(
            clientTable, "Liste complète des clients avec contacts"
        );
    }

    // Ajouter les boutons d'accessibilité au module client aussi
    addAccessibilityButtonsToClient();
}
```

---

## Checklist pour les contributeurs

- [ ] Code compilé sans erreurs/avertissements
- [ ] Documentation des nouvelles fonctions
- [ ] Tests unitaires pour les nouvelles fonctionnalités
- [ ] Exemples d'utilisation fournis
- [ ] Pas de dépendances externes non nécessaires
- [ ] Respecte le style de code Qt
- [ ] Compatible avec Qt 5.15+ et Qt 6
- [ ] Fonctionne sur Windows, Linux (et idéalement Mac)
- [ ] Pas de fuites mémoire
- [ ] Signaux/slots correctement connectés

---

## Ressources utiles

- **Qt Signals and Slots** : https://doc.qt.io/qt-6/signalsandslots.html
- **QAccessible** : https://doc.qt.io/qt-6/qaccessible.html
- **QProcess** : https://doc.qt.io/qt-6/qprocess.html
- **Python subprocess** : https://docs.python.org/3/library/subprocess.html

---

**Bonne codage! 🚀**
