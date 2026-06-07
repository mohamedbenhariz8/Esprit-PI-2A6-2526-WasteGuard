#ifndef STOCKMAPWIDGET_H
#define STOCKMAPWIDGET_H

#include <QDialog>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QVector>
#include <QString>
#include <QPropertyAnimation>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPointF>

// ──────────────────────────────────────────────
//  SlotInfo  –  one storage position in the depot
// ──────────────────────────────────────────────
struct SlotInfo {
    int col;           // 0-based column  (A–F)
    int row;           // 0-based row     (1–5)
    int level;         // 0-based level   (shelf 1–3)
    bool occupied;
    QString code;      // e.g. "A1-N1"
    QString product;   // mock product name if occupied
    QRectF screenRect; // bounding rect on screen for hit-testing
};

// ──────────────────────────────────────────────
//  Particle – floating dust/light
// ──────────────────────────────────────────────
struct Particle {
    qreal x, y;        // screen-relative
    qreal speed;
    qreal size;
    qreal alpha;
    QColor color;
};

// ──────────────────────────────────────────────
//  ViewMode
// ──────────────────────────────────────────────
enum class ViewMode { FirstPerson, TopDown };

// ──────────────────────────────────────────────
//  Warehouse3DView  –  dual-mode warehouse view
// ──────────────────────────────────────────────
class Warehouse3DView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal glowPhase READ glowPhase WRITE setGlowPhase)

public:
    explicit Warehouse3DView(QWidget *parent = nullptr);

    qreal glowPhase() const { return m_glowPhase; }
    void  setGlowPhase(qreal v) { m_glowPhase = v; update(); }

    QString selectedSlotCode() const;
    void    setOccupiedSlots(const QStringList &codes);

    ViewMode viewMode() const { return m_viewMode; }
    void     setViewMode(ViewMode mode);

signals:
    void slotSelected(const QString &code);
    void slotHovered(const QString &code);
    void aisleChanged(int aisle);
    void viewModeChanged(ViewMode mode);

protected:
    void paintEvent(QPaintEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;
    void keyPressEvent(QKeyEvent *) override;
    void resizeEvent(QResizeEvent *) override;

private:
    void rebuildSlots();
    void initParticles();

    // Drawing – First Person
    void drawBackground(QPainter &p);
    void drawFloor(QPainter &p);
    void drawCeiling(QPainter &p);
    void drawRackFace(QPainter &p, int col, int row, int side);
    void drawAisleSign(QPainter &p);
    void drawParticles(QPainter &p);
    void drawMinimap(QPainter &p);
    void drawHUD(QPainter &p);
    void drawNavigationArrows(QPainter &p);
    void drawLabibBack(QPainter &p);
    void drawLabibFaceMinimap(QPainter &p, qreal cx, qreal cy);

    // Drawing – Top Down
    void paintTopDown(QPainter &p);
    QPointF isoProject(qreal x, qreal y, qreal z) const;
    void drawTopDownSlotBox(QPainter &p, const SlotInfo &s, bool hovered, bool selected);
    void drawTopDownShelfFrame(QPainter &p, int col, int row);

    // Perspective helpers
    qreal perspScale(qreal depth) const;
    qreal screenX(qreal worldX, qreal depth) const;
    qreal screenY(qreal worldY, qreal depth) const;

    QVector<SlotInfo> m_slots;
    QVector<Particle> m_particles;

    int m_hoveredIdx  = -1;
    int m_selectedIdx = -1;
    qreal m_glowPhase = 0.0;
    ViewMode m_viewMode = ViewMode::FirstPerson;

    // Camera / navigation (first-person)
    int   m_currentAisle = 2;
    qreal m_cameraZ      = 0.5;
    qreal m_fov          = 300.0;

    // Camera (top-down isometric)
    qreal m_isoCamX = 0.0;
    qreal m_isoCamY = 0.0;
    qreal m_isoZoom = 1.0;
    qreal m_isoBaseScale = 1.0;
    QPointF m_isoOrigin;
    bool  m_isoDragging = false;
    QPointF m_isoDragLast;

    // Grid dimensions
    static constexpr int COLS   = 6;
    static constexpr int ROWS   = 5;
    static constexpr int LEVELS = 3;

    // Vanishing point (computed on resize)
    QPointF m_vp;
    qreal m_corridorHalfW = 200;

    // Animations
    QPropertyAnimation *m_glowAnim = nullptr;
    QTimer *m_particleTimer = nullptr;
    qreal m_labibAnimPhase = 0.0;
    bool  m_isMoving = false;

    // Navigation arrow rects for click
    QRectF m_arrowLeft, m_arrowRight, m_arrowFwd, m_arrowBack;
};

// ──────────────────────────────────────────────
//  StockMapDialog
// ──────────────────────────────────────────────
class StockMapDialog : public QDialog
{
    Q_OBJECT
public:
    explicit StockMapDialog(QWidget *parent = nullptr);
    QString selectedCode() const;
    void    setOccupiedSlots(const QStringList &codes);
private:
    Warehouse3DView *m_view;
    QLabel          *m_lblInfo;
    QLabel          *m_lblSelected;
    QLabel          *m_lblAisle;
    QPushButton     *m_btnConfirm;
    QPushButton     *m_btnCancel;
    QPushButton     *m_btnToggleView;
};

#endif // STOCKMAPWIDGET_H
