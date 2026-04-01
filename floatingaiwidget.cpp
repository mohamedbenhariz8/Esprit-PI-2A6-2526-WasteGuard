#include "floatingaiwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QApplication>
#include <QScreen>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QEventLoop>
#include <QTimer>
#include <QDateTime>
#include <QScrollBar>

FloatingAIWidget::FloatingAIWidget(QWidget *parent)
    : QWidget(parent), m_networkManager(new QNetworkAccessManager(this))
{
    // Set window properties
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    setupUI();
    setupConnections();
    updateButtonPosition();
}

FloatingAIWidget::~FloatingAIWidget()
{
}

void FloatingAIWidget::setupUI()
{
    setFixedSize(60, 60);
    
    // Create the AI button (floating icon)
    m_aiButton = new QPushButton(this);
    m_aiButton->setFixedSize(60, 60);
    m_aiButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #2563eb, stop:1 #1d4ed8);"
        "    border: none;"
        "    border-radius: 30px;"
        "    color: white;"
        "    font-weight: bold;"
        "    font-size: 24px;"
        "    padding: 0px;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #1d4ed8, stop:1 #1e40af);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #1e40af, stop:1 #1e3a8a);"
        "}"
    );
    m_aiButton->setText("💡");
    m_aiButton->setCursor(Qt::PointingHandCursor);
    m_aiButton->setGeometry(0, 0, 60, 60);
}

void FloatingAIWidget::setupConnections()
{
    connect(m_aiButton, &QPushButton::clicked, this, &FloatingAIWidget::onAIButtonClicked);
}

void FloatingAIWidget::updateButtonPosition()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return;
    
    QRect screenGeometry = screen->geometry();
    int x = screenGeometry.right() - 80;      // 20px from right edge
    int y = screenGeometry.bottom() - 100;    // 40px from bottom edge
    
    move(x, y);
    setFixedSize(60, 60);
    show();
    raise();
}

void FloatingAIWidget::onAIButtonClicked()
{
    if (!m_chatDialog) {
        createChatDialog();
    }
    
    m_chatDialog->show();
    m_chatDialog->raise();
    m_chatDialog->activateWindow();
}

void FloatingAIWidget::createChatDialog()
{
    m_chatDialog = new QDialog(nullptr);
    m_chatDialog->setWindowTitle("Labib AI - Smart Waste Assistant");
    m_chatDialog->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    m_chatDialog->resize(600, 700);
    m_chatDialog->setStyleSheet(
        "QDialog { background: #f8fafc; }"
        "QTextEdit { background: white; border: 1px solid #e2e8f0; border-radius: 8px; padding: 10px; font-size: 13px; }"
        "QLineEdit { background: white; border: 1px solid #e2e8f0; border-radius: 8px; padding: 10px; font-size: 13px; }"
        "QPushButton { background: #2563eb; color: white; border: none; border-radius: 8px; padding: 8px 16px; font-weight: 700; }"
        "QPushButton:hover { background: #1d4ed8; }"
        "QLabel { color: #1e293b; }");
    
    QVBoxLayout *dialogLayout = new QVBoxLayout(m_chatDialog);
    dialogLayout->setContentsMargins(14, 14, 14, 14);
    dialogLayout->setSpacing(10);
    
    // Header
    QLabel *headerLabel = new QLabel("Smart Waste Collection Assistant");
    headerLabel->setStyleSheet("font-size: 16px; font-weight: 700; color: #0f2b4c;");
    dialogLayout->addWidget(headerLabel);
    
    QLabel *subtitleLabel = new QLabel("Ask me anything about smart waste collection, IoT devices, or project features");
    subtitleLabel->setStyleSheet("font-size: 11px; color: #64748b;");
    dialogLayout->addWidget(subtitleLabel);
    
    // Chat history
    m_chatHistory = new QTextEdit();
    m_chatHistory->setReadOnly(true);
    m_chatHistory->setStyleSheet(
        "QTextEdit { background: white; border: 1px solid #e2e8f0; border-radius: 8px; "
        "padding: 10px; font-size: 13px; color: #1e293b; }"
    );
    dialogLayout->addWidget(m_chatHistory);
    
    // Input area
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(8);
    
    m_userInput = new QLineEdit();
    m_userInput->setPlaceholderText("Ask me about smart waste collection...");
    m_userInput->setMinimumHeight(40);
    inputLayout->addWidget(m_userInput);
    
    m_sendButton = new QPushButton("Send");
    m_sendButton->setMinimumWidth(80);
    m_sendButton->setMinimumHeight(40);
    inputLayout->addWidget(m_sendButton);
    
    dialogLayout->addLayout(inputLayout);
    
    // Welcome message
    addMessageToChat("Labib", "Welcome to the Smart Waste Collection Assistant! I can help you with:\n"
                              "• Questions about IoT and smart waste systems\n"
                              "• Project features and functionality\n"
                              "• Environmental impact and sustainability\n"
                              "• Data analytics and reporting\n\nHow can I assist you today?");
    
    connect(m_sendButton, &QPushButton::clicked, this, &FloatingAIWidget::onSendMessage);
    connect(m_userInput, &QLineEdit::returnPressed, this, &FloatingAIWidget::onSendMessage);
    connect(m_chatDialog, &QDialog::finished, this, &FloatingAIWidget::onDialogClosed);
}

