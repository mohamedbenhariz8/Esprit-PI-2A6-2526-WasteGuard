/**
 * FICHIER TEMPORAIRE - Ce code doit être copié/inclus dans mainwindow.cpp
 * Implémentations des fonctions d'accessibilité et de reconnaissance vocale
 */

// ============ SETUP ACCESSIBILITY ============
void MainWindow::setupAccessibilityModule()
{
    // Initialiser AccessibilityHelper
    m_accessibilityHelper = new AccessibilityHelper(this);

    // Initialiser VoiceAssistant
    m_voiceAssistant = new VoiceAssistant(this);

    // Déterminer le chemin du script Python
    QString pythonScriptPath = QCoreApplication::applicationDirPath() + "/voice_engine.py";
    
    // Essayer d'initialiser le moteur vocal
    if (!m_voiceAssistant->initialize(pythonScriptPath)) {
        qWarning() << "Impossible d'initialiser le moteur vocal";
    }

    // Connecter les signaux du VoiceAssistant
    connect(m_voiceAssistant, &VoiceAssistant::listeningStarted,
            this, &MainWindow::onVoiceListeningStarted);
    connect(m_voiceAssistant, &VoiceAssistant::listeningFinished,
            this, &MainWindow::onVoiceListeningFinished);
    connect(m_voiceAssistant, &VoiceAssistant::recognizedText,
            this, &MainWindow::onVoiceRecognized);
    connect(m_voiceAssistant, &VoiceAssistant::commandRecognized,
            this, &MainWindow::onVoiceCommandRecognized);
    connect(m_voiceAssistant, &VoiceAssistant::error,
            this, &MainWindow::onVoiceError);
    connect(m_voiceAssistant, &VoiceAssistant::speechFinished,
            this, &MainWindow::onVoiceSpeechFinished);

    // Ajouter les boutons d'accessibilité au module maintenance
    setupMaintenanceAccessibility();

    qDebug() << "Accessibility Module initialized successfully";
}

// ============ SETUP MAINTENANCE ACCESSIBILITY ============
void MainWindow::setupMaintenanceAccessibility()
{
    // Ajouter les boutons d'accessibilité à l'interface maintenance
    addAccessibilityButtonsToMaintenance();

    // Appliquer les noms accessibles aux widgets de maintenance
    QTableWidget *maintTable = maintenanceTable();
    if (maintTable) {
        m_accessibilityHelper->setAccessibleName(
            maintTable,
            "Tableau des interventions de maintenance"
        );
        m_accessibilityHelper->setAccessibleDescription(
            maintTable,
            "Liste complète des interventions: date, référence, durée, coût, statut, type et priorité"
        );
    }
}

