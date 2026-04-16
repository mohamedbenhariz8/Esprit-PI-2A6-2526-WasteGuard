#include "triposr3ddialog.h"
#include <QGraphicsOpacityEffect>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QDateTime>
#include <QPixmap>
#include <QFont>
#include <QFrame>
#include <QStringList>
#include <QTextStream>

namespace {
bool prepareTexturedMeshBundle(const QString &meshPath, QString *error = nullptr)
{
    if (meshPath.isEmpty() || !QFile::exists(meshPath)) {
        if (error) *error = "mesh.obj introuvable.";
        return false;
    }

    const QFileInfo meshInfo(meshPath);
    const QDir meshDir = meshInfo.absoluteDir();
    const QString texturePath = meshDir.absoluteFilePath("texture.png");
    if (!QFile::exists(texturePath)) {
        if (error) *error = QString("texture.png introuvable dans %1").arg(meshDir.absolutePath());
        return false;
    }

    const QString mtlPath = meshDir.absoluteFilePath("mesh.mtl");
    QFile mtlFile(mtlPath);
    if (!mtlFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        if (error) *error = QString("Impossible d'ecrire %1").arg(mtlPath);
        return false;
    }

    QTextStream mtlStream(&mtlFile);
    mtlStream << "newmtl material_0\n"
              << "Ka 1.000 1.000 1.000\n"
              << "Kd 1.000 1.000 1.000\n"
              << "Ks 0.000 0.000 0.000\n"
              << "d 1.0\n"
              << "illum 2\n"
              << "map_Kd texture.png\n";
    mtlFile.close();

    QFile objFile(meshPath);
    if (!objFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (error) *error = QString("Impossible de lire %1").arg(meshPath);
        return false;
    }

    const QString objContent = QString::fromUtf8(objFile.readAll());
    objFile.close();

    const QString mtlLine = "mtllib mesh.mtl";
    const QString useMtlLine = "usemtl material_0";
    QStringList headerLines;

    if (!objContent.contains(mtlLine)) {
        headerLines << mtlLine;
    }
    if (!objContent.contains(useMtlLine)) {
        headerLines << useMtlLine;
    }

    if (headerLines.isEmpty()) {
        return true;
    }

    QFile outObjFile(meshPath);
    if (!outObjFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        if (error) *error = QString("Impossible de mettre a jour %1").arg(meshPath);
        return false;
    }

    QTextStream objStream(&outObjFile);
    objStream << headerLines.join('\n') << "\n" << objContent;
    outObjFile.close();
    return true;
}
}

TripoSR3DDialog::TripoSR3DDialog(const QString &productName,
                                 const QString &reference,
                                 const QString &imagePath,
                                 QWidget *parent)
    : QDialog(parent),
      m_productName(productName),
      m_reference(reference),
      m_imagePath(imagePath),
      m_timerUpdate(nullptr)
{
    m_process = new QProcess(this);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &TripoSR3DDialog::onProcessOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &TripoSR3DDialog::onProcessErrorOutput);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &TripoSR3DDialog::onProcessFinished);

    setupUi();

    // ─── Check if a 3D model was already generated for this product ───
    checkExistingMesh();
}

TripoSR3DDialog::~TripoSR3DDialog()
{
    if (m_process && m_process->state() == QProcess::Running) {
        m_process->kill();
        m_process->waitForFinished(3000);
    }
}

QString TripoSR3DDialog::generatedMeshPath() const
{
    return m_generatedMeshPath;
}