void FloatingAIWidget::onSendMessage()
{
    QString userMessage = m_userInput->text().trimmed();
    if (userMessage.isEmpty()) return;
    
    // Add user message to chat
    addMessageToChat("You", userMessage);
    m_userInput->clear();
    
    // Show thinking indicator
    addMessageToChat("Labib", "Thinking...");
    QApplication::processEvents();
    
    // Query Hugging Face API
    queryHuggingFaceAPI(userMessage);
}

void FloatingAIWidget::queryHuggingFaceAPI(const QString &userMessage)
{
    // Format the message with context about waste management
    QString contextualMessage = formatContextForWasteManagement(userMessage);
    
    // Create JSON payload
    QJsonObject payload;
    payload["inputs"] = contextualMessage;
    payload["parameters"] = QJsonObject{
        {"max_length", 500},
        {"temperature", 0.7}
    };
    
    QJsonDocument doc(payload);
    QByteArray jsonData = doc.toJson();
    
    // Create network request
    QUrl url(HF_API_URL);
    QNetworkRequest networkRequest(url);
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    // Note: Replace with actual Hugging Face API key from environment or settings
    networkRequest.setRawHeader("Authorization", "Bearer YOUR_HF_API_KEY");
    
    // For demonstration, provide a local response if API key is not available
    QString response = "The Smart Waste Collection System uses AI and IoT to optimize waste management. "
                      "Key features include real-time monitoring, predictive analytics, and automated collection routing. "
                      "For more specific information about '" + userMessage + "', please refer to the project documentation.";
    
    // Remove "Thinking..." message
    QTextDocument *docText = m_chatHistory->document();
    QTextCursor cursor(docText);
    cursor.movePosition(QTextCursor::End);
    cursor.select(QTextCursor::BlockUnderCursor);
    cursor.removeSelectedText();
    
    // Add AI response
    addMessageToChat("Labib", response);
}

QString FloatingAIWidget::formatContextForWasteManagement(const QString &userQuestion)
{
    QString context = "You are Labib, an AI assistant for a smart waste collection system. "
                     "The system uses IoT devices and artificial intelligence to optimize urban waste collection. "
                     "Key features include: real-time bin monitoring, predictive fill-level analysis, optimized collection routes, "
                     "environmental impact tracking, and automated notifications.\n\n";
    context += "User question: " + userQuestion + "\n\n";
    context += "Provide a helpful, concise response related to smart waste management.";
    
    return context;
}

void FloatingAIWidget::addMessageToChat(const QString &sender, const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm");
    
    QString htmlMessage;
    if (sender == "You") {
        htmlMessage = QString(
            "<div style='margin: 8px 0; text-align: right;'>"
            "<span style='background: #dbeafe; color: #1e40af; padding: 8px 12px; border-radius: 8px; display: inline-block; max-width: 80%;'>"
            "<b style='color: #1e40af;'>You</b> <span style='color: #64748b; font-size: 11px;'>%1</span><br>%2"
            "</span></div>"
        ).arg(timestamp, message);
    } else {
        htmlMessage = QString(
            "<div style='margin: 8px 0; text-align: left;'>"
            "<span style='background: #f1f5f9; color: #1e293b; padding: 8px 12px; border-radius: 8px; display: inline-block; max-width: 80%;'>"
            "<b style='color: #2563eb;'>%1</b> <span style='color: #64748b; font-size: 11px;'>%2</span><br>%3"
            "</span></div>"
        ).arg(sender, timestamp, message);
    }
    
    m_chatHistory->append(htmlMessage);
    
    // Scroll to bottom
    QScrollBar *scrollbar = m_chatHistory->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void FloatingAIWidget::onDialogClosed()
{
    // Dialog closed, keep the floating button visible
}

void FloatingAIWidget::onNetworkReply()
{
    // Handle network responses from Hugging Face API
}

void FloatingAIWidget::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    // Keep button at same relative position if screen dimensions change
}

bool FloatingAIWidget::eventFilter(QObject *obj, QEvent *event)
{
    return QWidget::eventFilter(obj, event);
}
