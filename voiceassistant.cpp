#include "voiceassistant.h"
#include <QDebug>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDir>
#include <QSet>
#include <QProcessEnvironment>
#include <cstdlib>
#include <QString>

VoiceAssistant::VoiceAssistant(QObject *parent)
    : QObject(parent),
      m_pythonProcess(nullptr),
      m_server(nullptr),
      m_socket(nullptr),
      m_thread(nullptr),
      m_initialized(false),
      m_listening(false)
{
}

VoiceAssistant::~VoiceAssistant()
{
    if (m_pythonProcess) {
        m_pythonProcess->kill();
        m_pythonProcess->waitForFinished(3000);
        delete m_pythonProcess;
    }

    if (m_socket) {
        m_socket->disconnectFromServer();
        delete m_socket;
    }

    if (m_server) {
        m_server->close();
        delete m_server;
    }

    if (m_thread) {
        m_thread->quit();
        m_thread->wait();
        delete m_thread;
    }
}

bool VoiceAssistant::initialize(const QString &pythonScriptPath)
{
    m_pythonScriptPath = pythonScriptPath;
    m_initialized = false;
    m_listening = false;

    if (m_pythonScriptPath.trimmed().isEmpty() || !QFileInfo::exists(m_pythonScriptPath)) {
        qWarning() << "[VOICE] voice_engine.py not found:" << m_pythonScriptPath;
        emit this->error("Le script voice_engine.py est introuvable.");
        return false;
    }

    // Nettoyer un etat precedent pour permettre une re-initialisation fiable.
    if (m_pythonProcess) {
        m_pythonProcess->kill();
        m_pythonProcess->waitForFinished(1500);
        delete m_pythonProcess;
        m_pythonProcess = nullptr;
    }
    if (m_socket) {
        m_socket->disconnectFromServer();
        delete m_socket;
        m_socket = nullptr;
    }
    if (m_server) {
        m_server->close();
        delete m_server;
        m_server = nullptr;
    }

    // Créer le processus Python
    m_pythonProcess = new QProcess(this);

    connect(m_pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &VoiceAssistant::onPythonProcessFinished);
    connect(m_pythonProcess, QOverload<QProcess::ProcessError>::of(&QProcess::errorOccurred),
            this, &VoiceAssistant::onPythonProcessError);
    connect(m_pythonProcess, &QProcess::readyReadStandardOutput,
            this, &VoiceAssistant::onReadyReadStandardOutput);
    connect(m_pythonProcess, &QProcess::readyReadStandardError,
            this, &VoiceAssistant::onReadyReadStandardError);

    // Créer le serveur local pour compatibilite legacy (non bloquant)
    m_server = new QLocalServer(this);
    QString serverName = "WasteGuardVoiceServer";
    QLocalServer::removeServer(serverName);
    if (!m_server->listen(serverName)) {
        qWarning() << "[VOICE] Local server unavailable (continuing stdin/stdout mode):" << m_server->errorString();
    }

    connect(m_server, &QLocalServer::newConnection, [this]() {
        m_socket = m_server->nextPendingConnection();
        if (m_socket) {
            connect(m_socket, &QLocalSocket::connected, this, &VoiceAssistant::onSocketConnected);
            connect(m_socket, &QLocalSocket::disconnected, this, &VoiceAssistant::onSocketDisconnected);
            connect(m_socket, &QLocalSocket::readyRead, this, [this]() {
                QString data = QString::fromUtf8(m_socket->readAll());
                processReceivedData(data);
            });
        }
    });

    // Chercher Python dans plusieurs endroits
    const QString scriptDir = QFileInfo(m_pythonScriptPath).absolutePath();
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString cwd = QDir::currentPath();
    const QString sourceDir = QFileInfo(QString::fromUtf8(__FILE__)).absolutePath();

    QStringList pythonCandidates;
    auto addCandidate = [&pythonCandidates](const QString &candidate) {
        if (!candidate.trimmed().isEmpty()) {
            pythonCandidates << QDir::fromNativeSeparators(candidate.trimmed());
        }
    };

    auto addNearbyVenvPythons = [&addCandidate](const QString &basePath) {
        if (basePath.isEmpty()) {
            return;
        }

        QDir dir(basePath);
        if (!dir.exists()) {
            return;
        }

        // 1) Current tree upwards: .../.venv/Scripts/python.exe
        QDir cursor = dir;
        for (int i = 0; i < 8; ++i) {
            addCandidate(cursor.filePath(".venv/Scripts/python.exe"));
            if (!cursor.cdUp()) {
                break;
            }
        }

        // 2) Sibling projects: ../<project>/.venv/Scripts/python.exe
        cursor = dir;
        for (int i = 0; i < 6; ++i) {
            QDir parent(cursor);
            if (!parent.cdUp()) {
                break;
            }

            const QFileInfoList siblings = parent.entryInfoList(
                QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable
            );
            for (const QFileInfo &sib : siblings) {
                addCandidate(QDir(sib.absoluteFilePath()).filePath(".venv/Scripts/python.exe"));

                // Also probe one nested level (e.g. final/INTEGRATION_FINALE/.venv/Scripts/python.exe).
                QDir sibDir(sib.absoluteFilePath());
                const QFileInfoList children = sibDir.entryInfoList(
                    QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable
                );
                for (const QFileInfo &child : children) {
                    addCandidate(QDir(child.absoluteFilePath()).filePath(".venv/Scripts/python.exe"));
                }
            }
            cursor = parent;
        }
    };

    addNearbyVenvPythons(scriptDir);
    addNearbyVenvPythons(appDir);
    addNearbyVenvPythons(cwd);
    addNearbyVenvPythons(sourceDir);

    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QString virtualEnv = env.value("VIRTUAL_ENV");
    if (!virtualEnv.trimmed().isEmpty()) {
        addCandidate(QDir::fromNativeSeparators(virtualEnv) + "/Scripts/python.exe");
    }

    // Common command aliases / user-local python fallbacks.
    addCandidate("python");
    addCandidate("py");
    addCandidate("python3");
    addCandidate("python3.11");
    addCandidate("python3.14");
    addCandidate("C:/Users/" + QString::fromStdString(std::getenv("USERNAME") ? std::getenv("USERNAME") : "") + "/.local/bin/python3.14.exe");
    addCandidate("C:/Users/" + QString::fromStdString(std::getenv("USERNAME") ? std::getenv("USERNAME") : "") + "/.local/bin/python3.exe");

    // Keep order, remove duplicates, and ignore non-existent file paths.
    QStringList normalizedCandidates;
    QSet<QString> seen;
    for (const QString &candidate : pythonCandidates) {
        const bool isCommand = !candidate.contains('/');
        if (!isCommand && !QFileInfo::exists(candidate)) {
            continue;
        }
        if (!seen.contains(candidate)) {
            seen.insert(candidate);
            normalizedCandidates << candidate;
        }
    }
    pythonCandidates = normalizedCandidates;

    // Valider Python + dependances (commande one-shot "init") avant de lancer le mode persistent.
    QString selectedPython;
    QString dependencyError;
    for (const QString &candidate : pythonCandidates) {
        QProcess checkProcess;
        checkProcess.setProcessChannelMode(QProcess::MergedChannels);
        checkProcess.start(candidate, QStringList() << m_pythonScriptPath << "init");

        if (!checkProcess.waitForStarted(3500)) {
            continue;
        }

        checkProcess.waitForFinished(15000);
        const QString output = QString::fromUtf8(checkProcess.readAllStandardOutput()).trimmed();

        // Tolerate extra logs around JSON by scanning line-by-line and keeping the
        // last valid JSON object.
        QJsonDocument json;
        const QStringList outLines = output.split('\n', Qt::SkipEmptyParts);
        for (const QString &rawLine : outLines) {
            const QString line = rawLine.trimmed();
            if (!line.startsWith('{') || !line.endsWith('}')) {
                continue;
            }
            QJsonParseError parseError;
            const QJsonDocument candidateJson = QJsonDocument::fromJson(line.toUtf8(), &parseError);
            if (parseError.error == QJsonParseError::NoError && candidateJson.isObject()) {
                json = candidateJson;
            }
        }

        if (!json.isObject()) {
            QJsonParseError parseError;
            const QJsonDocument directJson = QJsonDocument::fromJson(output.toUtf8(), &parseError);
            if (parseError.error == QJsonParseError::NoError && directJson.isObject()) {
                json = directJson;
            }
        }

        if (json.isObject()) {
            const QJsonObject obj = json.object();
            const bool engineOk = obj.value("engine").toBool(false);
            const bool recognizeOk = obj.value("recognize").toBool(false);
            if (recognizeOk) {
                selectedPython = candidate;
                if (!engineOk) {
                    qWarning() << "Voice TTS unavailable, recognition only mode enabled";
                }
                break;
            }
            dependencyError = output;
        }
    }

    if (selectedPython.isEmpty()) {
        qWarning() << "Voice initialization failed - Python/dependencies unavailable";
        qWarning() << "Tried:" << pythonCandidates;
        emit this->error("La reconnaissance vocale n'est pas prete.\n\n"
                        "Installez les dependances Python puis relancez:\n"
                        "pip install SpeechRecognition pyttsx3 sounddevice numpy\n\n"
                        "Detail: " + (dependencyError.isEmpty() ? QString("dependances manquantes ou micro indisponible") : dependencyError));
        return false;
    }

    // Parametres pour le processus Python en mode boucle de commandes.
    QStringList arguments;
    arguments << m_pythonScriptPath;

    m_pythonProcess->setProcessChannelMode(QProcess::MergedChannels);
    m_pythonProcess->start(selectedPython, arguments);
    if (!m_pythonProcess->waitForStarted(1500)) {
        qWarning() << "Python process failed to start in persistent mode:" << selectedPython;
        emit this->error("Le moteur vocal n'a pas pu demarrer en mode service.");
        return false;
    }

    qDebug() << "Python found at:" << selectedPython;

    m_initialized = true;
    qDebug() << "VoiceAssistant initialized successfully";
    return true;
}

