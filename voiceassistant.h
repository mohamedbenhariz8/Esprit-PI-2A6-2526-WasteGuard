#ifndef VOICEASSISTANT_H
#define VOICEASSISTANT_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QLocalSocket>
#include <QLocalServer>
#include <QAbstractSocket>
#include <QThread>

/**
 * @class VoiceAssistant
 * @brief Gère la reconnaissance vocale et synthèse vocale via Python
 * 
 * Communique avec un moteur Python pour :
 * - Reconnaissance vocale en français
 * - Synthèse vocale (TTS)
 * - Traitement des commandes vocales
 */
class VoiceAssistant : public QObject
{
    Q_OBJECT

public:
    explicit VoiceAssistant(QObject *parent = nullptr);
    ~VoiceAssistant();

    // Initialization
    bool initialize(const QString &pythonScriptPath);
    bool isInitialized() const;

    // Reconnaissance vocale
    void startListening();
    void stopListening();
    bool isListening() const;

    // Synthèse vocale
    void speak(const QString &text);

    // Envoi de commande au moteur Python
    void sendCommand(const QString &command);

signals:
    // Signaux de reconnaissance vocale
    void listeningStarted();
    void listeningFinished();
    void recognizedText(const QString &text);
    void commandRecognized(const QString &command);
    void error(const QString &errorMsg);

    // Signaux de synthèse vocale
    void speechStarted();
    void speechFinished();

private slots:
    void onPythonProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onPythonProcessError(QProcess::ProcessError error);
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    QProcess *m_pythonProcess;
    QLocalServer *m_server;
    QLocalSocket *m_socket;
    QThread *m_thread;
    QString m_pythonScriptPath;
    bool m_initialized;
    bool m_listening;

    void processReceivedData(const QString &data);
    QStringList parseVoiceCommand(const QString &text);
};

#endif // VOICEASSISTANT_H