void TripoSR3DDialog::setupUi()
{
    setWindowTitle(QString::fromUtf8("Generation Modele 3D — %1").arg(m_productName));
    resize(640, 520);
    setMinimumSize(560, 440);
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    setStyleSheet(
        "QDialog {"
        "  background: qlineargradient(x1:0, y1:0, x2:0.4, y2:1,"
        "    stop:0 #0f0c29, stop:0.5 #1a1145, stop:1 #0a0e1a);"
        "  color: white;"
        "}"
    );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(32, 28, 32, 24);
    mainLayout->setSpacing(16);

    // ─── Title Row ───
    QHBoxLayout *titleBar = new QHBoxLayout();
    m_lblTitle = new QLabel(QString::fromUtf8("Generateur 3D"), this);
    m_lblTitle->setStyleSheet(
        "color: #c4b5fd; font-size: 22px; font-weight: 800; font-family: 'Segoe UI';"
        "background: transparent; border: none;"
    );
    titleBar->addWidget(m_lblTitle);
    titleBar->addStretch();

    m_lblTimer = new QLabel("00:00", this);
    m_lblTimer->setStyleSheet(
        "color: #818cf8; font-size: 16px; font-weight: bold; font-family: 'Consolas';"
        "background: rgba(30,27,75,0.7); border: 1px solid rgba(129,140,248,0.25);"
        "border-radius: 12px; padding: 6px 16px;"
    );
    m_lblTimer->hide();
    titleBar->addWidget(m_lblTimer);
    mainLayout->addLayout(titleBar);

    // ─── Product Card ───
    QFrame *productCard = new QFrame(this);
    productCard->setStyleSheet(
        "QFrame {"
        "  background: rgba(30, 27, 75, 0.5);"
        "  border: 1px solid rgba(139, 92, 246, 0.2);"
        "  border-radius: 16px;"
        "}"
    );
    QHBoxLayout *cardLayout = new QHBoxLayout(productCard);
    cardLayout->setContentsMargins(16, 16, 16, 16);
    cardLayout->setSpacing(16);

    m_lblImagePreview = new QLabel(this);
    m_lblImagePreview->setFixedSize(100, 100);
    m_lblImagePreview->setStyleSheet(
        "background: rgba(139,92,246,0.08); border: 2px solid rgba(139,92,246,0.25);"
        "border-radius: 12px;"
    );
    m_lblImagePreview->setAlignment(Qt::AlignCenter);
    if (!m_imagePath.isEmpty() && QFile::exists(m_imagePath)) {
        QPixmap pix(m_imagePath);
        if (!pix.isNull()) {
            m_lblImagePreview->setPixmap(pix.scaled(90, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    } else {
        m_lblImagePreview->setText(QString::fromUtf8("Pas\nd'image"));
        m_lblImagePreview->setStyleSheet(
            m_lblImagePreview->styleSheet() + "color: #94a3b8; font-size: 11px;"
        );
    }
    cardLayout->addWidget(m_lblImagePreview);

    m_lblProductInfo = new QLabel(this);
    m_lblProductInfo->setTextFormat(Qt::RichText);
    m_lblProductInfo->setText(QString(
        "<div>"
        "<span style='color: #e0d4fd; font-weight: bold; font-size: 16px;'>%1</span><br>"
        "<span style='color: #8b8fa3; font-size: 12px;'>Ref: </span>"
        "<span style='color: #c4b5fd; font-size: 12px; font-family: Consolas;'>%2</span><br>"
        "<span style='color: #8b8fa3; font-size: 12px;'>Image: </span>"
        "<span style='color: %3; font-size: 11px;'>%4</span>"
        "</div>"
    ).arg(m_productName, m_reference,
          m_imagePath.isEmpty() ? "#ef4444" : "#a3e635",
          m_imagePath.isEmpty() ? "Aucune image" : QFileInfo(m_imagePath).fileName()));
    m_lblProductInfo->setWordWrap(true);
    m_lblProductInfo->setStyleSheet("background: transparent; border: none; padding: 4px;");
    cardLayout->addWidget(m_lblProductInfo, 1);
    mainLayout->addWidget(productCard);

    // ─── Status Card ───
    QFrame *statusCard = new QFrame(this);
    statusCard->setStyleSheet(
        "QFrame {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "    stop:0 rgba(30, 27, 75, 0.7), stop:1 rgba(15, 12, 41, 0.8));"
        "  border: 1px solid rgba(139, 92, 246, 0.18);"
        "  border-radius: 20px;"
        "}"
    );
    QVBoxLayout *statusLayout = new QVBoxLayout(statusCard);
    statusLayout->setContentsMargins(32, 28, 32, 28);
    statusLayout->setSpacing(16);
    statusLayout->setAlignment(Qt::AlignCenter);

    m_lblStatus = new QLabel(QString::fromUtf8(
        "Pret a generer le modele 3D"), this);
    m_lblStatus->setAlignment(Qt::AlignCenter);
    m_lblStatus->setWordWrap(true);
    m_lblStatus->setStyleSheet(
        "color: #c4b5fd; font-size: 16px; font-weight: 600;"
        "background: transparent; border: none;"
    );
    statusLayout->addWidget(m_lblStatus);

    // Hidden log console (kept for code compatibility, never shown)
    m_logConsole = new QPlainTextEdit(this);
    m_logConsole->setReadOnly(true);
    m_logConsole->hide();
    m_logConsole->setMaximumHeight(0);

    // Progress Bar
    m_progressBar = new QProgressBar(statusCard);
    m_progressBar->setRange(0, 0);
    m_progressBar->hide();
    m_progressBar->setFixedHeight(6);
    m_progressBar->setTextVisible(false);
    m_progressBar->setStyleSheet(
        "QProgressBar {"
        "  border: none; background-color: rgba(49, 46, 129, 0.35); border-radius: 3px;"
        "}"
        "QProgressBar::chunk {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #7c3aed, stop:0.5 #a78bfa, stop:1 #38bdf8);"
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
        "QPushButton { background: rgba(30,27,75,0.6); color: #a5a3c0;"
        "  border: 1px solid rgba(99,102,241,0.2); border-radius: 12px;"
        "  padding: 12px 22px; font-weight: 600; font-size: 13px; }"
        "QPushButton:hover { background: rgba(49,46,129,0.7); color: #c4b5fd;"
        "  border-color: rgba(139,92,246,0.4); }"
    );
    m_btnClose->setCursor(Qt::PointingHandCursor);
    connect(m_btnClose, &QPushButton::clicked, this, &TripoSR3DDialog::closeDialog);

    m_btnOpenMesh = new QPushButton(QString::fromUtf8(
        "\xF0\x9F\x93\x82  Ouvrir Mesh"), this);
    m_btnOpenMesh->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "  stop:0 #059669, stop:1 #10b981);"
        "  color: white; border: none; border-radius: 12px;"
        "  padding: 12px 22px; font-weight: bold; font-size: 13px; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "  stop:0 #10b981, stop:1 #34d399); }"
    );
    m_btnOpenMesh->setCursor(Qt::PointingHandCursor);
    m_btnOpenMesh->hide();
    connect(m_btnOpenMesh, &QPushButton::clicked, this, &TripoSR3DDialog::onOpenMeshClicked);

    m_btnViewer3D = new QPushButton(QString::fromUtf8(
        "\xF0\x9F\x91\x81  Visualiser 3D"), this);
    m_btnViewer3D->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "  stop:0 #2563eb, stop:1 #7c3aed);"
        "  color: white; border: none; border-radius: 12px;"
        "  padding: 12px 22px; font-weight: bold; font-size: 13px; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "  stop:0 #3b82f6, stop:1 #8b5cf6); }"
    );
    m_btnViewer3D->setCursor(Qt::PointingHandCursor);
    m_btnViewer3D->hide();
    connect(m_btnViewer3D, &QPushButton::clicked, this, &TripoSR3DDialog::onViewer3DClicked);

    m_btnGenerate = new QPushButton(QString::fromUtf8(
        "\xF0\x9F\x9A\x80  Generer Modele 3D"), this);
    m_btnGenerate->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "  stop:0 #7c3aed, stop:1 #6366f1);"
        "  color: white; border: none; border-radius: 12px;"
        "  padding: 12px 26px; font-weight: bold; font-size: 14px; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "  stop:0 #8b5cf6, stop:1 #818cf8); }"
        "QPushButton:disabled { background: rgba(30,27,75,0.5); color: #6366f1; }"
    );
    m_btnGenerate->setCursor(Qt::PointingHandCursor);
    connect(m_btnGenerate, &QPushButton::clicked, this, &TripoSR3DDialog::onGenerateClicked);

    btnLayout->addWidget(m_btnClose);
    btnLayout->addStretch();
    btnLayout->addWidget(m_btnOpenMesh);
    btnLayout->addWidget(m_btnViewer3D);
    btnLayout->addWidget(m_btnGenerate);
    mainLayout->addLayout(btnLayout);

    // ─── Disable generate if no image ───
    if (m_imagePath.isEmpty() || !QFile::exists(m_imagePath)) {
        m_btnGenerate->setEnabled(false);
        m_lblStatus->setText(QString::fromUtf8("Ce produit n'a pas d'image.\nAjoutez une image d'abord."));
        m_lblStatus->setStyleSheet("color: #ef4444; font-size: 14px; font-weight: bold; background: transparent; border: none;");
    }

    // ─── Fade In ───
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
    a->setDuration(400);
    a->setStartValue(0);
    a->setEndValue(1);
    a->setEasingCurve(QEasingCurve::OutCubic);
    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void TripoSR3DDialog::appendLog(const QString &text, const QString &color)
{
    if (color == "#a3e635") {
        m_logConsole->appendPlainText(text);
    } else {
        // For colored output, use HTML
        m_logConsole->appendHtml(
            QString("<span style='color: %1;'>%2</span>").arg(color, text.toHtmlEscaped())
        );
    }
}

