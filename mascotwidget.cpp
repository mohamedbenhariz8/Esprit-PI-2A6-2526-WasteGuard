#include "mascotwidget.h"

MascotWidget::MascotWidget(QWidget *parent)
    : QWidget(parent), m_eyeCoverFactor(0.0), m_idleAngle(0.0)
{
    setMinimumSize(260, 260);
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        m_idleAngle += 0.04;
        update();
    });
    timer->start(16);
}

void MascotWidget::setEyeCoverFactor(qreal factor)
{
    m_eyeCoverFactor = factor;
    update();
}

void MascotWidget::setLookAt(const QPoint &globalPos)
{
    if (m_eyeCoverFactor > 0.6) return;
    QPoint localPos = mapFromGlobal(globalPos);
    QPoint center(width() / 2, height() / 2);
    qreal dx = (qreal)(localPos.x() - center.x()) / (width() / 2);
    qreal dy = (qreal)(localPos.y() - center.y()) / (height() / 2);
    m_lookPos = QPointF(std::max(-1.0, std::min(1.0, dx)) * 14.0, std::max(-1.0, std::min(1.0, dy)) * 14.0);
    update();
}

void MascotWidget::setState(State state)
{
    if (m_state == state) return;
    m_state = state;
    update();
}

void MascotWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    int w = width();
    int h = height();
    
    qreal breath = 1.0 + std::sin(m_idleAngle) * 0.012;
    qreal floatY = std::cos(m_idleAngle * 0.7) * 4.0;
    
    // --- TERRA PALETTE (Natural Fox) ---
    QColor luxeBase("#C66A4E");   // Terracotta/Fox Orange
    QColor luxeLight("#E9BC99");  // Warm Sand/Beige
    QColor luxeDark("#7A3B2E");   // Deep Earth/Marron
    QColor pearl("#F8F1E5");
    QColor earGlowColor(198, 106, 78, 100); // Orange Subsurface scattering
    QColor glassGloss(255, 255, 255, 180);

    painter.translate(w/2, h/2 + 35 + floatY);
    painter.scale(breath, breath);
    
    // Aura
    QRadialGradient aura(0, 0, 130);
    aura.setColorAt(0, QColor(198, 106, 78, 30));
    aura.setColorAt(1, Qt::transparent);
    painter.setBrush(aura);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(-130, -130, 260, 260);

    // Tail
    if (m_eyeCoverFactor > 0.05) {
        painter.save();
        qreal tailSway = std::sin(m_idleAngle * 0.6) * 6;
        painter.translate(38, 20);
        painter.rotate(-45 * m_eyeCoverFactor + tailSway);
        QPainterPath tail;
        tail.moveTo(0, 0);
        tail.cubicTo(45, -45, 65, 25, 0, 45);
        tail.cubicTo(-65, 25, -45, -45, 0, 0);
        QRadialGradient tG(0, -10, 80);
        tG.setColorAt(0, luxeLight); tG.setColorAt(1, luxeDark);
        painter.fillPath(tail, tG);
        painter.setBrush(luxeLight);
        painter.drawEllipse(-8, -52, 28, 32);
        painter.restore();
    }

    // Ears
    auto drawEar = [&](bool left) {
        painter.save();
        painter.translate(left ? -44 : 44, -45);
        qreal hideRot = left ? -78 * m_eyeCoverFactor : 78 * m_eyeCoverFactor;
        painter.rotate((left ? -32 : 32) + (m_lookPos.x()*0.65) + hideRot);
        QPainterPath ear;
        ear.moveTo(0, 15);
        ear.cubicTo(left?-55:55, -45, left?-45:45, -120, 0, -90);
        ear.cubicTo(left?45:-45, -120, left?55:-55, -45, 0, 15);
        QRadialGradient eG(0, -60, 110);
        eG.setColorAt(0, luxeBase); eG.setColorAt(1, luxeDark);
        painter.fillPath(ear, eG);
        QRadialGradient glow(left?10:-10, -50, 40);
        glow.setColorAt(0, earGlowColor); glow.setColorAt(1, Qt::transparent);
        painter.setBrush(glow);
        painter.drawPath(ear);
        QPainterPath inner;
        inner.moveTo(0, 5);
        inner.cubicTo(left?-38:38, -35, left?-32:32, -95, 0, -75);
        inner.cubicTo(left?32:-32, -95, left?38:-38, -35, 0, 5);
        QLinearGradient iG(0, -80, 0, 20);
        iG.setColorAt(0, "#A05030"); iG.setColorAt(0.5, "#C66A4E"); iG.setColorAt(1, luxeBase);
        painter.fillPath(inner, iG);
        painter.restore();
    };
    drawEar(true); drawEar(false);

    // Head
    painter.save();
    painter.rotate(10 * m_eyeCoverFactor);
    if (m_eyeCoverFactor > 0.5) {
        painter.setBrush(QColor(0,0,0,40));
        painter.drawEllipse(-55, -5, 110, 50);
    }
    QPainterPath head;
    head.moveTo(-58, 0); head.cubicTo(-65, -70, 65, -70, 58, 0);
    head.cubicTo(65, 55, -65, 55, -58, 0);
    QRadialGradient hG(-20, -30, 120);
    hG.setColorAt(0, luxeLight); hG.setColorAt(1, luxeDark);
    painter.setPen(Qt::NoPen);
    painter.setBrush(hG);
    painter.drawPath(head);
    painter.setBrush(glassGloss);
    QPolygonF diamond;
    diamond << QPointF(0, -45) << QPointF(9, -34) << QPointF(0, -23) << QPointF(-9, -34);
    painter.drawPolygon(diamond);
    painter.setBrush(QColor(255,255,255,80));
    painter.drawEllipse(-12, -40, 24, 12);
    QPainterPath snout;
    snout.moveTo(-42, 12); snout.cubicTo(-48, 48, 48, 48, 42, 12);
    painter.setBrush(luxeLight);
    painter.drawPath(snout);

    // Eyes
    auto drawEye = [&](bool left) {
        QPoint eyeC(left ? -28 : 28, -14);
        painter.setBrush(Qt::white);
        painter.drawEllipse(eyeC, 24, 24);
        if (m_eyeCoverFactor < 0.95) {
            QRadialGradient eyeG(eyeC + m_lookPos.toPoint(), 18);
            eyeG.setColorAt(0, "#000000"); eyeG.setColorAt(0.6, luxeDark); eyeG.setColorAt(1, luxeBase);
            painter.setBrush(eyeG);
            qreal pR = 13.0 * (1.0 - m_eyeCoverFactor);
            painter.drawEllipse(QPointF(eyeC) + m_lookPos, pR, pR);
            painter.setBrush(Qt::white);
            painter.drawEllipse(eyeC + m_lookPos.toPoint() + QPoint(-6, -6), 6, 6);
            painter.setBrush(QColor(255,255,255,100));
            painter.drawEllipse(eyeC + m_lookPos.toPoint() + QPoint(5, 6), 3, 3);
        }
    };
    drawEye(true); drawEye(false);

    // Nose
    painter.setBrush(Qt::black);
    painter.drawEllipse(-9, 22, 18, 12);
    painter.setBrush(Qt::white);
    painter.drawEllipse(-5, 23, 6, 4);

    // Paws
    if (m_eyeCoverFactor > 0.0) {
        qreal elastic = m_eyeCoverFactor < 0.82 ? m_eyeCoverFactor : 0.82 + (m_eyeCoverFactor-0.82)*1.4;
        int pY = 58 - (105 * elastic);
        auto drawPaw = [&](bool left) {
            int px = left ? -65 : 20;
            painter.setBrush(QColor(0,0,0,60));
            painter.drawRoundedRect(px+3, pY+3, 45, 60, 24, 24);
            QLinearGradient pG(px, pY, px, pY + 60);
            pG.setColorAt(0, luxeLight); pG.setColorAt(1, luxeDark);
            painter.setBrush(pG);
            painter.drawRoundedRect(px, pY, 45, 60, 24, 24);
        };
        drawPaw(true); drawPaw(false);
        painter.setBrush(luxeBase);
        int lidH = 28 * m_eyeCoverFactor;
        painter.drawRect(-52, -35, 48, lidH);
        painter.drawRect(4, -35, 48, lidH);
        painter.drawRect(-52, -35, 48, lidH);
        painter.drawRect(4, -35, 48, lidH);
    }

    // --- INTERACTIVE PROPS ---
    if (m_state == Notepad) {
        // Draw Notepad
        painter.save();
        painter.translate(-50, 40 + floatY*0.5); 
        painter.rotate(-15);
        
        // Pad
        painter.setBrush(QColor(245, 245, 220)); // Beige paper
        painter.setPen(QPen(QColor(139, 69, 19), 2));
        painter.drawRoundedRect(0, 0, 50, 60, 4, 4);
        
        // Lines
        painter.setPen(QPen(QColor(100, 100, 100, 50), 1));
        for(int i=10; i<55; i+=10) painter.drawLine(5, i, 45, i);

        painter.restore();

        // Draw Pencil
        painter.save();
        painter.translate(30, 45 + floatY*0.5);
        painter.rotate(15 + std::sin(m_idleAngle*2)*5); // Writing motion
        
        painter.setBrush(QColor(255, 204, 0)); // Yellow pencil
        painter.setPen(QPen(QColor(139, 69, 19), 1));
        QPolygon pencil;
        pencil << QPoint(0,0) << QPoint(8,0) << QPoint(8, -40) << QPoint(0, -40);
        painter.drawPolygon(pencil);
        
        // Tip
        painter.setBrush(QColor(245, 222, 179)); // Wood
        QPolygon tip;
        tip << QPoint(0,0) << QPoint(8,0) << QPoint(4, 10);
        painter.drawPolygon(tip);
        
        painter.setBrush(Qt::black); // Lead
        QPolygon lead;
        lead << QPoint(2,5) << QPoint(6,5) << QPoint(4, 10);
        painter.drawPolygon(lead);

        painter.restore();
    }
    else if (m_state == Magnifier) {
        // Draw Magnifying Glass
        painter.save();
        painter.translate(35, 30 + floatY*0.5);
        painter.rotate(25 + std::sin(m_idleAngle)*3);
        
        // Handle
        painter.setBrush(QColor(101, 67, 33)); // Dark wood
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(-6, 20, 12, 40, 4, 4);
        
        // Rim
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(QColor(192, 192, 192), 4)); // Silver rim
        painter.drawEllipse(-25, -25, 50, 50);
        
        // Glass
        QRadialGradient glassG(0,0, 25);
        glassG.setColorAt(0, QColor(255, 255, 255, 100));
        glassG.setColorAt(1, QColor(200, 230, 255, 50));
        painter.setBrush(glassG);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(-23, -23, 46, 46);
        
        // Reflection
        painter.setBrush(QColor(255, 255, 255, 180));
        painter.drawEllipse(-15, -15, 8, 6);

        painter.restore();
    }
}
