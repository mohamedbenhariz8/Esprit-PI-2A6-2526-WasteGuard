#include "mainwindow.h"
#include "mascotwidget.h"

#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QPropertyAnimation>
#include <QEvent>
#include <QMouseEvent>
#include <QStackedWidget>
#include <QTimer>
#include <QMediaCaptureSession>
#include <QCamera>
#include <QMediaDevices>
#include <QVideoSink>
#include <QVideoFrame>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QImage>
#include <QPixmap>
#include <QVector>
#include <QStringList>
#include <QBuffer>
#include <QByteArray>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslError>
#include <QSslSocket>
#include <QtMath>
#include <QDebug>
#include <cstdio>

#include "connection.h"

namespace {

QtMessageHandler g_previousMessageHandler = nullptr;

void filteredQtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (msg.startsWith("QPainter::end: Painter ended with")
        || msg.startsWith("QWidget::setMaximumSize: (prod_rightSidebar/QFrame) The largest allowed size is")
        || msg.startsWith("nmea: No known GPS device found.")) {
        return;
    }

    if (g_previousMessageHandler) {
        g_previousMessageHandler(type, context, msg);
        return;
    }

    const QByteArray localMsg = msg.toLocal8Bit();
    std::fprintf(stderr, "%s\n", localMsg.constData());
}

bool hasMissingColumnError(const QString &errorText)
{
    const QString u = errorText.toUpper();
    return u.contains("ORA-00904")
        || u.contains("INVALID IDENTIFIER")
        || u.contains("NO SUCH COLUMN")
        || u.contains("UNKNOWN COLUMN");
}

void ensureFaceColumnsExist()
{
    static bool checked = false;
    if (checked) return;

    QSqlQuery probe;
    if (probe.exec("SELECT face_template, face_template_updated_at, face_enabled FROM EMPLOYE WHERE 1=0")) {
        checked = true;
        return;
    }

    if (!hasMissingColumnError(probe.lastError().text())) {
        checked = true;
        return;
    }

    QSqlQuery q;
    q.exec("ALTER TABLE EMPLOYE ADD (face_template CLOB)");
    q.exec("ALTER TABLE EMPLOYE ADD (face_template_updated_at DATE)");
    q.exec("ALTER TABLE EMPLOYE ADD (face_enabled NUMBER(1) DEFAULT 1)");
    q.exec("UPDATE EMPLOYE SET face_enabled = 1 WHERE face_enabled IS NULL");
    checked = true;
}

void ensurePhotoColumnExists()
{
    static bool checked = false;
    if (checked) return;

    QSqlQuery probe;
    if (probe.exec("SELECT photo FROM EMPLOYE WHERE 1=0")) {
        checked = true;
        return;
    }

    if (!hasMissingColumnError(probe.lastError().text())) {
        checked = true;
        return;
    }

    QSqlQuery q;
    q.exec("ALTER TABLE EMPLOYE ADD (photo BLOB)");
    checked = true;
}