QString TripoSR3DDialog::findTripoSRPath() const
{
    // Check if TripoSR directory exists
    const QString tripoDir = QString::fromUtf8(TRIPOSR_DIR);
    if (QDir(tripoDir).exists()) {
        return tripoDir;
    }
    return QString();
}

QString TripoSR3DDialog::resolveOutputMeshPath() const
{
    if (m_outputDir.isEmpty()) return QString();
    // TripoSR outputs to <output_dir>/0/mesh.obj
    QString meshPath = QDir(m_outputDir).absoluteFilePath("0/mesh.obj");
    if (QFile::exists(meshPath)) {
        return meshPath;
    }
    return QString();
}

void TripoSR3DDialog::checkExistingMesh()
{
    const QString tripoDir = findTripoSRPath();
    if (tripoDir.isEmpty()) return;

    // Build the expected output directory name (same logic as onGenerateClicked)
    const QString safeRef = m_reference.isEmpty()
        ? QString() // Can't check without reference
        : m_reference;
    if (safeRef.isEmpty()) return;

    m_outputDir = QDir(tripoDir).absoluteFilePath("output_wasteguard_" + safeRef);
    const QString meshPath = QDir(m_outputDir).absoluteFilePath("0/mesh.obj");

    if (QFile::exists(meshPath)) {
        m_generatedMeshPath = meshPath;

        QFileInfo meshInfo(meshPath);

        // Update status — clean, user-friendly message
        m_lblStatus->setText(QString::fromUtf8(
            "\xe2\x9c\x85 Modele 3D pret!\nCliquez sur Visualiser 3D pour l'explorer."));
        m_lblStatus->setStyleSheet(
            "color: #a3e635; font-size: 16px; font-weight: bold;"
            "background: transparent; border: none;");

        // Show action buttons
        m_btnOpenMesh->show();
        m_btnViewer3D->show();

        // Change Generate button to "Re-generate"
        m_btnGenerate->setText(QString::fromUtf8(
            "\xF0\x9F\x94\x84  Re-generer Modele 3D"));
    }
}

