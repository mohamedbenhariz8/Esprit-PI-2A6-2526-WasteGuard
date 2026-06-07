#include "labibmediacircle.h"

#include <QAudioOutput>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QMouseEvent>
#include <QMovie>
#include <QPainter>
#include <QPainterPath>
#include <QUrl>
#include <QVideoFrame>
#include <QVideoSink>

LabibMediaCircle::LabibMediaCircle(QWidget *parent)
    : QWidget(parent)
    , m_borderColor("#ffffff")
    , m_ringColor("#27ae60")
    , m_loopCount(QMediaPlayer::Infinite)
{
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
}

LabibMediaCircle::~LabibMediaCircle() = default;

void LabibMediaCircle::setLoopCount(int count)
{
    m_loopCount = count;
    if (m_player) {
        m_player->setLoops(count);
    }
}

bool LabibMediaCircle::tryLoad(const QString &pathOrResource)
{
    if (pathOrResource.isEmpty()) return false;

    QString path = pathOrResource;
    const bool isResource = path.startsWith(":/") || path.startsWith("qrc:/");
    if (!isResource && !QFileInfo::exists(path)) {
        return false;
    }

    const QString ext = QFileInfo(path).suffix().toLower();

    // ─── GIF / animation simple via QMovie ───
    if (ext == "gif" || ext == "webp" || ext == "apng") {
        if (m_movie) { m_movie->stop(); m_movie->deleteLater(); m_movie = nullptr; }
        m_movie = new QMovie(path, QByteArray(), this);
        if (!m_movie->isValid()) {
            m_movie->deleteLater();
            m_movie = nullptr;
            return false;
        }
        m_movie->setCacheMode(QMovie::CacheAll);
        connect(m_movie, &QMovie::frameChanged, this, [this]() {
            if (m_movie) {
                m_currentFrame = m_movie->currentImage();
                update();
            }
        });
        m_movie->start();
        return true;
    }

    // ─── Vidéo via QMediaPlayer + QVideoSink ───
    if (ext == "mp4" || ext == "webm" || ext == "mov" || ext == "mkv" || ext == "avi") {
        if (m_player) { m_player->stop(); m_player->deleteLater(); m_player = nullptr; }
        if (m_sink)   { m_sink->deleteLater();   m_sink   = nullptr; }
        if (m_audio)  { m_audio->deleteLater();  m_audio  = nullptr; }

        m_player = new QMediaPlayer(this);
        m_audio  = new QAudioOutput(this);
        m_audio->setMuted(true);
        m_player->setAudioOutput(m_audio);

        m_sink = new QVideoSink(this);
        m_player->setVideoSink(m_sink);

        connect(m_sink, &QVideoSink::videoFrameChanged, this,
                [this](const QVideoFrame &frame) {
            QVideoFrame copy(frame);
            if (!copy.isValid()) return;
            if (!copy.map(QVideoFrame::ReadOnly)) {
                m_currentFrame = copy.toImage();
            } else {
                m_currentFrame = copy.toImage();
                copy.unmap();
            }
            update();
        });

        m_player->setLoops(m_loopCount);
        connect(m_player, &QMediaPlayer::mediaStatusChanged, this,
                [this](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::EndOfMedia) {
                emit playbackEnded();
            }
        });
        m_player->setSource(isResource ? QUrl(QStringLiteral("qrc") + path)
                                       : QUrl::fromLocalFile(path));
        m_player->play();
        return true;
    }

    return false;
}

bool LabibMediaCircle::tryLoadAny(const QStringList &candidates)
{
    for (const QString &p : candidates) {
        if (tryLoad(p)) return true;
    }
    return false;
}

void LabibMediaCircle::setFallbackPixmap(const QPixmap &p)
{
    m_fallback = p;
    update();
}

void LabibMediaCircle::setBorderColor(const QColor &c)
{
    m_borderColor = c;
    update();
}

void LabibMediaCircle::setRingColor(const QColor &c)
{
    m_ringColor = c;
    update();
}

void LabibMediaCircle::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    const QRectF outer = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    const qreal ringWidth = 3.0;
    const QRectF inner = outer.adjusted(ringWidth, ringWidth, -ringWidth, -ringWidth);

    // Halo vert externe (anneau de fond) : visible derrière le bord blanc.
    p.setBrush(m_ringColor);
    p.setPen(Qt::NoPen);
    p.drawEllipse(outer);

    // Contenu (vidéo / GIF / fallback) clippé en cercle.
    QPainterPath clipPath;
    clipPath.addEllipse(inner);
    p.setClipPath(clipPath);

    if (!m_currentFrame.isNull()) {
        // Centre-crop pour conserver l'aspect ratio de la vidéo.
        const QSizeF target = inner.size();
        const QSizeF src    = m_currentFrame.size();
        if (!src.isEmpty()) {
            const qreal scale = qMax(target.width() / src.width(),
                                     target.height() / src.height());
            const QSizeF scaled(src.width() * scale, src.height() * scale);
            const QPointF topLeft(inner.center().x() - scaled.width() / 2.0,
                                  inner.center().y() - scaled.height() / 2.0);
            p.drawImage(QRectF(topLeft, scaled), m_currentFrame);
        } else {
            p.drawImage(inner, m_currentFrame);
        }
    } else if (!m_fallback.isNull()) {
        const QSizeF target = inner.size();
        const QSizeF src    = m_fallback.size();
        if (!src.isEmpty()) {
            const qreal scale = qMax(target.width() / src.width(),
                                     target.height() / src.height());
            const QSizeF scaled(src.width() * scale, src.height() * scale);
            const QPointF topLeft(inner.center().x() - scaled.width() / 2.0,
                                  inner.center().y() - scaled.height() / 2.0);
            p.drawPixmap(QRectF(topLeft, scaled), m_fallback, m_fallback.rect());
        } else {
            p.drawPixmap(inner, m_fallback, m_fallback.rect());
        }
    } else {
        // Disque par défaut.
        p.fillRect(inner, m_ringColor);
    }

    // Liseré blanc fin (par-dessus le clip).
    p.setClipping(false);
    QPen pen(m_borderColor, ringWidth);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(outer.adjusted(ringWidth / 2.0, ringWidth / 2.0,
                                 -ringWidth / 2.0, -ringWidth / 2.0));
}

void LabibMediaCircle::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}
