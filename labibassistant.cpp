#include "labibassistant.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>

LabibAssistant::LabibAssistant(QWidget *parent)
    : QDialog(parent), m_selectedModule("Client")
{
    setupUI();
    setupConnections();
}

LabibAssistant::~LabibAssistant()
{
}

void LabibAssistant::setupUI()
{
    setWindowTitle("🤖 Labib AI Assistant — WasteGuard Intelligence");
    setWindowIcon(QIcon(":/assets/logo.png"));
    resize(1200, 750);
    setModal(false);
    
    // Professional Color Palette
    const QString PRIMARY_COLOR = "#0f2b4c";      // Deep Blue
    const QString SECONDARY_COLOR = "#2a5298";    // Medium Blue
    const QString ACCENT_COLOR = "#27ae60";       // Green
    const QString SUCCESS_COLOR = "#059669";      // Bright Green
    const QString WARNING_COLOR = "#f59e0b";      // Amber
    const QString ERROR_COLOR = "#dc2626";        // Red
    const QString TEXT_PRIMARY = "#1e293b";       // Dark Gray
    const QString TEXT_SECONDARY = "#64748b";     // Medium Gray
    const QString BG_LIGHT = "#f8fafc";           // Light Gray
    const QString BG_WHITE = "#ffffff";           // White
    const QString BORDER_COLOR = "#e2e8f0";       // Light Border
    
    QString stylesheet = QString(
        "QDialog { background: %1; }"
        "QTextEdit { background: %6; border: 1px solid %7; border-radius: 12px; padding: 12px; font-size: 13px; color: %2; }"
        "QLineEdit { background: %6; border: 1px solid %7; border-radius: 8px; padding: 10px; font-size: 13px; }"
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %0, stop:1 %1); color: white; border: none; border-radius: 10px; padding: 12px 18px; font-weight: 700; font-size: 13px; }"
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %1, stop:1 %0); }"
        "QPushButton:pressed { transform: scale(0.98); }"
        "QTableWidget { background: %6; border: 1px solid %7; border-radius: 8px; gridline-color: %7; }"
        "QHeaderView::section { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 %0, stop:1 %1); color: white; padding: 8px; font-weight: 700; border: none; }"
        "QListWidget { background: %6; border: 1px solid %7; border-radius: 8px; }"
        "QListWidget::item:selected { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %0, stop:1 %1); color: white; font-weight: 700; }"
        "QLabel { color: %2; }"
        )
        .arg(PRIMARY_COLOR, SECONDARY_COLOR, BG_LIGHT, TEXT_PRIMARY, TEXT_SECONDARY, BG_WHITE, BORDER_COLOR);
    
    setStyleSheet(stylesheet);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(14);
    
    // ===== PROFESSIONAL HEADER =====
    QFrame *headerFrame = new QFrame();
    headerFrame->setMinimumHeight(100);
    headerFrame->setStyleSheet(
        QString("QFrame { "
                "background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %0, stop:1 %1); "
                "border-radius: 16px; border: 2px solid %2; }")
        .arg(PRIMARY_COLOR, SECONDARY_COLOR, ACCENT_COLOR));
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(20, 16, 20, 16);
    headerLayout->setSpacing(16);
    
    // Logo/Icon Area
    QLabel *iconLabel = new QLabel("🤖");
    iconLabel->setStyleSheet("font-size: 48px;");
    headerLayout->addWidget(iconLabel);
    
    // Title & Description
    QVBoxLayout *titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(4);
    
    QLabel *titleLabel = new QLabel("Labib AI Assistant");
    titleLabel->setStyleSheet("color: white; font-size: 22px; font-weight: 900; letter-spacing: 0.5px;");
    titleLayout->addWidget(titleLabel);
    
    QLabel *subtitleLabel = new QLabel("Assistant Intelligent pour la Gestion Optimale des Déchets");
    subtitleLabel->setStyleSheet("color: #bfdbfe; font-size: 12px; font-weight: 600;");
    titleLayout->addWidget(subtitleLabel);
    
    headerLayout->addLayout(titleLayout, 1);
    
    // Status Badge
    QLabel *statusLabel = new QLabel("🟢 EN LIGNE");
    statusLabel->setStyleSheet(QString(
        "color: %1; font-weight: 700; font-size: 11px; "
        "background: rgba(255, 255, 255, 0.2); border-radius: 20px; "
        "padding: 6px 12px;").arg(ACCENT_COLOR));
    headerLayout->addWidget(statusLabel, 0, Qt::AlignRight | Qt::AlignVCenter);
    
    mainLayout->addWidget(headerFrame);
    
    // ===== MAIN CONTENT LAYOUT =====
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(14);
    
    // --- LEFT PANEL: Module Selection ---
    QFrame *leftPanel = new QFrame();
    leftPanel->setStyleSheet(QString(
        "QFrame { background: %1; border: 2px solid %2; border-radius: 14px; }")
        .arg(BG_WHITE, BORDER_COLOR));
    leftPanel->setMinimumWidth(200);
    leftPanel->setMaximumWidth(220);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(14, 14, 14, 14);
    leftLayout->setSpacing(10);
    
    QLabel *moduleLbl = new QLabel("📋 Modules");
    moduleLbl->setStyleSheet(QString("font-weight: 700; color: %0; font-size: 13px;").arg(PRIMARY_COLOR));
    leftLayout->addWidget(moduleLbl);
    
    m_moduleSelector = new QListWidget();
    m_moduleSelector->addItems({"👥 Clients", "👔 Employés", "📦 Produits", "🔧 Maintenance", "📋 Commandes", "📊 Stock"});
    m_moduleSelector->setCurrentRow(0);
    m_moduleSelector->setStyleSheet(QString(
        "QListWidget { background: %1; border: 1px solid %2; border-radius: 8px; }"
        "QListWidget::item { padding: 10px 8px; }"
        "QListWidget::item:selected { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %3, stop:1 %4); color: white; }")
        .arg(BG_LIGHT, BORDER_COLOR, PRIMARY_COLOR, SECONDARY_COLOR));
    leftLayout->addWidget(m_moduleSelector);
    
    QLabel *infoLbl = new QLabel("Sélectionnez le module pour ajouter, modifier ou analyser les données avec Labib.");
    infoLbl->setWordWrap(true);
    infoLbl->setStyleSheet(QString("color: %0; font-size: 11px; line-height: 1.4;").arg(TEXT_SECONDARY));
    leftLayout->addWidget(infoLbl);
    leftLayout->addStretch();
    
    contentLayout->addWidget(leftPanel);
    
    // --- CENTER PANEL: Chat + Input ---
    QFrame *centerPanel = new QFrame();
    centerPanel->setStyleSheet(QString(
        "QFrame { background: %1; border: 2px solid %2; border-radius: 14px; }")
        .arg(BG_WHITE, BORDER_COLOR));
    QVBoxLayout *centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setContentsMargins(14, 14, 14, 14);
    centerLayout->setSpacing(12);
    
    QLabel *chatLbl = new QLabel("💬 Conversation Intelligente");
    chatLbl->setStyleSheet(QString("font-weight: 700; color: %0; font-size: 13px;").arg(PRIMARY_COLOR));
    centerLayout->addWidget(chatLbl);
    
    m_chatHistory = new QTextEdit();
    m_chatHistory->setReadOnly(true);
    m_chatHistory->setStyleSheet(QString(
        "QTextEdit { background: %0; border: 1px solid %1; border-radius: 10px; padding: 12px; color: %2; }").arg(BG_LIGHT, BORDER_COLOR, TEXT_PRIMARY));
    centerLayout->addWidget(m_chatHistory);
    
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(10);
    m_userInput = new QLineEdit();
    m_userInput->setPlaceholderText("💬 Tapez votre question ou demande...");
    m_userInput->setMinimumHeight(44);
    m_userInput->setStyleSheet(QString(
        "QLineEdit { background: %0; border: 2px solid %1; border-radius: 10px; padding: 10px 14px; "
        "font-size: 13px; color: %2; } QLineEdit:focus { border: 2px solid %3; }").arg(BG_LIGHT, BORDER_COLOR, TEXT_PRIMARY, SECONDARY_COLOR));
    
    m_sendButton = new QPushButton("Envoyer 📤");
    m_sendButton->setMinimumWidth(120);
    m_sendButton->setMinimumHeight(44);
    m_sendButton->setStyleSheet(QString(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %0, stop:1 %1); "
        "color: white; border: none; border-radius: 10px; "
        "padding: 10px 18px; font-weight: 700; font-size: 13px; } "
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %1, stop:1 %0); } "
        "QPushButton:pressed { opacity: 0.8; }").arg(SECONDARY_COLOR, PRIMARY_COLOR));
    
    inputLayout->addWidget(m_userInput, 1);
    inputLayout->addWidget(m_sendButton);
    centerLayout->addLayout(inputLayout);
    
    contentLayout->addWidget(centerPanel, 1);
    
    // --- RIGHT PANEL: Import & Actions ---
    QFrame *rightPanel = new QFrame();
    rightPanel->setStyleSheet(QString(
        "QFrame { background: %1; border: 2px solid %2; border-radius: 14px; }")
        .arg(BG_WHITE, BORDER_COLOR));
    rightPanel->setMinimumWidth(220);
    rightPanel->setMaximumWidth(240);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(14, 14, 14, 14);
    rightLayout->setSpacing(10);
    
    QLabel *actionLbl = new QLabel("⚙️ Opérations");
    actionLbl->setStyleSheet(QString("font-weight: 700; color: %0; font-size: 13px;").arg(PRIMARY_COLOR));
    rightLayout->addWidget(actionLbl);
    
    m_importButton = new QPushButton("📁 Importer Fichier");
    m_importButton->setMinimumHeight(44);
    m_importButton->setStyleSheet(QString(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %0, stop:1 %1); "
        "color: white; border: none; border-radius: 10px; padding: 10px 12px; font-weight: 700; } "
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %1, stop:1 %0); }").arg(SUCCESS_COLOR, ACCENT_COLOR));
    rightLayout->addWidget(m_importButton);
    
    m_processButton = new QPushButton("✓ Traiter Données");
    m_processButton->setMinimumHeight(44);
    m_processButton->setEnabled(false);
    m_processButton->setStyleSheet(QString(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %0, stop:1 %1); "
        "color: white; border: none; border-radius: 10px; padding: 10px 12px; font-weight: 700; } "
        "QPushButton:hover:enabled { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %1, stop:1 %0); } "
        "QPushButton:disabled { background: #cbd5e1; color: #94a3b8; }").arg(WARNING_COLOR, ACCENT_COLOR));
    rightLayout->addWidget(m_processButton);
    
    QLabel *previewLbl = new QLabel("📊 Aperçu");
    previewLbl->setStyleSheet(QString("font-weight: 700; color: %0; font-size: 12px; margin-top: 6px;").arg(PRIMARY_COLOR));
    rightLayout->addWidget(previewLbl);
    
    m_importPreview = new QTableWidget();
    m_importPreview->setMaximumHeight(140);
    m_importPreview->setColumnCount(2);
    m_importPreview->setHorizontalHeaderLabels({"Champ", "Valeur"});
    m_importPreview->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_importPreview->setStyleSheet(QString(
        "QTableWidget { background: %0; border: 1px solid %1; border-radius: 8px; } "
        "QHeaderView::section { background: %2; color: white; padding: 6px; font-weight: 700; }").arg(BG_LIGHT, BORDER_COLOR, SECONDARY_COLOR));
    rightLayout->addWidget(m_importPreview);
    
    QFrame *capabilitiesFrame = new QFrame();
    capabilitiesFrame->setStyleSheet(QString(
        "QFrame { background: %0; border: 1px solid %1; border-radius: 8px; padding: 10px; }").arg(BG_LIGHT, BORDER_COLOR));
    QVBoxLayout *capLayout = new QVBoxLayout(capabilitiesFrame);
    capLayout->setContentsMargins(8, 8, 8, 8);
    capLayout->setSpacing(4);
    
    QLabel *capTitle = new QLabel("💡 Capacités");
    capTitle->setStyleSheet(QString("font-weight: 700; color: %0; font-size: 11px;").arg(PRIMARY_COLOR));
    capLayout->addWidget(capTitle);
    
    QLabel *capList = new QLabel("• Répondre aux questions\n• Importer des fichiers\n• Opérations rapides\n• Analyse de données");
    capList->setWordWrap(true);
    capList->setStyleSheet(QString("color: %0; font-size: 10px; line-height: 1.5;").arg(TEXT_SECONDARY));
    capLayout->addWidget(capList);
    
    rightLayout->addWidget(capabilitiesFrame);
    rightLayout->addStretch();
    
    contentLayout->addWidget(rightPanel);
    
    mainLayout->addLayout(contentLayout, 1);
    
    // Professional Welcome Message
    m_chatHistory->setText(
        "<div style='color: #0f2b4c; font-weight: bold; font-size: 15px; margin-bottom: 12px;'>"
        "🤖 Bienvenue dans Labib AI Assistant!</div>"
        "<div style='color: #64748b; font-size: 12px; line-height: 1.6;'>"
        "<b>Je suis votre assistant intelligent pour WasteGuard.</b><br><br>"
        "Je peux vous aider avec:<br>"
        "✓ <b>Questions</b> — Posez n'importe quelle question sur le système<br>"
        "✓ <b>Importation</b> — Importer des fichiers CSV ou Excel<br>"
        "✓ <b>Opérations groupées</b> — Ajouter/modifier des données en masse<br>"
        "✓ <b>Analyse</b> — Analyser et résumer les données<br><br>"
        "<i>Commencez par sélectionner un module et posez votre question...</i></div>");
}

