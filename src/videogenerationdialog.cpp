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

VideoGenerationDialog::VideoGenerationDialog(const QString &productName, const QString &reference, int capacity, int battery, const QStringList &features, const QString &imagePath, QWidget *parent)
    : QDialog(parent), 
      m_productName(productName), 
      m_reference(reference), 
      m_capacity(capacity), 
      m_battery(battery), 
      m_features(features),
      m_imagePath(imagePath),
      m_blockingOverlay(nullptr),
      m_overlayEnabled(isBlockingOverlayEnabled()),
      m_overlayDismissed(false)
{
    m_nodeProcess = new QProcess(this);
    connect(m_nodeProcess, &QProcess::readyReadStandardOutput, this, &VideoGenerationDialog::onProcessOutput);
    connect(m_nodeProcess, &QProcess::readyReadStandardError, this, &VideoGenerationDialog::onProcessOutput);
    connect(m_nodeProcess, &QProcess::finished, this, &VideoGenerationDialog::onProcessFinished);
    
    setupUi();
}

VideoGenerationDialog::~VideoGenerationDialog()
{
    if (m_nodeProcess && m_nodeProcess->state() != QProcess::NotRunning) {
        m_nodeProcess->terminate();
        if (!m_nodeProcess->waitForFinished(1500)) {
            m_nodeProcess->kill();
            m_nodeProcess->waitForFinished(1500);
        }
    }
}

QString VideoGenerationDialog::providerLabel() const
{
    return QStringLiteral("Luma Labs");
}