void TripoSR3DDialog::onGenerateClicked()
{
    m_btnGenerate->setEnabled(false);
    m_btnOpenMesh->hide();
    m_btnViewer3D->hide();
    m_progressBar->show();
    m_lblTimer->show();

    // ─── Verify TripoSR installation ───
    const QString tripoDir = findTripoSRPath();
    if (tripoDir.isEmpty()) {
        m_lblStatus->setText(QString::fromUtf8("Erreur: TripoSR introuvable.\nVerifiez l'installation."));
        m_lblStatus->setStyleSheet("color: #ef4444; font-size: 14px; font-weight: bold; background: transparent; border: none;");
        m_btnGenerate->setEnabled(true);
        m_progressBar->hide();
        m_lblTimer->hide();
        return;
    }

    // ─── Verify Python venv ───
    const QString pythonExe = QDir(tripoDir).absoluteFilePath("venv/Scripts/python.exe");
    if (!QFile::exists(pythonExe)) {
        m_lblStatus->setText(QString::fromUtf8("Erreur: Environnement Python introuvable."));
        m_lblStatus->setStyleSheet("color: #ef4444; font-size: 14px; font-weight: bold; background: transparent; border: none;");
        m_btnGenerate->setEnabled(true);
        m_progressBar->hide();
        m_lblTimer->hide();
        return;
    }

    // ─── Verify image exists ───
    if (!QFile::exists(m_imagePath)) {
        m_lblStatus->setText(QString::fromUtf8("Erreur: Image du produit introuvable."));
        m_lblStatus->setStyleSheet("color: #ef4444; font-size: 14px; font-weight: bold; background: transparent; border: none;");
        m_btnGenerate->setEnabled(true);
        m_progressBar->hide();
        m_lblTimer->hide();
        return;
    }

    // ─── Copy image to TripoSR examples folder ───
    const QString examplesDir = QDir(tripoDir).absoluteFilePath("examples");
    QDir().mkpath(examplesDir);

    const QString safeRef = m_reference.isEmpty()
        ? "product_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss")
        : m_reference;
    const QString ext = QFileInfo(m_imagePath).suffix();
    const QString destImageName = safeRef + "." + ext;
    const QString destImagePath = QDir(examplesDir).absoluteFilePath(destImageName);

    // Copy the image
    if (QFile::exists(destImagePath)) {
        QFile::remove(destImagePath);
    }
    if (!QFile::copy(m_imagePath, destImagePath)) {
        qDebug() << "[TripoSR] Could not copy image, using original path";
    }

    // ─── Build output directory name ───
    m_outputDir = QDir(tripoDir).absoluteFilePath(
        "output_wasteguard_" + safeRef
    );

    // ─── Build the command ───
    const QString runScript = QDir(tripoDir).absoluteFilePath("run.py");
    if (!QFile::exists(runScript)) {
        m_lblStatus->setText(QString::fromUtf8("Erreur: Script TripoSR introuvable."));
        m_lblStatus->setStyleSheet("color: #ef4444; font-size: 14px; font-weight: bold; background: transparent; border: none;");
        m_btnGenerate->setEnabled(true);
        m_progressBar->hide();
        m_lblTimer->hide();
        return;
    }

    // Use the copied image path (relative to TripoSR dir)
    const QString imageArg = QFile::exists(destImagePath)
        ? QString("examples/%1").arg(destImageName)
        : m_imagePath;

    m_lblStatus->setText(QString::fromUtf8("Initialisation de TripoSR..."));
    m_lblStatus->setStyleSheet(
        "color: #c4b5fd; font-size: 18px; font-weight: bold;"
        "background: transparent; border: none;");

    // ─── Start elapsed timer ───
    m_elapsedTimer.start();
    if (!m_timerUpdate) {
        m_timerUpdate = new QTimer(this);
        connect(m_timerUpdate, &QTimer::timeout, this, &TripoSR3DDialog::updateElapsedTime);
    }
    m_timerUpdate->start(1000);

    // ─── Launch the process ───
    QStringList arguments;
    arguments << runScript
              << imageArg
              << "--output-dir" << m_outputDir
              << "--bake-texture"
              << "--texture-resolution" << "4096";

    m_process->setWorkingDirectory(tripoDir);

    // Set environment to use the venv
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("VIRTUAL_ENV", QDir(tripoDir).absoluteFilePath("venv"));
    const QString venvScripts = QDir(tripoDir).absoluteFilePath("venv/Scripts");
    env.insert("PATH", venvScripts + ";" + env.value("PATH"));
    m_process->setProcessEnvironment(env);

    m_process->start(pythonExe, arguments);

    if (!m_process->waitForStarted(5000)) {
        m_lblStatus->setText(QString::fromUtf8("Erreur: Impossible de demarrer la generation."));
        m_lblStatus->setStyleSheet("color: #ef4444; font-size: 14px; font-weight: bold; background: transparent; border: none;");
        m_btnGenerate->setEnabled(true);
        m_progressBar->hide();
        m_lblTimer->hide();
        if (m_timerUpdate) m_timerUpdate->stop();
    }
}

