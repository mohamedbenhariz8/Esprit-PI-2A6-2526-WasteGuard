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
#include <QSettings>
#include <QProcessEnvironment>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <cstdio>

#include "connection.h"
#include "thememanager.h"

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

    auto columnMissing = [](const QString &col) {
        QSqlQuery probe;
        probe.exec(QString("SELECT %1 FROM EMPLOYE WHERE 1=0").arg(col));
        return probe.lastError().isValid();
    };

    QSqlQuery q;
    if (columnMissing("face_template"))
        q.exec("ALTER TABLE EMPLOYE ADD (face_template CLOB)");
    if (columnMissing("face_template_bin"))
        q.exec("ALTER TABLE EMPLOYE ADD (face_template_bin BLOB)");
    if (columnMissing("face_template_updated_at"))
        q.exec("ALTER TABLE EMPLOYE ADD (face_template_updated_at DATE)");
    if (columnMissing("face_enabled")) {
        q.exec("ALTER TABLE EMPLOYE ADD (face_enabled NUMBER(1) DEFAULT 1)");
        q.exec("UPDATE EMPLOYE SET face_enabled = 1 WHERE face_enabled IS NULL");
    }
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

QString faceApiCredential(const char *envKey, const QString &settingsKey, const QString &fallback = QString())
{
    // 1) Process env (what qEnvironmentVariable reads).
    const QString envValue = qEnvironmentVariable(envKey).trimmed();
    if (!envValue.isEmpty()) return envValue;

    // 2) System env snapshot (redundant but harmless safety net).
    const QString sysValue =
        QProcessEnvironment::systemEnvironment().value(QString::fromLatin1(envKey)).trimmed();
    if (!sysValue.isEmpty()) return sysValue;

    // 3) Persistent QSettings fallback — immune to Windows env-var propagation
    //    quirks (setx / SetEnvironmentVariable not visible to already-running
    //    parent processes like Qt Creator / Explorer).
    QSettings settings("WasteGuard", "WasteGuard");
    const QString stored = settings.value(settingsKey).toString().trimmed();
    if (!stored.isEmpty()) return stored;

    return fallback;
}