void VideoGenerationDialog::setupUi()
{
    this->setWindowTitle(QString("Generation Video Produit [%1]").arg(providerLabel()));
    this->resize(760, 580);
    // WasteGuard brand: deep navy -> dark teal gradient, emerald accents.
    this->setStyleSheet(
        "QDialog {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "    stop:0 #0a1628, stop:0.55 #0f2b4c, stop:1 #0c2a23);"
        "  color: #e2e8f0;"
        "}"
    );
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(32, 28, 32, 24);
    mainLayout->setSpacing(16);

    // ─── Header Row ───
    QHBoxLayout *headerRow = new QHBoxLayout();
    headerRow->setSpacing(12);

    QLabel *titleBadge = new QLabel(QString::fromUtf8("\xF0\x9F\x8E\xAC"), this); // clapper board emoji
    titleBadge->setFixedSize(40, 40);
    titleBadge->setAlignment(Qt::AlignCenter);
    titleBadge->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "  stop:0 #10b981, stop:1 #27ae60);"
        "border-radius: 10px; font-size: 20px;"
    );
    headerRow->addWidget(titleBadge);

    QVBoxLayout *titleCol = new QVBoxLayout();
    titleCol->setSpacing(0);
    m_lblTitle = new QLabel(QString::fromUtf8("Studio Video Produit"), this);
    m_lblTitle->setStyleSheet(
        "color: #34d399; font-size: 22px; font-weight: 800; font-family: 'Segoe UI';"
        "background: transparent; border: none;"
    );
    QLabel *subTitle = new QLabel(QString("WasteGuard  -  Moteur %1").arg(providerLabel()), this);
    subTitle->setStyleSheet(
        "color: #94a3b8; font-size: 11px; font-weight: 500; letter-spacing: 1px;"
        "background: transparent; border: none;"
    );
    titleCol->addWidget(m_lblTitle);
    titleCol->addWidget(subTitle);
    headerRow->addLayout(titleCol);
    headerRow->addStretch();

    QLabel *providerChip = new QLabel(providerLabel(), this);
    providerChip->setAlignment(Qt::AlignCenter);
    providerChip->setStyleSheet(
        "color: #fbbf24; font-size: 12px; font-weight: 700; font-family: 'Consolas';"
        "background: rgba(15, 43, 76, 0.7); border: 1px solid rgba(251, 191, 36, 0.35);"
        "border-radius: 12px; padding: 6px 14px;"
    );
    headerRow->addWidget(providerChip);
    mainLayout->addLayout(headerRow);

    // ─── Product Info Card ───
    QString featuresText = m_features.isEmpty() ? "Aucun capteur" : m_features.join(", ");
    QString infoHtml = QString(
        "<div>"
        "<span style='color: #f1f5f9; font-weight: bold; font-size: 15px;'>%1</span> &nbsp;"
        "<span style='color: #94a3b8; font-size: 11px;'>Ref </span>"
        "<span style='color: #6ee7b7; font-size: 11px; font-family: Consolas;'>%2</span><br>"
        "<span style='color: #6ee7b7; font-weight: 600; font-size: 12px;'>Capacite:</span> "
        "<span style='color: #e2e8f0; font-size: 12px;'>%3 L</span> &nbsp;|&nbsp; "
        "<span style='color: #6ee7b7; font-weight: 600; font-size: 12px;'>Batterie:</span> "
        "<span style='color: #e2e8f0; font-size: 12px;'>%4 mAh</span><br>"
        "<span style='color: #6ee7b7; font-weight: 600; font-size: 12px;'>Capteurs:</span> "
        "<span style='color: #e2e8f0; font-size: 12px;'>%5</span>"
        "</div>"
    ).arg(m_productName, m_reference).arg(m_capacity).arg(m_battery).arg(featuresText);

    QFrame *infoCard = new QFrame(this);
    infoCard->setStyleSheet(
        "QFrame {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "    stop:0 rgba(15, 43, 76, 0.75), stop:1 rgba(26, 58, 46, 0.65));"
        "  border: 1px solid rgba(39, 174, 96, 0.28);"
        "  border-radius: 16px;"
        "}"
    );
    QHBoxLayout *infoCardLayout = new QHBoxLayout(infoCard);
    infoCardLayout->setContentsMargins(18, 14, 18, 14);
    m_lblProductInfo = new QLabel(this);
    m_lblProductInfo->setTextFormat(Qt::RichText);
    m_lblProductInfo->setText(infoHtml);
    m_lblProductInfo->setWordWrap(true);
    m_lblProductInfo->setStyleSheet("background: transparent; border: none;");
    infoCardLayout->addWidget(m_lblProductInfo);
    mainLayout->addWidget(infoCard);

    // ─── Status (sans logs) ───
    QFrame *statusCard = new QFrame(this);
    statusCard->setStyleSheet(
        "QFrame {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "    stop:0 rgba(15, 43, 76, 0.80), stop:1 rgba(10, 23, 45, 0.85));"
        "  border: 1px solid rgba(39, 174, 96, 0.22);"
        "  border-radius: 20px;"
        "}"
    );
    QVBoxLayout *statusLayout = new QVBoxLayout(statusCard);
    statusLayout->setContentsMargins(32, 28, 32, 28);
    statusLayout->setSpacing(16);
    statusLayout->setAlignment(Qt::AlignCenter);

    m_lblStatus = new QLabel(QString::fromUtf8("Pret - Cliquez sur Generer Video pour demarrer"), this);
    m_lblStatus->setAlignment(Qt::AlignCenter);
    m_lblStatus->setWordWrap(true);
    m_lblStatus->setStyleSheet(
        "color: #6ee7b7; font-size: 16px; font-weight: 600;"
        "background: transparent; border: none;"
    );
    statusLayout->addWidget(m_lblStatus);

    // Hidden log console (kept for debugging, never shown)
    m_logConsole = new QPlainTextEdit(this);
    m_logConsole->setReadOnly(true);
    m_logConsole->hide();
    m_logConsole->setMaximumHeight(0);

    m_progressBar = new QProgressBar(statusCard);
    m_progressBar->setRange(0, 0); // Indeterminate
    m_progressBar->hide();
    m_progressBar->setFixedHeight(6);
    m_progressBar->setTextVisible(false);
    m_progressBar->setStyleSheet(
        "QProgressBar { border: none; background-color: rgba(15, 43, 76, 0.55); border-radius: 3px; }"
        "QProgressBar::chunk {"
        "  background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #27ae60, stop:0.5 #10b981, stop:1 #34d399);"
        "  border-radius: 3px;"
        "}"
    );
    statusLayout->addWidget(m_progressBar);
    mainLayout->addWidget(statusCard, 1);


    // ─── Buttons ───
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);

    m_btnClose = new QPushButton("Fermer", this);
    m_btnClose->setStyleSheet(
        "QPushButton { background: rgba(15, 43, 76, 0.65); color: #cbd5e1;"
        "  border: 1px solid rgba(71, 85, 105, 0.45); border-radius: 12px;"
        "  padding: 12px 22px; font-weight: 600; font-size: 13px; }"
        "QPushButton:hover { background: rgba(29, 79, 145, 0.75); color: #f1f5f9;"
        "  border-color: rgba(39, 174, 96, 0.55); }"
    );
    m_btnClose->setCursor(Qt::PointingHandCursor);
    connect(m_btnClose, &QPushButton::clicked, this, &VideoGenerationDialog::closeDialog);

    m_btnGenerate = new QPushButton(QString::fromUtf8("\xF0\x9F\x9A\x80  Generer Video"), this);
    m_btnGenerate->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "  stop:0 #059669, stop:1 #10b981);"
        "  color: white; border: none; border-radius: 12px;"
        "  padding: 12px 26px; font-weight: bold; font-size: 14px; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "  stop:0 #10b981, stop:1 #34d399); }"
        "QPushButton:disabled { background: rgba(15, 43, 76, 0.55); color: #64748b; }"
    );
    m_btnGenerate->setCursor(Qt::PointingHandCursor);
    connect(m_btnGenerate, &QPushButton::clicked, this, &VideoGenerationDialog::onGenerateClicked);

    btnLayout->addWidget(m_btnClose);
    btnLayout->addStretch();
    btnLayout->addWidget(m_btnGenerate);
    mainLayout->addLayout(btnLayout);

    // ─── Fade-in animation ───
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
    a->setDuration(400);
    a->setStartValue(0);
    a->setEndValue(1);
    a->setEasingCurve(QEasingCurve::OutCubic);
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
        // Allow UI automation clicks to reach the browser window underneath.
        m_blockingOverlay->setAttribute(Qt::WA_TransparentForMouseEvents, true);
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

