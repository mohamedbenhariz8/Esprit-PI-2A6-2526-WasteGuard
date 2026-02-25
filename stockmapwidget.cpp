#include "stockmapwidget.h"

#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QtMath>
#include <QApplication>
#include <QFontMetrics>
#include <QRandomGenerator>

// ════════════════════════════════════════════════════════════
//  Colors
// ════════════════════════════════════════════════════════════
static const QColor kBgTop         (0x02, 0x04, 0x0A);
static const QColor kBgBot         (0x08, 0x10, 0x22);
static const QColor kFloorDark     (0x12, 0x1A, 0x2E);
static const QColor kFloorLight    (0x1A, 0x25, 0x3C);
static const QColor kCeilingDark   (0x08, 0x0C, 0x18);
static const QColor kCeilingLight  (0x10, 0x18, 0x2A);
static const QColor kRackMetal     (0x2E, 0x40, 0x5C);
static const QColor kRackEdge      (0x48, 0x60, 0x80);
static const QColor kSlotAvail     (0x00, 0xE6, 0x76);
static const QColor kSlotOccupied  (0xFF, 0x3B, 0x5C);
static const QColor kSlotSelected  (0xFF, 0xD7, 0x00);
static const QColor kSlotHover     (0x00, 0xCC, 0xFF);
static const QColor kTextBright    (0xE8, 0xF0, 0xF8);
static const QColor kTextDim       (0x5A, 0x6A, 0x82);
static const QColor kAccent        (0x3B, 0x82, 0xF6);
static const QColor kLedStrip      (0x22, 0xD3, 0xEE);
static const QColor kWarnStripe    (0xFB, 0xBF, 0x24);

static const QStringList kProducts = {
    "Bac 120L", "Bac 240L", "Bac 360L", "Bac Bio",
    "Sac Tri", "Container", "Poubelle", "Benne",
    "Bac Jaune", "Bac Vert", "Bac Bleu", "Compacteur"
};

// ════════════════════════════════════════════════════════════
//  Warehouse3DView — Constructor
// ════════════════════════════════════════════════════════════

Warehouse3DView::Warehouse3DView(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(750, 500);

    // Glow animation
    m_glowAnim = new QPropertyAnimation(this, "glowPhase", this);
    m_glowAnim->setDuration(2200);
    m_glowAnim->setStartValue(0.0);
    m_glowAnim->setEndValue(1.0);
    m_glowAnim->setLoopCount(-1);
    m_glowAnim->setEasingCurve(QEasingCurve::InOutSine);
    m_glowAnim->start();

    // Particle tick
    m_particleTimer = new QTimer(this);
    connect(m_particleTimer, &QTimer::timeout, this, [this]() {
        for (auto &pt : m_particles) {
            pt.y += pt.speed;
            pt.alpha -= 0.004;
            if (pt.alpha <= 0 || pt.y > height() + 10) {
                pt.x = QRandomGenerator::global()->bounded((int)width());
                pt.y = QRandomGenerator::global()->bounded((int)(height() * 0.3));
                pt.alpha = 0.15 + QRandomGenerator::global()->bounded(50) / 100.0;
            }
        }
        update();
    });
    m_particleTimer->start(50);

    initParticles();
    rebuildSlots();
}

void Warehouse3DView::initParticles()
{
    m_particles.resize(25);
    for (auto &pt : m_particles) {
        pt.x     = QRandomGenerator::global()->bounded(900);
        pt.y     = QRandomGenerator::global()->bounded(600);
        pt.speed = 0.2 + QRandomGenerator::global()->bounded(80) / 100.0;
        pt.size  = 1.0 + QRandomGenerator::global()->bounded(25) / 10.0;
        pt.alpha = 0.1 + QRandomGenerator::global()->bounded(40) / 100.0;
        int k = QRandomGenerator::global()->bounded(3);
        pt.color = (k == 0) ? kLedStrip : (k == 1) ? kAccent : kSlotAvail;
    }
}

void Warehouse3DView::setOccupiedSlots(const QStringList &codes)
{
    for (auto &s : m_slots) s.occupied = codes.contains(s.code);
    update();
}

QString Warehouse3DView::selectedSlotCode() const
{
    if (m_selectedIdx >= 0 && m_selectedIdx < m_slots.size())
        return m_slots[m_selectedIdx].code;
    return {};
}

void Warehouse3DView::rebuildSlots()
{
    m_slots.clear();
    m_slots.reserve(COLS * ROWS * LEVELS);
    QRandomGenerator rng(42);
    for (int c = 0; c < COLS; ++c)
        for (int r = 0; r < ROWS; ++r)
            for (int l = 0; l < LEVELS; ++l) {
                SlotInfo si;
                si.col = c; si.row = r; si.level = l;
                si.occupied = rng.bounded(100) < 35;
                si.code = QString("%1%2-N%3").arg(QChar('A' + c)).arg(r + 1).arg(l + 1);
                si.product = si.occupied ? kProducts[rng.bounded(kProducts.size())] : QString();
                m_slots.append(si);
            }
}

void Warehouse3DView::setViewMode(ViewMode mode)
{
    if (m_viewMode == mode) return;
    m_viewMode = mode;
    m_hoveredIdx = -1;
    for (auto &s : m_slots) s.screenRect = QRectF();
    emit viewModeChanged(mode);
    update();
}

// ════════════════════════════════════════════════════════════
//  Perspective helpers (first-person)
// ════════════════════════════════════════════════════════════

qreal Warehouse3DView::perspScale(qreal depth) const
{
    return m_fov / qMax(depth, 0.4);
}

qreal Warehouse3DView::screenX(qreal worldX, qreal depth) const
{
    return m_vp.x() + worldX * perspScale(depth);
}

qreal Warehouse3DView::screenY(qreal worldY, qreal depth) const
{
    return m_vp.y() - worldY * perspScale(depth);
}

// ── Isometric projection (top-down) ──
static constexpr qreal ISO_SLOT_W = 48.0;
static constexpr qreal ISO_SLOT_H = 24.0;
static constexpr qreal ISO_SLOT_D = 16.0;
static constexpr qreal ISO_GAP_COL = 14.0;
static constexpr qreal ISO_GAP_ROW = 18.0;
static constexpr qreal ISO_LEVEL_H = ISO_SLOT_H + 6.0;

