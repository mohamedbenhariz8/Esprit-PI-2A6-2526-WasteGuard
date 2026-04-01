#include "videogenerationdialog.h"
#include <QGraphicsOpacityEffect>
#include <QFontDatabase>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QCoreApplication>

VideoGenerationDialog::VideoGenerationDialog(const QString &productName, const QString &reference, int capacity, int battery, const QStringList &features, const QString &imagePath, QWidget *parent)
    : QDialog(parent), 
      m_productName(productName), 
      m_reference(reference), 
      m_capacity(capacity), 
      m_battery(battery), 
      m_features(features),
      m_imagePath(imagePath)
{
    m_nodeProcess = new QProcess(this);
    connect(m_nodeProcess, &QProcess::readyReadStandardOutput, this, &VideoGenerationDialog::onProcessOutput);
    connect(m_nodeProcess, &QProcess::readyReadStandardError, this, &VideoGenerationDialog::onProcessOutput);
    connect(m_nodeProcess, &QProcess::finished, this, &VideoGenerationDialog::onProcessFinished);
    
    setupUi();
}

VideoGenerationDialog::~VideoGenerationDialog()
{
}

void VideoGenerationDialog::setupUi()
{
    this->setWindowTitle("Luma Labs — Génération Vidéo Produit");
    this->resize(720, 520);
    this->setStyleSheet("QDialog { background-color: #0f172a; color: white; }");
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(15);

    // Title
    m_lblTitle = new QLabel("🎬 Luma Labs — Génération Vidéo Produit", this);
    m_lblTitle->setStyleSheet("color: #60a5fa; font-size: 18px; font-weight: bold; font-family: 'Segoe UI';");
    m_lblTitle->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_lblTitle);

    // Product Info Card
    QString featuresText = m_features.isEmpty() ? "Aucun capteur" : m_features.join(", ");
    QString infoHtml = QString(
        "<div style='background: rgba(96,165,250,0.08); border: 1px solid rgba(96,165,250,0.25); "
        "border-radius: 10px; padding: 12px; margin: 4px 0;'>"
        "<span style='color: #93c5fd; font-weight: bold;'>Produit:</span> "
        "<span style='color: white;'>%1</span> &nbsp; "
        "<span style='color: #93c5fd; font-weight: bold;'>Réf:</span> "
        "<span style='color: white;'>%2</span><br>"
        "<span style='color: #93c5fd; font-weight: bold;'>Capacité:</span> "
        "<span style='color: white;'>%3 L</span> &nbsp; "
        "<span style='color: #93c5fd; font-weight: bold;'>Batterie:</span> "
        "<span style='color: white;'>%4 mAh</span><br>"
        "<span style='color: #93c5fd; font-weight: bold;'>Capteurs:</span> "
        "<span style='color: white;'>%5</span>"
        "</div>"
    ).arg(m_productName, m_reference).arg(m_capacity).arg(m_battery).arg(featuresText);

    m_lblProductInfo = new QLabel(this);
    m_lblProductInfo->setTextFormat(Qt::RichText);
    m_lblProductInfo->setText(infoHtml);
    m_lblProductInfo->setWordWrap(true);
    mainLayout->addWidget(m_lblProductInfo);

    // Status
    m_lblStatus = new QLabel("Prêt — Cliquez sur 'Générer Vidéo' pour démarrer", this);
    m_lblStatus->setStyleSheet("color: #94a3b8; font-size: 13px;");
    m_lblStatus->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_lblStatus);
    
    // Console Log Area
    m_logConsole = new QPlainTextEdit(this);
    m_logConsole->setReadOnly(true);
    m_logConsole->setStyleSheet("background-color: #1e293b; border: 2px dashed #475569; border-radius: 12px; color: #a3e635; font-family: Consolas; padding: 10px;");
    m_logConsole->setPlainText("En attente du lancement...\n\nLe navigateur s'ouvrira automatiquement.\nLors du premier lancement, connectez-vous à Luma Labs (Dream Machine).\nLes sessions suivantes seront automatiques.");
    mainLayout->addWidget(m_logConsole, 1);

    // Progress Bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 0); // Indeterminate
    m_progressBar->hide();
    m_progressBar->setFixedHeight(6);
    m_progressBar->setStyleSheet(
        "QProgressBar { border: none; background-color: #334155; border-radius: 3px; }"
        "QProgressBar::chunk { background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #3b82f6, stop:1 #60a5fa); border-radius: 3px; }"
    );
    mainLayout->addWidget(m_progressBar);

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_btnGenerate = new QPushButton("🚀  Générer Vidéo", this);
    m_btnGenerate->setStyleSheet(
        "QPushButton { background-color: #3b82f6; color: white; border: none; border-radius: 8px; padding: 12px 20px; font-weight: bold; font-size: 14px; }"
        "QPushButton:hover { background-color: #60a5fa; }"
        "QPushButton:disabled { background-color: #475569; color: #94a3b8; }"
    );
    m_btnGenerate->setCursor(Qt::PointingHandCursor);
    connect(m_btnGenerate, &QPushButton::clicked, this, &VideoGenerationDialog::onGenerateClicked);
    
    m_btnClose = new QPushButton("Fermer", this);
    m_btnClose->setStyleSheet(
        "QPushButton { background-color: #475569; color: white; border: none; border-radius: 8px; padding: 12px 20px; font-weight: bold; font-size: 14px; }"
        "QPushButton:hover { background-color: #64748b; }"
    );
    m_btnClose->setCursor(Qt::PointingHandCursor);
    connect(m_btnClose, &QPushButton::clicked, this, &VideoGenerationDialog::closeDialog);
    
    btnLayout->addWidget(m_btnClose);
    btnLayout->addWidget(m_btnGenerate);
    mainLayout->addLayout(btnLayout);

    // Fade in effect
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
    a->setDuration(500);
    a->setStartValue(0);
    a->setEndValue(1);
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void VideoGenerationDialog::onGenerateClicked()
{
    m_btnGenerate->setEnabled(false);
    m_progressBar->show();
    m_lblStatus->setText("Lancement du Robot Luma Labs...");
    m_logConsole->clear();

    // Resolve the script path by walking up from the executable directory
    QString scriptDir = QCoreApplication::applicationDirPath();
    QString scriptPath;

    const QByteArray envPath = qgetenv("LUMA_VIDEO_BOT_PATH");
    if (!envPath.isEmpty()) {
        const QString candidate = QString::fromUtf8(envPath);
        if (QFile::exists(candidate)) {
            scriptPath = QDir::cleanPath(candidate);
        }
    }

    if (scriptPath.isEmpty()) {
        QDir dir(scriptDir);
        for (int i = 0; i < 12; ++i) {
            const QString candidate = dir.absoluteFilePath("luma_video_bot.js");
            if (QFile::exists(candidate)) {
                scriptPath = QDir::cleanPath(candidate);
                break;
            }
            if (!dir.cdUp()) {
                break;
            }
        }
    }
    if (scriptPath.isEmpty()) {
        QDir dir(QDir::currentPath());
        for (int i = 0; i < 12; ++i) {
            const QString candidate = dir.absoluteFilePath("luma_video_bot.js");
            if (QFile::exists(candidate)) {
                scriptPath = QDir::cleanPath(candidate);
                break;
            }
            if (!dir.cdUp()) {
                break;
            }
        }
    }

    // Fallback: look next to the source file location (compile-time path)
    if (scriptPath.isEmpty()) {
        const QString sourceDir = QFileInfo(QStringLiteral(__FILE__)).absolutePath();
        const QString candidate = QDir(sourceDir).absoluteFilePath("luma_video_bot.js");
        if (QFile::exists(candidate)) {
            scriptPath = QDir::cleanPath(candidate);
        }
    }

    if (scriptPath.isEmpty()) {
        // Fallback: recursive search under the app directory (limited)
        QDirIterator it(scriptDir, QStringList() << "luma_video_bot.js", QDir::Files, QDirIterator::Subdirectories);
        if (it.hasNext()) {
            scriptPath = QDir::cleanPath(it.next());
        }
    }

    if (!QFile::exists(scriptPath)) {
        m_logConsole->appendPlainText("ERREUR: luma_video_bot.js introuvable !");
        m_logConsole->appendPlainText("Chemin cherche: " + (scriptPath.isEmpty() ? QString("<vide>") : scriptPath));
        m_logConsole->appendPlainText("CWD: " + QDir::currentPath());
        m_logConsole->appendPlainText("AppDir: " + QCoreApplication::applicationDirPath());
        const QByteArray envPathErr = qgetenv("LUMA_VIDEO_BOT_PATH");
        if (!envPathErr.isEmpty()) {
            m_logConsole->appendPlainText("LUMA_VIDEO_BOT_PATH: " + QString::fromUtf8(envPathErr));
        }
        m_lblStatus->setText("Erreur: script introuvable");
        m_btnGenerate->setEnabled(true);
        m_progressBar->hide();
        return;
    }

    m_logConsole->appendPlainText("Script: " + scriptPath);

    QString program = "node";
    QStringList arguments;
    arguments << scriptPath;
    arguments << "--product_name" << m_productName;
    arguments << "--reference" << m_reference;
    arguments << "--capacity" << QString::number(m_capacity);
    arguments << "--battery" << QString::number(m_battery);
    arguments << "--features" << m_features.join(",");

    // Pass image path if available
    if (!m_imagePath.isEmpty() && QFile::exists(m_imagePath)) {
        arguments << "--image_path" << m_imagePath;
        m_logConsole->appendPlainText("INFO: Image du produit trouvée: " + m_imagePath);
    } else {
        m_logConsole->appendPlainText("ATTENTION: Aucune image de produit. La vidéo sera générée sans image de référence.");
    }

    // Use UI automation (no CDP) for now
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LUMA_USE_UI", "1");
    m_nodeProcess->setProcessEnvironment(env);

    // Set working directory to the script's directory
    m_nodeProcess->setWorkingDirectory(QFileInfo(scriptPath).absolutePath());
    m_nodeProcess->start(program, arguments);
}