void LabibAssistant::setupConnections()
{
    connect(m_sendButton, &QPushButton::clicked, this, &LabibAssistant::onSendMessage);
    connect(m_userInput, &QLineEdit::returnPressed, this, &LabibAssistant::onSendMessage);
    connect(m_importButton, &QPushButton::clicked, this, &LabibAssistant::onImportFile);
    connect(m_processButton, &QPushButton::clicked, this, &LabibAssistant::onProcessImportedData);
    connect(m_moduleSelector, &QListWidget::itemSelectionChanged, this, [this]() {
        m_selectedModule = m_moduleSelector->currentItem()->text();
    });
}

void LabibAssistant::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    m_userInput->setFocus();
}

void LabibAssistant::onSendMessage()
{
    QString userMessage = m_userInput->text().trimmed();
    if (userMessage.isEmpty()) return;
    
    // Display user message
    m_chatHistory->append(
        QString("<div style='color: #1e40af; font-weight: 700; margin: 10px 0px;'>"
                "👤 Vous: %1</div>").arg(userMessage));
    
    m_userInput->clear();
    
    // Generate AI response
    handleAiResponse(userMessage);
}

void LabibAssistant::handleAiResponse(const QString &userMessage)
{
    QString response = generateAiResponse(userMessage);
    displayAiAnswer(response);
}