QImage makeFaceCrop(const QImage &src)
{
    if (src.isNull()) return QImage();

    QImage gray = src.convertToFormat(QImage::Format_Grayscale8);
    const int minSide = qMin(gray.width(), gray.height());
    if (minSide < 40) return QImage();

    const int cropSide = qMax(40, static_cast<int>(minSide * 0.62));
    const int x = (gray.width() - cropSide) / 2;
    const int y = (gray.height() - cropSide) / 2;
    const QRect roi(x, y, cropSide, cropSide);

    return gray.copy(roi).scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

QVector<float> computeFaceDescriptor(const QImage &src)
{
    QImage img = makeFaceCrop(src);
    if (img.isNull()) return {};

    QVector<float> descriptor;
    descriptor.reserve(16 + 64);

    float hist[16] = {0.0f};
    for (int y = 0; y < img.height(); ++y) {
        const uchar *row = img.constScanLine(y);
        for (int x = 0; x < img.width(); ++x) {
            const int bin = row[x] / 16;
            hist[bin] += 1.0f;
        }
    }

    const float totalPix = static_cast<float>(img.width() * img.height());
    for (float &v : hist) {
        descriptor.append(v / totalPix);
    }

    const int cell = 8; // 8x8 grid, each cell 8x8 px.
    for (int gy = 0; gy < 8; ++gy) {
        for (int gx = 0; gx < 8; ++gx) {
            float sum = 0.0f;
            for (int y = gy * cell; y < (gy + 1) * cell; ++y) {
                const uchar *row = img.constScanLine(y);
                for (int x = gx * cell; x < (gx + 1) * cell; ++x) {
                    sum += static_cast<float>(row[x]) / 255.0f;
                }
            }
            descriptor.append(sum / static_cast<float>(cell * cell));
        }
    }

    float norm = 0.0f;
    for (const float v : descriptor) norm += v * v;
    norm = qSqrt(norm);
    if (norm > 1e-6f) {
        for (float &v : descriptor) v /= norm;
    }
    return descriptor;
}

QString descriptorToString(const QVector<float> &desc)
{
    QStringList parts;
    parts.reserve(desc.size());
    for (const float v : desc) {
        parts << QString::number(static_cast<double>(v), 'f', 6);
    }
    return parts.join(';');
}

QVector<float> descriptorFromString(const QString &raw)
{
    QVector<float> out;
    const QStringList parts = raw.split(';', Qt::SkipEmptyParts);
    out.reserve(parts.size());
    for (const QString &p : parts) {
        bool ok = false;
        const float v = p.toFloat(&ok);
        if (ok) out.append(v);
    }
    return out;
}

double cosineSimilarity(const QVector<float> &a, const QVector<float> &b)
{
    if (a.isEmpty() || b.isEmpty() || a.size() != b.size()) return -1.0;
    double dot = 0.0;
    double na = 0.0;
    double nb = 0.0;
    for (int i = 0; i < a.size(); ++i) {
        dot += static_cast<double>(a[i]) * static_cast<double>(b[i]);
        na += static_cast<double>(a[i]) * static_cast<double>(a[i]);
        nb += static_cast<double>(b[i]) * static_cast<double>(b[i]);
    }
    if (na <= 1e-12 || nb <= 1e-12) return -1.0;
    return dot / qSqrt(na * nb);
}

bool isFaceApiEnabled()
{
    const QString key = qEnvironmentVariable("WG_FACE_API_KEY").trimmed();
    const QString secret = qEnvironmentVariable("WG_FACE_API_SECRET").trimmed();
    return !key.isEmpty() && !secret.isEmpty();
}

QByteArray toJpegBytes(const QImage &image, int quality = 92)
{
    if (image.isNull()) return QByteArray();

    QImage prepared = image;
    const int maxSide = 720;
    if (prepared.width() > maxSide || prepared.height() > maxSide) {
        prepared = prepared.scaled(maxSide, maxSide, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    QByteArray data;
    QBuffer buffer(&data);
    if (!buffer.open(QIODevice::WriteOnly)) {
        return QByteArray();
    }
    if (!prepared.save(&buffer, "JPG", quality)) {
        return QByteArray();
    }
    return data;
}

QByteArray normalizeImageBytesForApi(const QByteArray &rawImage, QString &errorText)
{
    errorText.clear();
    if (rawImage.isEmpty()) {
        errorText = "Image vide.";
        return QByteArray();
    }

    QImage img;
    if (!img.loadFromData(rawImage)) {
        errorText = "Image invalide pour Face API.";
        return QByteArray();
    }

    QByteArray encoded = toJpegBytes(img, 85);
    if (encoded.isEmpty()) {
        errorText = "Conversion image API impossible.";
        return QByteArray();
    }

    const int maxBytes = 900 * 1024;
    if (encoded.size() > maxBytes) {
        QImage scaled = img.scaled(560, 560, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        encoded = toJpegBytes(scaled, 72);
    }

    if (encoded.isEmpty()) {
        errorText = "Compression image API impossible.";
        return QByteArray();
    }

    return encoded;
}

bool compareFacesWithApi(const QByteArray &probeImage,
                         const QByteArray &referenceImage,
                         double &confidence,
                         double &strictThreshold,
                         QString &errorText)
{
    confidence = -1.0;
    strictThreshold = -1.0;
    errorText.clear();

    const QString key = qEnvironmentVariable("WG_FACE_API_KEY").trimmed();
    const QString secret = qEnvironmentVariable("WG_FACE_API_SECRET").trimmed();
    const QString endpoint = qEnvironmentVariable(
        "WG_FACE_API_URL",
        "https://api-us.faceplusplus.com/facepp/v3/compare").trimmed();

    if (key.isEmpty() || secret.isEmpty()) {
        errorText = "Face API non configuree.";
        return false;
    }
    if (probeImage.isEmpty() || referenceImage.isEmpty()) {
        errorText = "Image visage manquante pour la verification API.";
        return false;
    }
    if (!QSslSocket::supportsSsl()) {
        errorText = "SSL/TLS indisponible sur ce poste. Installez les DLL OpenSSL puis relancez l'application.";
        return false;
    }

    QString normalizeError;
    const QByteArray probeNormalized = normalizeImageBytesForApi(probeImage, normalizeError);
    if (probeNormalized.isEmpty()) {
        errorText = "Face API: image camera invalide. " + normalizeError;
        return false;
    }
    const QByteArray referenceNormalized = normalizeImageBytesForApi(referenceImage, normalizeError);
    if (referenceNormalized.isEmpty()) {
        errorText = "Face API: photo de reference invalide. " + normalizeError;
        return false;
    }

    const QUrl endpointUrl(endpoint);
    QNetworkRequest req{endpointUrl};

    auto *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    auto appendTextPart = [multiPart](const QString &name, const QString &value) {
        QHttpPart part;
        part.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QString("form-data; name=\"%1\"").arg(name));
        part.setBody(value.toUtf8());
        multiPart->append(part);
    };
    auto appendImagePart = [multiPart](const QString &name, const QByteArray &imageBytes, const QString &filename) {
        QHttpPart part;
        part.setHeader(QNetworkRequest::ContentTypeHeader, "image/jpeg");
        part.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QString("form-data; name=\"%1\"; filename=\"%2\"").arg(name, filename));
        part.setBody(imageBytes);
        multiPart->append(part);
    };

    appendTextPart("api_key", key);
    appendTextPart("api_secret", secret);
    appendImagePart("image_file1", probeNormalized, "probe.jpg");
    appendImagePart("image_file2", referenceNormalized, "reference.jpg");

    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply = manager.post(req, multiPart);
    multiPart->setParent(reply);
    QStringList sslErrors;
    QObject::connect(reply, &QNetworkReply::sslErrors, &loop, [&](const QList<QSslError> &errors) {
        sslErrors.clear();
        for (const QSslError &e : errors) {
            sslErrors << e.errorString();
        }
    });
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    QObject::connect(&timeoutTimer, &QTimer::timeout, &loop, [&]() {
        if (reply && reply->isRunning()) {
            reply->abort();
        }
        loop.quit();
    });
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timeoutTimer.start(8000);
    loop.exec();
    timeoutTimer.stop();

    const QByteArray body = reply->readAll();
    const QJsonDocument docForError = QJsonDocument::fromJson(body);
    const QString apiBodyError = docForError.isObject()
                                     ? docForError.object().value("error_message").toString().trimmed()
                                     : QString();
    if (reply->error() != QNetworkReply::NoError) {
        const auto netErr = reply->error();
        if (!apiBodyError.isEmpty()) {
            errorText = "Face API: " + apiBodyError;
        } else if (netErr == QNetworkReply::HostNotFoundError) {
            errorText = "Face API: hote introuvable. Verifiez internet/DNS.";
        } else if (netErr == QNetworkReply::TimeoutError || netErr == QNetworkReply::OperationCanceledError) {
            errorText = "Face API: delai depasse. Reessayez dans quelques secondes.";
        } else if (netErr == QNetworkReply::SslHandshakeFailedError || !sslErrors.isEmpty()) {
            const QString sslDetail = sslErrors.isEmpty() ? reply->errorString() : sslErrors.join(" | ");
            errorText = "Face API: echec SSL/TLS (" + sslDetail + ").";
        } else {
            errorText = "Face API indisponible: " + reply->errorString();
        }
        reply->deleteLater();
        return false;
    }
    reply->deleteLater();

    QJsonParseError jsonError;
    const QJsonDocument doc = QJsonDocument::fromJson(body, &jsonError);
    if (jsonError.error != QJsonParseError::NoError || !doc.isObject()) {
        errorText = "Face API: reponse JSON invalide.";
        return false;
    }

    const QJsonObject obj = doc.object();
    const QString apiError = obj.value("error_message").toString().trimmed();
    if (!apiError.isEmpty()) {
        if (apiError.contains("BAD_ARGUMENTS", Qt::CaseInsensitive) &&
            (apiError.contains("image_base64", Qt::CaseInsensitive) ||
             apiError.contains("image_file", Qt::CaseInsensitive))) {
            errorText = "Face API refuse l'image (format/taille). Recadrez puis reessayez.";
            return false;
        }
        errorText = "Face API: " + apiError;
        return false;
    }

    confidence = obj.value("confidence").toDouble(-1.0);
    const QJsonObject thresholds = obj.value("thresholds").toObject();
    strictThreshold = thresholds.value("1e-5").toDouble(-1.0);
    if (strictThreshold < 0.0) strictThreshold = thresholds.value("1e-4").toDouble(-1.0);
    if (strictThreshold < 0.0) strictThreshold = thresholds.value("1e-3").toDouble(-1.0);
    return confidence >= 0.0;
}

} // namespace

class LoginDialog : public QDialog
{
public:
    LoginDialog(QWidget *parent = nullptr) : QDialog(parent)
    {
        setWindowTitle("WasteGuard - Connexion [gestion-employee]");
        setMinimumSize(450, 650);
        setStyleSheet("QDialog { background-color: #1F110B; }");

        setFixedSize(450, 600);
        setWindowFlags(Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);

        const QString blueBase = "#0F1A2B";
        const QString accentBlue = "#3498DB";
        const QString lightBlue = "#D4ECFF";

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        QFrame *container = new QFrame(this);
        container->setObjectName("loginContainer");
        container->setStyleSheet(QString(
            "QFrame#loginContainer {"
            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2);"
            "   border: 2px solid rgba(52, 152, 219, 0.3);"
            "   border-radius: 40px;"
            "}"
            "QLineEdit {"
            "   background: rgba(255, 255, 255, 0.08);"
            "   border: 1px solid rgba(52, 152, 219, 0.4);"
            "   border-radius: 12px;"
            "   padding: 12px;"
            "   color: white;"
            "   font-size: 14px;"
            "}"
            "QLineEdit:focus {"
            "   border: 2px solid %3;"
            "   background: rgba(255, 255, 255, 0.12);"
            "}"
            "QPushButton#btnLogin {"
            "   background: %3;"
            "   color: white;"
            "   border-radius: 15px;"
            "   padding: 15px;"
            "   font-weight: 800;"
            "   font-size: 16px;"
            "}"
            "QPushButton#btnLogin:hover {"
            "   background: #2980B9;"
            "}"
        ).arg(blueBase).arg("#1A2F45").arg(accentBlue));

        QVBoxLayout *containerLayout = new QVBoxLayout(container);
        containerLayout->setContentsMargins(40, 20, 40, 40);
        containerLayout->setSpacing(15);

        mascot = new MascotWidget(this);
        mascot->setMinimumHeight(240);
        containerLayout->addWidget(mascot);

        QLabel *lblMascotName = new QLabel("LABIB", this);
        lblMascotName->setAlignment(Qt::AlignCenter);
        lblMascotName->setStyleSheet("font-size: 14px; font-weight: 800; color: #00F2FE; letter-spacing: 4px; margin-top: 5px; margin-bottom: 15px;");
        containerLayout->addWidget(lblMascotName);

        stackedWidget = new QStackedWidget(this);

        QWidget *pageLogin = new QWidget();
        QVBoxLayout *loginLayout = new QVBoxLayout(pageLogin);
        loginLayout->setSpacing(14);

        QLabel *lblTitle = new QLabel("ACCES SECURISE - EMAIL/CIN", this);
        lblTitle->setStyleSheet(QString("font-size: 20px; font-weight: 800; color: %1; letter-spacing: 2px;").arg(accentBlue));
        lblTitle->setAlignment(Qt::AlignCenter);
        loginLayout->addWidget(lblTitle);
        loginLayout->addSpacing(6);

        txtUser = new QLineEdit(this);
        txtUser->setPlaceholderText("Email");
        txtUser->setMinimumHeight(48);
        loginLayout->addWidget(txtUser);
        loginLayout->addSpacing(4);

        txtPass = new QLineEdit(this);
        txtPass->setPlaceholderText("CIN / Mot de passe admin");
        txtPass->setEchoMode(QLineEdit::Password);
        txtPass->setMinimumHeight(48);
        loginLayout->addWidget(txtPass);
        loginLayout->addSpacing(8);

        QPushButton *btnLogin = new QPushButton("CONNEXION", this);
        btnLogin->setObjectName("btnLogin");
        btnLogin->setMinimumHeight(52);
        btnLogin->setCursor(Qt::PointingHandCursor);
        loginLayout->addWidget(btnLogin);
        loginLayout->addSpacing(6);

        QPushButton *btnFaceId = new QPushButton("CONNEXION PAR VISAGE", this);
        btnFaceId->setStyleSheet(QString(
            "QPushButton { background: transparent; color: %1; font-weight: 700; "
            "font-size: 13px; border-radius: 10px; padding: 10px; border: 1px solid rgba(52, 152, 219, 0.5); } "
            "QPushButton:hover { background-color: rgba(52, 152, 219, 0.1); }"
        ).arg(accentBlue));
        btnFaceId->setMinimumHeight(45);
        btnFaceId->setCursor(Qt::PointingHandCursor);
        loginLayout->addWidget(btnFaceId);
        loginLayout->addSpacing(4);

        QLabel *lblAuthModes = new QLabel("Choix: (1) Email + CIN, ou (2) Visage + Email (CIN requis seulement a la 1ere inscription)", this);
        lblAuthModes->setAlignment(Qt::AlignCenter);
        lblAuthModes->setStyleSheet("color: #93c5fd; font-size: 12px;");
        loginLayout->addWidget(lblAuthModes);

        QWidget *pageFaceId = new QWidget();
        QVBoxLayout *faceLayout = new QVBoxLayout(pageFaceId);
        faceLayout->setSpacing(15);

        QLabel *lblFaceTitle = new QLabel("SCAN BIOMETRIQUE", this);
        lblFaceTitle->setStyleSheet("font-size: 18px; font-weight: 800; color: #E9BC99;");
        lblFaceTitle->setAlignment(Qt::AlignCenter);
        faceLayout->addWidget(lblFaceTitle);

        facePreview = new QLabel(this);
        facePreview->setMinimumSize(320, 240);
        facePreview->setAlignment(Qt::AlignCenter);
        facePreview->setText("Camera non demarree");
        facePreview->setStyleSheet(QString("background-color: rgba(0,0,0,0.5); border: 2px solid %1; border-radius: 15px; color: #dbeafe; font-size: 13px;").arg(accentBlue));
        faceLayout->addWidget(facePreview);

        lblStatus = new QLabel("Initialisation du capteur...", this);
        lblStatus->setStyleSheet(QString("font-size: 14px; color: %1;").arg(lightBlue));
        lblStatus->setAlignment(Qt::AlignCenter);
        faceLayout->addWidget(lblStatus);

        btnFaceCapture = new QPushButton("Capturer et verifier", this);
        btnFaceCapture->setObjectName("btnFaceCapture");
        btnFaceCapture->setMinimumHeight(44);
        btnFaceCapture->setCursor(Qt::PointingHandCursor);
        btnFaceCapture->setStyleSheet(QString(
            "QPushButton#btnFaceCapture { background: %1; color: white; border-radius: 10px; font-weight: 700; }"
            "QPushButton#btnFaceCapture:hover { background: #2563eb; }"
            "QPushButton#btnFaceCapture:disabled { background: #64748b; }"
        ).arg(accentBlue));
        faceLayout->addWidget(btnFaceCapture);

        QPushButton *btnBack = new QPushButton("Annuler et revenir", this);
        btnBack->setStyleSheet(QString("color: %1; text-decoration: underline; font-size: 12px; border: none;").arg(accentBlue));
        btnBack->setFlat(true);
        btnBack->setCursor(Qt::PointingHandCursor);
        faceLayout->addWidget(btnBack);

        stackedWidget->addWidget(pageLogin);
        stackedWidget->addWidget(pageFaceId);
        stackedWidget->setCurrentIndex(0);

        containerLayout->addWidget(stackedWidget);
        mainLayout->addWidget(container);

        eyeAnim = new QPropertyAnimation(mascot, "eyeCoverFactor", this);
        eyeAnim->setDuration(450);
        eyeAnim->setEasingCurve(QEasingCurve::InOutBack);

        txtPass->installEventFilter(this);
        txtUser->installEventFilter(this);
        setMouseTracking(true);
        pageLogin->setMouseTracking(true);

        connect(btnLogin, &QPushButton::clicked, this, &LoginDialog::attemptLogin);
        connect(txtPass, &QLineEdit::returnPressed, this, &LoginDialog::attemptLogin);
        connect(txtUser, &QLineEdit::returnPressed, this, [this]() {
            txtPass->setFocus();
        });

        connect(btnFaceId, &QPushButton::clicked, this, [this]() {
            startFaceId();
            mascot->setState(MascotWidget::Magnifier);
        });

        connect(btnBack, &QPushButton::clicked, this, [this]() {
            stopFaceId();
            stackedWidget->setCurrentIndex(0);
            mascot->setState(MascotWidget::Idle);
            lblStatus->setText("Initialisation du capteur...");
            lblStatus->setStyleSheet("font-size: 14px; color: #EBF5FB;");
        });

        connect(btnFaceCapture, &QPushButton::clicked, this, &LoginDialog::attemptFaceLogin);

        connect(txtUser, &QLineEdit::textEdited, this, [this]() {
            mascot->setState(MascotWidget::Notepad);
            QTimer::singleShot(1500, this, [this]() {
                if (!txtUser->hasFocus()) {
                    mascot->setState(MascotWidget::Idle);
                }
            });
        });
    }

    bool isAuthenticated() const
    {
        return m_authenticated;
    }

    bool isAdminLogin() const
    {
        return m_isAdminLogin;
    }

    QString authenticatedEmail() const
    {
        return m_authenticatedEmail;
    }

    bool eventFilter(QObject *obj, QEvent *event) override
    {
        if (obj == txtPass) {
            if (event->type() == QEvent::FocusIn) {
                eyeAnim->stop();
                eyeAnim->setEndValue(1.0);
                eyeAnim->start();
                mascot->setState(MascotWidget::Idle);
            } else if (event->type() == QEvent::FocusOut) {
                eyeAnim->stop();
                eyeAnim->setEndValue(0.0);
                eyeAnim->start();
            }
        } else if (obj == txtUser) {
            if (event->type() == QEvent::FocusIn) {
                mascot->setState(MascotWidget::Notepad);
            } else if (event->type() == QEvent::FocusOut) {
                mascot->setState(MascotWidget::Idle);
            }
        }
        return QDialog::eventFilter(obj, event);
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        mascot->setLookAt(event->globalPosition().toPoint());
        QDialog::mouseMoveEvent(event);
    }

    void accept() override
    {
        // Block any accidental accept path unless auth was explicitly validated.
        if (m_authenticated) {
            QDialog::accept();
        } else {
            attemptLogin();
        }
    }

private:
    void attemptLogin()
    {
        const QString email = txtUser->text().trimmed();
        const QString password = txtPass->text().trimmed();
        m_faceMatchedEmail.clear();

        if (email.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "Connexion", "Veuillez saisir l'email et le mot de passe.");
            return;
        }

        // Admin pass-partout
        if (email.compare("admin@wasteguard.com", Qt::CaseInsensitive) == 0 && password == "0000") {
            grantAccess(true);
            return;
        }

        Connection *connection = Connection::instance();
        if (!connection->isOpen() && !connection->createConnect()) {
            QMessageBox::critical(
                this,
                "Connexion base",
                "Impossible de joindre la base de donnees.\n\nDetails: " + connection->lastError());
            return;
        }

        QSqlQuery query;
        query.prepare(
            "SELECT COUNT(1) "
            "FROM EMPLOYE "
            "WHERE LOWER(TRIM(email)) = LOWER(TRIM(:email)) "
            "AND TRIM(cin) = TRIM(:cin)");
        query.bindValue(":email", email);
        query.bindValue(":cin", password);

        if (!query.exec()) {
            QMessageBox::critical(
                this,
                "Echec de connexion",
                "Erreur pendant la verification des identifiants.\n\nDetails: " + query.lastError().text());
            txtPass->clear();
            txtPass->setFocus();
            return;
        }

        int matched = 0;
        if (query.next()) {
            matched = query.value(0).toInt();
        }

        if (matched > 0) {
            grantAccess(false);
            return;
        }

        QMessageBox::warning(
            this,
            "Echec de connexion",
            "Email ou CIN invalide.\n\nUtilisez un email/CIN employe valide, ou admin@wasteguard.com / 0000.");
        txtPass->clear();
        txtPass->setFocus();
    }

    void startFaceId()
    {
        stackedWidget->setCurrentIndex(1);
        lblStatus->setText("Demarrage camera...");
        lblStatus->setStyleSheet("font-size: 15px; color: #E9BC99; font-weight: 700;");

        Connection *connection = Connection::instance();
        if (!connection->isOpen() && !connection->createConnect()) {
            lblStatus->setText("Base indisponible pour Face ID.");
            lblStatus->setStyleSheet("font-size: 14px; color: #ef4444; font-weight: 700;");
            return;
        }

        ensurePhotoColumnExists();
        ensureFaceColumnsExist();

        if (!videoSink) {
            videoSink = new QVideoSink(this);
            connect(videoSink, &QVideoSink::videoFrameChanged, this, [this](const QVideoFrame &frame) {
                if (!frame.isValid()) return;
                const QImage img = frame.toImage();
                if (img.isNull()) return;

                lastFrame = img;
                if (facePreview) {
                    const QPixmap px = QPixmap::fromImage(
                        img.scaled(facePreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    facePreview->setPixmap(px);
                }
            });
        }

        if (!captureSession) {
            captureSession = new QMediaCaptureSession(this);
        }

        if (!camera) {
            const QList<QCameraDevice> cams = QMediaDevices::videoInputs();
            if (cams.isEmpty()) {
                lblStatus->setText("Aucune camera detectee.");
                lblStatus->setStyleSheet("font-size: 14px; color: #ef4444; font-weight: 700;");
                return;
            }
            camera = new QCamera(cams.first(), this);
            captureSession->setCamera(camera);
            captureSession->setVideoSink(videoSink);
        }

        camera->start();
        if (isFaceApiEnabled()) {
            if (!QSslSocket::supportsSsl()) {
                lblStatus->setText("Mode IA 100% actif, mais SSL manquant. Installez OpenSSL (libssl/libcrypto).");
                lblStatus->setStyleSheet("font-size: 13px; color: #ef4444; font-weight: 700;");
                return;
            }
            lblStatus->setText("Mode IA 100% actif (API). Positionnez le visage et capturez.");
        } else {
            lblStatus->setText("Mode IA local actif. Positionnez le visage au centre puis cliquez Capturer.");
        }
        lblStatus->setStyleSheet("font-size: 14px; color: #dbeafe; font-weight: 600;");
    }

    void stopFaceId()
    {
        if (camera) camera->stop();
        lastFrame = QImage();
        if (facePreview) {
            facePreview->setPixmap(QPixmap());
            facePreview->setText("Camera arretee");
        }
    }

    void attemptFaceLogin()
    {
        const QString emailInput = txtUser ? txtUser->text().trimmed() : QString();
        const QString cinInput = txtPass ? txtPass->text().trimmed() : QString();
        if (emailInput.isEmpty()) {
            lblStatus->setText("Email obligatoire pour la connexion faciale.");
            lblStatus->setStyleSheet("font-size: 13px; color: #ef4444; font-weight: 700;");
            return;
        }

        // Admin pass-partout also supported from Face page.
        if (emailInput.compare("admin@wasteguard.com", Qt::CaseInsensitive) == 0 && cinInput == "0000") {
            lblStatus->setText("Acces admin valide.");
            lblStatus->setStyleSheet("font-size: 15px; color: #22c55e; font-weight: 800;");
            QTimer::singleShot(300, this, [this]() {
                stopFaceId();
                grantAccess(true);
            });
            return;
        }
        if (emailInput.compare("admin@wasteguard.com", Qt::CaseInsensitive) == 0) {
            lblStatus->setText("Pour admin, saisissez le code 0000.");
            lblStatus->setStyleSheet("font-size: 13px; color: #ef4444; font-weight: 700;");
            return;
        }

        if (lastFrame.isNull()) {
            lblStatus->setText("Aucune image captee. Verifiez la camera.");
            lblStatus->setStyleSheet("font-size: 14px; color: #ef4444; font-weight: 700;");
            return;
        }

        QString accountName;
        int accountId = -1;
        QByteArray accountPhoto;
        QString verifyError;
        double faceScore = -1.0;
        bool hasTemplate = false;
        const bool apiOnly = isFaceApiEnabled();
        if (apiOnly) {
            const QVector<float> emptyDesc;
            if (!verifyFaceForEmail(emailInput, emptyDesc, accountName, accountId, accountPhoto, faceScore, hasTemplate, verifyError)) {
                lblStatus->setText(verifyError.isEmpty() ? "Compte introuvable pour cet email." : verifyError);
                lblStatus->setStyleSheet("font-size: 13px; color: #ef4444; font-weight: 700;");
                return;
            }

            const QByteArray probeJpeg = toJpegBytes(lastFrame);
            if (probeJpeg.isEmpty()) {
                lblStatus->setText("Image visage invalide pour la verification IA.");
                lblStatus->setStyleSheet("font-size: 13px; color: #ef4444; font-weight: 700;");
                return;
            }
            if (accountPhoto.isEmpty()) {
                lblStatus->setText("Photo de reference absente pour ce compte. Re-enregistrez la photo employe.");
                lblStatus->setStyleSheet("font-size: 13px; color: #ef4444; font-weight: 700;");
                return;
            }

            double apiConfidence = -1.0;
            double apiThreshold = -1.0;
            QString apiError;
            if (!compareFacesWithApi(probeJpeg, accountPhoto, apiConfidence, apiThreshold, apiError)) {
                lblStatus->setText(apiError.isEmpty() ? "Verification IA impossible." : apiError);
                lblStatus->setStyleSheet("font-size: 13px; color: #ef4444; font-weight: 700;");
                return;
            }

            const double requiredApi = (apiThreshold > 0.0) ? apiThreshold : 80.0;
            if (apiConfidence < requiredApi) {
                lblStatus->setText(QString("IA: visage rejete (%1% < %2%).")
                                       .arg(QString::number(apiConfidence, 'f', 1))
                                       .arg(QString::number(requiredApi, 'f', 1)));
                lblStatus->setStyleSheet("font-size: 13px; color: #ef4444; font-weight: 700;");
                return;
            }

            if (txtUser) txtUser->setText(emailInput);
            m_faceMatchedEmail = emailInput;
            lblStatus->setText(QString("Bienvenue %1 (IA %2%)")
                                   .arg(accountName.isEmpty() ? emailInput : accountName)
                                   .arg(QString::number(apiConfidence, 'f', 1)));
            lblStatus->setStyleSheet("font-size: 16px; color: #22c55e; font-weight: 800;");

            QTimer::singleShot(600, this, [this]() {
                stopFaceId();
                grantAccess(false);
            });
            return;
        }

        const QVector<float> liveDesc = computeFaceDescriptor(lastFrame);
        if (liveDesc.isEmpty()) {
            lblStatus->setText("Visage non exploitable. Reessayez avec plus de lumiere.");
            lblStatus->setStyleSheet("font-size: 14px; color: #ef4444; font-weight: 700;");
            return;
        }
        if (!verifyFaceForEmail(emailInput, liveDesc, accountName, accountId, accountPhoto, faceScore, hasTemplate, verifyError)) {
            lblStatus->setText(verifyError.isEmpty() ? "Compte introuvable pour cet email." : verifyError);
            lblStatus->setStyleSheet("font-size: 13px; color: #ef4444; font-weight: 700;");
            return;
        }

        static constexpr double kThreshold = 0.96;
        static constexpr double kAmbiguousThreshold = 0.92;
        static constexpr double kMinGap = 0.08;
        if (hasTemplate) {
            if (faceScore < kThreshold) {
                lblStatus->setText(QString("Visage non reconnu pour %1 (score: %2%).")
                                       .arg(emailInput)
                                       .arg(QString::number(faceScore * 100.0, 'f', 1)));
                lblStatus->setStyleSheet("font-size: 13px; color: #f59e0b; font-weight: 700;");
                return;
            }

            QString otherEmail;
            QString otherName;
            QString bestErr;
            double otherScore = -1.0;
            double otherSecond = -1.0;
            if (findBestFaceMatch(liveDesc, otherEmail, otherName, otherScore, bestErr, accountId, &otherSecond)) {
                if (otherScore >= kAmbiguousThreshold && (faceScore - otherScore) < kMinGap) {
                    lblStatus->setText(QString("Match ambigu (compte proche: %1). Reessayez avec CIN.")
                                           .arg(otherName.isEmpty() ? otherEmail : otherName));
                    lblStatus->setStyleSheet("font-size: 13px; color: #ef4444; font-weight: 700;");
                    return;
                }
            }

            if (txtUser) txtUser->setText(emailInput);
            m_faceMatchedEmail = emailInput;
            lblStatus->setText(QString("Bienvenue %1 (%2%)")
                                   .arg(accountName.isEmpty() ? emailInput : accountName)
                                   .arg(QString::number(faceScore * 100.0, 'f', 1)));
            lblStatus->setStyleSheet("font-size: 16px; color: #22c55e; font-weight: 800;");

            QTimer::singleShot(600, this, [this]() {
                stopFaceId();
                grantAccess(false);
            });
            return;
        }

        const bool hasCin = txtPass && !txtPass->text().trimmed().isEmpty();
        if (!hasCin) {
            lblStatus->setText("Aucun visage enregistre pour cet email. Saisissez CIN pour la 1ere inscription.");
            lblStatus->setStyleSheet("font-size: 13px; color: #ef4444; font-weight: 700;");
            return;
        }

        QString enrolledEmail;
        QString enrolledName;
        QString enrollError;
        if (enrollFirstFaceForCurrentCredentials(liveDesc, enrolledEmail, enrolledName, enrollError)) {
            if (txtUser) txtUser->setText(enrolledEmail);
            m_faceMatchedEmail = enrolledEmail;
            lblStatus->setText(QString("Visage enregistre pour %1. Connexion validee.").arg(enrolledName));
            lblStatus->setStyleSheet("font-size: 15px; color: #22c55e; font-weight: 800;");

            QTimer::singleShot(800, this, [this]() {
                stopFaceId();
                grantAccess(false);
            });
            return;
        }

        lblStatus->setText(enrollError.isEmpty() ? "Inscription visage impossible." : enrollError);
        lblStatus->setStyleSheet("font-size: 13px; color: #ef4444; font-weight: 700;");
    }

private:
    bool verifyFaceForEmail(const QString &email,
                            const QVector<float> &liveDesc,
                            QString &accountName,
                            int &accountId,
                            QByteArray &accountPhoto,
                            double &score,
                            bool &hasTemplate,
                            QString &errorText)
    {
        accountName.clear();
        accountId = -1;
        accountPhoto.clear();
        score = -1.0;
        hasTemplate = false;
        errorText.clear();

        QSqlQuery q;
        q.prepare(
            "SELECT id_emp, nom, face_template, COALESCE(face_enabled,1), photo "
            "FROM EMPLOYE "
            "WHERE LOWER(TRIM(email)) = LOWER(TRIM(:email))");
        q.bindValue(":email", email);
        if (!q.exec()) {
            if (hasMissingColumnError(q.lastError().text())) {
                QSqlQuery fallback;
                fallback.prepare(
                    "SELECT id_emp, nom, face_template, COALESCE(face_enabled,1) "
                    "FROM EMPLOYE "
                    "WHERE LOWER(TRIM(email)) = LOWER(TRIM(:email))");
                fallback.bindValue(":email", email);
                if (!fallback.exec()) {
                    errorText = "Erreur SQL Face ID: " + fallback.lastError().text();
                    return false;
                }
                if (!fallback.next()) {
                    errorText = "Aucun compte trouve pour cet email.";
                    return false;
                }

                accountId = fallback.value(0).toInt();
                accountName = fallback.value(1).toString().trimmed();
                const QString templateRaw = fallback.value(2).toString();
                const bool enabled = fallback.value(3).toInt() != 0;
                if (!enabled) {
                    errorText = "Face ID desactive pour ce compte.";
                    return false;
                }

                const QVector<float> dbDesc = resolveEmployeeDescriptor(templateRaw);
                hasTemplate = !dbDesc.isEmpty();
                if (!hasTemplate) return true;

                score = cosineSimilarity(liveDesc, dbDesc);
                return true;
            }
            errorText = "Erreur SQL Face ID: " + q.lastError().text();
            return false;
        }
        if (!q.next()) {
            errorText = "Aucun compte trouve pour cet email.";
            return false;
        }

        accountId = q.value(0).toInt();
        accountName = q.value(1).toString().trimmed();
        const QString templateRaw = q.value(2).toString();
        const bool enabled = q.value(3).toInt() != 0;
        accountPhoto = q.value(4).toByteArray();
        if (!enabled) {
            errorText = "Face ID desactive pour ce compte.";
            return false;
        }

        const QVector<float> dbDesc = resolveEmployeeDescriptor(templateRaw);
        hasTemplate = !dbDesc.isEmpty();
        if (!hasTemplate) return true;

        score = cosineSimilarity(liveDesc, dbDesc);
        return true;
    }

    QVector<float> resolveEmployeeDescriptor(const QString &templateRaw)
    {
        QVector<float> dbDesc = descriptorFromString(templateRaw);
        return dbDesc;
    }

    bool findBestFaceMatch(const QVector<float> &liveDesc,
                           QString &bestEmail,
                           QString &bestName,
                           double &bestScore,
                           QString &errorText,
                           int excludeEmpId = -1,
                           double *secondBestScore = nullptr)
    {
        bestEmail.clear();
        bestName.clear();
        bestScore = -1.0;
        if (secondBestScore) *secondBestScore = -1.0;

        QSqlQuery query;
        if (!query.exec("SELECT id_emp, email, nom, face_template, COALESCE(face_enabled,1) FROM EMPLOYE")) {
            errorText = "Erreur SQL Face ID: " + query.lastError().text();
            return false;
        }

        bool anyTemplate = false;
        double secondScore = -1.0;
        while (query.next()) {
            const int idEmp = query.value(0).toInt();
            if (excludeEmpId > 0 && idEmp == excludeEmpId) continue;
            const QString email = query.value(1).toString().trimmed();
            const QString nom = query.value(2).toString().trimmed();
            if (email.isEmpty()) continue;

            const QString templateRaw = query.value(3).toString();
            const bool enabled = query.value(4).toInt() != 0;
            if (!enabled) continue;

            const QVector<float> dbDesc = resolveEmployeeDescriptor(templateRaw);
            if (dbDesc.isEmpty()) continue;
            anyTemplate = true;

            const double score = cosineSimilarity(liveDesc, dbDesc);
            if (score > bestScore) {
                secondScore = bestScore;
                bestScore = score;
                bestEmail = email;
                bestName = nom.isEmpty() ? email : nom;
            } else if (score > secondScore) {
                secondScore = score;
            }
        }

        if (secondBestScore) *secondBestScore = secondScore;
        if (!anyTemplate) {
            errorText = "Aucun template visage trouve. Ajoutez des photos employes.";
            return false;
        }
        return bestScore >= 0.0;
    }

    bool enrollFirstFaceForCurrentCredentials(const QVector<float> &liveDesc,
                                              QString &enrolledEmail,
                                              QString &enrolledName,
                                              QString &errorText)
    {
        enrolledEmail.clear();
        enrolledName.clear();
        errorText.clear();

        const QString email = txtUser ? txtUser->text().trimmed() : QString();
        const QString cin = txtPass ? txtPass->text().trimmed() : QString();
        if (email.isEmpty() || cin.isEmpty()) {
            errorText = "Pour inscrire un premier visage, saisissez Email + CIN puis capturez.";
            return false;
        }

        QSqlQuery q;
        q.prepare(
            "SELECT id_emp, email, nom, face_template, COALESCE(face_enabled,1) "
            "FROM EMPLOYE "
            "WHERE LOWER(TRIM(email)) = LOWER(TRIM(:email)) "
            "AND TRIM(cin) = TRIM(:cin)");
        q.bindValue(":email", email);
        q.bindValue(":cin", cin);
        if (!q.exec()) {
            errorText = "Erreur SQL inscription visage: " + q.lastError().text();
            return false;
        }
        if (!q.next()) {
            errorText = "Compte introuvable pour cet email/CIN.";
            return false;
        }

        const int targetId = q.value(0).toInt();
        enrolledEmail = q.value(1).toString().trimmed();
        enrolledName = q.value(2).toString().trimmed();
        const QString currentTemplate = q.value(3).toString();
        const bool enabled = q.value(4).toInt() != 0;
        if (!enabled) {
            errorText = "Face ID desactive pour ce compte.";
            return false;
        }

        const QVector<float> existingDesc = descriptorFromString(currentTemplate);
        if (!existingDesc.isEmpty()) {
            errorText = "Ce compte a deja un visage enregistre. Utilisez la reconnaissance normale.";
            return false;
        }

        QString otherEmail;
        QString otherName;
        double otherScore = -1.0;
        double otherSecond = -1.0;
        QString dummyError;
        const bool hasOtherFace = findBestFaceMatch(liveDesc, otherEmail, otherName, otherScore, dummyError, targetId, &otherSecond);
        static constexpr double kDupThreshold = 0.90;
        static constexpr double kDupGap = 0.06;
        const bool duplicateLikely = hasOtherFace &&
                                     otherScore >= kDupThreshold &&
                                     (otherSecond < 0.0 || (otherScore - otherSecond) >= kDupGap);
        if (duplicateLikely) {
            errorText = QString("Visage deja utilise par %1 (%2%).")
                            .arg(otherName)
                            .arg(QString::number(otherScore * 100.0, 'f', 1));
            return false;
        }

        QSqlQuery up;
        up.prepare(
            "UPDATE EMPLOYE "
            "SET face_template=:tpl, face_template_updated_at=SYSDATE, face_enabled=1 "
            "WHERE id_emp=:id");
        up.bindValue(":tpl", descriptorToString(liveDesc));
        up.bindValue(":id", targetId);
        if (!up.exec()) {
            errorText = "Impossible d'enregistrer le visage: " + up.lastError().text();
            return false;
        }

        if (enrolledName.isEmpty()) enrolledName = enrolledEmail;
        return true;
    }

    void grantAccess(bool isAdmin)
    {
        m_authenticated = true;
        m_isAdminLogin = isAdmin;
        if (isAdmin) {
            m_authenticatedEmail = "admin@wasteguard.com";
        } else {
            const QString faceEmail = m_faceMatchedEmail.trimmed();
            const QString typedEmail = txtUser ? txtUser->text().trimmed() : QString();
            m_authenticatedEmail = !faceEmail.isEmpty() ? faceEmail : typedEmail;
        }
        QDialog::accept();
    }

    MascotWidget *mascot = nullptr;
    QStackedWidget *stackedWidget = nullptr;
    QLineEdit *txtUser = nullptr;
    QLineEdit *txtPass = nullptr;
    QLabel *lblStatus = nullptr;
    QLabel *facePreview = nullptr;
    QPushButton *btnFaceCapture = nullptr;
    QCamera *camera = nullptr;
    QMediaCaptureSession *captureSession = nullptr;
    QVideoSink *videoSink = nullptr;
    QImage lastFrame;
    QString m_faceMatchedEmail;
    QString m_authenticatedEmail;
    QPropertyAnimation *eyeAnim = nullptr;
    bool m_authenticated = false;
    bool m_isAdminLogin = false;
};

int main(int argc, char *argv[])
{
    if (!g_previousMessageHandler) {
        g_previousMessageHandler = qInstallMessageHandler(filteredQtMessageHandler);
    }

    // Stabilite Windows/Qt: evite certains crashes lies aux styles/renderers natifs.
    if (qEnvironmentVariableIsEmpty("QT_STYLE_OVERRIDE")) {
        qputenv("QT_STYLE_OVERRIDE", QByteArrayLiteral("Fusion"));
    }
    if (qEnvironmentVariableIsEmpty("QT_OPENGL")) {
        qputenv("QT_OPENGL", QByteArrayLiteral("software"));
    }
    if (qEnvironmentVariableIsEmpty("QSG_RHI_BACKEND")) {
        qputenv("QSG_RHI_BACKEND", QByteArrayLiteral("software"));
    }
    if (qEnvironmentVariableIsEmpty("QT_LOGGING_RULES")) {
        qputenv("QT_LOGGING_RULES",
                QByteArrayLiteral("qt.core.qmetaobject.connectslotsbyname.warning=false"));
    }

    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    qDebug() << "Drivers disponibles:" << QSqlDatabase::drivers();

    while (true) {
        LoginDialog login;
        if (login.exec() != QDialog::Accepted || !login.isAuthenticated()) {
            return 0;
        }

        Connection *c = Connection::instance();
        const bool dbReady = c->isOpen() || c->createConnect();
        if (!dbReady) {
            if (!login.isAdminLogin()) {
                QMessageBox::critical(
                    nullptr,
                    QObject::tr("database is not open"),
                    QObject::tr("connection failed.\nClick Cancel to exit."),
                    QMessageBox::Cancel);
                return 0;
            }

            QMessageBox::warning(
                nullptr,
                QObject::tr("Mode admin hors ligne"),
                QObject::tr("Connexion base indisponible.\nOuverture en mode admin hors ligne."));
        }

        auto *w = new MainWindow;
        
        QFile logFile("startup_crash_log.txt");
        if(logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&logFile);
            ts << "MainWindow created successfully!\n";
            logFile.close();
        }
        
        bool logoutTriggered = false;
        QEventLoop sessionLoop;

        QObject::connect(w, &MainWindow::logoutRequested, &a, [&]() {
            logoutTriggered = true;
            w->close();
        });
        QObject::connect(w, &QObject::destroyed, &sessionLoop, &QEventLoop::quit);

        w->setAttribute(Qt::WA_DeleteOnClose);
        w->setSessionContext(login.isAdminLogin(), login.authenticatedEmail());
        w->showMaximized();
        sessionLoop.exec();

        if (!logoutTriggered) {
            break;
        }
    }

    return 0;
}