QPointF Warehouse3DView::isoProject(qreal x, qreal y, qreal z) const
{
    const qreal rad = qDegreesToRadians(30.0);
    const qreal cosA = qCos(rad);
    const qreal sinA = qSin(rad);
    qreal cx = x - m_isoCamX;
    qreal cy = y - m_isoCamY;
    qreal px = (cx - cy) * cosA;
    qreal py = (cx + cy) * sinA - z;
    qreal s = m_isoBaseScale * m_isoZoom;
    return QPointF(m_isoOrigin.x() + px * s,
                   m_isoOrigin.y() + py * s);
}

void Warehouse3DView::resizeEvent(QResizeEvent *)
{
    // First-person
    m_vp = QPointF(width() * 0.5, height() * 0.46);
    m_corridorHalfW = width() * 0.32;
    m_fov = width() * 0.28;

    // Top-down isometric
    qreal totalW = COLS * (ISO_SLOT_W + ISO_GAP_COL) + 80;
    qreal totalH = ROWS * (ISO_SLOT_D + ISO_GAP_ROW) + LEVELS * ISO_LEVEL_H + 80;
    qreal scaleX = width()  / (totalW * 2.2);
    qreal scaleY = height() / (totalH * 1.6);
    m_isoBaseScale = qMin(scaleX, scaleY);
    m_isoOrigin = QPointF(width() * 0.50, height() * 0.78);

    for (auto &pt : m_particles) {
        pt.x = QRandomGenerator::global()->bounded((int)width());
        pt.y = QRandomGenerator::global()->bounded((int)height());
    }
    update();
}

// ════════════════════════════════════════════════════════════
//  MAIN PAINT — First-person corridor
// ════════════════════════════════════════════════════════════

void Warehouse3DView::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    for (auto &s : m_slots) s.screenRect = QRectF();

    if (m_viewMode == ViewMode::TopDown) {
        paintTopDown(p);
        return;
    }

    // ── First-person mode ──
    drawBackground(p);
    drawFloor(p);
    drawCeiling(p);

    for (int r = ROWS - 1; r >= 0; --r) {
        qreal depth = (r - m_cameraZ) + 1.5;
        if (depth < 0.6) continue;
        int leftCol  = m_currentAisle;
        int rightCol = m_currentAisle + 1;
        if (leftCol >= 0 && leftCol < COLS)
            drawRackFace(p, leftCol, r, -1);
        if (rightCol >= 0 && rightCol < COLS)
            drawRackFace(p, rightCol, r, +1);
    }

    drawAisleSign(p);
    drawParticles(p);
    drawNavigationArrows(p);
    drawMinimap(p);
    drawHUD(p);
}

// ── Background ──
void Warehouse3DView::drawBackground(QPainter &p)
{
    QLinearGradient bg(0, 0, 0, height());
    bg.setColorAt(0.0, kBgTop);
    bg.setColorAt(0.5, kBgBot);
    bg.setColorAt(1.0, kBgTop);
    p.fillRect(rect(), bg);
}

// ── Floor with perspective lines ──
void Warehouse3DView::drawFloor(QPainter &p)
{
    // Floor quad
    QPolygonF floor;
    floor << QPointF(0, m_vp.y())
          << QPointF(width(), m_vp.y())
          << QPointF(width(), height())
          << QPointF(0, height());

    QLinearGradient floorGrad(0, m_vp.y(), 0, height());
    floorGrad.setColorAt(0.0, kFloorDark);
    floorGrad.setColorAt(1.0, kFloorLight);
    p.setBrush(floorGrad);
    p.setPen(Qt::NoPen);
    p.drawPolygon(floor);

    // Perspective grid lines on floor
    for (int i = 1; i <= 12; ++i) {
        qreal d = i * 0.8;
        qreal sy = screenY(0, d);
        if (sy < m_vp.y() || sy > height()) continue;
        int alpha = qMax(10, 80 - i * 5);
        p.setPen(QPen(QColor(40, 65, 100, alpha), 0.8));
        p.drawLine(0, (int)sy, width(), (int)sy);
    }

    // Center lane dashes (yellow)
    QPen lanePen(QColor(kWarnStripe.red(), kWarnStripe.green(), kWarnStripe.blue(), 70), 2, Qt::DashLine);
    p.setPen(lanePen);
    for (int i = 1; i <= 8; ++i) {
        qreal d = i * 1.0;
        qreal y1 = screenY(0, d);
        qreal y2 = screenY(0, d + 0.3);
        if (y1 < m_vp.y()) continue;
        p.drawLine((int)m_vp.x(), (int)y1, (int)m_vp.x(), (int)y2);
    }

    // Safety stripes at bottom (close to camera)
    for (int i = 0; i < 20; ++i) {
        int sx = i * (width() / 20);
        QColor sc = (i % 2 == 0) ? QColor(255, 200, 0, 25) : QColor(0, 0, 0, 0);
        p.fillRect(sx, height() - 8, width() / 20, 8, sc);
    }
}

// ── Ceiling ──
void Warehouse3DView::drawCeiling(QPainter &p)
{
    QPolygonF ceiling;
    ceiling << QPointF(0, 0)
            << QPointF(width(), 0)
            << QPointF(width(), m_vp.y())
            << QPointF(0, m_vp.y());

    QLinearGradient ceilGrad(0, 0, 0, m_vp.y());
    ceilGrad.setColorAt(0.0, kCeilingDark);
    ceilGrad.setColorAt(1.0, kCeilingLight);
    p.setBrush(ceilGrad);
    p.setPen(Qt::NoPen);
    p.drawPolygon(ceiling);

    // Ceiling lights (LED strips)
    for (int i = 1; i <= 6; ++i) {
        qreal d = i * 1.2;
        qreal sy = screenY(0.8, d);
        qreal sx1 = screenX(-0.15, d);
        qreal sx2 = screenX(0.15, d);
        if (sy > m_vp.y() - 5) continue;
        qreal gl = 0.3 + 0.4 * m_glowPhase;
        QColor lc = kLedStrip;
        lc.setAlphaF(gl * 0.6);
        p.setPen(QPen(lc, 2.5));
        p.drawLine(QPointF(sx1, sy), QPointF(sx2, sy));

        // Light cone
        QRadialGradient glow(QPointF((sx1 + sx2) / 2, sy), 30);
        glow.setColorAt(0, QColor(kLedStrip.red(), kLedStrip.green(), kLedStrip.blue(), (int)(40 * gl)));
        glow.setColorAt(1, QColor(0, 0, 0, 0));
        p.setBrush(glow);
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF((sx1 + sx2) / 2, sy), 25, 12);
    }
}

// ════════════════════════════════════════════════════════════
//  Draw one RACK FACE  (the key rendering function)
//  side: -1 = left wall, +1 = right wall
// ════════════════════════════════════════════════════════════

