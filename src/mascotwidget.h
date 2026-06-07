#ifndef MASCOTWIDGET_H
#define MASCOTWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QLinearGradient>
#include <QTimer>
#include <QPointF>
#include <cmath>

class MascotWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal eyeCoverFactor READ eyeCoverFactor WRITE setEyeCoverFactor)

public:
    explicit MascotWidget(QWidget *parent = nullptr);

    qreal eyeCoverFactor() const { return m_eyeCoverFactor; }
    void setEyeCoverFactor(qreal factor);

    void setLookAt(const QPoint &globalPos);

    enum State {
        Idle,
        Notepad,
        Magnifier
    };
    Q_ENUM(State)

    void setState(State params);
    State state() const { return m_state; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal m_eyeCoverFactor;
    qreal m_idleAngle;
    QPointF m_lookPos;
    State m_state = Idle;
};

#endif // MASCOTWIDGET_H