bool VoiceAssistant::isInitialized() const
{
    return m_initialized;
}

void VoiceAssistant::startListening()
{
    if (!m_initialized) return;

    m_listening = true;
    sendCommand("START_LISTENING");
    emit listeningStarted();
}

void VoiceAssistant::stopListening()
{
    if (!m_initialized) return;

    m_listening = false;
    sendCommand("STOP_LISTENING");
    emit listeningFinished();
}

bool VoiceAssistant::isListening() const
{
    return m_listening;
}

void VoiceAssistant::speak(const QString &text)
{
    if (!m_initialized) return;

    QString command = QString("SPEAK:%1").arg(text);
    sendCommand(command);
    emit speechStarted();
}

void VoiceAssistant::sendCommand(const QString &command)
{
    if (m_pythonProcess && m_pythonProcess->state() == QProcess::Running) {
        m_pythonProcess->write((command + "\n").toUtf8());
        m_pythonProcess->waitForBytesWritten();
    }
}

void VoiceAssistant::onPythonProcessFinished(int exitCode, QProcess::ExitStatus /*exitStatus*/)
{
    qDebug() << "Python process finished with exit code:" << exitCode;
    m_initialized = false;
    m_listening = false;
}

void VoiceAssistant::onPythonProcessError(QProcess::ProcessError error)
{
    qWarning() << "[VOICE SILENT ERROR] Python process error:" << error;
    // NE PAS émettre d'erreur visible - just log silently
    // emit this->error("Erreur du processus Python");  // DISABLED
}