void Warehouse3DView::drawRackFace(QPainter &p, int col, int row, int side)
{
    qreal depth  = (row - m_cameraZ) + 1.5;
    qreal depth2 = depth + 0.9; // back edge

    if (depth < 0.5 || depth > 12.0) return;

    qreal wallDist = 0.9; // how far the rack is from center
    qreal rackW = 0.8;    // how deep the rack is into the aisle direction

    // ── Rack outer quad (side of the rack facing the aisle) ──
    qreal xOuter = side * wallDist;
    qreal topY   = 0.75;  // height of top of rack
    qreal botY   = 0.0;   // floor

    qreal x1 = screenX(xOuter, depth);
    qreal x2 = screenX(xOuter, depth2);
    qreal yt1 = screenY(topY, depth);
    qreal yt2 = screenY(topY, depth2);
    qreal yb1 = screenY(botY, depth);
    qreal yb2 = screenY(botY, depth2);

    // Background of the rack face
    QPolygonF faceQuad;
    faceQuad << QPointF(x1, yt1) << QPointF(x2, yt2)
             << QPointF(x2, yb2) << QPointF(x1, yb1);

    QLinearGradient rackGrad(QPointF(x1, yt1), QPointF(x1, yb1));
    rackGrad.setColorAt(0.0, kRackMetal.lighter(110));
    rackGrad.setColorAt(1.0, kRackMetal.darker(130));
    p.setBrush(rackGrad);
    p.setPen(QPen(kRackEdge, 1.0));
    p.drawPolygon(faceQuad);

    // ── Draw 3 level slots on this face ──
    qreal slotMargin = 0.03;
    qreal levelH = (topY - botY - slotMargin * 4) / LEVELS;

    for (int l = 0; l < LEVELS; ++l) {
        qreal slotBot = botY + slotMargin + l * (levelH + slotMargin);
        qreal slotTop = slotBot + levelH;

        // Find this slot in our data
        int idx = col * (ROWS * LEVELS) + row * LEVELS + l;
        if (idx < 0 || idx >= m_slots.size()) continue;
        SlotInfo &si = m_slots[idx];

        // Slot quad
        qreal sx1 = screenX(xOuter + side * 0.01, depth  + 0.05);
        qreal sx2 = screenX(xOuter + side * 0.01, depth2 - 0.05);
        qreal syt1 = screenY(slotTop, depth  + 0.05);
        qreal syt2 = screenY(slotTop, depth2 - 0.05);
        qreal syb1 = screenY(slotBot, depth  + 0.05);
        qreal syb2 = screenY(slotBot, depth2 - 0.05);

        QPolygonF slotQuad;
        slotQuad << QPointF(sx1, syt1) << QPointF(sx2, syt2)
                 << QPointF(sx2, syb2) << QPointF(sx1, syb1);

        si.screenRect = slotQuad.boundingRect();

        bool hovered  = (idx == m_hoveredIdx);
        bool selected = (idx == m_selectedIdx);

        QColor baseCol = si.occupied ? kSlotOccupied : kSlotAvail;
        if (selected)  baseCol = kSlotSelected;
        if (hovered && !si.occupied) baseCol = kSlotHover;

        qreal gl = (hovered || selected) ? m_glowPhase : 0.0;

        // Fill slot
        QLinearGradient sg(QPointF(sx1, syt1), QPointF(sx1, syb1));
        QColor c1 = baseCol; c1.setAlphaF(0.75 + 0.15 * gl);
        QColor c2 = baseCol.darker(180); c2.setAlphaF(0.45);
        sg.setColorAt(0, c1);
        sg.setColorAt(1, c2);
        p.setBrush(sg);
        p.setPen(QPen(baseCol.lighter(140), 0.6));
        p.drawPolygon(slotQuad);

        // Text label on slot
        QRectF textRect = slotQuad.boundingRect();
        if (textRect.height() > 14 && textRect.width() > 20) {
            if (si.occupied) {
                QFont pf("Segoe UI", qMax(6, qMin(9, (int)(textRect.height() * 0.3))));
                p.setFont(pf);
                p.setPen(QColor(255, 255, 255, 200));
                p.drawText(textRect, Qt::AlignCenter, "📦 " + si.product);
            } else {
                QFont cf("Consolas", qMax(6, qMin(10, (int)(textRect.height() * 0.35))), QFont::Bold);
                p.setFont(cf);
                p.setPen(QColor(255, 255, 255, 190));
                p.drawText(textRect, Qt::AlignCenter, si.code);
            }
        }

        // Glow border
        if (gl > 0.1) {
            QColor gc = baseCol; gc.setAlphaF(0.35 * gl);
            p.setBrush(Qt::NoBrush);
            p.setPen(QPen(gc, 2.0 + gl));
            p.drawPolygon(slotQuad);
        }

        // Shelf divider line
        p.setPen(QPen(kRackEdge, 0.5));
        p.drawLine(QPointF(sx1, syb1), QPointF(sx2, syb2));
    }

    // LED strip on top of rack
    qreal ledY = screenY(topY + 0.02, depth);
    qreal ledY2 = screenY(topY + 0.02, depth2);
    QColor ledC = kLedStrip;
    ledC.setAlphaF(0.35 + 0.3 * m_glowPhase);
    p.setPen(QPen(ledC, 2));
    p.drawLine(QPointF(x1, ledY), QPointF(x2, ledY2));
}

// ── Aisle sign at the end of the corridor ──
void Warehouse3DView::drawAisleSign(QPainter &p)
{
    int leftCol  = m_currentAisle;
    int rightCol = m_currentAisle + 1;
    QString label = QString("ALLÉE  %1 — %2")
                        .arg(leftCol < COLS ? QChar('A' + leftCol) : QChar('?'))
                        .arg(rightCol < COLS ? QChar('A' + rightCol) : QChar('?'));

    // Sign at vanishing point distance
    qreal d = 8.0;
    qreal signW = 0.5;
    qreal signH = 0.12;
    qreal signTop = 0.65;

    qreal sx1 = screenX(-signW / 2, d);
    qreal sx2 = screenX(signW / 2, d);
    qreal sy1 = screenY(signTop + signH, d);
    qreal sy2 = screenY(signTop, d);

    QRectF signRect(sx1, sy1, sx2 - sx1, sy2 - sy1);
    p.setBrush(QColor(0x0A, 0x15, 0x30, 200));
    p.setPen(QPen(kAccent, 1.2));
    p.drawRoundedRect(signRect, 4, 4);

    QFont sf("Segoe UI", 9, QFont::Bold);
    p.setFont(sf);
    p.setPen(kAccent.lighter(130));
    p.drawText(signRect, Qt::AlignCenter, label);
}