bool isFaceApiEnabled()
{
    const QString key = faceApiCredential("WG_FACE_API_KEY", "faceApi/key");
    const QString secret = faceApiCredential("WG_FACE_API_SECRET", "faceApi/secret");
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

    const QString key = faceApiCredential("WG_FACE_API_KEY", "faceApi/key");
    const QString secret = faceApiCredential("WG_FACE_API_SECRET", "faceApi/secret");
    QString endpoint = faceApiCredential("WG_FACE_API_URL", "faceApi/url",
                                         "https://api-us.faceplusplus.com/facepp/v3/compare");
    if (endpoint.isEmpty()) {
        endpoint = "https://api-us.faceplusplus.com/facepp/v3/compare";
    }
    // Auto-correct common misconfiguration: base URL without /compare action.
    // Face++ replies "API_NOT_FOUND" when hitting /facepp/v3 directly.
    {
        QString trimmedEndpoint = endpoint.trimmed();
        while (trimmedEndpoint.endsWith('/')) trimmedEndpoint.chop(1);
        const bool missingAction =
            trimmedEndpoint.endsWith("/facepp/v3", Qt::CaseInsensitive) ||
            trimmedEndpoint.endsWith("/facepp/v3.0", Qt::CaseInsensitive) ||
            trimmedEndpoint.endsWith("faceplusplus.com", Qt::CaseInsensitive);
        if (missingAction) {
            const QString corrected = trimmedEndpoint + "/compare";
            qWarning().nospace()
                << "[FaceAPI] URL endpoint semble incomplet (" << endpoint
                << "), redirection automatique vers " << corrected;
            endpoint = corrected;
        }
    }

    if (key.isEmpty() || secret.isEmpty()) {
        errorText = "Face API non configuree. Definissez WG_FACE_API_KEY / WG_FACE_API_SECRET "
                    "(relancez Qt Creator apres setx) ou les cles faceApi/key et faceApi/secret "
                    "dans QSettings WasteGuard.";
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

    qInfo().nospace() << "[FaceAPI] POST " << endpoint
                      << " ssl=" << QSslSocket::supportsSsl()
                      << " sslBuild=" << QSslSocket::sslLibraryBuildVersionString()
                      << " sslRuntime=" << QSslSocket::sslLibraryVersionString()
                      << " probe=" << probeNormalized.size() << "B"
                      << " ref=" << referenceNormalized.size() << "B";

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
        qWarning() << "[FaceAPI] SSL errors:" << sslErrors;
    });
    QObject::connect(reply, &QNetworkReply::errorOccurred, &loop, [&](QNetworkReply::NetworkError err) {
        qWarning() << "[FaceAPI] errorOccurred:" << err << reply->errorString();
    });
    bool timedOut = false;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    QObject::connect(&timeoutTimer, &QTimer::timeout, &loop, [&]() {
        if (reply && reply->isRunning()) {
            qWarning() << "[FaceAPI] Timeout after 20s, aborting.";
            timedOut = true;
            reply->abort();
        }
        loop.quit();
    });
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timeoutTimer.start(20000);
    loop.exec();
    timeoutTimer.stop();

    const QByteArray body = timedOut ? QByteArray() : reply->readAll();
    qInfo().nospace() << "[FaceAPI] reply err=" << reply->error()
                      << " http=" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()
                      << " bodyBytes=" << body.size()
                      << " bodyHead=" << QString::fromUtf8(body.left(300));
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
        qWarning() << "[FaceAPI] FAIL ->" << errorText << "(netErr=" << netErr << ")";
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

        setFixedSize(460, 720);
        setWindowFlags(Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);

        const QString accentBlue  = "#1e40af";
        const QString accentHover = "#1e3a8a";
        const QString lightBlue   = "#dbeafe";

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        QFrame *container = new QFrame(this);
        container->setObjectName("loginContainer");
        container->setStyleSheet(
            "QFrame#loginContainer {"
            "   background: #ffffff;"
            "   border: 1px solid #e2e8f0;"
            "   border-radius: 24px;"
            "}"
            "QLineEdit {"
            "   background: #f8fafc;"
            "   border: 1.5px solid #cbd5e1;"
            "   border-radius: 10px;"
            "   padding: 12px 14px;"
            "   color: #1e293b;"
            "   font-size: 14px;"
            "}"
            "QLineEdit:focus {"
            "   border: 2px solid #1e40af;"
            "   background: #ffffff;"
            "}"
            "QLineEdit::placeholder { color: #94a3b8; }"
            "QPushButton#btnLogin {"
            "   background: #1e40af;"
            "   color: white;"
            "   border-radius: 12px;"
            "   padding: 14px;"
            "   font-weight: 800;"
            "   font-size: 15px;"
            "   border: none;"
            "}"
            "QPushButton#btnLogin:hover { background: #1e3a8a; }"
            "QPushButton#btnLogin:pressed { background: #172554; }"
        );

        QVBoxLayout *containerLayout = new QVBoxLayout(container);
        containerLayout->setContentsMargins(0, 0, 0, 0);
        containerLayout->setSpacing(0);

        // ── Dark green header strip (brand identity) ──────────────────
        QFrame *headerBand = new QFrame(container);
        headerBand->setObjectName("loginHeader");
        headerBand->setStyleSheet(
            "QFrame#loginHeader {"
            "   background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #1a3a2e,stop:1 #2d5a42);"
            "   border-top-left-radius: 24px;"
            "   border-top-right-radius: 24px;"
            "   border-bottom-left-radius: 0px;"
            "   border-bottom-right-radius: 0px;"
            "}"
        );
        QVBoxLayout *headerLayout = new QVBoxLayout(headerBand);
        headerLayout->setContentsMargins(30, 20, 30, 16);
        headerLayout->setSpacing(6);

        // WasteGuard brand label
        QLabel *lblBrand = new QLabel("WASTEGUARD", headerBand);
        lblBrand->setAlignment(Qt::AlignCenter);
        lblBrand->setStyleSheet("font-size:22px;font-weight:900;color:#ffffff;letter-spacing:4px;background:transparent;");
        headerLayout->addWidget(lblBrand);

        QLabel *lblTagline = new QLabel("Gestion Intelligente des Dechets", headerBand);
        lblTagline->setAlignment(Qt::AlignCenter);
        lblTagline->setStyleSheet("font-size:11px;color:#a8d5b5;letter-spacing:1px;background:transparent;");
        headerLayout->addWidget(lblTagline);

        containerLayout->addWidget(headerBand);

        // ── Mascot + form area ────────────────────────────────────────
        QFrame *formArea = new QFrame(container);
        formArea->setStyleSheet("QFrame{background:transparent;border:none;}");
        QVBoxLayout *formAreaLayout = new QVBoxLayout(formArea);
        formAreaLayout->setContentsMargins(36, 16, 36, 32);
        formAreaLayout->setSpacing(14);
        containerLayout->addWidget(formArea);

        mascot = new MascotWidget(this);
        mascot->setMinimumHeight(170);
        mascot->setMaximumHeight(170);
        formAreaLayout->addWidget(mascot);

        QLabel *lblMascotName = new QLabel("LABIB", this);
        lblMascotName->setAlignment(Qt::AlignCenter);
        lblMascotName->setStyleSheet("font-size:12px;font-weight:800;color:#1e40af;letter-spacing:4px;background:transparent;");
        formAreaLayout->addWidget(lblMascotName);

        stackedWidget = new QStackedWidget(this);

        QWidget *pageLogin = new QWidget();
        pageLogin->setStyleSheet("background:transparent;");
        QVBoxLayout *loginLayout = new QVBoxLayout(pageLogin);
        loginLayout->setSpacing(12);
        loginLayout->setContentsMargins(0,0,0,0);

        // Divider
        QFrame *divider = new QFrame(); divider->setFrameShape(QFrame::HLine);
        divider->setStyleSheet("background:#e2e8f0; border:none; max-height:1px;");
        loginLayout->addWidget(divider);
        loginLayout->addSpacing(4);

        QLabel *lblTitle = new QLabel("Acces Securise", this);
        lblTitle->setStyleSheet("font-size:18px;font-weight:800;color:#1e293b;background:transparent;");
        lblTitle->setAlignment(Qt::AlignCenter);
        loginLayout->addWidget(lblTitle);

        QLabel *lblSub = new QLabel("Connectez-vous avec votre email et CIN", this);
        lblSub->setStyleSheet("font-size:12px;color:#64748b;background:transparent;");
        lblSub->setAlignment(Qt::AlignCenter);
        loginLayout->addWidget(lblSub);
        loginLayout->addSpacing(6);

        const QString inputStyle =
            "QLineEdit { background:#f8fafc; border:1.5px solid #cbd5e1; border-radius:10px;"
            " padding:12px 14px; color:#1e293b; font-size:14px; }"
            "QLineEdit:focus { border:2px solid #1e40af; background:#ffffff; }";

        txtUser = new QLineEdit(this);
        txtUser->setPlaceholderText("Adresse email");
        txtUser->setMinimumHeight(48);
        txtUser->setStyleSheet(inputStyle);
        loginLayout->addWidget(txtUser);

        txtPass = new QLineEdit(this);
        txtPass->setPlaceholderText("CIN / Mot de passe admin");
        txtPass->setEchoMode(QLineEdit::Password);
        txtPass->setMinimumHeight(48);
        txtPass->setStyleSheet(inputStyle);
        loginLayout->addWidget(txtPass);
        loginLayout->addSpacing(4);

        QPushButton *btnLogin = new QPushButton("CONNEXION", this);
        btnLogin->setObjectName("btnLogin");
        btnLogin->setMinimumHeight(50);
        btnLogin->setCursor(Qt::PointingHandCursor);
        btnLogin->setStyleSheet(
            "QPushButton { background:#1e40af; color:#ffffff; border-radius:12px;"
            " padding:14px; font-weight:800; font-size:15px; border:none; }"
            "QPushButton:hover { background:#1e3a8a; }"
            "QPushButton:pressed { background:#172554; }"
        );
        loginLayout->addWidget(btnLogin);

        QHBoxLayout *altAuthRow = new QHBoxLayout();
        altAuthRow->setSpacing(10);
        altAuthRow->setContentsMargins(0, 0, 0, 0);

        QPushButton *btnFaceId = new QPushButton("Visage", this);
        btnFaceId->setStyleSheet(
            "QPushButton { background: transparent; color: #1e40af; font-weight: 700; "
            "font-size: 13px; border-radius: 10px; padding: 10px; "
            "border: 1.5px solid #bfdbfe; } "
            "QPushButton:hover { background-color: #eff6ff; border-color: #1e40af; }"
        );
        btnFaceId->setMinimumHeight(42);
        btnFaceId->setCursor(Qt::PointingHandCursor);
        altAuthRow->addWidget(btnFaceId, 1);

        QPushButton *btnRfidLogin = new QPushButton("Badge RFID", this);
        btnRfidLogin->setStyleSheet(
            "QPushButton { background: transparent; color: #b45309; font-weight: 700; "
            "font-size: 13px; border-radius: 10px; padding: 10px; "
            "border: 1.5px solid #fde68a; } "
            "QPushButton:hover { background-color: #fffbeb; border-color: #b45309; }"
        );
        btnRfidLogin->setMinimumHeight(42);
        btnRfidLogin->setCursor(Qt::PointingHandCursor);
        altAuthRow->addWidget(btnRfidLogin, 1);

        loginLayout->addLayout(altAuthRow);

        QLabel *lblAuthModes = new QLabel("Email + CIN  ·  Visage  ·  Badge RFID", this);
        lblAuthModes->setAlignment(Qt::AlignCenter);
        lblAuthModes->setStyleSheet("color:#94a3b8;font-size:11px;background:transparent;");
        loginLayout->addWidget(lblAuthModes);

        QWidget *pageFaceId = new QWidget();
        pageFaceId->setStyleSheet("background:transparent;");
        QVBoxLayout *faceLayout = new QVBoxLayout(pageFaceId);
        faceLayout->setSpacing(12);
        faceLayout->setContentsMargins(0,0,0,0);

        QLabel *lblFaceTitle = new QLabel("Reconnaissance Faciale", this);
        lblFaceTitle->setStyleSheet("font-size:17px;font-weight:800;color:#1e293b;background:transparent;");
        lblFaceTitle->setAlignment(Qt::AlignCenter);
        faceLayout->addWidget(lblFaceTitle);

        facePreview = new QLabel(this);
        facePreview->setMinimumSize(300, 220);
        facePreview->setAlignment(Qt::AlignCenter);
        facePreview->setText("Camera non demarree");
        facePreview->setStyleSheet("background:#f1f5f9;border:2px solid #bfdbfe;border-radius:14px;color:#64748b;font-size:13px;");
        faceLayout->addWidget(facePreview);

        lblStatus = new QLabel("Initialisation du capteur...", this);
        lblStatus->setStyleSheet("font-size:13px;color:#475569;background:transparent;");
        lblStatus->setAlignment(Qt::AlignCenter);
        faceLayout->addWidget(lblStatus);

        btnFaceCapture = new QPushButton("Capturer et verifier", this);
        btnFaceCapture->setObjectName("btnFaceCapture");
        btnFaceCapture->setMinimumHeight(48);
        btnFaceCapture->setCursor(Qt::PointingHandCursor);
        btnFaceCapture->setStyleSheet(
            "QPushButton#btnFaceCapture { background:#1e40af; color:white; border-radius:10px; font-weight:700; border:none; }"
            "QPushButton#btnFaceCapture:hover { background:#1e3a8a; }"
            "QPushButton#btnFaceCapture:disabled { background:#94a3b8; }"
        );
        faceLayout->addWidget(btnFaceCapture);

        QPushButton *btnBack = new QPushButton("Annuler et revenir", this);
        btnBack->setStyleSheet("color:#1e40af;font-size:12px;border:none;background:transparent;text-decoration:underline;");
        btnBack->setFlat(true);
        btnBack->setCursor(Qt::PointingHandCursor);
        faceLayout->addWidget(btnBack);

        stackedWidget->addWidget(pageLogin);
        stackedWidget->addWidget(pageFaceId);
        stackedWidget->setCurrentIndex(0);

        formAreaLayout->addWidget(stackedWidget);
        containerLayout->addWidget(formArea);
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

        connect(btnRfidLogin, &QPushButton::clicked, this, &LoginDialog::attemptRfidLogin);

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

    void attemptRfidLogin()
    {
        Connection *connection = Connection::instance();
        if (!connection->isOpen() && !connection->createConnect()) {
            QMessageBox::critical(
                this,
                "Connexion base",
                "Impossible de joindre la base de donnees.\n\nDetails: " + connection->lastError());
            return;
        }

        // Locate an Arduino-like serial port and open it.
        QString portName;
        for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
            const QString desc = info.description().toLower();
            const QString mfr = info.manufacturer().toLower();
            if (desc.contains("bluetooth") || mfr.contains("bluetooth")) continue;
            if (info.hasVendorIdentifier() && info.vendorIdentifier() == 9025
                && info.hasProductIdentifier() && info.productIdentifier() == 67) {
                portName = info.portName();
                break;
            }
            if (portName.isEmpty()
                && (desc.contains("arduino") || desc.contains("ch340")
                    || desc.contains("ch341") || desc.contains("usb-serial")
                    || desc.contains("ftdi") || desc.contains("cp210")
                    || mfr.contains("arduino") || mfr.contains("ftdi")
                    || mfr.contains("silicon labs") || mfr.contains("wch"))) {
                portName = info.portName();
            }
        }

        QSerialPort *rfidSerial = new QSerialPort(this);
        if (!portName.isEmpty()) {
            rfidSerial->setPortName(portName);
            rfidSerial->setBaudRate(QSerialPort::Baud9600);
            rfidSerial->setDataBits(QSerialPort::Data8);
            rfidSerial->setParity(QSerialPort::NoParity);
            rfidSerial->setStopBits(QSerialPort::OneStop);
            rfidSerial->setFlowControl(QSerialPort::NoFlowControl);
            if (!rfidSerial->open(QIODevice::ReadOnly)) {
                qWarning() << "[RFID Login] Failed to open" << portName << ":" << rfidSerial->errorString();
            }
        }

        QDialog dlg(this);
        dlg.setWindowTitle("Connexion par badge RFID");
        dlg.setModal(true);
        dlg.setMinimumWidth(420);

        auto *layout = new QVBoxLayout(&dlg);
        layout->setContentsMargins(24, 24, 24, 20);
        layout->setSpacing(14);

        auto *info = new QLabel(QString::fromUcs4(U"\U0001F4E1") +
                                "  Approchez votre badge du lecteur...", &dlg);
        info->setAlignment(Qt::AlignCenter);
        info->setStyleSheet("font-size: 15px; font-weight: 600; color:#1e293b; padding: 8px;");
        layout->addWidget(info);

        auto *portLbl = new QLabel(&dlg);
        portLbl->setAlignment(Qt::AlignCenter);
        portLbl->setStyleSheet("font-size: 12px; color: #475569;");
        if (rfidSerial->isOpen()) {
            portLbl->setText(QString("Lecteur connecte sur %1 (9600 bauds)").arg(portName));
        } else {
            portLbl->setText(portName.isEmpty()
                                 ? "Lecteur RFID non detecte. Branchez l'Arduino puis cliquez Reessayer."
                                 : QString("Echec ouverture %1. Fermez le moniteur serie puis Reessayer.").arg(portName));
            portLbl->setStyleSheet("font-size: 12px; color: #c0392b; font-weight: 600;");
        }
        layout->addWidget(portLbl);

        auto *status = new QLabel(&dlg);
        status->setAlignment(Qt::AlignCenter);
        status->setStyleSheet("font-size: 13px; color: #475569;");
        status->setWordWrap(true);
        layout->addWidget(status);

        auto *btnRow = new QHBoxLayout();
        auto *retryBtn = new QPushButton("Reessayer", &dlg);
        auto *cancelBtn = new QPushButton("Annuler", &dlg);
        btnRow->addWidget(retryBtn);
        btnRow->addStretch();
        btnRow->addWidget(cancelBtn);
        layout->addLayout(btnRow);

        QByteArray buffer;
        QString matchedEmail;

        auto tryReopen = [rfidSerial, portLbl, status]() {
            if (rfidSerial->isOpen()) rfidSerial->close();
            QString newPort;
            for (const QSerialPortInfo &i : QSerialPortInfo::availablePorts()) {
                const QString d = i.description().toLower();
                const QString m = i.manufacturer().toLower();
                if (d.contains("bluetooth") || m.contains("bluetooth")) continue;
                if (d.contains("arduino") || d.contains("ch340") || d.contains("ch341")
                    || d.contains("usb-serial") || d.contains("ftdi") || d.contains("cp210")
                    || m.contains("arduino") || m.contains("ftdi")
                    || m.contains("silicon labs") || m.contains("wch")) {
                    newPort = i.portName();
                    break;
                }
            }
            if (newPort.isEmpty()) {
                portLbl->setText("Lecteur RFID non detecte. Branchez l'Arduino puis Reessayer.");
                portLbl->setStyleSheet("font-size: 12px; color: #c0392b; font-weight: 600;");
                return;
            }
            rfidSerial->setPortName(newPort);
            rfidSerial->setBaudRate(QSerialPort::Baud9600);
            rfidSerial->setDataBits(QSerialPort::Data8);
            rfidSerial->setParity(QSerialPort::NoParity);
            rfidSerial->setStopBits(QSerialPort::OneStop);
            rfidSerial->setFlowControl(QSerialPort::NoFlowControl);
            if (rfidSerial->open(QIODevice::ReadOnly)) {
                portLbl->setText(QString("Lecteur connecte sur %1 (9600 bauds)").arg(newPort));
                portLbl->setStyleSheet("font-size: 12px; color: #16a34a; font-weight: 600;");
                status->setText("");
            } else {
                portLbl->setText(QString("Echec ouverture %1.").arg(newPort));
                portLbl->setStyleSheet("font-size: 12px; color: #c0392b; font-weight: 600;");
            }
        };

        connect(retryBtn, &QPushButton::clicked, &dlg, tryReopen);
        connect(cancelBtn, &QPushButton::clicked, &dlg, &QDialog::reject);

        connect(rfidSerial, &QSerialPort::readyRead, &dlg, [&]() {
            buffer.append(rfidSerial->readAll());
            while (true) {
                const int nl = buffer.indexOf('\n');
                if (nl < 0) break;
                const QByteArray line = buffer.left(nl);
                buffer.remove(0, nl + 1);
                const QString text = QString::fromLatin1(line).trimmed();
                if (text.isEmpty()) continue;
                static const QString kRfidPrefix = "RFID_TAG:";
                if (!text.startsWith(kRfidPrefix, Qt::CaseInsensitive)) continue;
                const QString tag = text.mid(kRfidPrefix.length()).trimmed().toUpper();
                if (tag.isEmpty()) continue;

                QSqlQuery q;
                q.prepare(
                    "SELECT id_emp, email FROM EMPLOYE "
                    "WHERE UPPER(TRIM(rfid)) = UPPER(TRIM(:tag)) "
                    "AND rfid IS NOT NULL");
                q.bindValue(":tag", tag);
                if (!q.exec()) {
                    status->setText(QString("Erreur base: %1").arg(q.lastError().text()));
                    status->setStyleSheet("font-size: 13px; color: #c0392b; font-weight: 700;");
                    continue;
                }
                if (q.next()) {
                    const int idEmp = q.value(0).toInt();
                    matchedEmail = q.value(1).toString().trimmed();

                    QSqlQuery upd;
                    upd.prepare(
                        "UPDATE EMPLOYE "
                        "SET disponibilite = 'DISPONIBLE', last_pointage_date = SYSDATE "
                        "WHERE id_emp = :id");
                    upd.bindValue(":id", idEmp);
                    if (!upd.exec()) {
                        qWarning() << "[RFID Login] Pointage update failed (with date):"
                                   << upd.lastError().text();
                        QSqlQuery fallback;
                        fallback.prepare(
                            "UPDATE EMPLOYE SET disponibilite = 'DISPONIBLE' "
                            "WHERE id_emp = :id");
                        fallback.bindValue(":id", idEmp);
                        if (!fallback.exec()) {
                            qWarning() << "[RFID Login] Pointage fallback failed:"
                                       << fallback.lastError().text();
                        }
                    }

                    status->setText(QString::fromUcs4(U"✓") +
                                    QString(" Badge reconnu : %1\nPresence enregistree.").arg(matchedEmail));
                    status->setStyleSheet("font-size: 14px; color: #16a34a; font-weight: 800;");
                    QTimer::singleShot(550, &dlg, &QDialog::accept);
                    return;
                }
                status->setText(QString("Badge inconnu : %1. Approchez un autre badge.").arg(tag));
                status->setStyleSheet("font-size: 13px; color: #c0392b; font-weight: 700;");
            }
        });

        const int rc = dlg.exec();
        if (rfidSerial->isOpen()) rfidSerial->close();
        rfidSerial->deleteLater();

        if (rc == QDialog::Accepted && !matchedEmail.isEmpty()) {
            if (txtUser) txtUser->setText(matchedEmail);
            m_faceMatchedEmail.clear();
            grantAccess(false);
        }
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

        // Lecture robuste : BLOB binaire d'abord, fallback CLOB texte.
        QSqlQuery q;
        q.prepare(
            "SELECT id_emp, nom, face_template_bin, face_template, "
            "       COALESCE(face_enabled,1), photo "
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
        // 1) BLOB binaire
        QString templateRaw;
        const QByteArray bin = q.value(2).toByteArray();
        if (!bin.isEmpty()) templateRaw = QString::fromUtf8(bin);
        // 2) Fallback CLOB texte (anciens enregistrements)
        if (templateRaw.trimmed().isEmpty()) {
            templateRaw = q.value(3).toString();
        }
        const bool enabled = q.value(4).toInt() != 0;
        accountPhoto = q.value(5).toByteArray();
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

        // Lecture robuste : BLOB binaire d'abord, fallback CLOB texte.
        QSqlQuery query;
        bool useBlob = true;
        if (!query.exec("SELECT id_emp, email, nom, face_template_bin, face_template, "
                        "COALESCE(face_enabled,1) FROM EMPLOYE")) {
            // Colonne BLOB absente : on retombe sur l'ancienne SELECT.
            useBlob = false;
            if (!query.exec("SELECT id_emp, email, nom, face_template, COALESCE(face_enabled,1) "
                           "FROM EMPLOYE")) {
                errorText = "Erreur SQL Face ID: " + query.lastError().text();
                return false;
            }
        }

        bool anyTemplate = false;
        double secondScore = -1.0;
        while (query.next()) {
            const int idEmp = query.value(0).toInt();
            if (excludeEmpId > 0 && idEmp == excludeEmpId) continue;
            const QString email = query.value(1).toString().trimmed();
            const QString nom = query.value(2).toString().trimmed();
            if (email.isEmpty()) continue;

            QString templateRaw;
            bool enabled = true;
            if (useBlob) {
                const QByteArray bin = query.value(3).toByteArray();
                if (!bin.isEmpty()) templateRaw = QString::fromUtf8(bin);
                if (templateRaw.trimmed().isEmpty())
                    templateRaw = query.value(4).toString();
                enabled = query.value(5).toInt() != 0;
            } else {
                templateRaw = query.value(3).toString();
                enabled = query.value(4).toInt() != 0;
            }
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

        // Double ecriture : BLOB binaire (robuste sur QODBC/Oracle) + CLOB (compat).
        const QString tplStr = descriptorToString(liveDesc);
        const QByteArray tplBin = tplStr.toUtf8();
        QSqlQuery up;
        up.prepare(
            "UPDATE EMPLOYE "
            "SET face_template=:tpl, face_template_bin=:bin, "
            "    face_template_updated_at=SYSDATE, face_enabled=1 "
            "WHERE id_emp=:id");
        up.bindValue(":tpl", tplStr);
        up.bindValue(":bin", tplBin);
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
    if (qEnvironmentVariableIsEmpty("QT_LOGGING_RULES")) {
        qputenv("QT_LOGGING_RULES",
                QByteArrayLiteral("qt.core.qmetaobject.connectslotsbyname.warning=false"));
    }

    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    QCoreApplication::setOrganizationName("WasteGuard");
    QCoreApplication::setOrganizationDomain("wasteguard.local");
    QCoreApplication::setApplicationName("WasteGuard");
    ThemeManager::instance()->applyToApplication();
    qDebug() << "Drivers disponibles:" << QSqlDatabase::drivers();

    // One-shot CLI registration for Face API creds (bypasses Windows env-var
    // propagation issues). Usage:
    //   WasteGuard.exe --set-face-api KEY SECRET [URL]
    {
        const QStringList args = QCoreApplication::arguments();
        const int idx = args.indexOf("--set-face-api");
        if (idx >= 0 && idx + 2 < args.size()) {
            QSettings settings("WasteGuard", "WasteGuard");
            settings.setValue("faceApi/key", args.at(idx + 1).trimmed());
            settings.setValue("faceApi/secret", args.at(idx + 2).trimmed());
            if (idx + 3 < args.size() && !args.at(idx + 3).startsWith("--")) {
                settings.setValue("faceApi/url", args.at(idx + 3).trimmed());
            }
            settings.sync();
            qInfo() << "[FaceAPI] Cles enregistrees dans QSettings WasteGuard.";
            return 0;
        }
    }

    // Diagnostic: report which source (env / settings) provided Face API creds.
    {
        const bool envKey = !qEnvironmentVariable("WG_FACE_API_KEY").trimmed().isEmpty();
        const bool envSec = !qEnvironmentVariable("WG_FACE_API_SECRET").trimmed().isEmpty();
        QSettings s("WasteGuard", "WasteGuard");
        const bool setKey = !s.value("faceApi/key").toString().trimmed().isEmpty();
        const bool setSec = !s.value("faceApi/secret").toString().trimmed().isEmpty();
        qInfo().nospace() << "[FaceAPI] env(KEY=" << envKey << ",SECRET=" << envSec
                          << ") settings(KEY=" << setKey << ",SECRET=" << setSec
                          << ") -> enabled=" << isFaceApiEnabled();
    }

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

