#include "videogenerationdialog.h"
#include <QGraphicsOpacityEffect>
#include <QFontDatabase>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QCoreApplication>
#include <QFileInfo>
#include <QGuiApplication>
#include <QScreen>
#include <QProcessEnvironment>
#include <QKeyEvent>
#include <QPixmap>
#include <QShortcut>

VideoGenerationDialog::VideoGenerationDialog(const QString &productName, const QString &reference, int capacity, int battery, const QStringList &features, const QString &provider, const QString &imagePath, QWidget *parent)
    : QDialog(parent), 
      m_productName(productName), 
      m_reference(reference), 
      m_capacity(capacity), 
      m_battery(battery), 
      m_features(features),
      m_provider(provider.trimmed().toLower()),
      m_imagePath(imagePath),
      m_blockingOverlay(nullptr),
      m_overlayEnabled(isBlockingOverlayEnabled()),
      m_overlayDismissed(false)
{
    if (m_provider != "meta") {
        m_provider = "luma";
    }

    m_nodeProcess = new QProcess(this);
    connect(m_nodeProcess, &QProcess::readyReadStandardOutput, this, &VideoGenerationDialog::onProcessOutput);
    connect(m_nodeProcess, &QProcess::readyReadStandardError, this, &VideoGenerationDialog::onProcessOutput);
    connect(m_nodeProcess, &QProcess::finished, this, &VideoGenerationDialog::onProcessFinished);
    
    setupUi();
}

VideoGenerationDialog::~VideoGenerationDialog()
{
}

QString VideoGenerationDialog::providerLabel() const
{
    return m_provider == "meta"
        ? QStringLiteral("Meta AI")
        : QStringLiteral("Luma Labs");
}

void VideoGenerationDialog::setupUi()
{
    this->setWindowTitle(QString("Generation Video Produit - %1").arg(providerLabel()));
    this->resize(720, 520);
    this->setStyleSheet("QDialog { background-color: #0f172a; color: white; }");
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(15);

    // Title
    m_lblTitle = new QLabel(QString("Generation Video Produit - %1").arg(providerLabel()), this);
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

QString VideoGenerationDialog::resolveScriptPath(const QString &scriptFileName, const QString &envVarName) const
{
    QString scriptPath;
    const QString scriptDir = QCoreApplication::applicationDirPath();

    const QByteArray envPath = qgetenv(envVarName.toUtf8().constData());
    if (!envPath.isEmpty()) {
        const QString candidate = QString::fromUtf8(envPath);
        if (QFile::exists(candidate)) {
            return QDir::cleanPath(candidate);
        }
    }

    QDir dir(scriptDir);
    for (int i = 0; i < 12; ++i) {
        const QString candidate = dir.absoluteFilePath(scriptFileName);
        if (QFile::exists(candidate)) {
            return QDir::cleanPath(candidate);
        }
        if (!dir.cdUp()) {
            break;
        }
    }

    dir = QDir(QDir::currentPath());
    for (int i = 0; i < 12; ++i) {
        const QString candidate = dir.absoluteFilePath(scriptFileName);
        if (QFile::exists(candidate)) {
            return QDir::cleanPath(candidate);
        }
        if (!dir.cdUp()) {
            break;
        }
    }

    const QString sourceDir = QFileInfo(QStringLiteral(__FILE__)).absolutePath();
    const QString sourceCandidate = QDir(sourceDir).absoluteFilePath(scriptFileName);
    if (QFile::exists(sourceCandidate)) {
        return QDir::cleanPath(sourceCandidate);
    }

    QDirIterator it(scriptDir, QStringList() << scriptFileName, QDir::Files, QDirIterator::Subdirectories);
    if (it.hasNext()) {
        return QDir::cleanPath(it.next());
    }

    return scriptPath;
}

bool VideoGenerationDialog::isBlockingOverlayEnabled() const
{
    const QString raw = QString::fromUtf8(qgetenv("VIDEO_BLOCKING_OVERLAY")).trimmed().toLower();
    if (raw.isEmpty()) {
        return true;
    }
    return !(raw == "0" || raw == "false" || raw == "no" || raw == "off");
}

void VideoGenerationDialog::showBlockingOverlay()
{
    if (!m_overlayEnabled || m_overlayDismissed) {
        return;
    }

    if (!m_blockingOverlay) {
        m_blockingOverlay = new QWidget(nullptr, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        m_blockingOverlay->setAttribute(Qt::WA_DeleteOnClose, false);
        m_blockingOverlay->setFocusPolicy(Qt::StrongFocus);
        m_blockingOverlay->setStyleSheet("background-color: black;");

        // Ensure ESC works even when overlay takes focus.
        QShortcut *escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), m_blockingOverlay);
        escShortcut->setContext(Qt::ApplicationShortcut);
        connect(escShortcut, &QShortcut::activated, this, [this]() {
            m_overlayDismissed = true;
            hideBlockingOverlay();
        });

        QVBoxLayout *overlayLayout = new QVBoxLayout(m_blockingOverlay);
        overlayLayout->setContentsMargins(0, 0, 0, 0);
        overlayLayout->setSpacing(0);

        QLabel *img = new QLabel(m_blockingOverlay);
        img->setAlignment(Qt::AlignCenter);
        img->setStyleSheet("background-color: black;");

        // Prefer an image named overlay_generating.png in the app dir or project root.
        QString imgPath;
        const QString appDir = QCoreApplication::applicationDirPath();
        const QStringList candidates = {
            QDir(appDir).absoluteFilePath("overlay_generating.png"),
            QDir(appDir).absoluteFilePath("generating.png"),
            QDir(QDir::currentPath()).absoluteFilePath("overlay_generating.png"),
            QDir(QDir::currentPath()).absoluteFilePath("generating.png")
        };
        for (const QString &c : candidates) {
            if (QFile::exists(c)) { imgPath = c; break; }
        }

        if (!imgPath.isEmpty()) {
            QPixmap px(imgPath);
            img->setPixmap(px);
            img->setScaledContents(true);
        } else {
            img->setText("Generation video...\nAppuyez sur Echap pour masquer cet ecran.");
            img->setStyleSheet("color: white; font-size: 28px; font-family: 'Segoe UI';");
        }

        overlayLayout->addWidget(img);
    }

    const QList<QScreen*> screens = QGuiApplication::screens();
    if (!screens.isEmpty() && screens.first()) {
        m_blockingOverlay->setGeometry(screens.first()->geometry());
    }

    m_blockingOverlay->showFullScreen();
    m_blockingOverlay->raise();
    m_blockingOverlay->activateWindow();
    m_blockingOverlay->setFocus();
}