// ── Particles ──
void Warehouse3DView::drawParticles(QPainter &p)
{
    for (const auto &pt : m_particles) {
        QColor c = pt.color;
        c.setAlphaF(qBound(0.0, pt.alpha * 0.5, 1.0));
        p.setBrush(c);
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(pt.x, pt.y), pt.size, pt.size);
    }
}

// ── Navigation arrows ──
void Warehouse3DView::drawNavigationArrows(QPainter &p)
{
    int aw = 52, ah = 52;
    int cx = width() / 2, by = height() - 80;

    // Forward
    m_arrowFwd = QRectF(cx - aw / 2, by - ah - 8, aw, ah);
    // Back
    m_arrowBack = QRectF(cx - aw / 2, by + 8, aw, ah);
    // Left
    m_arrowLeft = QRectF(cx - aw - aw / 2 - 8, by - ah / 2 + 4, aw, ah);
    // Right
    m_arrowRight = QRectF(cx + aw / 2 + 8, by - ah / 2 + 4, aw, ah);

    auto drawArrow = [&](const QRectF &r, const QString &icon, const QString &hint, bool enabled) {
        QColor bg = enabled ? QColor(20, 35, 60, 180) : QColor(15, 20, 35, 100);
        QColor border = enabled ? kAccent : QColor(40, 55, 75);
        p.setBrush(bg);
        p.setPen(QPen(border, 1.2));
        p.drawRoundedRect(r, 12, 12);

        QFont af("Segoe UI", 18);
        p.setFont(af);
        p.setPen(enabled ? kTextBright : kTextDim);
        p.drawText(r, Qt::AlignCenter, icon);
    };

    drawArrow(m_arrowFwd,   "▲", "Avancer",   m_cameraZ < ROWS - 1);
    drawArrow(m_arrowBack,  "▼", "Reculer",   m_cameraZ > 0);
    drawArrow(m_arrowLeft,  "◄", "Allée ←",   m_currentAisle > 0);
    drawArrow(m_arrowRight, "►", "Allée →",   m_currentAisle < COLS - 2);

    // Labels
    QFont lf("Segoe UI", 7);
    p.setFont(lf);
    p.setPen(kTextDim);
    p.drawText(m_arrowFwd.adjusted(0, ah + 2, 0, 14), Qt::AlignHCenter | Qt::AlignTop, "W / ↑");
    p.drawText(m_arrowBack.adjusted(0, ah + 2, 0, 14), Qt::AlignHCenter | Qt::AlignTop, "S / ↓");
    p.drawText(m_arrowLeft.adjusted(0, ah + 2, 0, 14), Qt::AlignHCenter | Qt::AlignTop, "A / ←");
    p.drawText(m_arrowRight.adjusted(0, ah + 2, 0, 14), Qt::AlignHCenter | Qt::AlignTop, "D / →");
}

// ── Minimap ──
void Warehouse3DView::drawMinimap(QPainter &p)
{
    int mw = 160, mh = 130;
    int mx = width() - mw - 14, my = 14;

    p.setBrush(QColor(6, 10, 20, 220));
    p.setPen(QPen(kAccent, 1.0));
    p.drawRoundedRect(mx, my, mw, mh, 8, 8);

    QFont mf("Segoe UI", 7, QFont::Bold);
    p.setFont(mf);
    p.setPen(kTextDim);
    p.drawText(mx + 8, my + 13, "VUE D'ENSEMBLE");

    int padX = 12, padY = 22;
    qreal scX = (mw - padX * 2.0) / COLS;
    qreal scY = (mh - padY - 10.0) / ROWS;

    // Draw grid cells
    for (int c = 0; c < COLS; ++c) {
        for (int r = 0; r < ROWS; ++r) {
            int dx = mx + padX + (int)(c * scX);
            int dy = my + padY + (int)(r * scY);
            int dw = qMax(2, (int)scX - 2);
            int dh = qMax(2, (int)scY - 2);

            // Count status for this rack
            int occ = 0, avail = 0;
            for (int l = 0; l < LEVELS; ++l) {
                int idx = c * (ROWS * LEVELS) + r * LEVELS + l;
                if (idx >= 0 && idx < m_slots.size()) {
                    if (m_slots[idx].occupied) ++occ;
                    else ++avail;
                }
            }
            QColor dc = (occ > avail) ? kSlotOccupied : kSlotAvail;
            if (occ == 0) dc = kSlotAvail;
            if (avail == 0) dc = kSlotOccupied;
            dc.setAlphaF(0.6);
            p.setBrush(dc);
            p.setPen(Qt::NoPen);
            p.drawRoundedRect(dx, dy, dw, dh, 2, 2);
        }
    }

    // Column labels
    QFont cf("Consolas", 6);
    p.setFont(cf);
    p.setPen(kTextDim);
    for (int c = 0; c < COLS; ++c) {
        int dx = mx + padX + (int)(c * scX) + (int)(scX / 2) - 3;
        p.drawText(dx, my + mh - 3, QString(QChar('A' + c)));
    }

    // Draw camera position (the aisle the camera is in)
    qreal camAisleX = mx + padX + (m_currentAisle + 0.5) * scX;
    qreal camRowY   = my + padY + m_cameraZ * scY;
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(kSlotSelected, 2.0));
    p.drawEllipse(QPointF(camAisleX, camRowY), 5, 5);

    // Camera direction line
    p.setPen(QPen(kSlotSelected, 1.5));
    p.drawLine(QPointF(camAisleX, camRowY), QPointF(camAisleX, camRowY + scY * 1.5));

    // Highlight current aisle columns
    p.setPen(QPen(kAccent, 1.0, Qt::DotLine));
    {
        int lc = m_currentAisle;
        int rc = m_currentAisle + 1;
        if (lc >= 0 && lc < COLS) {
            int dx = mx + padX + (int)(lc * scX) - 1;
            p.drawRect(dx, my + padY - 2, (int)scX, (int)(ROWS * scY) + 2);
        }
        if (rc >= 0 && rc < COLS) {
            int dx = mx + padX + (int)(rc * scX) - 1;
            p.drawRect(dx, my + padY - 2, (int)scX, (int)(ROWS * scY) + 2);
        }
    }
}

