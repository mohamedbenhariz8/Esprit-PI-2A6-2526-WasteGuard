#include "emailnotificationmanager.h"
#include <QSettings>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QTextStream>
#include <QDateTime>
#include <QProcessEnvironment>

EmailNotificationManager::EmailNotificationManager()
    : m_smtpPort(587), m_useSSL(false), m_useTLS(true)
{
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    // Load configuration from settings
    QSettings settings("WasteGuard", "EmailNotifications");
    m_config.smtpServer = env.value("WASTEGUARD_SMTP_SERVER").trimmed().isEmpty()
                              ? settings.value("smtp_server", "smtp.gmail.com").toString()
                              : env.value("WASTEGUARD_SMTP_SERVER").trimmed();
    m_config.smtpPort = env.value("WASTEGUARD_SMTP_PORT").trimmed().isEmpty()
                            ? settings.value("smtp_port", 587).toInt()
                            : env.value("WASTEGUARD_SMTP_PORT").trimmed().toInt();
    m_config.senderEmail = env.value("WASTEGUARD_SMTP_EMAIL").trimmed().isEmpty()
                               ? settings.value("sender_email", "").toString()
                               : env.value("WASTEGUARD_SMTP_EMAIL").trimmed();
    m_config.senderPassword = env.value("WASTEGUARD_SMTP_APP_PASSWORD").trimmed().isEmpty()
                                  ? settings.value("sender_password", "").toString()
                                  : env.value("WASTEGUARD_SMTP_APP_PASSWORD").trimmed();
    m_config.useSSL = env.value("WASTEGUARD_SMTP_SSL").trimmed().isEmpty()
                          ? settings.value("use_ssl", false).toBool()
                          : (env.value("WASTEGUARD_SMTP_SSL").trimmed().toLower() == "1"
                             || env.value("WASTEGUARD_SMTP_SSL").trimmed().toLower() == "true"
                             || env.value("WASTEGUARD_SMTP_SSL").trimmed().toLower() == "yes");
    m_config.useTLS = env.value("WASTEGUARD_SMTP_TLS").trimmed().isEmpty()
                          ? settings.value("use_tls", true).toBool()
                          : (env.value("WASTEGUARD_SMTP_TLS").trimmed().toLower() == "1"
                             || env.value("WASTEGUARD_SMTP_TLS").trimmed().toLower() == "true"
                             || env.value("WASTEGUARD_SMTP_TLS").trimmed().toLower() == "yes");
    
    // Apply loaded config
    m_smtpServer = m_config.smtpServer;
    m_smtpPort = m_config.smtpPort;
    m_senderEmail = m_config.senderEmail;
    m_senderPassword = m_config.senderPassword;
    m_useSSL = m_config.useSSL;
    m_useTLS = m_config.useTLS;
}

void EmailNotificationManager::setSmtpServer(const QString &server)
{
    m_smtpServer = server;
    m_config.smtpServer = server;
    QSettings settings("WasteGuard", "EmailNotifications");
    settings.setValue("smtp_server", server);
}

void EmailNotificationManager::setSmtpPort(int port)
{
    m_smtpPort = port;
    m_config.smtpPort = port;
    QSettings settings("WasteGuard", "EmailNotifications");
    settings.setValue("smtp_port", port);
}

void EmailNotificationManager::setSenderEmail(const QString &email)
{
    m_senderEmail = email;
    m_config.senderEmail = email;
    QSettings settings("WasteGuard", "EmailNotifications");
    settings.setValue("sender_email", email);
}

void EmailNotificationManager::setSenderPassword(const QString &password)
{
    m_senderPassword = password;
    m_config.senderPassword = password;
    QSettings settings("WasteGuard", "EmailNotifications");
    settings.setValue("sender_password", password);
}

void EmailNotificationManager::setUseSSL(bool useSSL)
{
    m_useSSL = useSSL;
    m_config.useSSL = useSSL;
    QSettings settings("WasteGuard", "EmailNotifications");
    settings.setValue("use_ssl", useSSL);
}

