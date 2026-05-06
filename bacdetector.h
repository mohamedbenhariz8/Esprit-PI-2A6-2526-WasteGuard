#ifndef BACDETECTOR_H
#define BACDETECTOR_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QProcess>

// ─────────────────────────────────────────────────────────────────────────────
// BacDetector — fully async AI inference
//
// Business logic (detectPair):
//   Python AI returns: "READY" (clean) or "MAINTENANCE" (problem)
//   
//   Step 1: Analyze AVANT image → store in resAvant
//   Step 2: Analyze APRES image → store in resApres
//   Step 3: Apply decision matrix:
//     - MAINTENANCE + OK          → TERMINEE    (Fixed! - Green)
//     - OK + MAINTENANCE          → EN_ATTENTE  (New issue! - Orange)
//     - MAINTENANCE + MAINTENANCE → ANNULEE     (Not fixed! - Red)
//     - Default                   → EN_COURS    (Blue)
//
// Signal emitted: detectionFinished(QString statut)
//   statut is one of "TERMINEE", "EN_ATTENTE", "ANNULEE", "EN_COURS", or "" (error)
// ─────────────────────────────────────────────────────────────────────────────
class BacDetector : public QObject {
    Q_OBJECT

public:
    explicit BacDetector(QObject* parent = nullptr);
    ~BacDetector();

    // Two-image comparison — applies business logic, emits detectionFinished()
    void detectPair(const QImage& avant, const QImage& apres);

    // Single-image raw inference — emits rawResult("OK" | "NOT_OK" | "")
    void detectRaw(const QImage& img);

signals:
    void detectionFinished(const QString& statut);   // EN_ATTENTE / TERMINEE / EN_COURS / ""
    void rawResult(const QString& result);            // OK / NOT_OK / ""

private slots:
    void onRawFinished(int exitCode, QProcess::ExitStatus);
    void onPairAvantFinished(int exitCode, QProcess::ExitStatus);

private:
    QString   pythonExecutable;
    QString   pythonScriptPath;
    QString   modelPath;

    // raw single-shot
    QProcess* m_rawProcess   = nullptr;
    QString   m_rawTempPath;

    // pair workflow - STRICT SEQUENTIAL STATE VARIABLES
    QProcess* m_pairProcess  = nullptr;
    QString   m_pairTempPath;
    QString   m_pairApresPath;  // Store APRES path for phase 2
    
    // State variables for strict sequencing
    QString   resAvant;      // Result from AVANT image
    QString   resApres;      // Result from APRES image
    int       currentStep = 0;  // 0 = AVANT phase, 1 = APRES phase

    QString saveTempImage(const QImage& img);
    void    killProcess(QProcess*& proc, QString& tempPath);
    bool    startProcess(QProcess*& proc, const QString& tempPath,
                         void (BacDetector::*finishedSlot)(int, QProcess::ExitStatus));
};

#endif // BACDETECTOR_H