// ── HUD ──
void Warehouse3DView::drawHUD(QPainter &p)
{
    // Title
    QFont tf("Segoe UI", 14, QFont::Bold);
    p.setFont(tf);
    p.setPen(kTextBright);
    p.drawText(18, 30, "🏭  Dépôt WasteGuard — Navigation Immersive");

    // Current aisle indicator
    QString aisleText = QString("📍  Allée %1-%2   |   Position %3/%4")
                            .arg(QChar('A' + m_currentAisle))
                            .arg(QChar('A' + m_currentAisle + 1))
                            .arg(qRound(m_cameraZ) + 1)
                            .arg(ROWS);
    QFont af("Segoe UI", 10, QFont::DemiBold);
    p.setFont(af);
    p.setPen(kAccent.lighter(120));
    p.drawText(18, 52, aisleText);

    // Legend
    int ly = 72;
    QFont lf("Segoe UI", 8, QFont::DemiBold);
    p.setFont(lf);
    auto drawLeg = [&](const QColor &c, const QString &label) {
        p.setBrush(c); p.setPen(Qt::NoPen);
        p.drawRoundedRect(18, ly, 11, 11, 3, 3);
        p.setPen(kTextBright);
        p.drawText(34, ly + 10, label);
        ly += 17;
    };
    drawLeg(kSlotAvail,    "Disponible");
    drawLeg(kSlotOccupied, "Occupé");
    drawLeg(kSlotSelected, "Sélectionné");

    // Stats bar
    int total = m_slots.size();
    int occ = 0;
    for (const auto &s : m_slots) if (s.occupied) ++occ;
    int avail = total - occ;
    double pct = 100.0 * avail / qMax(1, total);

    QFont sf("Consolas", 9, QFont::Bold);
    p.setFont(sf);
    QString stats = QString("Total: %1  │  Disponibles: %2  │  Occupés: %3  │  Taux: %4%")
                        .arg(total).arg(avail).arg(occ).arg(pct, 0, 'f', 1);
    QFontMetrics fm(sf);
    int sw = fm.horizontalAdvance(stats) + 28;
    QRectF bar(width() / 2 - sw / 2, 8, sw, 24);
    p.setBrush(QColor(6, 10, 22, 210));
    p.setPen(QPen(kAccent, 0.8));
    p.drawRoundedRect(bar, 6, 6);
    p.setPen(kTextBright);
    p.drawText(bar, Qt::AlignCenter, stats);

    // ── Tooltip ──
    if (m_hoveredIdx >= 0 && m_hoveredIdx < m_slots.size()) {
        const SlotInfo &hs = m_slots[m_hoveredIdx];
        QString info = hs.occupied
            ? QString("📦  %1  —  %2  [OCCUPÉ]").arg(hs.code, hs.product)
            : QString("✅  %1  —  DISPONIBLE  (cliquer pour sélectionner)").arg(hs.code);
        QFont tipF("Segoe UI", 11, QFont::Bold);
        p.setFont(tipF);
        QFontMetrics tfm(tipF);
        int tw = tfm.horizontalAdvance(info) + 30;
        QRectF tip(width() / 2 - tw / 2, height() - 40, tw, 32);
        p.setBrush(QColor(8, 16, 32, 230));
        p.setPen(QPen(hs.occupied ? kSlotOccupied : kSlotAvail, 1.5));
        p.drawRoundedRect(tip, 10, 10);
        p.setPen(hs.occupied ? kSlotOccupied : kSlotAvail);
        p.drawText(tip, Qt::AlignCenter, info);
    }
}

// ════════════════════════════════════════════════════════════
//  TOP-DOWN ISOMETRIC RENDERING
// ════════════════════════════════════════════════════════════