// ============ ADD ACCESSIBILITY BUTTONS ============
void MainWindow::addAccessibilityButtonsToMaintenance()
{
    if (!ui) return;

    if (m_btnMicrophone != nullptr) {
        return;
    }

    // Trouver le widget parent du module maintenance
    QWidget *maintWidget = maintenanceTable();
    if (!maintWidget) {
        qWarning() << "Cannot find maintenance table widget";
        return;
    }

    // Obtenir le widget parent (probablement un panel ou dock)
    QWidget *parentWidget = maintWidget->parentWidget();
    if (!parentWidget) return;

    // Créer une toolbar pour les boutons d'accessibilité
    QHBoxLayout *accessibilityLayout = new QHBoxLayout();
    accessibilityLayout->setContentsMargins(0, 0, 0, 0);
    accessibilityLayout->setSpacing(5);

    // ===== BOUTON MICROPHONE =====
    m_btnMicrophone = new QPushButton("🎤 Voix", this);
    m_btnMicrophone->setObjectName("btnMaintAccessibilityMicrophone");
    m_btnMicrophone->setToolTip("Démarrer la reconnaissance vocale (Français)");
    m_btnMicrophone->setMaximumWidth(130);
    m_btnMicrophone->setStyleSheet(
        "QPushButton {"
        "   background-color: #0078D4;"
        "   color: white;"
        "   border: 2px solid #0078D4;"
        "   border-radius: 5px;"
        "   padding: 8px 12px;"
        "   font-weight: bold;"
        "   font-size: 10pt;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1084D8;"
        "   border: 2px solid #0078D4;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #0063B1;"
        "   border: 3px solid #00A4EF;"
        "}"
    );
    connect(m_btnMicrophone, &QPushButton::clicked, this, &MainWindow::on_btnMicrophone_clicked);
    m_accessibilityHelper->setAccessibleName(m_btnMicrophone, "Reconnaissance vocale");
    
    // Désactiver visuellement le bouton si la voix n'est pas disponible
    // IMPORTANT: NE PAS utiliser setEnabled(false) - cela bloque le signal clicked!
    if (!m_voiceAssistant || !m_voiceAssistant->isInitialized()) {
        m_btnMicrophone->setToolTip("Reconnaissance vocale indisponible - Cliquez pour voir comment activer");
        m_btnMicrophone->setStyleSheet(
            "QPushButton {"
            "   background-color: #CCCCCC;"
            "   color: #666666;"
            "   border: 2px solid #999999;"
            "   border-radius: 5px;"
            "   padding: 8px 12px;"
            "   font-weight: bold;"
            "   font-size: 10pt;"
            "}"
        );
    }
    
    accessibilityLayout->addWidget(m_btnMicrophone);

    // ===== BOUTON CONTRASTE ÉLEVÉ =====
    m_btnHighContrast = new QPushButton("⚫ Contraste", this);
    m_btnHighContrast->setObjectName("btnMaintAccessibilityHighContrast");
    m_btnHighContrast->setToolTip("Mode contraste élevé (noir et jaune)");
    m_btnHighContrast->setMaximumWidth(120);
    m_btnHighContrast->setCheckable(true);
    m_btnHighContrast->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border: 2px solid #4CAF50;"
        "   border-radius: 5px;"
        "   padding: 8px 12px;"
        "   font-weight: bold;"
        "   font-size: 10pt;"
        "}"
        "QPushButton:hover {"
        "   background-color: #66BB6A;"
        "   border: 2px solid #4CAF50;"
        "}"
        "QPushButton:checked {"
        "   background-color: #000000;"
        "   color: #FFFF00;"
        "   border: 3px solid #FFFF00;"
        "}"
    );
    connect(m_btnHighContrast, &QPushButton::clicked, this, &MainWindow::on_btnHighContrast_clicked);
    m_accessibilityHelper->setAccessibleName(m_btnHighContrast, "Mode contraste");
    accessibilityLayout->addWidget(m_btnHighContrast);

    // ===== BOUTON ZOOM TEXTE =====
    m_btnTextZoom = new QPushButton("🔍 Zoom", this);
    m_btnTextZoom->setObjectName("btnMaintAccessibilityTextZoom");
    m_btnTextZoom->setToolTip("Augmenter la taille du texte (100% -> 150% -> 200%)");
    m_btnTextZoom->setMaximumWidth(120);
    m_btnTextZoom->setStyleSheet(
        "QPushButton {"
        "   background-color: #FF9800;"
        "   color: white;"
        "   border: 2px solid #FF9800;"
        "   border-radius: 5px;"
        "   padding: 8px 12px;"
        "   font-weight: bold;"
        "   font-size: 10pt;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FFB74D;"
        "   border: 2px solid #FF9800;"
        "}"
    );
    connect(m_btnTextZoom, &QPushButton::clicked, this, &MainWindow::on_btnTextZoom_clicked);
    m_accessibilityHelper->setAccessibleName(m_btnTextZoom, "Zoom texte");
    accessibilityLayout->addWidget(m_btnTextZoom);

    // ===== BOUTON ZOOM TEXTE MOINS =====
    m_btnTextZoomMinus = new QPushButton("🔍 Zoom -", this);
    m_btnTextZoomMinus->setObjectName("btnMaintAccessibilityTextZoomMinus");
    m_btnTextZoomMinus->setToolTip("Diminuer la taille du texte (200% -> 150% -> 100%)");
    m_btnTextZoomMinus->setMaximumWidth(120);
    m_btnTextZoomMinus->setStyleSheet(
        "QPushButton {"
        "   background-color: #FF9800;"
        "   color: white;"
        "   border: 2px solid #FF9800;"
        "   border-radius: 5px;"
        "   padding: 8px 12px;"
        "   font-weight: bold;"
        "   font-size: 10pt;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FFB74D;"
        "   border: 2px solid #FF9800;"
        "}"
    );
    connect(m_btnTextZoomMinus, &QPushButton::clicked, this, &MainWindow::on_btnTextZoomMinus_clicked);
    m_accessibilityHelper->setAccessibleName(m_btnTextZoomMinus, "Zoom moins");
    accessibilityLayout->addWidget(m_btnTextZoomMinus);

    accessibilityLayout->addStretch();

    // Créer un widget conteneur
    QWidget *accessibilityWidget = new QWidget(this);
    accessibilityWidget->setLayout(accessibilityLayout);
    accessibilityWidget->setMaximumHeight(50);
    accessibilityWidget->setStyleSheet(
        "QWidget {"
        "   background-color: #F5F5F5;"
        "   border-bottom: 1px solid #CCCCCC;"
        "   padding: 5px;"
        "}"
    );

    // Trouver le layout parent et ajouter les boutons
    QVBoxLayout *mainLayout = nullptr;
    if (auto stack = qobject_cast<QStackedWidget*>(parentWidget)) {
        if (auto widget = stack->currentWidget()) {
            mainLayout = qobject_cast<QVBoxLayout*>(widget->layout());
        }
    } else if (auto scrollArea = qobject_cast<QScrollArea*>(parentWidget)) {
        if (auto widget = scrollArea->widget()) {
            mainLayout = qobject_cast<QVBoxLayout*>(widget->layout());
        }
    } else {
        mainLayout = qobject_cast<QVBoxLayout*>(parentWidget->layout());
    }

    if (mainLayout) {
        mainLayout->insertWidget(0, accessibilityWidget);
    } else {
        accessibilityWidget->setParent(ui->centralwidget);
        accessibilityWidget->move(10, 70);
        accessibilityWidget->resize(380, 50);
    }

    // Désactiver visuellement le bouton microphone si le moteur vocal n'est pas disponible
    // IMPORTANT: NE PAS utiliser setEnabled(false) - cela bloque le signal clicked!
    if (!m_voiceAssistant || !m_voiceAssistant->isInitialized()) {
        m_btnMicrophone->setStyleSheet(
            "QPushButton {"
            "   background-color: #CCCCCC;"
            "   color: #888888;"
            "   border: 2px solid #CCCCCC;"
            "   border-radius: 5px;"
            "   padding: 8px 12px;"
            "   font-weight: bold;"
            "   font-size: 10pt;"
            "}"
            "QPushButton:hover {"
            "   background-color: #CCCCCC;"
            "   border: 2px solid #CCCCCC;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #CCCCCC;"
            "   border: 3px solid #CCCCCC;"
            "}"
        );
        m_btnMicrophone->setToolTip("Reconnaissance vocale indisponible - Cliquez pour voir comment activer");
    }

    qDebug() << "Accessibility buttons added to maintenance";
}

