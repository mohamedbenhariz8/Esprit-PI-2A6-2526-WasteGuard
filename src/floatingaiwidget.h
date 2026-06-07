#ifndef FLOATINGAIWIDGET_H
#define FLOATINGAIWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

class FloatingAIWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingAIWidget(QWidget *parent = nullptr);
    ~FloatingAIWidget();

protected:
    void moveEvent(QMoveEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onAIButtonClicked();
    void onSendMessage();
    void onNetworkReply();
    void onDialogClosed();

private:
    // UI Components
    QPushButton *m_aiButton;
    QDialog *m_chatDialog;
    QTextEdit *m_chatHistory;
    QLineEdit *m_userInput;
    QPushButton *m_sendButton;
    
    // Network
    QNetworkAccessManager *m_networkManager;
    
    // Data
    QString m_huggingFaceApiKey;
    static constexpr const char* HF_API_URL = "https://api-inference.huggingface.co/models/mistral-community/Mistral-7B-Instruct-v0.1";
    
    // Helper methods
    void setupUI();
    void setupConnections();
    void createChatDialog();
    void updateButtonPosition();
    void queryHuggingFaceAPI(const QString &userMessage);
    QString formatContextForWasteManagement(const QString &userQuestion);
    void addMessageToChat(const QString &sender, const QString &message);
};

#endif // FLOATINGAIWIDGET_H