void Warehouse3DView::paintTopDown(QPainter &p)
{
    // Background
    QLinearGradient bg(0, 0, 0, height());
    bg.setColorAt(0.0, kBgTop);
    bg.setColorAt(1.0, kBgBot);
    p.fillRect(rect(), bg);
    QRadialGradient glow(width()/2, height()/2, width()*0.6);
    glow.setColorAt(0, QColor(59,130,246,10));
    glow.setColorAt(1, QColor(0,0,0,0));
    p.fillRect(rect(), glow);

    // Floor grid
    p.setPen(QPen(QColor(25,40,65,40), 0.5));
    for (int i = -5; i <= 25; ++i) {
        QPointF a = isoProject(i*30, -80, 0), b = isoProject(i*30, 500, 0);
        p.drawLine(a, b);
        QPointF c = isoProject(-80, i*30, 0), d = isoProject(500, i*30, 0);
        p.drawLine(c, d);
    }

    // Draw racks & slots back-to-front
    for (int r = ROWS - 1; r >= 0; --r) {
        for (int c = 0; c < COLS; ++c) {
            drawTopDownShelfFrame(p, c, r);
            for (int l = 0; l < LEVELS; ++l) {
                int idx = c * (ROWS * LEVELS) + r * LEVELS + l;
                if (idx < 0 || idx >= m_slots.size()) continue;
                SlotInfo &si = m_slots[idx];

                qreal wx = c * (ISO_SLOT_W + ISO_GAP_COL) + 40;
                qreal wy = r * (ISO_SLOT_D + ISO_GAP_ROW) + 40;
                qreal wz = l * ISO_LEVEL_H;

                QPointF tl = isoProject(wx, wy, wz + ISO_SLOT_H);
                QPointF br = isoProject(wx + ISO_SLOT_W, wy + ISO_SLOT_D, wz);
                si.screenRect = QRectF(tl, br).normalized();

                drawTopDownSlotBox(p, si, (idx == m_hoveredIdx), (idx == m_selectedIdx));
            }
        }
    }

    // Aisle labels
    QFont af("Consolas", 10, QFont::Bold);
    p.setFont(af);
    for (int c = 0; c < COLS; ++c) {
        qreal wx = c * (ISO_SLOT_W + ISO_GAP_COL) + 40 + ISO_SLOT_W / 2;
        QPointF pos = isoProject(wx - 14, 15, 0);
        QFontMetrics fm(af);
        QString lbl = QString("ALLÉE %1").arg(QChar('A' + c));
        int tw = fm.horizontalAdvance(lbl) + 14;
        QRectF pill(pos.x() - tw/2, pos.y() - 9, tw, 20);
        p.setBrush(QColor(59,130,246,30));
        p.setPen(QPen(kAccent, 0.8));
        p.drawRoundedRect(pill, 5, 5);
        p.setPen(kAccent.lighter(130));
        p.drawText(pill, Qt::AlignCenter, lbl);
    }

    drawParticles(p);

    // Title
    QFont tf("Segoe UI", 14, QFont::Bold);
    p.setFont(tf);
    p.setPen(kTextBright);
    p.drawText(18, 30, "🏭  Dépôt WasteGuard — Vue Aérienne");

    // Legend
    int ly = 52;
    QFont lf("Segoe UI", 8, QFont::DemiBold);
    p.setFont(lf);
    auto drawLeg = [&](const QColor &co, const QString &label) {
        p.setBrush(co); p.setPen(Qt::NoPen);
        p.drawRoundedRect(18, ly, 11, 11, 3, 3);
        p.setPen(kTextBright);
        p.drawText(34, ly + 10, label);
        ly += 17;
    };
    drawLeg(kSlotAvail, "Disponible");
    drawLeg(kSlotOccupied, "Occupé");
    drawLeg(kSlotSelected, "Sélectionné");

    // Controls hint
    p.setFont(QFont("Segoe UI", 8));
    p.setPen(kTextDim);
    p.drawText(18, height() - 20, "🖱️ Glisser pour naviguer  |  🔍 Molette pour zoomer");

    // Stats
    int total = m_slots.size(), occ = 0;
    for (const auto &s : m_slots) if (s.occupied) ++occ;
    int avail = total - occ;
    double pct = 100.0 * avail / qMax(1, total);
    QFont sf("Consolas", 9, QFont::Bold);
    p.setFont(sf);
    QString stats = QString("Total: %1  │  Disponibles: %2  │  Occupés: %3  │  Taux: %4%")
                        .arg(total).arg(avail).arg(occ).arg(pct,0,'f',1);
    QFontMetrics sfm(sf);
    int sw = sfm.horizontalAdvance(stats) + 28;
    QRectF bar(width()/2 - sw/2, 8, sw, 24);
    p.setBrush(QColor(6,10,22,210));
    p.setPen(QPen(kAccent, 0.8));
    p.drawRoundedRect(bar, 6, 6);
    p.setPen(kTextBright);
    p.drawText(bar, Qt::AlignCenter, stats);

    // Tooltip
    if (m_hoveredIdx >= 0 && m_hoveredIdx < m_slots.size()) {
        const SlotInfo &hs = m_slots[m_hoveredIdx];
        QString info = hs.occupied
            ? QString("📦  %1  —  %2  [OCCUPÉ]").arg(hs.code, hs.product)
            : QString("✅  %1  —  DISPONIBLE").arg(hs.code);
        QFont tipF("Segoe UI", 11, QFont::Bold);
        p.setFont(tipF);
        QFontMetrics tfm2(tipF);
        int tw2 = tfm2.horizontalAdvance(info) + 30;
        QRectF tip(width()/2 - tw2/2, height() - 45, tw2, 32);
        p.setBrush(QColor(8,16,32,230));
        p.setPen(QPen(hs.occupied ? kSlotOccupied : kSlotAvail, 1.5));
        p.drawRoundedRect(tip, 10, 10);
        p.setPen(hs.occupied ? kSlotOccupied : kSlotAvail);
        p.drawText(tip, Qt::AlignCenter, info);
    }

    // Zoom indicator
    p.setFont(QFont("Consolas", 8));
    p.setPen(kTextDim);
    p.drawText(width()-80, height()-10, QString("Zoom: %1x").arg(m_isoZoom, 0, 'f', 1));
}

void Warehouse3DView::drawTopDownShelfFrame(QPainter &p, int col, int row)
{
    qreal wx = col * (ISO_SLOT_W + ISO_GAP_COL) + 40;
    qreal wy = row * (ISO_SLOT_D + ISO_GAP_ROW) + 40;
    qreal totalH = LEVELS * ISO_LEVEL_H + 6;

    QPen postPen(kRackEdge.lighter(120), 1.4);
    p.setPen(postPen);
    auto drawPost = [&](qreal ox, qreal oy) {
        QPointF bot = isoProject(wx+ox, wy+oy, 0);
        QPointF top = isoProject(wx+ox, wy+oy, totalH);
        p.drawLine(bot, top);
    };
    drawPost(-1,-1); drawPost(ISO_SLOT_W+1,-1);
    drawPost(-1,ISO_SLOT_D+1); drawPost(ISO_SLOT_W+1,ISO_SLOT_D+1);

    QPen beamPen(kRackMetal, 0.6);
    p.setPen(beamPen);
    for (int l = 0; l <= LEVELS; ++l) {
        qreal z = l * ISO_LEVEL_H;
        QPointF fl = isoProject(wx-1, wy-1, z);
        QPointF fr = isoProject(wx+ISO_SLOT_W+1, wy-1, z);
        QPointF bl = isoProject(wx-1, wy+ISO_SLOT_D+1, z);
        QPointF br = isoProject(wx+ISO_SLOT_W+1, wy+ISO_SLOT_D+1, z);
        p.drawLine(fl, fr); p.drawLine(fl, bl);
        p.drawLine(fr, br); p.drawLine(bl, br);
    }
}

void Warehouse3DView::drawTopDownSlotBox(QPainter &p, const SlotInfo &s, bool hovered, bool selected)
{
    qreal wx = s.col * (ISO_SLOT_W + ISO_GAP_COL) + 40;
    qreal wy = s.row * (ISO_SLOT_D + ISO_GAP_ROW) + 40;
    qreal wz = s.level * ISO_LEVEL_H;
    qreal m = 2.0;

    QPointF ftl = isoProject(wx+m, wy+1, wz+ISO_SLOT_H-m);
    QPointF ftr = isoProject(wx+ISO_SLOT_W-m, wy+1, wz+ISO_SLOT_H-m);
    QPointF fbl = isoProject(wx+m, wy+1, wz+m);
    QPointF fbr = isoProject(wx+ISO_SLOT_W-m, wy+1, wz+m);
    QPointF btl = isoProject(wx+m, wy+ISO_SLOT_D-1, wz+ISO_SLOT_H-m);
    QPointF btr = isoProject(wx+ISO_SLOT_W-m, wy+ISO_SLOT_D-1, wz+ISO_SLOT_H-m);
    QPointF bbr = isoProject(wx+ISO_SLOT_W-m, wy+ISO_SLOT_D-1, wz+m);

    QColor baseCol = s.occupied ? kSlotOccupied : kSlotAvail;
    if (selected)  baseCol = kSlotSelected;
    if (hovered && !s.occupied) baseCol = kSlotHover;
    qreal gl = (hovered || selected) ? m_glowPhase : 0.0;

    // Top face
    { QPolygonF f; f << ftl << ftr << btr << btl;
      QColor c = baseCol.lighter(150); c.setAlphaF(0.65+0.2*gl);
      p.setBrush(c); p.setPen(QPen(baseCol.lighter(180),0.5)); p.drawPolygon(f); }

    // Front face
    { QPolygonF f; f << ftl << ftr << fbr << fbl;
      QLinearGradient g(ftl,fbl);
      QColor c1=baseCol; c1.setAlphaF(0.75+0.15*gl);
      QColor c2=baseCol.darker(170); c2.setAlphaF(0.5);
      g.setColorAt(0,c1); g.setColorAt(1,c2);
      p.setBrush(g); p.setPen(QPen(baseCol.lighter(120),0.4)); p.drawPolygon(f);
      // Code on front
      QRectF tr2 = QRectF(fbl, ftr).normalized();
      if (tr2.height() > 8) {
          QFont cf("Consolas", 6, QFont::Bold); p.setFont(cf);
          p.setPen(QColor(255,255,255,180));
          p.drawText(tr2, Qt::AlignCenter, s.code);
      }
    }

    // Right face
    { QPolygonF f; f << ftr << btr << bbr << fbr;
      QColor c=baseCol.darker(150); c.setAlphaF(0.5+0.1*gl);
      p.setBrush(c); p.setPen(QPen(baseCol.darker(120),0.3)); p.drawPolygon(f); }

    // Glow
    if (gl > 0.1) {
        QRectF gr = s.screenRect.adjusted(-4,-4,4,4);
        QColor gc=baseCol; gc.setAlphaF(0.2*gl);
        p.setBrush(Qt::NoBrush); p.setPen(QPen(gc,1.5+gl));
        p.drawRoundedRect(gr, 3, 3);
    }
}