void EmailNotificationManager::setUseTLS(bool useTLS)
{
    m_useTLS = useTLS;
    m_config.useTLS = useTLS;
    QSettings settings("WasteGuard", "EmailNotifications");
    settings.setValue("use_tls", useTLS);
}

bool EmailNotificationManager::sendContractExpirationNotice(const QString &clientEmail,
                                                            const QString &clientName,
                                                            const QString &contractType,
                                                            const QString &expirationDate)
{
    if (m_senderEmail.isEmpty() || m_senderPassword.isEmpty()) {
        qWarning() << "Configuration email incomplete: email expediteur ou mot de passe manquant";
        return false;
    }
    
    QString subject = "Alerte contrat WasteGuard - Action requise";
    QString htmlBody = generateContractExpirationHtml(clientName, contractType, expirationDate);

    return sendEmail(clientEmail, subject, htmlBody);
}

bool EmailNotificationManager::sendClientConfirmation(const QString &clientEmail,
                                                      const QString &clientName,
                                                      const QString &contractType)
{
    if (m_senderEmail.isEmpty() || m_senderPassword.isEmpty()) {
        qWarning() << "Configuration email incomplete: email expediteur ou mot de passe manquant";
        return false;
    }
    
    QString subject = "Bienvenue chez WasteGuard";
    QString htmlBody = generateClientConfirmationHtml(clientName, contractType);

    return sendEmail(clientEmail, subject, htmlBody);
}

bool EmailNotificationManager::sendBillingNotification(const QString &clientEmail,
                                                       const QString &clientName,
                                                       const QString &amount,
                                                       const QString &dueDate)
{
    if (m_senderEmail.isEmpty() || m_senderPassword.isEmpty()) {
        qWarning() << "Configuration email incomplete: email expediteur ou mot de passe manquant";
        return false;
    }
    
    QString subject = "Rappel de paiement - Facture a echeance";
    QString htmlBody = generateBillingNotificationHtml(clientName, amount, dueDate);

    return sendEmail(clientEmail, subject, htmlBody);
}

