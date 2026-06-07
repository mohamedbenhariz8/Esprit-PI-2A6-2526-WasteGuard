#ifndef LABIBMEDIACIRCLE_H
#define LABIBMEDIACIRCLE_H

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QColor>

class QMovie;
class QMediaPlayer;
class QAudioOutput;
class QVideoSink;

/**
 * Widget circulaire qui peut afficher :
 *   - un GIF animé (via QMovie),
 *   - une vidéo (mp4/webm/mov, via QMediaPlayer + QVideoSink, audio coupé),
 *   - ou en repli un QPixmap statique (logo).
 * Le rendu est clippé en cercle avec un liseré blanc + bordure verte.
 * Émet clicked() sur clic gauche.
 */
class LabibMediaCircle : public QWidget
{
    Q_OBJECT

public:
    explicit LabibMediaCircle(QWidget *parent = nullptr);
    ~LabibMediaCircle() override;

    // Tente de charger un média. Retourne false si le fichier n'existe pas
    // ou ne peut pas être lu. Cherche aussi dans les ressources (":/...").
    bool tryLoad(const QString &pathOrResource);

    // Tente plusieurs chemins, dans l'ordre. Retourne le premier qui marche.
    bool tryLoadAny(const QStringList &candidates);

    // Image affichée si aucun média n'est chargé (ou en attente du 1er frame).
    void setFallbackPixmap(const QPixmap &p);

    // Couleur du liseré blanc et de la bordure externe.
    void setBorderColor(const QColor &c);
    void setRingColor(const QColor &c);

    // Nombre de fois que le media est joué (1 = une seule fois,
    // QMediaPlayer::Infinite = boucle infinie). Par défaut: infinie.
    // Doit être appelé AVANT tryLoad pour s'appliquer dès le 1er chargement,
    // mais s'applique aussi au player courant s'il existe.
    void setLoopCount(int count);

signals:
    void clicked();
    // Emis quand le QMediaPlayer atteint la fin du media (utile en mode
    // setLoopCount(1) pour synchroniser avec d'autres animations).
    void playbackEnded();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QMovie        *m_movie  = nullptr;
    QMediaPlayer  *m_player = nullptr;
    QAudioOutput  *m_audio  = nullptr;
    QVideoSink    *m_sink   = nullptr;
    QImage         m_currentFrame;
    QPixmap        m_fallback;
    QColor         m_borderColor; // liseré interne (blanc)
    QColor         m_ringColor;   // halo externe (vert)
    int            m_loopCount = -1; // -1 = QMediaPlayer::Infinite
};

#endif // LABIBMEDIACIRCLE_H