void VideoGenerationDialog::onProcessOutput()
{
    QByteArray output = m_nodeProcess->readAllStandardOutput();
    QByteArray error = m_nodeProcess->readAllStandardError();
    
    if(!output.isEmpty()) {
        m_logConsole->appendPlainText(QString::fromUtf8(output).trimmed());
    }
    if(!error.isEmpty()) {
        // Don't prefix with ERROR for Node.js debug output
        QString errText = QString::fromUtf8(error).trimmed();
        if (errText.contains("Error") || errText.contains("FATAL") || errText.contains("Cannot find")) {
            m_logConsole->appendPlainText("ERROR: " + errText);
        } else {
            m_logConsole->appendPlainText(errText);
        }
    }
}

void VideoGenerationDialog::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_progressBar->hide();
    m_btnGenerate->setEnabled(true);
    
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        m_lblStatus->setText("Processus terminé normalement.");
    } else {
        m_lblStatus->setText("Processus terminé (Code " + QString::number(exitCode) + ")");
        m_logConsole->appendPlainText("\nASTUCE: Si le navigateur s'est fermé, relancez la génération.");
    }
}

void VideoGenerationDialog::closeDialog()
{
    if (m_nodeProcess && m_nodeProcess->state() == QProcess::Running) {
        // Closing the dialog stops the automation and closes the browser.
        m_nodeProcess->kill();
        m_nodeProcess->waitForFinished(3000);
    }
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
    a->setDuration(300);
    a->setStartValue(1);
    a->setEndValue(0);
    a->setEasingCurve(QEasingCurve::OutBack);
    connect(a, &QPropertyAnimation::finished, this, &QDialog::accept);
    a->start(QPropertyAnimation::DeleteWhenStopped);
}