// ════════════════════════════════════════════════════════════
//  Mouse & Keyboard  (dual-mode)
// ════════════════════════════════════════════════════════════

void Warehouse3DView::mousePressEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton) return;
    QPointF pos = e->position();

    if (m_viewMode == ViewMode::TopDown) {
        // Check slot clicks first
        for (int i = m_slots.size()-1; i >= 0; --i) {
            if (m_slots[i].screenRect.isValid() && m_slots[i].screenRect.contains(pos)) {
                if (!m_slots[i].occupied) {
                    m_selectedIdx = i;
                    emit slotSelected(m_slots[i].code);
                }
                update(); return;
            }
        }
        // Start dragging
        m_isoDragging = true;
        m_isoDragLast = pos;
        setCursor(Qt::ClosedHandCursor);
        return;
    }

    // First-person: navigation arrows
    if (m_arrowFwd.contains(pos) && m_cameraZ < ROWS-1) { m_cameraZ += 1.0; update(); return; }
    if (m_arrowBack.contains(pos) && m_cameraZ > 0)     { m_cameraZ -= 1.0; update(); return; }
    if (m_arrowLeft.contains(pos) && m_currentAisle > 0) { m_currentAisle--; emit aisleChanged(m_currentAisle); update(); return; }
    if (m_arrowRight.contains(pos) && m_currentAisle < COLS-2) { m_currentAisle++; emit aisleChanged(m_currentAisle); update(); return; }

    // First-person: slot clicks
    for (int i = m_slots.size()-1; i >= 0; --i) {
        if (m_slots[i].screenRect.isValid() && m_slots[i].screenRect.contains(pos)) {
            if (!m_slots[i].occupied) {
                m_selectedIdx = i;
                emit slotSelected(m_slots[i].code);
            }
            update(); return;
        }
    }
}

void Warehouse3DView::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && m_isoDragging) {
        m_isoDragging = false;
        setCursor(m_viewMode == ViewMode::TopDown ? Qt::OpenHandCursor : Qt::ArrowCursor);
    }
}