// ============ BUTTON SLOTS ============
void MainWindow::on_btnMicrophone_clicked()
{
    if (!m_voiceAssistant || !m_voiceAssistant->isInitialized()) {
        QMessageBox::information(this, "Reconnaissance Vocale Indisponible", 
            "Le moteur vocal n'est pas disponible.\n\n"
            "Pour utiliser la reconnaissance vocale, vous devez:\n"
            "1. Avoir Python installé\n"
            "2. Avoir le fichier 'voice_engine.py' dans le dossier de l'application\n"
            "3. Installer les dépendances Python:\n"
            "   - pip install speech_recognition\n"
            "   - pip install pyttsx3\n"
            "   - pip install sounddevice\n\n"
            "Pour le moment, vous pouvez utiliser les autres fonctionnalités d'accessibilité.");
        return;
    }

    if (m_voiceAssistant->isListening()) {
        m_voiceAssistant->stopListening();
        m_btnMicrophone->setText("🎤 Voix");
    } else {
        m_voiceAssistant->startListening();
        m_btnMicrophone->setText("🔴 Écoute...");
        m_voiceAssistant->speak("Je vous écoute");
    }
}

void MainWindow::on_btnHighContrast_clicked()
{
    if (!m_accessibilityHelper || !m_btnHighContrast) return;

    const bool enable = m_btnHighContrast->isChecked();
    m_accessibilityHelper->enableHighContrast(this, enable);

    // Update button label so the user knows whether contrast is ON or OFF.
    if (enable) {
        m_btnHighContrast->setText(QString::fromUtf8("\u2600 Normal"));
        m_btnHighContrast->setToolTip(QString::fromUtf8("D\u00e9sactiver le mode contraste \u00e9lev\u00e9"));
    } else {
        m_btnHighContrast->setText(QString::fromUtf8("\u26ab Contraste"));
        m_btnHighContrast->setToolTip(QString::fromUtf8("Activer le mode contraste \u00e9lev\u00e9 (noir / jaune)"));
    }

    if (m_voiceAssistant && m_voiceAssistant->isInitialized()) {
        m_voiceAssistant->speak(enable
            ? QString::fromUtf8("Mode contraste \u00e9lev\u00e9 activ\u00e9")
            : QString::fromUtf8("Mode contraste d\u00e9sactiv\u00e9"));
    }
}