QString LabibAssistant::generateAiResponse(const QString &userMessage)
{
    QString lowerMsg = userMessage.toLower();
    
    // Questions about the system
    if (lowerMsg.contains("smart waste") || lowerMsg.contains("système") || lowerMsg.contains("project")) {
        return "WasteGuard est un système intelligent de gestion des déchets utilisant l'IA et l'IoT. "
               "Il peut:<br>• Collecter les déchets automatiquement<br>• Trier les matériaux<br>"
               "• Optimiser les routes de collecte<br>• Fournir des rapports d'analyse<br><br>"
               "Quel module voulez-vous explorer?";
    }
    
    // Questions about modules
    if (lowerMsg.contains("client") || lowerMsg.contains("customer")) {
        return "Module Clients: Gérez vos clients, contrats et paiements. "
               "Vous pouvez ajouter, modifier ou analyser les données clients. "
               "Le système suit automatiquement l'expiration des contrats.";
    }
    
    if (lowerMsg.contains("employe") || lowerMsg.contains("employee")) {
        return "Module Employés: Gérez le personnel, les salaires et les tâches. "
               "Incluez les absences, les congés et l'analyse de performance.";
    }
    
    if (lowerMsg.contains("produit") || lowerMsg.contains("product")) {
        return "Module Produits: Gestion du stock et des références produit. "
               "Suivez les niveaux de stock, les alertes et les prix.";
    }
    
    if (lowerMsg.contains("maintenance")) {
        return "Module Maintenance: Suivi des interventions sur équipements. "
               "Gestion des coûts, durées et priorités d'intervention.";
    }
    
    if (lowerMsg.contains("commande") || lowerMsg.contains("order")) {
        return "Module Commandes: Gestion des commandes clients. "
               "Suivi des références, quantités et dates de livraison.";
    }
    
    if (lowerMsg.contains("stock")) {
        return "Module Stock: Gestion du stock intelligent. "
               "Alertes automatiques pour les ruptures et niveaux bas.";
    }
    
    // Help commands
    if (lowerMsg.contains("aide") || lowerMsg.contains("help")) {
        return "Je peux vous aider avec:<br>"
               "• <b>Questions</b> sur le système/modules<br>"
               "• <b>Importer</b> des fichiers CSV/Excel<br>"
               "• <b>Ajouter/Modifier</b> des records en masse<br>"
               "• <b>Analyser</b> les données<br><br>"
               "Essayez: 'Importe un fichier clients' ou 'Explique le moduleStock'";
    }
    
    // Default response
    return "Je comprends votre question. Pour '" + userMessage + "', "
           "pouvez-vous préciser quel module vous concerne ou si vous avez besoin d'importer des données?";
}

