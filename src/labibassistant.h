#ifndef LABIBASSISTANT_H
#define LABIBASSISTANT_H

#include <QWidget>
#include <QEvent>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QListWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlQueryModel>
#include <QTimer>

class LabibMediaCircle;
class QFrame;

/**
 * Labib AI Assistant Dialog
 * Universal AI assistant for WasteGuard Smart Waste Collection System
 * Features:
 * - Answer project-related questions
 * - Import files to add/modify records across all modules
 * - Bulk operations on Clients, Employees, Products, Maintenance, Orders
 * - Accessible from anywhere in the application
 */
class LabibAssistant : public QWidget
{
    Q_OBJECT

public:
    explicit LabibAssistant(QWidget *parent = nullptr);
    ~LabibAssistant();

signals:
    void requestAddClient();
    void requestViewStock();
    void requestCheckOrders();

protected:
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onSendMessage();
    void onImportFile();
    void onProcessImportedData();
    void handleAiResponse(const QString &userMessage);
    void displayAiAnswer(const QString &answer);
    void processBulkImport(const QList<QJsonObject> &records);
    void showImportPreview(const QList<QJsonObject> &records);

private:
    // UI Components
    QTextEdit *m_chatHistory;
    QTextEdit *m_userInput;
    QPushButton *m_sendButton;
    QPushButton *m_importButton;
    QPushButton *m_processButton;
    QTableWidget *m_importPreview;
    QListWidget *m_moduleSelector;
    
    // Data
    QString m_selectedModule;
    QList<QJsonObject> m_importedRecords;
    QString m_importedFilePath;
    
    // Helper methods
    QString generateAiResponse(const QString &userMessage);
    bool validateRecord(const QJsonObject &record, const QString &module);
    bool addRecordToDatabase(const QJsonObject &record, const QString &module);
    bool modifyRecordInDatabase(const QJsonObject &record, const QString &module);
    QList<QJsonObject> parseImportedFile(const QString &filePath);
    QString getModuleDescription(const QString &module);
    void appendChatBubble(const QString &message, bool fromUser, bool allowHtml = false);
    void scrollChatToBottom();
    void refreshSmartStats();
    void startTypingIndicator();
    void stopTypingIndicator();
    void setupUI();
    void setupConnections();

    QLabel *m_statsLabel = nullptr;
    QLabel *m_typingLabel = nullptr;
    QTimer *m_typingTimer = nullptr;
    int m_typingStep = 0;
    QString m_lastAssistantHtml;

    // Clustering Messenger : -1 = aucun, 0 = Labib, 1 = utilisateur.
    int m_lastBubbleFromUser = -1;

    LabibMediaCircle *m_avatarCircle = nullptr;
    QStringList m_idleVideoCandidates;        // legacy alias = waiting
    QStringList m_typingVideoCandidates;      // legacy alias = speaking
    QStringList m_waitingVideoCandidates;     // download.mp4  : Labib attend
    QStringList m_listeningVideoCandidates;   // download2.mp4 : Labib ecoute
    QStringList m_speakingVideoCandidates;    // download3.mp4 : Labib parle
    void loadAvatarVideo(const QStringList &candidates);
    QStringList buildVideoCandidates(const QString &fileName) const;

    enum class AvatarState { Waiting, Listening, Speaking };
    AvatarState m_avatarState = AvatarState::Waiting;
    QLabel  *m_avatarStatusLabel = nullptr;
    QTimer  *m_idleRevertTimer   = nullptr;
    void setAvatarState(AvatarState s);

    QFrame            *m_typingBubble = nullptr;
    LabibMediaCircle  *m_typingAvatar = nullptr;
    bool               m_typingAvatarLoaded = false;

    // Synchronisation : on attend que download3.mp4 ait fini de jouer
    // avant d'afficher la reponse (evite la coupure brutale du video).
    QString  m_pendingResponse;
    bool     m_responseReady = false;
    bool     m_typingVideoFinished = true;
    void     onTypingVideoFinished();
    void     deliverPendingResponseIfReady();
};

#endif // LABIBASSISTANT_H