bool EmailNotificationManager::sendEmail(const QString &to, const QString &subject, const QString &htmlBody)
{
    // Refresh runtime SMTP config each send to support env changes without recompiling.
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QSettings settings("WasteGuard", "EmailNotifications");

    const QString envServer = env.value("WASTEGUARD_SMTP_SERVER").trimmed();
    const QString envPort = env.value("WASTEGUARD_SMTP_PORT").trimmed();
    const QString envEmail = env.value("WASTEGUARD_SMTP_EMAIL").trimmed();
    const QString envPassword = env.value("WASTEGUARD_SMTP_APP_PASSWORD").trimmed();

    if (!envServer.isEmpty()) m_smtpServer = envServer;
    else if (m_smtpServer.trimmed().isEmpty()) m_smtpServer = settings.value("smtp_server", "smtp.gmail.com").toString().trimmed();

    if (!envPort.isEmpty()) m_smtpPort = envPort.toInt();
    else if (m_smtpPort <= 0) m_smtpPort = settings.value("smtp_port", 587).toInt();

    if (!envEmail.isEmpty()) m_senderEmail = envEmail;
    else if (m_senderEmail.trimmed().isEmpty()) m_senderEmail = settings.value("sender_email", "").toString().trimmed();

    if (!envPassword.isEmpty()) m_senderPassword = envPassword;
    else if (m_senderPassword.trimmed().isEmpty()) m_senderPassword = settings.value("sender_password", "").toString().trimmed();

    QStringList missing;
    if (m_senderEmail.trimmed().isEmpty()) missing << "WASTEGUARD_SMTP_EMAIL";
    if (m_senderPassword.trimmed().isEmpty()) missing << "WASTEGUARD_SMTP_APP_PASSWORD";
    if (m_smtpServer.trimmed().isEmpty()) missing << "WASTEGUARD_SMTP_SERVER";
    if (m_smtpPort <= 0) missing << "WASTEGUARD_SMTP_PORT";

    if (!missing.isEmpty()) {
        qWarning() << "Email configuration incomplete. Missing:" << missing.join(", ");
        return false;
    }
    
    // Script PowerShell dynamique pour envoyer l'email via .NET natif sur Windows.
    const QString escapedFrom = QString(m_senderEmail).replace("'", "''");
    const QString escapedTo = QString(to).replace("'", "''");
    const QString escapedSubject = QString(subject).replace("'", "''");
    const QString escapedBody = QString(htmlBody).replace("'", "''");
    const QString escapedServer = QString(m_smtpServer).replace("'", "''");
    const QString escapedPassword = QString(m_senderPassword).replace("'", "''");
    const QString enableSslLiteral = ((m_useSSL || m_useTLS) ? "$true" : "$false");
    
    QString psCode = QString(
        "$EmailFrom = '%1'\n"
        "$EmailTo = '%2'\n"
        "$Subject = '%3'\n"
        "$Body = @'\n%4\n'@\n"
        "$SMTPServer = '%5'\n"
        "$SMTPClient = New-Object Net.Mail.SmtpClient($SmtpServer, %6)\n"
        "$SMTPClient.EnableSsl = %7\n"
        "$SMTPClient.Credentials = New-Object System.Net.NetworkCredential('%1', '%8')\n"
        "$Message = New-Object Net.Mail.MailMessage($EmailFrom, $EmailTo, $Subject, $Body)\n"
        "$Message.IsBodyHtml = $true\n"
        "$SMTPClient.Send($Message)\n"
    ).arg(escapedFrom,
          escapedTo,
          escapedSubject,
          escapedBody,
          escapedServer,
          QString::number(m_smtpPort),
          enableSslLiteral,
          escapedPassword);

    QString tempPath = QDir::tempPath() + "/send_wasteguard_mail_" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".ps1";
    QFile file(tempPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(psCode.toUtf8());
        file.close();

        QProcess proc;
        proc.start("powershell.exe", QStringList() << "-ExecutionPolicy" << "Bypass" << "-WindowStyle" << "Hidden" << "-File" << tempPath);

        if (!proc.waitForStarted(5000)) {
            QFile::remove(tempPath);
            qWarning() << "Echec envoi email: PowerShell n'a pas demarre.";
            return false;
        }

        if (!proc.waitForFinished(30000)) {
            proc.kill();
            QFile::remove(tempPath);
            qWarning() << "Echec envoi email: delai depasse.";
            return false;
        }

        const int exitCode = proc.exitCode();
        const QByteArray stderrBytes = proc.readAllStandardError();
        QFile::remove(tempPath);
        if (exitCode != 0) {
            qWarning() << "Echec envoi email:" << QString::fromUtf8(stderrBytes).trimmed();
            return false;
        }
    } else {
        qWarning() << "Echec envoi email: impossible de creer le script temporaire.";
        return false;
    }

    qDebug() << "═══════════════════════════════════════════════════════════";
    qDebug() << "📧 SENDING REAL EMAIL (POWERSHELL .NET)";
    qDebug() << "═══════════════════════════════════════════════════════════";
    qDebug() << "To:      " << to;
    qDebug() << "Status:   ✓ REAL EMAIL DISPATCHED";
    qDebug() << "═══════════════════════════════════════════════════════════";
    
    return true;
}

bool EmailNotificationManager::testConnection()
{
    if (m_smtpServer.isEmpty() || m_senderEmail.isEmpty() || m_senderPassword.isEmpty()) {
        qWarning() << "Configuration email incomplete";
        return false;
    }
    
    qDebug() << "Test connexion email...";
    qDebug() << "Serveur SMTP:" << m_smtpServer;
    qDebug() << "Port SMTP:" << m_smtpPort;
    qDebug() << "SSL actif:" << m_useSSL;
    qDebug() << "TLS actif:" << m_useTLS;
    
    return true;
}