void TripoSR3DDialog::onProcessOutput()
{
    const QByteArray output = m_process->readAllStandardOutput();
    if (!output.isEmpty()) {
        const QString text = QString::fromUtf8(output).trimmed();
        // Show user-friendly progress stages
        if (text.contains("Initializing model"))
            m_lblStatus->setText(QString::fromUtf8("Initialisation du modele IA..."));
        else if (text.contains("Processing images"))
            m_lblStatus->setText(QString::fromUtf8("Analyse de l'image..."));
        else if (text.contains("Running model"))
            m_lblStatus->setText(QString::fromUtf8("Generation 3D en cours..."));
        else if (text.contains("Extracting mesh"))
            m_lblStatus->setText(QString::fromUtf8("Extraction du maillage 3D..."));
        else if (text.contains("Baking texture"))
            m_lblStatus->setText(QString::fromUtf8("Application des textures..."));
        else if (text.contains("Exporting mesh"))
            m_lblStatus->setText(QString::fromUtf8("Exportation du modele..."));
    }
}

void TripoSR3DDialog::onProcessErrorOutput()
{
    const QByteArray errBytes = m_process->readAllStandardError();
    if (!errBytes.isEmpty()) {
        const QString text = QString::fromUtf8(errBytes).trimmed();
        // Show progress stages from stderr (TripoSR logs to stderr)
        if (text.contains("Initializing model"))
            m_lblStatus->setText(QString::fromUtf8("Initialisation du modele IA..."));
        else if (text.contains("Processing images"))
            m_lblStatus->setText(QString::fromUtf8("Analyse de l'image..."));
        else if (text.contains("Running model"))
            m_lblStatus->setText(QString::fromUtf8("Generation 3D en cours..."));
        else if (text.contains("Extracting mesh"))
            m_lblStatus->setText(QString::fromUtf8("Extraction du maillage 3D..."));
        else if (text.contains("Baking texture"))
            m_lblStatus->setText(QString::fromUtf8("Application des textures..."));
        else if (text.contains("Exporting mesh"))
            m_lblStatus->setText(QString::fromUtf8("Exportation du modele..."));
        // Errors are silently logged to debug, not shown to user
        if (text.contains("ERROR") || text.contains("Traceback")) {
            qWarning() << "[TripoSR]" << text;
        }
    }
}