void LabibAssistant::displayAiAnswer(const QString &answer)
{
    m_chatHistory->append(
        QString("<div style='background: #f0f9ff; color: #0f2b4c; padding: 12px; border-radius: 8px; margin: 10px 0px;'>"
                "<b>🤖 Labib:</b> %1</div>").arg(answer));
}

void LabibAssistant::onImportFile()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        "Importer Fichier", "",
        "CSV Files (*.csv);;Excel Files (*.xlsx);;JSON Files (*.json);;All Files (*)");
    
    if (filePath.isEmpty()) return;
    
    m_importedFilePath = filePath;
    m_importedRecords = parseImportedFile(filePath);
    
    if (m_importedRecords.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucune donnée trouvée dans le fichier.");
        return;
    }
    
    m_processButton->setEnabled(true);
    displayAiAnswer(QString("📁 Fichier importé avec succès! %1 enregistrements trouvés. "
                           "Cliquez sur 'Traiter' pour continuer.").arg(m_importedRecords.size()));
    
    showImportPreview(m_importedRecords);
}

QList<QJsonObject> LabibAssistant::parseImportedFile(const QString &filePath)
{
    QList<QJsonObject> records;
    
    if (filePath.endsWith(".csv")) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return records;
        }
        
        QTextStream stream(&file);
        QStringList headers = stream.readLine().split(",");
        
        while (!stream.atEnd()) {
            QStringList values = stream.readLine().split(",");
            if (values.size() == headers.size()) {
                QJsonObject obj;
                for (int i = 0; i < headers.size(); ++i) {
                    obj[headers[i].trimmed()] = values[i].trimmed();
                }
                records.append(obj);
            }
        }
        file.close();
    } 
    else if (filePath.endsWith(".json")) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            return records;
        }
        
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        
        if (doc.isArray()) {
            for (const QJsonValue &val : doc.array()) {
                if (val.isObject()) {
                    records.append(val.toObject());
                }
            }
        }
    }
    
    return records;
}

