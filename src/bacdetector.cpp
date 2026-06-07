#include "bacdetector.h"
#include <QFile>
#include <QDebug>
#include <QStandardPaths>
#include <QRandomGenerator>
#include <QCoreApplication>
#include <QDir>

namespace {

QString firstExistingFile(const QStringList &candidates)
{
    for (const QString &candidate : candidates) {
        if (!candidate.isEmpty() && QFile::exists(candidate)) {
            return QDir::cleanPath(candidate);
        }
    }
    return {};
}

QStringList uniqueCandidateDirs(const QString &appDir)
{
    QStringList dirs;
    auto appendDir = [&dirs](const QString &path) {
        const QString cleaned = QDir::cleanPath(path);
        if (!cleaned.isEmpty() && !dirs.contains(cleaned)) {
            dirs << cleaned;
        }
    };

    appendDir(appDir);
    appendDir(QDir::currentPath());
    appendDir(QDir(appDir).absoluteFilePath(".."));
    appendDir(QDir(appDir).absoluteFilePath("../.."));
    appendDir(QDir(appDir).absoluteFilePath("../../.."));

    return dirs;
}

QString resolveWorkerScriptPath(const QStringList &dirs)
{
    QStringList candidates;
    for (const QString &dir : dirs) {
        candidates << QDir(dir).absoluteFilePath("ai_worker_simple.py");
        candidates << QDir(dir).absoluteFilePath("ai_worker.py");
    }
    return firstExistingFile(candidates);
}

QString resolveModelPath(const QStringList &dirs)
{
    QStringList candidates;
    for (const QString &dir : dirs) {
        candidates << QDir(dir).absoluteFilePath("models/bin_detector.onnx");
    }
    return firstExistingFile(candidates);
}

QString resolvePythonExecutable(const QStringList &dirs)
{
    QStringList candidates;
    for (const QString &dir : dirs) {
        candidates << QDir(dir).absoluteFilePath(".venv/Scripts/python.exe");
        candidates << QDir(dir).absoluteFilePath("venv/Scripts/python.exe");
    }

    const QString venvPython = firstExistingFile(candidates);
    if (!venvPython.isEmpty()) {
        return venvPython;
    }

    return QStringLiteral("python");
}

} // namespace

// ─────────────────────────────────────────────────────────────────────────────

BacDetector::BacDetector(QObject* parent) : QObject(parent)
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QStringList candidateDirs = uniqueCandidateDirs(appDir);

    pythonScriptPath = resolveWorkerScriptPath(candidateDirs);
    if (pythonScriptPath.isEmpty()) {
        pythonScriptPath = QDir(appDir).absoluteFilePath("ai_worker_simple.py");
    }

    modelPath = resolveModelPath(candidateDirs);
    if (modelPath.isEmpty()) {
        modelPath = QDir(appDir).absoluteFilePath("models/bin_detector.onnx");
    }

    pythonExecutable = resolvePythonExecutable(candidateDirs);
    
    qDebug() << "";
    qDebug() << "╔════════════════════════════════════════╗";
    qDebug() << "║   BacDetector Initialization           ║";
    qDebug() << "╚════════════════════════════════════════╝";
    qDebug() << "";
    qDebug() << "📁 Application Directory:";
    qDebug() << "   " << appDir;
    qDebug() << "";
    qDebug() << "🐍 Python Script Path:";
    qDebug() << "   " << pythonScriptPath;
    qDebug() << "   Exists: " << (QFile::exists(pythonScriptPath) ? "✅ YES" : "❌ NO");
    qDebug() << "";
    qDebug() << "🐍 Python Executable:";
    qDebug() << "   " << pythonExecutable;
    qDebug() << "";
    qDebug() << "🤖 Model Path:";
    qDebug() << "   " << modelPath;
    qDebug() << "   Exists: " << (QFile::exists(modelPath) ? "✅ YES" : "⚠️  NO (optional)");
    qDebug() << "";
    qDebug() << "════════════════════════════════════════";
    qDebug() << "";
}

BacDetector::~BacDetector() {
    killProcess(m_rawProcess,  m_rawTempPath);
    killProcess(m_pairProcess, m_pairTempPath);
}

// ── helpers ───────────────────────────────────────────────────────────────────