void TripoSR3DDialog::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_progressBar->hide();
    m_btnGenerate->setEnabled(true);
    if (m_timerUpdate) m_timerUpdate->stop();

    const qint64 elapsed = m_elapsedTimer.elapsed();
    const int secs = static_cast<int>(elapsed / 1000);
    const int mins = secs / 60;
    const int remSecs = secs % 60;

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        m_generatedMeshPath = resolveOutputMeshPath();

        if (!m_generatedMeshPath.isEmpty()) {
            QString materialError;
            prepareTexturedMeshBundle(m_generatedMeshPath, &materialError);

            m_lblStatus->setText(QString::fromUtf8(
                "\xe2\x9c\x85 Modele 3D genere avec succes!\nTemps: %1:%2")
                .arg(mins, 2, 10, QChar('0'))
                .arg(remSecs, 2, 10, QChar('0')));
            m_lblStatus->setStyleSheet(
                "color: #a3e635; font-size: 16px; font-weight: bold;"
                "background: transparent; border: none;");

            m_btnOpenMesh->show();
            m_btnViewer3D->show();
            m_btnGenerate->setText(QString::fromUtf8(
                "\xF0\x9F\x94\x84  Re-generer Modele 3D"));
        } else {
            m_lblStatus->setText(QString::fromUtf8(
                "\xe2\x9a\xa0 Generation terminee mais fichier introuvable."));
            m_lblStatus->setStyleSheet(
                "color: #fbbf24; font-size: 14px; font-weight: bold;"
                "background: transparent; border: none;");
        }
    } else {
        m_lblStatus->setText(QString::fromUtf8(
            "Erreur lors de la generation.\nReessayez ou verifiez la configuration."));
        m_lblStatus->setStyleSheet(
            "color: #ef4444; font-size: 14px; font-weight: bold;"
            "background: transparent; border: none;");
    }
}