void LabibAssistant::showImportPreview(const QList<QJsonObject> &records)
{
    m_importPreview->setRowCount(0);
    
    if (records.isEmpty()) return;
    
    const QJsonObject &firstRecord = records.first();
    int row = 0;
    
    for (auto it = firstRecord.begin(); it != firstRecord.end(); ++it) {
        m_importPreview->insertRow(row);
        m_importPreview->setItem(row, 0, new QTableWidgetItem(it.key()));
        m_importPreview->setItem(row, 1, new QTableWidgetItem(it.value().toString()));
        row++;
    }
}

void LabibAssistant::onProcessImportedData()
{
    if (m_importedRecords.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucune donnée à traiter.");
        return;
    }
    
    processBulkImport(m_importedRecords);
    m_processButton->setEnabled(false);
}

void LabibAssistant::processBulkImport(const QList<QJsonObject> &records)
{
    int successCount = 0;
    int errorCount = 0;
    
    for (const QJsonObject &record : records) {
        if (validateRecord(record, m_selectedModule)) {
            if (addRecordToDatabase(record, m_selectedModule)) {
                successCount++;
            } else {
                errorCount++;
            }
        } else {
            errorCount++;
        }
    }
    
    QString result = QString("✓ Traitement terminé!<br>"
                            "Succès: %1<br>"
                            "Erreurs: %2<br><br>"
                            "Les données ont été importées dans le module '%3'.")
                    .arg(successCount).arg(errorCount).arg(m_selectedModule);
    
    displayAiAnswer(result);
    m_importedRecords.clear();
}