void Warehouse3DView::mouseMoveEvent(QMouseEvent *e)
{
    QPointF pos = e->position();

    if (m_viewMode == ViewMode::TopDown && m_isoDragging) {
        QPointF d = pos - m_isoDragLast;
        qreal sp = 1.0 / (m_isoBaseScale * m_isoZoom);
        m_isoCamX -= d.x() * sp * 0.8;
        m_isoCamY -= d.y() * sp * 1.4;
        m_isoDragLast = pos;
        update(); return;
    }

    int old = m_hoveredIdx;
    m_hoveredIdx = -1;
    for (int i = m_slots.size()-1; i >= 0; --i) {
        if (m_slots[i].screenRect.isValid() && m_slots[i].screenRect.contains(pos)) {
            m_hoveredIdx = i;
            break;
        }
    }
    if (m_hoveredIdx != old) {
        if (m_viewMode == ViewMode::TopDown)
            setCursor(m_hoveredIdx >= 0 ? Qt::PointingHandCursor : Qt::OpenHandCursor);
        else
            setCursor(m_hoveredIdx >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
        if (m_hoveredIdx >= 0)
            emit slotHovered(m_slots[m_hoveredIdx].code);
        update();
    }
}

void Warehouse3DView::wheelEvent(QWheelEvent *e)
{
    qreal delta = e->angleDelta().y() / 120.0;
    if (m_viewMode == ViewMode::TopDown) {
        m_isoZoom = qBound(0.4, m_isoZoom + delta * 0.15, 3.5);
    } else {
        if (delta > 0 && m_cameraZ < ROWS-1)
            m_cameraZ = qMin((qreal)(ROWS-1), m_cameraZ + 0.5);
        else if (delta < 0 && m_cameraZ > 0)
            m_cameraZ = qMax(0.0, m_cameraZ - 0.5);
    }
    update();
}

void Warehouse3DView::keyPressEvent(QKeyEvent *e)
{
    if (m_viewMode == ViewMode::TopDown) {
        qreal step = 20.0 / m_isoZoom;
        switch (e->key()) {
        case Qt::Key_W: case Qt::Key_Up:    m_isoCamY -= step; break;
        case Qt::Key_S: case Qt::Key_Down:  m_isoCamY += step; break;
        case Qt::Key_A: case Qt::Key_Left:  m_isoCamX -= step; break;
        case Qt::Key_D: case Qt::Key_Right: m_isoCamX += step; break;
        case Qt::Key_Home: m_isoCamX=0; m_isoCamY=0; m_isoZoom=1.0; break;
        default: QWidget::keyPressEvent(e); return;
        }
        update(); return;
    }

    switch (e->key()) {
    case Qt::Key_W: case Qt::Key_Up:
        if (m_cameraZ < ROWS-1) { m_cameraZ += 1.0; update(); } break;
    case Qt::Key_S: case Qt::Key_Down:
        if (m_cameraZ > 0) { m_cameraZ -= 1.0; update(); } break;
    case Qt::Key_A: case Qt::Key_Left:
        if (m_currentAisle > 0) { m_currentAisle--; emit aisleChanged(m_currentAisle); update(); } break;
    case Qt::Key_D: case Qt::Key_Right:
        if (m_currentAisle < COLS-2) { m_currentAisle++; emit aisleChanged(m_currentAisle); update(); } break;
    default:
        QWidget::keyPressEvent(e); return;
    }
}


// ════════════════════════════════════════════════════════════
//  StockMapDialog
// ════════════════════════════════════════════════════════════

StockMapDialog::StockMapDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("🗺️  Plan du Dépôt — Navigation Immersive");
    setMinimumSize(900, 640);
    resize(1100, 750);

    setStyleSheet(
        "StockMapDialog { background-color: #050810; }"
        "QLabel { color: #E0E8F0; background: transparent; }"
        "QPushButton {"
        "  border-radius: 10px; padding: 10px 28px;"
        "  font-size: 13px; font-weight: 700; border: none;"
        "}"
    );

    auto *ml = new QVBoxLayout(this);
    ml->setContentsMargins(10, 8, 10, 8);
    ml->setSpacing(6);

    // ── Top bar ──
    auto *topBar = new QHBoxLayout;
    m_lblInfo = new QLabel("🎯  Utilisez WASD / flèches / boutons pour naviguer dans les allées. Cliquez un slot vert.", this);
    m_lblInfo->setStyleSheet("color:#7A8AA0; font-size:11px; padding:6px 14px;"
                             "background:rgba(12,20,38,220); border-radius:8px;"
                             "border:1px solid rgba(59,130,246,0.2);");
    m_lblInfo->setAlignment(Qt::AlignCenter);

    m_lblAisle = new QLabel("Allée A-B", this);
    m_lblAisle->setStyleSheet("color:#3B82F6; font-size:12px; font-weight:700;"
                              "padding:6px 14px; background:rgba(59,130,246,0.08);"
                              "border-radius:8px; border:1px solid rgba(59,130,246,0.25);");
    m_lblAisle->setAlignment(Qt::AlignCenter);
    m_lblAisle->setFixedWidth(130);

    topBar->addWidget(m_lblInfo, 1);
    topBar->addWidget(m_lblAisle);
    ml->addLayout(topBar);

    // ── 3D View ──
    m_view = new Warehouse3DView(this);
    m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ml->addWidget(m_view, 1);

    // ── Bottom bar ──
    auto *bb = new QHBoxLayout;
    bb->setSpacing(10);

    m_lblSelected = new QLabel("  Aucun emplacement sélectionné", this);
    m_lblSelected->setStyleSheet("color:#6B7B93; font-size:13px; font-weight:600;"
                                 "padding:8px 16px; background:rgba(15,25,42,200);"
                                 "border:1px solid rgba(50,70,100,0.3); border-radius:10px;");
    bb->addWidget(m_lblSelected, 1);

    // Toggle view button
    m_btnToggleView = new QPushButton("🛰️  Vue Aérienne", this);
    m_btnToggleView->setStyleSheet(
        "QPushButton { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #1E40AF,stop:1 #3B82F6); color:white; }"
        "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #2563EB,stop:1 #60A5FA); }"
    );
    m_btnToggleView->setCursor(Qt::PointingHandCursor);
    m_btnToggleView->setMinimumHeight(42);
    m_btnToggleView->setMinimumWidth(160);

    m_btnCancel = new QPushButton("Annuler", this);
    m_btnCancel->setStyleSheet("QPushButton { background:#1E293B; color:#94A3B8; border:1px solid #334155; }"
                               "QPushButton:hover { background:#334155; color:#E2E8F0; }");
    m_btnCancel->setCursor(Qt::PointingHandCursor);
    m_btnCancel->setMinimumHeight(42);

    m_btnConfirm = new QPushButton("✅  Confirmer l'emplacement", this);
    m_btnConfirm->setEnabled(false);
    m_btnConfirm->setStyleSheet(
        "QPushButton { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #00C853,stop:1 #00E676); color:#050810; font-size:14px; }"
        "QPushButton:hover { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #00E676,stop:1 #69F0AE); }"
        "QPushButton:disabled { background:#121A2E; color:#3A4A62; }"
    );
    m_btnConfirm->setCursor(Qt::PointingHandCursor);
    m_btnConfirm->setMinimumHeight(42);
    m_btnConfirm->setMinimumWidth(220);

    bb->addWidget(m_btnToggleView);
    bb->addWidget(m_btnCancel);
    bb->addWidget(m_btnConfirm);
    ml->addLayout(bb);

    // ── Connections ──
    connect(m_btnCancel,  &QPushButton::clicked, this, &QDialog::reject);
    connect(m_btnConfirm, &QPushButton::clicked, this, &QDialog::accept);

    connect(m_btnToggleView, &QPushButton::clicked, this, [this]() {
        if (m_view->viewMode() == ViewMode::FirstPerson) {
            m_view->setViewMode(ViewMode::TopDown);
            m_btnToggleView->setText("🚶  Vue Immersive");
            m_lblInfo->setText("🛰️  Vue aérienne — Glissez pour naviguer, molette pour zoomer. Cliquez un slot vert.");
        } else {
            m_view->setViewMode(ViewMode::FirstPerson);
            m_btnToggleView->setText("🛰️  Vue Aérienne");
            m_lblInfo->setText("🎯  Utilisez WASD / flèches / boutons pour naviguer dans les allées. Cliquez un slot vert.");
        }
        m_view->setFocus();
    });

    connect(m_view, &Warehouse3DView::slotSelected, this, [this](const QString &code) {
        m_lblSelected->setText(QString("📍  Emplacement : %1").arg(code));
        m_lblSelected->setStyleSheet("color:#FFD700; font-size:14px; font-weight:700;"
                                     "padding:8px 16px; background:rgba(255,215,0,0.08);"
                                     "border:1px solid rgba(255,215,0,0.3); border-radius:10px;");
        m_btnConfirm->setEnabled(true);
    });

    connect(m_view, &Warehouse3DView::aisleChanged, this, [this](int aisle) {
        m_lblAisle->setText(QString("Allée %1-%2")
                                .arg(QChar('A' + aisle))
                                .arg(QChar('A' + aisle + 1)));
    });

    m_view->setFocus();
}

QString StockMapDialog::selectedCode() const { return m_view->selectedSlotCode(); }
void StockMapDialog::setOccupiedSlots(const QStringList &codes) { m_view->setOccupiedSlots(codes); }