void TripoSR3DDialog::onOpenMeshClicked()
{
    if (m_generatedMeshPath.isEmpty() || !QFile::exists(m_generatedMeshPath)) {
        QMessageBox::warning(this, "Fichier introuvable",
            "Le fichier mesh.obj n'a pas ete trouve.\nRelancez la generation.");
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_generatedMeshPath));
}

void TripoSR3DDialog::onViewer3DClicked()
{
    if (!m_btnViewer3D || !m_btnViewer3D->isEnabled()) return;

    if (m_generatedMeshPath.isEmpty() || !QFile::exists(m_generatedMeshPath)) {
        QMessageBox::warning(this, "Fichier introuvable",
            "Le fichier mesh.obj n'a pas ete trouve.\nRelancez la generation.");
        return;
    }

    appendLog(QString("Ouverture du Viewer 3D interactif: %1").arg(m_generatedMeshPath), "#a78bfa");
    m_btnViewer3D->setEnabled(false);

    // Create the viewer as a child of this dialog so it stays alive
    Viewer3DDialog *viewer = new Viewer3DDialog(m_generatedMeshPath, m_productName, this);
    viewer->setAttribute(Qt::WA_DeleteOnClose, true);

    // Re-enable button when viewer closes (normal path)
    connect(viewer, &QDialog::finished, this, [this](int) {
        if (m_btnViewer3D) m_btnViewer3D->setEnabled(true);
    });
    // Re-enable button if viewer is destroyed without finishing (crash path)
    connect(viewer, &QObject::destroyed, this, [this]() {
        if (m_btnViewer3D) m_btnViewer3D->setEnabled(true);
    });

    // Safety timer: re-enable button after 5 seconds no matter what
    // (covers silent OpenGL crash where neither signal fires)
    QTimer::singleShot(5000, this, [this, viewer]() {
        if (m_btnViewer3D && !m_btnViewer3D->isEnabled()) {
            // Check if viewer is still alive
            if (!viewer || !viewer->isVisible()) {
                m_btnViewer3D->setEnabled(true);
                qWarning() << "[TripoSR] Viewer 3D did not open correctly";
            }
        }
    });

    viewer->show();
    viewer->raise();
    viewer->activateWindow();
}

void TripoSR3DDialog::closeDialog()
{
    if (m_process && m_process->state() == QProcess::Running) {
        const int choice = QMessageBox::question(this,
            "Generation en cours",
            "La generation 3D est encore en cours.\nVoulez-vous vraiment annuler?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (choice != QMessageBox::Yes) return;

        m_process->kill();
        m_process->waitForFinished(3000);
    }

    // Fade out
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
    a->setDuration(300);
    a->setStartValue(1);
    a->setEndValue(0);
    a->setEasingCurve(QEasingCurve::OutCubic);
    connect(a, &QPropertyAnimation::finished, this, &QDialog::accept);
    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void TripoSR3DDialog::updateElapsedTime()
{
    const qint64 elapsed = m_elapsedTimer.elapsed();
    const int secs = static_cast<int>(elapsed / 1000);
    const int mins = secs / 60;
    const int remSecs = secs % 60;
    m_lblTimer->setText(QString("%1:%2")
        .arg(mins, 2, 10, QChar('0'))
        .arg(remSecs, 2, 10, QChar('0')));
}