void VideoGenerationDialog::hideBlockingOverlay()
{
    if (m_blockingOverlay) {
        m_blockingOverlay->hide();
    }
}

void VideoGenerationDialog::keyPressEvent(QKeyEvent *event)
{
    if (event && event->key() == Qt::Key_Escape) {
        m_overlayDismissed = true;
        hideBlockingOverlay();
        event->accept();
        return;
    }
    QDialog::keyPressEvent(event);
}

void VideoGenerationDialog::onGenerateClicked()
{
    if (m_nodeProcess && m_nodeProcess->state() == QProcess::Running) {
        m_lblStatus->setText("Generation deja en cours. Veuillez patienter...");
        m_logConsole->appendPlainText("INFO: Une generation est deja en cours.");
        return;
    }

    // Reset overlay dismissal each run
    m_overlayDismissed = false;

    m_btnGenerate->setEnabled(false);
    m_progressBar->show();
    m_lblStatus->setText(QString("Lancement du Robot %1...").arg(providerLabel()));
    m_logConsole->clear();

    showBlockingOverlay();

    const QString selectedScript = (m_provider == "meta")
        ? QStringLiteral("meta_video_bot.js")
        : QStringLiteral("luma_video_bot.js");
    const QString envVarName = (m_provider == "meta")
        ? QStringLiteral("META_VIDEO_BOT_PATH")
        : QStringLiteral("LUMA_VIDEO_BOT_PATH");

    const QString scriptPath = resolveScriptPath(selectedScript, envVarName);

    if (!QFile::exists(scriptPath)) {
        m_logConsole->appendPlainText("ERREUR: " + selectedScript + " introuvable !");
        m_lblStatus->setText("Erreur: script introuvable");
        m_btnGenerate->setEnabled(true);
        m_progressBar->hide();
        hideBlockingOverlay();
        return;
    }

    m_logConsole->appendPlainText("Modele: " + providerLabel());
    m_logConsole->appendPlainText("Script: " + scriptPath);

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

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (m_provider == "luma") {
        // Luma bot currently uses UI automation.
        env.insert("LUMA_USE_UI", "1");
    }
    m_nodeProcess->setProcessEnvironment(env);

    m_nodeProcess->setWorkingDirectory(QFileInfo(scriptPath).absolutePath());
    m_nodeProcess->start("node", arguments);

    if (!m_nodeProcess->waitForStarted(3000)) {
        m_logConsole->appendPlainText("ERREUR: Echec du lancement du processus Node.");
        m_logConsole->appendPlainText("DETAIL: " + m_nodeProcess->errorString());
        m_lblStatus->setText("Erreur: lancement impossible");
        m_btnGenerate->setEnabled(true);
        m_progressBar->hide();
        hideBlockingOverlay();
        return;
    }

    m_logConsole->appendPlainText("INFO: Processus lance avec succes.");
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
    hideBlockingOverlay();

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
    hideBlockingOverlay();
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