void VideoGenerationDialog::setAutomationUiHidden(bool hidden)
{
    if (hidden == m_hiddenForAutomation) {
        return;
    }

    m_hiddenForAutomation = hidden;
    if (hidden) {
        hideBlockingOverlay();
        lower();
        clearFocus();
        return;
    }

    showNormal();
}

void VideoGenerationDialog::keyPressEvent(QKeyEvent *event)
{
    if (event && event->key() == Qt::Key_Escape && m_blockingOverlay && m_blockingOverlay->isVisible()) {
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
    m_lblStatus->setStyleSheet(
        "color: #6ee7b7; font-size: 16px; font-weight: 600;"
        "background: transparent; border: none;"
    );
    m_lblStatus->setText(QString("Lancement du Robot %1...").arg(providerLabel()));
    m_logConsole->clear();

    const QString selectedScript = QStringLiteral("luma_video_bot.js");
    const QString envVarName = QStringLiteral("LUMA_VIDEO_BOT_PATH");

    const QString scriptPath = resolveScriptPath(selectedScript, envVarName);

    if (!QFile::exists(scriptPath)) {
        m_logConsole->appendPlainText("ERREUR: " + selectedScript + " introuvable !");
        m_lblStatus->setText(QString::fromUtf8("Erreur: script introuvable."));
        m_lblStatus->setStyleSheet("color: #fca5a5; font-size: 14px; font-weight: bold; background: transparent; border: none;");
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
    // Luma bot currently uses UI automation.
    env.insert("LUMA_USE_UI", "1");
    m_nodeProcess->setProcessEnvironment(env);

    m_nodeProcess->setWorkingDirectory(QFileInfo(scriptPath).absolutePath());
    m_nodeProcess->start("node", arguments);

    if (!m_nodeProcess->waitForStarted(3000)) {
        m_logConsole->appendPlainText("ERREUR: Echec du lancement du processus Node.");
        m_logConsole->appendPlainText("DETAIL: " + m_nodeProcess->errorString());
        m_lblStatus->setText(QString::fromUtf8("Erreur: lancement impossible."));
        m_lblStatus->setStyleSheet("color: #fca5a5; font-size: 14px; font-weight: bold; background: transparent; border: none;");
        m_btnGenerate->setEnabled(true);
        m_progressBar->hide();
        hideBlockingOverlay();
        return;
    }

    m_logConsole->appendPlainText("INFO: Processus lance avec succes.");
}

void VideoGenerationDialog::onProcessOutput()
{
    const QByteArray outBytes = m_nodeProcess->readAllStandardOutput();
    const QByteArray errBytes = m_nodeProcess->readAllStandardError();

    auto handleLine = [this](const QString &line) {
        const QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) return;

        if (m_logConsole) {
            m_logConsole->appendPlainText(trimmed);
        }

        QString t = trimmed;
        if (t.startsWith('[')) {
            const int close = t.indexOf(']');
            if (close > 0 && close < 32) {
                t = t.mid(close + 1).trimmed();
            }
        }

        const QString low = t.toLower();
        if (low.contains("ouverture") && (low.contains("luma") || low.contains("chrome"))) {
            m_lblStatus->setText(QString::fromUtf8("Ouverture de Luma Labs..."));
            return;
        }
        if (low.contains("kling")) {
            m_lblStatus->setText(QString::fromUtf8("Preparation du modele video..."));
            return;
        }
        if (low.contains("image") || low.contains("upload") || low.contains("start frame")) {
            m_lblStatus->setText(QString::fromUtf8("Ajout de l'image..."));
            return;
        }
        if (low.contains("prompt")) {
            m_lblStatus->setText(QString::fromUtf8("Redaction du prompt..."));
            return;
        }
        if (low.contains("generation") && (low.contains("lancement") || low.contains("lancee") || low.contains("lanc"))) {
            m_lblStatus->setText(QString::fromUtf8("Generation video lancee.\nConsultez Luma Labs dans le navigateur."));
            return;
        }
        if (low.contains("ui automation")) {
            m_lblStatus->setText(QString::fromUtf8("✅ Demande envoyee a Luma Labs.\nLa video se genere dans le navigateur."));
            return;
        }
        if (low.contains("erreur") || low.contains("error") || low.contains("fatal")) {
            m_lblStatus->setText(QString::fromUtf8("Erreur pendant l'automatisation."));
            m_lblStatus->setStyleSheet("color: #fca5a5; font-size: 14px; font-weight: bold; background: transparent; border: none;");
            return;
        }
    };

    auto handleBytes = [&](const QByteArray &bytes) {
        if (bytes.isEmpty()) return;
        const QString text = QString::fromUtf8(bytes);
        const QStringList lines = text.split('\n');
        for (const QString &line : lines) {
            handleLine(line);
        }
    };

    handleBytes(outBytes);
    handleBytes(errBytes);
}

void VideoGenerationDialog::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_progressBar->hide();
    m_btnGenerate->setEnabled(true);
    hideBlockingOverlay();

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        m_lblStatus->setText(QString::fromUtf8("✅ Demande envoyee a Luma Labs.\nLa video se genere dans le navigateur."));
        m_lblStatus->setStyleSheet(
            "color: #a3e635; font-size: 16px; font-weight: bold;"
            "background: transparent; border: none;"
        );
        return;
    }

    m_lblStatus->setText(QString::fromUtf8("Erreur: generation interrompue.\nRelancez et verifiez Chrome/Luma Labs."));
    m_lblStatus->setStyleSheet(
        "color: #fca5a5; font-size: 14px; font-weight: bold;"
        "background: transparent; border: none;"
    );
}

void VideoGenerationDialog::closeDialog()
{
    if (m_nodeProcess && m_nodeProcess->state() == QProcess::Running) {
        // Closing the dialog stops the automation and closes the browser.
        m_nodeProcess->kill();
        m_nodeProcess->waitForFinished(3000);
    }
    m_hiddenForAutomation = false;
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

