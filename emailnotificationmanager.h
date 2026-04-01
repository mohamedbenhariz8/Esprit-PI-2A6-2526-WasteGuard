#ifndef EMAILNOTIFICATIONMANAGER_H
#define EMAILNOTIFICATIONMANAGER_H

#include <QString>
#include <QStringList>
#include <QMap>

class EmailNotificationManager
{
public:
    EmailNotificationManager();
    
    // Configuration methods
    void setSmtpServer(const QString &server);
    void setSmtpPort(int port);
    void setSenderEmail(const QString &email);
    void setSenderPassword(const QString &password);
    void setUseSSL(bool useSSL);
    void setUseTLS(bool useTLS);
    
    // Notification methods
    bool sendContractExpirationNotice(const QString &clientEmail, 
                                      const QString &clientName,
                                      const QString &contractType,
                                      const QString &expirationDate);
    
    bool sendClientConfirmation(const QString &clientEmail,
                               const QString &clientName,
                               const QString &contractType);
    
    bool sendBillingNotification(const QString &clientEmail,
                                const QString &clientName,
                                const QString &amount,
                                const QString &dueDate);
    
    // Generic email sending method
    bool sendEmail(const QString &to, const QString &subject, const QString &htmlBody);
    
    // Test connection
    bool testConnection();
    
private:
    // SMTP Configuration
    QString m_smtpServer;
    int m_smtpPort;
    QString m_senderEmail;
    QString m_senderPassword;
    bool m_useSSL;
    bool m_useTLS;
    
    // Helper methods
    QString generateContractExpirationHtml(const QString &clientName,
                                          const QString &contractType,
                                          const QString &expirationDate);
    QString generateClientConfirmationHtml(const QString &clientName,
                                          const QString &contractType);
    QString generateBillingNotificationHtml(const QString &clientName,
                                           const QString &amount,
                                           const QString &dueDate);
    
    // Internal structures for configuration
    struct EmailConfig {
        QString smtpServer;
        int smtpPort;
        QString senderEmail;
        QString senderPassword;
        bool useSSL;
        bool useTLS;
    } m_config;
};

#endif // EMAILNOTIFICATIONMANAGER_H