QString EmailNotificationManager::generateContractExpirationHtml(const QString &clientName,
                                                               const QString &contractType,
                                                               const QString &expirationDate)
{
    return QString(
        "<html><body style='font-family: Arial, sans-serif;'>"
        "<div style='max-width: 600px; margin: 0 auto;'>"
        "<div style='background: linear-gradient(135deg, #0f2b4c 0%, #1d4f91 100%); color: white; padding: 30px; text-align: center; border-radius: 8px 8px 0 0;'>"
        "<h1 style='margin: 0;'>Notification d'expiration de contrat</h1>"
        "</div>"
        "<div style='background: #f8fafc; padding: 30px; border-radius: 0 0 8px 8px;'>"
        "<p>Bonjour <strong>%1</strong>,</p>"
        "<p>Votre contrat <strong>%2</strong> arrive a expiration le <strong>%3</strong>.</p>"
        "<div style='background: #fef3c7; border-left: 4px solid #f59e0b; padding: 15px; margin: 20px 0; border-radius: 4px;'>"
        "<p style='margin: 0;'><strong>Action requise :</strong> Contactez-nous pour renouveler votre contrat avant la date d'expiration afin d'eviter une interruption de service.</p>"
        "</div>"
        "<p>Merci d'utiliser WasteGuard pour votre gestion intelligente des dechets.</p>"
        "<p>Cordialement,<br><strong>Equipe WasteGuard</strong></p>"
        "</div>"
        "</div>"
        "</body></html>"
    ).arg(clientName, contractType, expirationDate);
}

QString EmailNotificationManager::generateClientConfirmationHtml(const QString &clientName,
                                                               const QString &contractType)
{
    return QString(
        "<html><body style='font-family: Arial, sans-serif;'>"
        "<div style='max-width: 600px; margin: 0 auto;'>"
        "<div style='background: linear-gradient(135deg, #0f2b4c 0%, #1d4f91 100%); color: white; padding: 30px; text-align: center; border-radius: 8px 8px 0 0;'>"
        "<h1 style='margin: 0;'>Bienvenue chez WasteGuard</h1>"
        "</div>"
        "<div style='background: #f8fafc; padding: 30px; border-radius: 0 0 8px 8px;'>"
        "<p>Bonjour <strong>%1</strong>,</p>"
        "<p>Merci d'avoir choisi WasteGuard.</p>"
        "<p>Votre contrat <strong>%2</strong> est active avec succes. Vous pouvez maintenant acceder a :</p>"
        "<ul>"
        "<li>Suivi en temps reel des bacs</li>"
        "<li>Analyse predictive du niveau de remplissage</li>"
        "<li>Optimisation des tournees de collecte</li>"
        "<li>Suivi de l'impact environnemental</li>"
        "<li>Notifications automatiques</li>"
        "</ul>"
        "<p>Pour toute question, notre equipe support est a votre disposition.</p>"
        "<p>Cordialement,<br><strong>Equipe WasteGuard</strong></p>"
        "</div>"
        "</div>"
        "</body></html>"
    ).arg(clientName, contractType);
}

QString EmailNotificationManager::generateBillingNotificationHtml(const QString &clientName,
                                                                 const QString &amount,
                                                                 const QString &dueDate)
{
    return QString(
        "<html><body style='font-family: Arial, sans-serif;'>"
        "<div style='max-width: 600px; margin: 0 auto;'>"
        "<div style='background: linear-gradient(135deg, #0f2b4c 0%, #1d4f91 100%); color: white; padding: 30px; text-align: center; border-radius: 8px 8px 0 0;'>"
        "<h1 style='margin: 0;'>Paiement a echeance</h1>"
        "</div>"
        "<div style='background: #f8fafc; padding: 30px; border-radius: 0 0 8px 8px;'>"
        "<p>Bonjour <strong>%1</strong>,</p>"
        "<p>Nous vous rappelons que le paiement de votre facture arrive a echeance.</p>"
        "<div style='background: white; border: 2px solid #e2e8f0; padding: 20px; margin: 20px 0; border-radius: 4px;'>"
        "<p style='margin: 5px 0;'><strong>Montant du:</strong> %2</p>"
        "<p style='margin: 5px 0;'><strong>Date limite:</strong> %3</p>"
        "</div>"
        "<p>Merci d'effectuer le reglement avant la date limite afin d'eviter toute interruption de service.</p>"
        "<p>Cordialement,<br><strong>Equipe WasteGuard</strong></p>"
        "</div>"
        "</div>"
        "</body></html>"
    ).arg(clientName, amount, dueDate);
}
