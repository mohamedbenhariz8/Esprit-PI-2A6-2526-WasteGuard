#ifndef TRIPOSR3DDIALOG_H
#define TRIPOSR3DDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QPropertyAnimation>
#include <QProcess>
#include <QPlainTextEdit>
#include <QElapsedTimer>
#include "viewer3ddialog.h"

class TripoSR3DDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TripoSR3DDialog(const QString &productName,
                             const QString &reference,
                             const QString &imagePath,
                             QWidget *parent = nullptr);
    ~TripoSR3DDialog();

    /// Returns the path to the generated mesh.obj (empty if not generated yet)
    QString generatedMeshPath() const;

private slots:
    void onGenerateClicked();
    void onProcessOutput();
    void onProcessErrorOutput();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onOpenMeshClicked();
    void onViewer3DClicked();
    void closeDialog();
    void updateElapsedTime();

private:
    void setupUi();
    void appendLog(const QString &text, const QString &color = "#a3e635");
    QString findTripoSRPath() const;
    QString resolveOutputMeshPath() const;
    void checkExistingMesh();

    // Product info
    QString m_productName;
    QString m_reference;
    QString m_imagePath;

    // Generated output
    QString m_generatedMeshPath;
    QString m_outputDir;

    // UI widgets
    QLabel *m_lblTitle;
    QLabel *m_lblStatus;
    QLabel *m_lblProductInfo;
    QLabel *m_lblImagePreview;
    QLabel *m_lblTimer;
    QProgressBar *m_progressBar;
    QPlainTextEdit *m_logConsole;
    QPushButton *m_btnClose;
    QPushButton *m_btnGenerate;
    QPushButton *m_btnOpenMesh;
    QPushButton *m_btnViewer3D;

    // Process
    QProcess *m_process;
    QElapsedTimer m_elapsedTimer;
    QTimer *m_timerUpdate;

    // TripoSR paths
    static constexpr const char *TRIPOSR_DIR = "C:/Users/gigabyte/Desktop/triposr/TripoSR";
};

#endif // TRIPOSR3DDIALOG_H