QString BacDetector::saveTempImage(const QImage& img) {
    if (img.isNull()) return {};
    QString path = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                   + "/wasteguard_"
                   + QString::number(QRandomGenerator::global()->generate())
                   + ".png";
    return img.save(path, "PNG") ? path : QString();
}

void BacDetector::killProcess(QProcess*& proc, QString& tempPath) {
    if (proc) {
        proc->disconnect();
        proc->kill();
        proc->waitForFinished(500);
        proc->deleteLater();
        proc = nullptr;
    }
    if (!tempPath.isEmpty()) {
        QFile::remove(tempPath);
        tempPath.clear();
    }
}

bool BacDetector::startProcess(QProcess*& proc, const QString& tempPath,
                               void (BacDetector::*finishedSlot)(int, QProcess::ExitStatus))
{
    // Check if Python script exists
    if (!QFile::exists(pythonScriptPath)) {
        QString appDir = QCoreApplication::applicationDirPath();
        qWarning() << "❌ BacDetector: Python script NOT FOUND!";
        qWarning() << "   Looking for:" << pythonScriptPath;
        qWarning() << "   Application directory:" << appDir;
        qWarning() << "   Full path would be:" << appDir + "/" + pythonScriptPath;
        qWarning() << "   Please ensure ai_worker_simple.py (or ai_worker.py) is available";
        return false;
    }

    const QStringList pythonCandidates = {
        pythonExecutable,
        QStringLiteral("python"),
        QStringLiteral("py"),
        QStringLiteral("python3")
    };

    proc = new QProcess(this);
    proc->setProcessChannelMode(QProcess::SeparateChannels);

    // Set working directory to application directory
    QString workDir = QCoreApplication::applicationDirPath();
    proc->setWorkingDirectory(workDir);

    connect(proc,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, finishedSlot);

    bool started = false;
    QString lastError;
    QString startedWith;
    for (const QString &candidate : pythonCandidates) {
        if (candidate.isEmpty()) continue;

        QStringList args;
        if (candidate.compare("py", Qt::CaseInsensitive) == 0) {
            args << "-3";
        }
        args << pythonScriptPath << tempPath;

        proc->start(candidate, args);
        if (proc->waitForStarted(2500)) {
            started = true;
            startedWith = candidate;
            break;
        }
        lastError = proc->errorString();
    }

    if (!started) {
        qWarning() << "❌ BacDetector: failed to start Python process.";
        qWarning() << "   Last error:" << lastError;
        proc->deleteLater();
        proc = nullptr;
        return false;
    }

    qDebug() << "✅ BacDetector: Starting Python process";
    qDebug() << "   Working directory:" << workDir;
    qDebug() << "   Python:" << startedWith;
    qDebug() << "   Script:" << pythonScriptPath;
    qDebug() << "   Image:" << tempPath;
    return true;
}

// ── raw single-image API ──────────────────────────────────────────────────────

void BacDetector::detectRaw(const QImage& img) {
    if (img.isNull()) { 
        emit rawResult({}); 
        return; 
    }
    
    // Check if required files exist
    if (!QFile::exists(pythonScriptPath)) {
        QString appDir = QCoreApplication::applicationDirPath();
        qWarning() << "❌ BacDetector detectRaw: Python script NOT FOUND!";
        qWarning() << "   Looking for:" << pythonScriptPath;
        qWarning() << "   Application directory:" << appDir;
        emit rawResult({});
        return;
    }
    
    if (!QFile::exists(modelPath)) {
        qWarning() << "⚠️  BacDetector detectRaw: Model file not found (optional)";
        qWarning() << "   Looking for:" << modelPath;
    }
    
    m_rawTempPath = saveTempImage(img);
    if (m_rawTempPath.isEmpty()) { 
        emit rawResult({}); 
        return; 
    }

    killProcess(m_rawProcess, m_rawTempPath);
    m_rawTempPath = saveTempImage(img); // re-save after kill cleared it
    if (!startProcess(m_rawProcess, m_rawTempPath,
                      &BacDetector::onRawFinished)) {
        QFile::remove(m_rawTempPath);
        m_rawTempPath.clear();
        emit rawResult({});
    }
}