void MainWindow::on_btnTextZoom_clicked()
{
    int currentZoom = m_accessibilityHelper->getTextZoom();
    int newZoom = currentZoom;

    // Zoom AUGMENTE: 100% -> 150% -> 200% -> 100%
    if (currentZoom == 100) {
        newZoom = 150;
    } else if (currentZoom == 150) {
        newZoom = 200;
    } else {
        newZoom = 100;  // 200% retour à 100%
    }

    qDebug() << "[ZOOM+] Changing from" << currentZoom << "% to" << newZoom << "%";

    m_accessibilityHelper->setTextZoom(newZoom);
    
    // Appliquer le zoom à TOUS les widgets importants
    m_accessibilityHelper->applyZoomToWidget(this);
    
    // Appliquer aussi aux tables de maintenance si visibles
    if (ui && ui->centralwidget) {
        QList<QTableWidget*> tables = ui->centralwidget->findChildren<QTableWidget*>();
        for (QTableWidget *table : tables) {
            m_accessibilityHelper->applyZoomToWidget(table);
        }
    }
    
    // IMPORTANT: Mettre à jour les DEUX boutons avec le zoom courant
    m_btnTextZoom->setText(QString("🔍 %1%").arg(newZoom));
    m_btnTextZoomMinus->setText(QString("🔍 Zoom -"));
    
    // Force la mise à jour de tous les widgets
    update();
    repaint();
    
    qDebug() << "[ZOOM+] Zoom applied successfully, new zoom:" << m_accessibilityHelper->getTextZoom() << "%";
    
    if (m_voiceAssistant && m_voiceAssistant->isInitialized()) {
        m_voiceAssistant->speak(QString("Zoom: %1 pour cent").arg(newZoom));
    }
}