void VoiceAssistant::onReadyReadStandardOutput()
{
    QString output = QString::fromUtf8(m_pythonProcess->readAllStandardOutput());
    processReceivedData(output);
}

void VoiceAssistant::onReadyReadStandardError()
{
    QString error = QString::fromUtf8(m_pythonProcess->readAllStandardError());
    qWarning() << "Python stderr:" << error;
    emit this->error(error);
}

void VoiceAssistant::onSocketConnected()
{
    qDebug() << "Voice assistant socket connected";
}

void VoiceAssistant::onSocketDisconnected()
{
    qDebug() << "Voice assistant socket disconnected";
}

void VoiceAssistant::onSocketError(QAbstractSocket::SocketError socketError)
{
    qWarning() << "Socket error:" << socketError;
}

void VoiceAssistant::processReceivedData(const QString &data)
{
    qDebug() << "Received from Python:" << data;

    // Parser ligne par ligne pour tolerer les logs melanges au protocole.
    const QStringList lines = data.split('\n', Qt::SkipEmptyParts);
    for (const QString &rawLine : lines) {
        const QString line = rawLine.trimmed();
        if (line.isEmpty()) {
            continue;
        }

        if (line.startsWith("RECOGNIZED:")) {
            QString recognizedMsg = line.mid(11).trimmed();
            emit recognizedText(recognizedMsg);

            // Identifier la commande
            QStringList commands = parseVoiceCommand(recognizedMsg);
            for (const QString &cmd : commands) {
                emit commandRecognized(cmd);
            }
        } else if (line.startsWith("SPEECH_DONE")) {
            emit speechFinished();
        } else if (line.startsWith("ERROR:")) {
            QString errorMsg = line.mid(6).trimmed();
            emit error(errorMsg);
        }
    }
}

QStringList VoiceAssistant::parseVoiceCommand(const QString &text)
{
    QStringList commands;
    QString lowerText = text.toLower();

    if (lowerText.contains("ajouter") && lowerText.contains("intervention")) {
        commands.append("ADD_INTERVENTION");
    }

    if ((lowerText.contains("modifier") || lowerText.contains("modifie") || lowerText.contains("update") || lowerText.contains("edit"))
            && lowerText.contains("intervention")) {
        commands.append("MODIFY_INTERVENTION");
    }

    if ((lowerText.contains("enregistrer") || lowerText.contains("valider") || lowerText.contains("save"))
            && (lowerText.contains("ajout") || lowerText.contains("nouvelle") || lowerText.contains("new"))) {
        commands.append("SAVE_ADD_INTERVENTION");
    }

    if ((lowerText.contains("enregistrer") || lowerText.contains("valider") || lowerText.contains("save"))
            && (lowerText.contains("modification") || lowerText.contains("modif") || lowerText.contains("modifier") || lowerText.contains("edit"))) {
        commands.append("SAVE_MOD_INTERVENTION");
    }

    if ((lowerText.contains("supprimer") || lowerText.contains("effacer") || lowerText.contains("retirer") || lowerText.contains("delete"))
            && lowerText.contains("intervention")) {
        commands.append("DELETE_INTERVENTION");
    }

    if (lowerText.contains("rechercher") || lowerText.contains("search")) {
        commands.append("SEARCH");
    }

    if (lowerText.contains("afficher") || lowerText.contains("show")) {
        commands.append("SHOW_LIST");
    }

    if (lowerText.contains("exporter") || lowerText.contains("export")) {
        commands.append("EXPORT");
    }

    if (lowerText.contains("aide") || lowerText.contains("help")) {
        commands.append("HELP");
    }

    return commands;
}