void BacDetector::onRawFinished(int exitCode, QProcess::ExitStatus) {
    QFile::remove(m_rawTempPath); m_rawTempPath.clear();
    if (exitCode != 0) {
        qWarning() << "BacDetector raw: exit" << exitCode
                   << QString::fromUtf8(m_rawProcess->readAllStandardError()).trimmed();
        m_rawProcess->deleteLater(); m_rawProcess = nullptr;
        emit rawResult({});
        return;
    }
    QString out = QString::fromUtf8(m_rawProcess->readAllStandardOutput()).trimmed();
    m_rawProcess->deleteLater(); m_rawProcess = nullptr;
    qDebug() << "BacDetector raw output:" << out;
    // Python returns "READY" (clean) or "MAINTENANCE" (problem)
    QString result = (out == "READY") ? "OK" : (out == "MAINTENANCE") ? "NOT_OK" : QString();
    emit rawResult(result);
}

// ── pair two-image API ────────────────────────────────────────────────────────

void BacDetector::detectPair(const QImage& avant, const QImage& apres) {
    if (avant.isNull() || apres.isNull()) {
        qWarning() << "❌ BacDetector: null image in pair";
        emit detectionFinished({});
        return;
    }
    
    // Check if required files exist
    if (!QFile::exists(pythonScriptPath)) {
        QString appDir = QCoreApplication::applicationDirPath();
        qWarning() << "❌ BacDetector detectPair: Python script NOT FOUND!";
        qWarning() << "   Looking for:" << pythonScriptPath;
        qWarning() << "   Application directory:" << appDir;
        qWarning() << "   Full path would be:" << appDir + "/" + pythonScriptPath;
        qWarning() << "   Please ensure ai_worker_simple.py is in the application directory";
        emit detectionFinished({});
        return;
    }
    
    if (!QFile::exists(modelPath)) {
        qWarning() << "⚠️  BacDetector detectPair: Model file not found (optional)";
        qWarning() << "   Looking for:" << modelPath;
    }

    // Initialize state variables
    resAvant.clear();
    resApres.clear();
    currentStep = 0;  // Start with AVANT phase

    // Clean up any existing processes
    killProcess(m_pairProcess, m_pairTempPath);
    killProcess(m_rawProcess,  m_rawTempPath);

    // Save both images to temp files
    m_pairTempPath = saveTempImage(avant);    // For AVANT (phase 0)
    m_pairApresPath = saveTempImage(apres);   // For APRES (phase 1)

    if (m_pairTempPath.isEmpty() || m_pairApresPath.isEmpty()) {
        emit detectionFinished({});
        return;
    }

    qDebug() << "";
    qDebug() << "╔════════════════════════════════════════╗";
    qDebug() << "║  BacDetector: Starting Pair Detection  ║";
    qDebug() << "╚════════════════════════════════════════╝";
    qDebug() << "";
    qDebug() << "📋 STEP 1: Analyzing AVANT image...";
    qDebug() << "   currentStep = 0";
    qDebug() << "";

    // STEP 1: Start ai_worker.py with PHOTO_AVANT
    if (!startProcess(m_pairProcess, m_pairTempPath,
                      &BacDetector::onPairAvantFinished)) {
        QFile::remove(m_pairTempPath);
        QFile::remove(m_pairApresPath);
        m_pairTempPath.clear();
        m_pairApresPath.clear();
        emit detectionFinished({});
    }
}