bool LabibAssistant::validateRecord(const QJsonObject &record, const QString &module)
{
    if (module == "Client") {
        return record.contains("NOM") && record.contains("MATRICULE");
    } else if (module == "Employe") {
        return record.contains("NOM") && record.contains("EMAIL");
    } else if (module == "Produit") {
        return record.contains("MODEL") && record.contains("TYPE");
    }
    
    return !record.isEmpty();
}

bool LabibAssistant::addRecordToDatabase(const QJsonObject &record, const QString &module)
{
    QSqlQuery query;
    
    if (module == "Client") {
        query.prepare("INSERT INTO CLIENT (ID_CLIENT, NOM, MATRICULE, EMAIL, TYPE_CONTRAT, DATE_EXPIRATION_CONTRAT, STATUT_PAIEMENT) "
                     "VALUES (seq_client.nextval, :nom, :matricule, :email, :type_contrat, TO_DATE(:date_exp, 'YYYY-MM-DD'), :statut)");
        query.bindValue(":nom", record["NOM"].toString());
        query.bindValue(":matricule", record["MATRICULE"].toString());
        query.bindValue(":email", record["EMAIL"].toString());
        query.bindValue(":type_contrat", record["TYPE_CONTRAT"].toString("Mensuel"));
        query.bindValue(":date_exp", record["DATE_EXPIRATION_CONTRAT"].toString("2026-12-31"));
        query.bindValue(":statut", record["STATUT_PAIEMENT"].toString("Payé"));
    } 
    else if (module == "Employe") {
        query.prepare("INSERT INTO EMPLOYE (ID_EMPLOYE, NOM, EMAIL, CIN, SPECIALITE, SALAIRE, STATUT) "
                     "VALUES (seq_employe.nextval, :nom, :email, :cin, :specialite, :salaire, :statut)");
        query.bindValue(":nom", record["NOM"].toString());
        query.bindValue(":email", record["EMAIL"].toString());
        query.bindValue(":cin", record["CIN"].toString());
        query.bindValue(":specialite", record["SPECIALITE"].toString("General"));
        query.bindValue(":salaire", record["SALAIRE"].toInt(2000));
        query.bindValue(":statut", record["STATUT"].toString("Disponible"));
    }
    else if (module == "Produit") {
        query.prepare("INSERT INTO PRODUITS (ID_PRODUIT, MODEL, TYPE, CAPACITE, PRIX_UNITAIRE, QUANTITE, ALLEE) "
                     "VALUES (seq_produit.nextval, :model, :type, :capacite, :prix, :quantite, :allee)");
        query.bindValue(":model", record["MODEL"].toString());
        query.bindValue(":type", record["TYPE"].toString());
        query.bindValue(":capacite", record["CAPACITE"].toString("100"));
        query.bindValue(":prix", record["PRIX_UNITAIRE"].toDouble(0.0));
        query.bindValue(":quantite", record["QUANTITE"].toInt(0));
        query.bindValue(":allee", record["ALLEE"].toString("A"));
    }
    
    if (!query.exec()) {
        qWarning() << "Database insert error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool LabibAssistant::modifyRecordInDatabase(const QJsonObject &record, const QString &module)
{
    (void)record;
    (void)module;
    // Implementation for modify - similar to add but with UPDATE
    return true;
}

QString LabibAssistant::getModuleDescription(const QString &module)
{
    if (module == "Client") {
        return "Gestion des clients et contrats";
    } else if (module == "Employe") {
        return "Gestion des employés";
    } else if (module == "Produit") {
        return "Catalogue de produits";
    } else if (module == "Maintenance") {
        return "Suivi des interventions";
    } else if (module == "Commande") {
        return "Gestion des commandes";
    } else if (module == "Stock") {
        return "Gestion du stock";
    }
    return "";
}