void MainWindow::on_btnTextZoomMinus_clicked()
{
    int currentZoom = m_accessibilityHelper->getTextZoom();
    int newZoom = currentZoom;

    // Zoom DIMINUE: TOUJOURS diminue ou reste au minimum
    if (currentZoom == 200) {
        newZoom = 150;      // 200% -> 150% (diminue)
    } else if (currentZoom == 150) {
        newZoom = 100;      // 150% -> 100% (diminue)
    } else {
        newZoom = 100;      // 100% reste 100% (minimum)
    }

    qDebug() << "[ZOOM-] Changing from" << currentZoom << "% to" << newZoom << "%";

    m_accessibilityHelper->setTextZoom(newZoom);
    m_accessibilityHelper->applyZoomToWidget(this);
    
    // IMPORTANT: Mettre à jour les DEUX boutons toujours!
    m_btnTextZoom->setText(QString("🔍 %1%").arg(newZoom));
    m_btnTextZoomMinus->setText(QString("🔍 Zoom -"));
    
    // Force la mise à jour de tous les widgets
    update();
    repaint();
    
    qDebug() << "[ZOOM-] Zoom applied successfully, new zoom:" << m_accessibilityHelper->getTextZoom() << "%";
    
    if (m_voiceAssistant && m_voiceAssistant->isInitialized()) {
        m_voiceAssistant->speak(QString("Zoom: %1 pour cent").arg(newZoom));
    }
}

// ============ VOICE ASSISTANT SLOTS ============
void MainWindow::onVoiceListeningStarted()
{
    qDebug() << "[VOICE] Listening started";
}

void MainWindow::onVoiceListeningFinished()
{
    qDebug() << "[VOICE] Listening finished";
    if (m_btnMicrophone) {
        m_btnMicrophone->setText("🎤 Voix");
    }
}

void MainWindow::onVoiceRecognized(const QString &text)
{
    qDebug() << "[VOICE] Recognized:" << text;
}

void MainWindow::onVoiceCommandRecognized(const QString &command)
{
    qDebug() << "[VOICE] Command:" << command;

    if (command == "ADD_INTERVENTION") {
        handleVoiceAddIntervention();
    } else if (command == "DELETE_INTERVENTION") {
        handleVoiceDeleteIntervention();
    } else if (command == "SEARCH") {
        handleVoiceSearchIntervention("");
    } else if (command == "SHOW_LIST") {
        handleVoiceShowList();
    } else if (command == "EXPORT") {
        handleVoiceExport();
    } else if (command == "HELP") {
        handleVoiceHelp();
    }
}

void MainWindow::onVoiceError(const QString &errorMsg)
{
    qWarning() << "[VOICE ERROR]" << errorMsg;
    if (m_btnMicrophone) {
        m_btnMicrophone->setText("🎤 Voix");
    }
}

void MainWindow::onVoiceSpeechFinished()
{
    qDebug() << "[VOICE] Speech finished";
}

// ============ VOICE COMMANDS HANDLERS ============
void MainWindow::handleVoiceAddIntervention()
{
    qDebug() << "Voice: Add Intervention";
    if (m_voiceAssistant && m_voiceAssistant->isInitialized()) {
        m_voiceAssistant->speak("Ajout d'une nouvelle intervention");
    }
}

void MainWindow::handleVoiceDeleteIntervention()
{
    qDebug() << "Voice: Delete Intervention";
    if (m_voiceAssistant && m_voiceAssistant->isInitialized()) {
        m_voiceAssistant->speak("Suppression d'une intervention");
    }
}

void MainWindow::handleVoiceSearchIntervention(const QString &searchTerm)
{
    qDebug() << "Voice: Search:" << searchTerm;
}

void MainWindow::handleVoiceShowList()
{
    qDebug() << "Voice: Show List";
    if (m_voiceAssistant && m_voiceAssistant->isInitialized()) {
        m_voiceAssistant->speak("Affichage des interventions");
    }
}

void MainWindow::handleVoiceExport()
{
    qDebug() << "Voice: Export";
    if (m_voiceAssistant && m_voiceAssistant->isInitialized()) {
        m_voiceAssistant->speak("Export des données");
    }
}

void MainWindow::handleVoiceHelp()
{
    QString help = "Commandes: ajouter intervention, supprimer intervention, rechercher, afficher, exporter";
    qDebug() << "Voice: Help";
    if (m_voiceAssistant && m_voiceAssistant->isInitialized()) {
        m_voiceAssistant->speak(help);
    }
}