void BacDetector::onPairAvantFinished(int exitCode, QProcess::ExitStatus) {
    QFile::remove(m_pairTempPath); m_pairTempPath.clear();

    if (exitCode != 0) {
        qWarning() << "BacDetector AVANT: Python exit code" << exitCode;
        qWarning() << "Error:" << QString::fromUtf8(m_pairProcess->readAllStandardError()).trimmed();
        m_pairProcess->deleteLater(); m_pairProcess = nullptr;
        QFile::remove(m_pairApresPath); m_pairApresPath.clear();
        emit detectionFinished({});
        return;
    }

    // STEP 2: Check currentStep and process accordingly
    if (currentStep == 0) {
        // ═══════════════════════════════════════════════════════════════
        // PHASE 0: AVANT ANALYSIS
        // ═══════════════════════════════════════════════════════════════
        
        // Capture and normalize AVANT output
        QString rawOutput = QString::fromUtf8(m_pairProcess->readAllStandardOutput());
        m_pairProcess->deleteLater(); m_pairProcess = nullptr;
        
        // Store result: resAvant = output.trimmed().toUpper()
        resAvant = rawOutput.trimmed().toUpper();
        
        qDebug() << "✅ PHASE 0 COMPLETE: AVANT Analysis";
        qDebug() << "   Raw output:" << rawOutput;
        qDebug() << "   Normalized: resAvant =" << resAvant;
        qDebug() << "";
        
        // Set currentStep = 1
        currentStep = 1;
        
        qDebug() << "📋 STEP 2: Analyzing APRES image...";
        qDebug() << "   currentStep = 1";
        qDebug() << "";
        
        // Immediately restart the process with PHOTO_APRES
        if (!startProcess(m_pairProcess, m_pairApresPath,
                          &BacDetector::onPairAvantFinished)) {
            QFile::remove(m_pairApresPath);
            m_pairApresPath.clear();
            emit detectionFinished({});
        }
    }
    else if (currentStep == 1) {
        // ═══════════════════════════════════════════════════════════════
        // PHASE 1: APRES ANALYSIS
        // ═══════════════════════════════════════════════════════════════
        
        // Capture and normalize APRES output
        QString rawOutput = QString::fromUtf8(m_pairProcess->readAllStandardOutput());
        m_pairProcess->deleteLater(); m_pairProcess = nullptr;
        
        // Store result: resApres = output.trimmed().toUpper()
        resApres = rawOutput.trimmed().toUpper();
        
        qDebug() << "✅ PHASE 1 COMPLETE: APRES Analysis";
        qDebug() << "   Raw output:" << rawOutput;
        qDebug() << "   Normalized: resApres =" << resApres;
        qDebug() << "";
        
        // Clean up temp file
        QFile::remove(m_pairApresPath); m_pairApresPath.clear();
        
        // ═══════════════════════════════════════════════════════════════
        // STEP 3: DECISION MATRIX
        // ═══════════════════════════════════════════════════════════════
        
        qDebug() << "╔════════════════════════════════════════╗";
        qDebug() << "║   STEP 3: DECISION MATRIX             ║";
        qDebug() << "╚════════════════════════════════════════╝";
        qDebug() << "";
        qDebug() << "📊 INPUT:";
        qDebug() << "   resAvant =" << resAvant;
        qDebug() << "   resApres =" << resApres;
        qDebug() << "";
        
        QString finalStatus;
        
        if (resAvant == "MAINTENANCE" && resApres == "OK") {
            finalStatus = "TERMINEE";
            qDebug() << "✅ MATCH: MAINTENANCE + OK → TERMINEE (Green)";
            qDebug() << "   Explanation: Was broken, now fixed!";
        }
        else if (resAvant == "OK" && resApres == "MAINTENANCE") {
            finalStatus = "EN_ATTENTE";
            qDebug() << "✅ MATCH: OK + MAINTENANCE → EN_ATTENTE (Orange)";
            qDebug() << "   Explanation: Was ok, now broken!";
        }
        else if (resAvant == "MAINTENANCE" && resApres == "MAINTENANCE") {
            finalStatus = "ANNULEE";
            qDebug() << "✅ MATCH: MAINTENANCE + MAINTENANCE → ANNULEE (Red)";
            qDebug() << "   Explanation: Still broken!";
        }
        else {
            finalStatus = "EN_COURS";
            qDebug() << "✅ MATCH: Default case → EN_COURS (Blue)";
            qDebug() << "   Explanation: No change or ongoing process";
        }
        
        qDebug() << "";
        qDebug() << "╔════════════════════════════════════════╗";
        qDebug() << "║   FINAL RESULT                        ║";
        qDebug() << "╚════════════════════════════════════════╝";
        qDebug() << "";
        qDebug() << "📋 SUMMARY:";
        qDebug() << "   AVANT: " << resAvant;
        qDebug() << "   APRES: " << resApres;
        qDebug() << "   STATUS: " << finalStatus;
        qDebug() << "";
        qDebug() << "════════════════════════════════════════";
        qDebug() << "";
        
        // Reset step counter for next detection
        currentStep = 0;
        
        // Emit the final status
        emit detectionFinished(finalStatus);
    }
}


