#include "viewer3ddialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QScrollArea>
#include <QJsonDocument>
#include <QJsonArray>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDebug>
#include <QStyle>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QResizeEvent>
#include <algorithm>

// ═══════════════════════════════════════════════════════════════════════════
// Color palette
// ═══════════════════════════════════════════════════════════════════════════
static const QColor PALETTE_COLORS[] = {
    QColor(239, 68, 68),    // Red
    QColor(249, 115, 22),   // Orange
    QColor(234, 179, 8),    // Yellow
    QColor(34, 197, 94),    // Green
    QColor(6, 182, 212),    // Cyan
    QColor(59, 130, 246),   // Blue
    QColor(20, 184, 166),   // Teal
    QColor(236, 72, 153),   // Pink
    QColor(255, 255, 255),  // White
    QColor(148, 163, 184),  // Gray
    QColor(30, 41, 59),     // Dark
    QColor(250, 204, 21),   // Gold
};
static const int PALETTE_COUNT = sizeof(PALETTE_COLORS) / sizeof(PALETTE_COLORS[0]);

// ═══════════════════════════════════════════════════════════════════════════
// FeatureCalloutOverlay — transparent widget that paints the infographic callouts
// on top of the 3D viewer. Lives as a sibling stacked above ModelViewerWidget;
// pulls projected anchors from the viewer whenever the view changes.
// ═══════════════════════════════════════════════════════════════════════════
class FeatureCalloutOverlay : public QWidget
{
public:
    explicit FeatureCalloutOverlay(ModelViewerWidget *viewer, QWidget *parent)
        : QWidget(parent), m_viewer(viewer)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
        setAttribute(Qt::WA_TranslucentBackground, true);
        setFocusPolicy(Qt::NoFocus);
        if (m_viewer) {
            connect(m_viewer, &ModelViewerWidget::featureAnchorsChanged,
                    this, QOverload<>::of(&QWidget::update));
        }
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        if (!m_viewer) return;
        const QStringList features = m_viewer->featureCallouts();
        const QVector<QPointF> anchors = m_viewer->featureAnchors();
        const QVector<bool> visible   = m_viewer->featureAnchorsVisible();
        if (features.isEmpty() || anchors.size() != features.size()) return;

        static const QColor kPalette[] = {
            QColor( 52, 211, 153),
            QColor( 96, 165, 250),
            QColor(251, 191,  36),
            QColor(248, 113, 113),
            QColor(167, 139, 250),
            QColor( 45, 212, 191),
            QColor(244, 114, 182),
            QColor(125, 211, 252),
        };
        const int kPaletteN = int(sizeof(kPalette) / sizeof(kPalette[0]));

        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setRenderHint(QPainter::TextAntialiasing, true);

        // Translate anchor coordinates: viewer's client coords → our client coords.
        // Both widgets have the same parent and (via geometry) the same top-left,
        // so the mapping is an identity — but use mapTo/mapFrom to be safe.
        auto toLocal = [&](const QPointF &viewerPt) {
            const QPoint global = m_viewer->mapToGlobal(QPoint(int(viewerPt.x()),
                                                               int(viewerPt.y())));
            return QPointF(mapFromGlobal(global));
        };

        const int n = features.size();
        const qreal circleR       = 17.0;
        const int   edgePad       = 18;
        const int   labelMaxW     = qBound(130, width() / 5, 220);
        const int   pillH1Line    = 34;
        const int   pillH2Line    = 50;
        const int   gapCirclePill = 8;
        const int   gapLeaderStart = 4;

        QVector<int> leftCol, rightCol;
        for (int i = 0; i < n; ++i) {
            const qreal ax = visible.value(i, false)
                ? toLocal(anchors[i]).x() : width() / 2.0;
            if (ax < width() / 2.0) leftCol.append(i);
            else rightCol.append(i);
        }

        auto rebalance = [&](QVector<int> &from, QVector<int> &to, bool moveLargestX) {
            while (from.size() > to.size() + 1) {
                int bestK = 0;
                for (int k = 1; k < from.size(); ++k) {
                    const qreal xk = toLocal(anchors[from[k]]).x();
                    const qreal xb = toLocal(anchors[from[bestK]]).x();
                    if (moveLargestX ? (xk > xb) : (xk < xb)) bestK = k;
                }
                to.append(from.takeAt(bestK));
            }
        };
        rebalance(leftCol, rightCol, true);
        rebalance(rightCol, leftCol, false);

        auto sortCol = [&](QVector<int> &col) {
            std::sort(col.begin(), col.end(),
                      [&](int a, int b) {
                          return toLocal(anchors[a]).y() < toLocal(anchors[b]).y();
                      });
        };
        sortCol(leftCol);
        sortCol(rightCol);

        const int maxRows = qMax(leftCol.size(), rightCol.size());
        int rowHeight = 78;
        if (maxRows > 0) {
            const int availH = qMax(160, height() - 140);
            if (maxRows * rowHeight > availH) {
                rowHeight = qMax(54, availH / maxRows);
            }
        }

        QFont titleFont("Segoe UI Semibold", 11);
        titleFont.setBold(true);
        QFontMetrics fm(titleFont);

        QFont numFont("Segoe UI", 12);
        numFont.setBold(true);

        auto drawColumn = [&](const QVector<int> &col, bool leftSide) {
            const int total = col.size();
            if (total == 0) return;

            const qreal totalH = total * rowHeight;
            qreal startY = (height() - totalH) / 2.0 + rowHeight / 2.0;
            startY = qMax<qreal>(60.0, startY);

            const qreal circleX = leftSide
                ? edgePad + labelMaxW + gapCirclePill + circleR
                : width() - edgePad - labelMaxW - gapCirclePill - circleR;

            for (int k = 0; k < total; ++k) {
                const int i = col[k];
                const QColor accent = kPalette[i % kPaletteN];
                const QPointF circleC(circleX, startY + k * rowHeight);

                QPointF anchor = visible.value(i, false)
                    ? toLocal(anchors[i])
                    : QPointF(width() / 2.0, circleC.y());
                anchor.setX(qBound<qreal>(20.0, anchor.x(), width() - 20.0));
                anchor.setY(qBound<qreal>(20.0, anchor.y(), height() - 20.0));

                const QPointF leaderStart(
                    leftSide ? circleC.x() + circleR + gapLeaderStart
                             : circleC.x() - circleR - gapLeaderStart,
                    circleC.y());

                QPainterPath leader;
                leader.moveTo(leaderStart);
                const qreal midX = (leaderStart.x() + anchor.x()) * 0.5;
                leader.cubicTo(QPointF(midX, leaderStart.y()),
                               QPointF(midX, anchor.y()),
                               anchor);

                p.setBrush(Qt::NoBrush);
                p.setPen(QPen(QColor(0, 0, 0, 200), 5.0, Qt::SolidLine,
                              Qt::RoundCap, Qt::RoundJoin));
                p.drawPath(leader);
                p.setPen(QPen(QColor(255, 255, 255, 235), 3.0, Qt::SolidLine,
                              Qt::RoundCap, Qt::RoundJoin));
                p.drawPath(leader);
                p.setPen(QPen(accent, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                p.drawPath(leader);

                p.setPen(Qt::NoPen);
                p.setBrush(QColor(0, 0, 0, 170));
                p.drawEllipse(anchor, 10.5, 10.5);
                p.setBrush(QColor(255, 255, 255, 245));
                p.drawEllipse(anchor, 7.0, 7.0);
                p.setBrush(accent);
                p.drawEllipse(anchor, 4.0, 4.0);

                p.setPen(Qt::NoPen);
                p.setBrush(QColor(0, 0, 0, 160));
                p.drawEllipse(circleC, circleR + 3, circleR + 3);

                p.setBrush(accent);
                p.setPen(QPen(QColor(255, 255, 255), 2.2));
                p.drawEllipse(circleC, circleR, circleR);

                p.setPen(QColor(255, 255, 255));
                p.setFont(numFont);
                p.drawText(QRectF(circleC.x() - circleR, circleC.y() - circleR,
                                  circleR * 2, circleR * 2),
                           Qt::AlignCenter,
                           QString::number(k + 1 + (leftSide ? 0 : leftCol.size())));

                const QString rawText = features.at(i).trimmed();
                const int textBudget = labelMaxW - 22;
                QString line1, line2;
                bool useTwoLines = false;
                // Always break a multi-word feature name onto two lines so the
                // first word ("Capteur", "Module", etc.) is fully visible and
                // the rest fits on the second line — even when the available
                // width would otherwise truncate the suffix to "Capteur na".
                const int firstSpace = rawText.indexOf(' ');
                if (firstSpace > 0 && firstSpace < rawText.size() - 1) {
                    line1 = rawText.left(firstSpace).trimmed();
                    line2 = rawText.mid(firstSpace + 1).trimmed();
                    line2 = fm.elidedText(line2, Qt::ElideRight, textBudget);
                    useTwoLines = !line2.isEmpty();
                } else if (fm.horizontalAdvance(rawText) > textBudget) {
                    // Single very long word with no space: split in half so the
                    // full label still appears across two lines.
                    const int splitPos = rawText.size() / 2;
                    line1 = rawText.left(splitPos).trimmed();
                    line2 = rawText.mid(splitPos).trimmed();
                    line2 = fm.elidedText(line2, Qt::ElideRight, textBudget);
                    useTwoLines = !line2.isEmpty();
                }
                if (!useTwoLines) {
                    line1 = fm.elidedText(rawText, Qt::ElideRight, textBudget);
                    line2.clear();
                }

                const int pillH = useTwoLines ? pillH2Line : pillH1Line;
                const int line1W = fm.horizontalAdvance(line1);
                const int line2W = useTwoLines ? fm.horizontalAdvance(line2) : 0;
                // Pill must fit text PLUS the inset textRect padding (12 + 20 = 32)
                // PLUS the colored accent dot on one side (~14). Use 48px so both
                // lines stay fully visible regardless of which side the pill is on.
                const qreal pillW = qMax(line1W, line2W) + 48;
                const qreal pillX = leftSide
                    ? circleC.x() - circleR - gapCirclePill - pillW
                    : circleC.x() + circleR + gapCirclePill;
                const QRectF pillRect(pillX, circleC.y() - pillH / 2.0, pillW, pillH);
                const qreal pillRadius = pillH1Line / 2.0;

                p.setBrush(QColor(0, 0, 0, 140));
                p.setPen(Qt::NoPen);
                p.drawRoundedRect(pillRect.translated(0, 2), pillRadius, pillRadius);

                QLinearGradient pg(pillRect.topLeft(), pillRect.bottomLeft());
                pg.setColorAt(0.0, QColor(253, 252, 238));
                pg.setColorAt(1.0, QColor(238, 233, 208));
                p.setBrush(pg);
                p.setPen(QPen(accent, 1.8));
                p.drawRoundedRect(pillRect, pillRadius, pillRadius);

                p.setPen(Qt::NoPen);
                p.setBrush(accent);
                const qreal dotR = 3.2;
                const qreal dotX = leftSide
                    ? pillRect.right() - 10.0
                    : pillRect.left() + 10.0;
                p.drawEllipse(QPointF(dotX, pillRect.center().y()), dotR, dotR);

                p.setPen(QColor(24, 28, 46));
                p.setFont(titleFont);
                const QRectF textRect = leftSide
                    ? pillRect.adjusted(12, 0, -20, 0)
                    : pillRect.adjusted(20, 0, -12, 0);
                if (useTwoLines) {
                    p.drawText(textRect,
                               Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextDontClip,
                               line1 + "\n" + line2);
                } else {
                    p.drawText(textRect,
                               Qt::AlignVCenter | Qt::AlignLeft | Qt::TextDontClip,
                               line1);
                }
            }
        };

        drawColumn(leftCol, true);
        drawColumn(rightCol, false);
    }

private:
    ModelViewerWidget *m_viewer;
};

namespace {
QStringList buildFeatureCallouts(const QStringList &featureNames, const QString &productName)
{
    QStringList cleaned;
    for (const QString &feature : featureNames) {
        const QString trimmed = feature.trimmed();
        if (!trimmed.isEmpty() && !cleaned.contains(trimmed, Qt::CaseInsensitive)) {
            cleaned << trimmed;
        }
    }
    if (cleaned.isEmpty()) {
        const QString trimmedProduct = productName.trimmed();
        if (!trimmedProduct.isEmpty()) cleaned << trimmedProduct;
    }
    return cleaned;
}
}

// ═══════════════════════════════════════════════════════════════════════════
// Constructor / Destructor
// ═══════════════════════════════════════════════════════════════════════════

Viewer3DDialog::Viewer3DDialog(const QString &meshPath,
                               const QString &productName,
                               const QStringList &featureNames,
                               QWidget *parent)
    : QDialog(parent),
      m_meshPath(meshPath),
      m_productName(productName),
      m_featureNames(featureNames)
{
    setupUi();
    if (m_viewer) {
        m_viewer->setFeatureCallouts(buildFeatureCallouts(m_featureNames, m_productName));
        m_viewer->setAutoSpinEnabled(true);
    }

    // Defer heavy mesh loading until the dialog is fully shown and GL initialized.
    // 500ms ensures initializeGL() has completed.
    QTimer::singleShot(500, this, [this]() {
        if (!m_meshPath.isEmpty() && m_viewer) {
            if (m_lblStatus) {
                m_lblStatus->setText("Chargement du modele 3D en cours...");
                m_lblStatus->setStyleSheet("color: #6ee7b7; font-size: 11px; border: none; padding: 2px;");
            }
            // Process events to show the status update before loading
            QCoreApplication::processEvents();

            qDebug() << "[Viewer3DDialog] Loading OBJ:" << m_meshPath;
            bool ok = m_viewer->loadOBJ(m_meshPath);
            if (!ok) {
                qWarning() << "[Viewer3DDialog] Failed to load OBJ model";
                if (m_lblStatus) {
                    m_lblStatus->setText("Erreur: Impossible de charger le modele 3D");
                    m_lblStatus->setStyleSheet("color: #ef4444; font-size: 11px; border: none; padding: 2px;");
                }
            }
        }
    });
}

Viewer3DDialog::~Viewer3DDialog()
{
    stopHandTracker();
}

bool Viewer3DDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_viewer && m_calloutOverlay) {
        if (event->type() == QEvent::Resize || event->type() == QEvent::Show) {
            m_calloutOverlay->setGeometry(m_viewer->rect());
            m_calloutOverlay->raise();
        }
    }
    return QDialog::eventFilter(obj, event);
}

// ═══════════════════════════════════════════════════════════════════════════
// UI Setup
// ═══════════════════════════════════════════════════════════════════════════

void Viewer3DDialog::setupUi()
{
    setWindowTitle(QString("Visualiseur 3D — %1").arg(
        m_productName.isEmpty() ? "Modele" : m_productName));
    resize(1200, 750);
    setMinimumSize(900, 550);
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setAutoFillBackground(true);
    setStyleSheet("QDialog { background-color: #0a1628; }");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(12);

    // ── Left: 3D Viewer ──
    QWidget *viewerContainer = new QWidget(this);
    QGridLayout *viewerLayout = new QGridLayout(viewerContainer);
    viewerLayout->setContentsMargins(0, 0, 0, 0);

    m_viewer = new ModelViewerWidget(viewerContainer);
    m_viewer->setStyleSheet(
        "ModelViewerWidget {"
        "background-color: #111827;"
        "border: 1px solid #1f2937;"
        "border-radius: 14px;"
        "}"
    );
    viewerLayout->addWidget(m_viewer, 0, 0);

    // Transparent infographic overlay stacked on top of the viewer.
    FeatureCalloutOverlay *overlay = new FeatureCalloutOverlay(m_viewer, m_viewer);
    overlay->setGeometry(m_viewer->rect());
    overlay->raise();
    overlay->show();
    m_calloutOverlay = overlay;
    m_viewer->installEventFilter(this);

    m_webcamPreview = new QLabel(viewerContainer);
    m_webcamPreview->setFixedSize(200, 150);
    m_webcamPreview->setAlignment(Qt::AlignCenter);
    m_webcamPreview->setText("Camera...");
    m_webcamPreview->setStyleSheet(
        "QLabel { background-color: rgba(0, 0, 0, 210); color: #94a3b8; "
        "border: 2px solid #34d399; border-radius: 10px; font-size: 12px; }"
    );
    m_webcamPreview->hide();
    viewerLayout->addWidget(m_webcamPreview, 0, 0, Qt::AlignTop | Qt::AlignRight);

    connect(m_viewer, &ModelViewerWidget::modelLoaded,
            this, &Viewer3DDialog::onModelLoaded);
    mainLayout->addWidget(viewerContainer, 3);

    // ── Right: Tool Panel ──
    QFrame *panel = new QFrame(this);
    panel->setFixedWidth(260);
    panel->setStyleSheet(
        "QFrame { background-color: #111827; border-radius: 14px; "
        "border: 1px solid #1a3a2e; }"
    );

    QVBoxLayout *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(14, 16, 14, 16);
    panelLayout->setSpacing(10);

    // ── Title ──
    QLabel *lblTitle = new QLabel(QString::fromUtf8(
        "\xF0\x9F\x8E\xAE Outils 3D"), panel);
    lblTitle->setStyleSheet(
        "color: #6ee7b7; font-size: 17px; font-weight: bold; "
        "font-family: 'Segoe UI'; border: none;"
    );
    panelLayout->addWidget(lblTitle);

    // ── Status ──
    m_lblStatus = new QLabel("Chargement...", panel);
    m_lblStatus->setStyleSheet(
        "color: #94a3b8; font-size: 11px; border: none; padding: 2px;"
    );
    m_lblStatus->setWordWrap(true);
    panelLayout->addWidget(m_lblStatus);

    // ── Separator ──
    auto addSeparator = [&]() {
        QFrame *sep = new QFrame(panel);
        sep->setFrameShape(QFrame::HLine);
        sep->setStyleSheet("background-color: #0f2b4c; border: none; max-height: 1px;");
        panelLayout->addWidget(sep);
    };

    addSeparator();

    // ── Mode Buttons ──
    QLabel *lblMode = new QLabel("MODE", panel);
    lblMode->setStyleSheet(
        "color: #34d399; font-size: 10px; font-weight: bold; "
        "letter-spacing: 2px; border: none;"
    );
    panelLayout->addWidget(lblMode);

    m_btnRotate = makeToolButton(QString::fromUtf8("\xF0\x9F\x94\x84  Rotation"), "Rotation / Zoom / Pan");
    m_btnDraw = makeToolButton(QString::fromUtf8("\xe2\x9c\x8f\xef\xb8\x8f  Dessiner"), "Dessiner sur le modele");
    m_btnErase = makeToolButton(QString::fromUtf8("\xF0\x9F\xA7\xB9  Gomme"), "Effacer des traits");

    connect(m_btnRotate, &QPushButton::clicked, this, &Viewer3DDialog::onRotateToggled);
    connect(m_btnDraw, &QPushButton::clicked, this, &Viewer3DDialog::onDrawToggled);
    connect(m_btnErase, &QPushButton::clicked, this, &Viewer3DDialog::onEraseToggled);

    QHBoxLayout *modeRow = new QHBoxLayout();
    modeRow->setSpacing(6);
    modeRow->addWidget(m_btnRotate);
    modeRow->addWidget(m_btnDraw);
    modeRow->addWidget(m_btnErase);
    panelLayout->addLayout(modeRow);

    updateToolButtonStates();

    addSeparator();

    // ── Color Palette ──
    QLabel *lblColor = new QLabel("COULEURS", panel);
    lblColor->setStyleSheet(
        "color: #34d399; font-size: 10px; font-weight: bold; "
        "letter-spacing: 2px; border: none;"
    );
    panelLayout->addWidget(lblColor);

    QGridLayout *colorGrid = new QGridLayout();
    colorGrid->setSpacing(6);
    for (int i = 0; i < PALETTE_COUNT; ++i) {
        QPushButton *btn = makeColorButton(PALETTE_COLORS[i]);
        connect(btn, &QPushButton::clicked, this, &Viewer3DDialog::onColorClicked);
        colorGrid->addWidget(btn, i / 4, i % 4);
        // Select first color by default
        if (i == 0) {
            m_selectedColorBtn = btn;
            btn->setStyleSheet(btn->styleSheet() +
                "border: 3px solid white;");
        }
    }
    panelLayout->addLayout(colorGrid);

    // ── Brush Size ──
    QLabel *lblBrush = new QLabel("TAILLE DU PINCEAU", panel);
    lblBrush->setStyleSheet(
        "color: #34d399; font-size: 10px; font-weight: bold; "
        "letter-spacing: 2px; border: none;"
    );
    panelLayout->addWidget(lblBrush);

    QHBoxLayout *brushRow = new QHBoxLayout();
    m_sliderBrush = new QSlider(Qt::Horizontal, panel);
    m_sliderBrush->setRange(1, 15);
    m_sliderBrush->setValue(3);
    m_sliderBrush->setStyleSheet(
        "QSlider::groove:horizontal { background: #0f2b4c; height: 6px; border-radius: 3px; }"
        "QSlider::handle:horizontal { background: #10b981; width: 16px; height: 16px; "
        "  margin: -5px 0; border-radius: 8px; }"
        "QSlider::sub-page:horizontal { background: #34d399; border-radius: 3px; }"
    );
    connect(m_sliderBrush, &QSlider::valueChanged, this, &Viewer3DDialog::onBrushSizeChanged);

    m_lblBrushSize = new QLabel("3px", panel);
    m_lblBrushSize->setStyleSheet("color: #6ee7b7; font-size: 12px; border: none; min-width: 30px;");
    m_lblBrushSize->setAlignment(Qt::AlignCenter);

    brushRow->addWidget(m_sliderBrush, 1);
    brushRow->addWidget(m_lblBrushSize);
    panelLayout->addLayout(brushRow);

    addSeparator();

    // ── Actions ──
    QLabel *lblActions = new QLabel("ACTIONS", panel);
    lblActions->setStyleSheet(
        "color: #34d399; font-size: 10px; font-weight: bold; "
        "letter-spacing: 2px; border: none;"
    );
    panelLayout->addWidget(lblActions);

    m_btnClear = makeToolButton(QString::fromUtf8("\xF0\x9F\x97\x91  Tout effacer"), "Supprimer tous les dessins");
    connect(m_btnClear, &QPushButton::clicked, this, &Viewer3DDialog::onClearDrawings);
    panelLayout->addWidget(m_btnClear);

    m_btnReset = makeToolButton(QString::fromUtf8("\xF0\x9F\x94\x84  Reset Vue"), "Reinitialiser la camera");
    connect(m_btnReset, &QPushButton::clicked, this, &Viewer3DDialog::onResetView);
    panelLayout->addWidget(m_btnReset);

    addSeparator();

    // ── Webcam / Hand Tracking ──
    QLabel *lblTracking = new QLabel("HAND TRACKING", panel);
    lblTracking->setStyleSheet(
        "color: #34d399; font-size: 10px; font-weight: bold; "
        "letter-spacing: 2px; border: none;"
    );
    panelLayout->addWidget(lblTracking);

    m_btnWebcam = makeToolButton(QString::fromUtf8(
        "\xF0\x9F\x93\xB7  Activer Webcam"), "Activer le suivi gestuel");
    connect(m_btnWebcam, &QPushButton::clicked, this, &Viewer3DDialog::onToggleWebcam);
    panelLayout->addWidget(m_btnWebcam);

    m_lblGesture = new QLabel("Inactif", panel);
    m_lblGesture->setStyleSheet(
        "color: #475569; font-size: 11px; border: none; padding: 4px;"
    );
    m_lblGesture->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(m_lblGesture);

    addSeparator();

    // ── Gesture Guide ──
    QLabel *lblGuideTitle = new QLabel("GUIDE GESTUEL", panel);
    lblGuideTitle->setStyleSheet(
        "color: #34d399; font-size: 10px; font-weight: bold; "
        "letter-spacing: 2px; border: none;"
    );
    panelLayout->addWidget(lblGuideTitle);

    QLabel *lblGuide = new QLabel(
        "<span style='color:#6ee7b7;'>&#x261D; Index Leve :</span> Dessiner<br>"
        "<span style='color:#6ee7b7;'>&#x270B; Main ouverte :</span> Effacer<br>"
        "<span style='color:#6ee7b7;'>&#x1F44C; Pincer :</span> Rotation", panel);
    lblGuide->setStyleSheet("color: #94a3b8; font-size: 11px; border: none; padding-top: 4px;");
    panelLayout->addWidget(lblGuide);

    // ── Spacer ──
    panelLayout->addStretch();

    // ── Close button ──
    QPushButton *btnClose = new QPushButton("Fermer", panel);
    btnClose->setStyleSheet(
        "QPushButton { background-color: #0f2b4c; color: #6ee7b7; "
        "border: 1px solid #1a3a2e; border-radius: 8px; "
        "padding: 10px; font-weight: bold; font-size: 13px; }"
        "QPushButton:hover { background-color: #1a3a2e; }"
    );
    btnClose->setCursor(Qt::PointingHandCursor);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
    panelLayout->addWidget(btnClose);

    mainLayout->addWidget(panel);
}

QPushButton *Viewer3DDialog::makeColorButton(const QColor &color, int size)
{
    QPushButton *btn = new QPushButton(this);
    btn->setFixedSize(size, size);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setProperty("paletteColor", color);
    btn->setStyleSheet(QString(
        "QPushButton { background-color: %1; border: 2px solid #374151; "
        "border-radius: %2px; }"
        "QPushButton:hover { border: 2px solid #6ee7b7; }"
    ).arg(color.name()).arg(size / 2));
    return btn;
}

QPushButton *Viewer3DDialog::makeToolButton(const QString &text, const QString &tooltip)
{
    QPushButton *btn = new QPushButton(text, this);
    btn->setToolTip(tooltip);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(
        "QPushButton { background-color: #0f2b4c; color: #6ee7b7; "
        "border: 1px solid #1a3a2e; border-radius: 8px; "
        "padding: 8px 6px; font-size: 12px; font-weight: bold; }"
        "QPushButton:hover { background-color: #1a3a2e; border-color: #059669; }"
        "QPushButton[active=\"true\"] { background-color: #059669; "
        "border-color: #10b981; color: white; }"
    );
    return btn;
}

void Viewer3DDialog::updateToolButtonStates()
{
    auto mode = m_viewer->interactionMode();
    m_btnRotate->setProperty("active", mode == ModelViewerWidget::ModeRotate);
    m_btnDraw->setProperty("active", mode == ModelViewerWidget::ModeDraw);
    m_btnErase->setProperty("active", mode == ModelViewerWidget::ModeErase);

    // Force style refresh
    m_btnRotate->style()->unpolish(m_btnRotate);
    m_btnRotate->style()->polish(m_btnRotate);
    m_btnDraw->style()->unpolish(m_btnDraw);
    m_btnDraw->style()->polish(m_btnDraw);
    m_btnErase->style()->unpolish(m_btnErase);
    m_btnErase->style()->polish(m_btnErase);
}

// ═══════════════════════════════════════════════════════════════════════════
// Slots
// ═══════════════════════════════════════════════════════════════════════════

void Viewer3DDialog::onColorClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    QColor color = btn->property("paletteColor").value<QColor>();
    m_viewer->setDrawColor(color);

    // Update visual selection
    if (m_selectedColorBtn) {
        QColor oldColor = m_selectedColorBtn->property("paletteColor").value<QColor>();
        m_selectedColorBtn->setStyleSheet(QString(
            "QPushButton { background-color: %1; border: 2px solid #374151; border-radius: 16px; }"
            "QPushButton:hover { border: 2px solid #6ee7b7; }"
        ).arg(oldColor.name()));
    }
    m_selectedColorBtn = btn;
    btn->setStyleSheet(QString(
        "QPushButton { background-color: %1; border: 3px solid white; border-radius: 16px; }"
    ).arg(color.name()));
}

void Viewer3DDialog::onDrawToggled()
{
    m_viewer->setInteractionMode(ModelViewerWidget::ModeDraw);
    updateToolButtonStates();
}

void Viewer3DDialog::onEraseToggled()
{
    m_viewer->setInteractionMode(ModelViewerWidget::ModeErase);
    updateToolButtonStates();
}

void Viewer3DDialog::onRotateToggled()
{
    m_viewer->setInteractionMode(ModelViewerWidget::ModeRotate);
    updateToolButtonStates();
}

void Viewer3DDialog::onClearDrawings()
{
    m_viewer->clearDrawings();
}

void Viewer3DDialog::onResetView()
{
    m_viewer->resetView();
}

void Viewer3DDialog::onBrushSizeChanged(int val)
{
    m_viewer->setDrawWidth(val);
    m_lblBrushSize->setText(QString("%1px").arg(val));
}

void Viewer3DDialog::onModelLoaded(bool success, int vertexCount, int faceCount)
{
    if (success) {
        m_lblStatus->setText(QString(
            "%1\n%2 sommets | %3 faces")
            .arg(QFileInfo(m_meshPath).fileName())
            .arg(vertexCount)
            .arg(faceCount));
        m_lblStatus->setStyleSheet(
            "color: #a3e635; font-size: 11px; border: none; padding: 2px;");
    } else {
        m_lblStatus->setText("Erreur de chargement du modele");
        m_lblStatus->setStyleSheet(
            "color: #ef4444; font-size: 11px; border: none; padding: 2px;");
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Hand Tracking
// ═══════════════════════════════════════════════════════════════════════════

void Viewer3DDialog::onToggleWebcam()
{
    if (m_webcamActive) {
        stopHandTracker();
        m_btnWebcam->setText(QString::fromUtf8("\xF0\x9F\x93\xB7  Activer Webcam"));
        m_lblGesture->setText("Inactif");
        m_lblGesture->setStyleSheet("color: #475569; font-size: 11px; border: none; padding: 4px;");
        m_viewer->setShowWebcam(false);
        if (m_webcamPreview) {
            m_webcamPreview->hide();
            m_webcamPreview->setText("Camera...");
            m_webcamPreview->setPixmap(QPixmap());
        }
        if (m_viewer) {
            m_viewer->setAutoSpinEnabled(true);
        }
    } else {
        startHandTracker();
    }
}

void Viewer3DDialog::startHandTracker()
{
    if (m_trackerProcess) {
        stopHandTracker();
    }

    // Find hand_tracker.py - check multiple locations
    QString scriptPath;
    QStringList searchPaths = {
        QCoreApplication::applicationDirPath() + "/hand_tracker.py",
        QCoreApplication::applicationDirPath() + "/../hand_tracker.py",
        QDir::currentPath() + "/hand_tracker.py",
        // Source directory (for development)
        QFileInfo(m_meshPath).absolutePath() + "/../../hand_tracker.py",
    };

    // Also check project source dir
    QString srcDir = QString::fromUtf8(
        "C:/Users/gigabyte/Downloads/INTEGRATION_FINALE new - "
        "Cochkemlay/INTEGRATION_FINALE new - Copy/INTEGRATION_FINALE1");
    searchPaths.append(srcDir + "/hand_tracker.py");

    for (const auto &p : searchPaths) {
        if (QFile::exists(p)) {
            scriptPath = p;
            break;
        }
    }

    if (scriptPath.isEmpty()) {
        QMessageBox::warning(this, "Hand Tracker",
            "hand_tracker.py introuvable.\n\n"
            "Placez le script hand_tracker.py dans le dossier du projet\n"
            "et assurez-vous que mediapipe et opencv-python sont installes:\n\n"
            "pip install mediapipe opencv-python");
        return;
    }

    m_trackerProcess = new QProcess(this);
    connect(m_trackerProcess, &QProcess::readyReadStandardOutput,
            this, &Viewer3DDialog::onTrackerOutput);
    connect(m_trackerProcess, &QProcess::readyReadStandardError,
            this, &Viewer3DDialog::onTrackerError);
    connect(m_trackerProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &Viewer3DDialog::onTrackerFinished);

    // Try to find python
    QString python = "python";
#ifdef Q_OS_WIN
    // Check common python locations
    QStringList pythonPaths = {"python", "python3", "py"};
    for (const auto &p : pythonPaths) {
        QProcess test;
        test.start(p, {"--version"});
        if (test.waitForFinished(2000) && test.exitCode() == 0) {
            python = p;
            break;
        }
    }
#endif

    m_trackerProcess->start(python, {"-u", scriptPath});

    if (!m_trackerProcess->waitForStarted(5000)) {
        QMessageBox::warning(this, "Hand Tracker",
            "Impossible de lancer le script Python.\n\n"
            "Verifiez que Python est installe et dans le PATH,\n"
            "et que mediapipe et opencv-python sont installes.");
        delete m_trackerProcess;
        m_trackerProcess = nullptr;
        return;
    }

    m_webcamActive = true;
    m_viewer->setShowWebcam(true);
    m_viewer->setAutoSpinEnabled(false);
    if (m_webcamPreview) {
        m_webcamPreview->setText("Camera...");
        m_webcamPreview->setPixmap(QPixmap());
        m_webcamPreview->show();
        m_webcamPreview->raise();
    }
    m_btnWebcam->setText(QString::fromUtf8("\xF0\x9F\x94\xB4  Desactiver Webcam"));
    m_lblGesture->setText("Initialisation...");
    m_lblGesture->setStyleSheet("color: #6ee7b7; font-size: 11px; border: none; padding: 4px;");
}

void Viewer3DDialog::stopHandTracker()
{
    if (m_trackerProcess) {
        // Send quit command
        m_trackerProcess->write("QUIT\n");
        if (!m_trackerProcess->waitForFinished(2000)) {
            m_trackerProcess->kill();
            m_trackerProcess->waitForFinished(1000);
        }
        delete m_trackerProcess;
        m_trackerProcess = nullptr;
    }
    m_webcamActive = false;
    if (m_viewer) {
        m_viewer->setAutoSpinEnabled(true);
    }
    if (m_webcamPreview) {
        m_webcamPreview->hide();
        m_webcamPreview->setText("Camera...");
        m_webcamPreview->setPixmap(QPixmap());
    }
}

void Viewer3DDialog::onTrackerOutput()
{
    if (!m_trackerProcess) return;

    while (m_trackerProcess->canReadLine()) {
        QByteArray line = m_trackerProcess->readLine().trimmed();
        if (line.isEmpty()) continue;

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(line, &err);
        if (err.error != QJsonParseError::NoError) continue;

        if (!doc.isObject()) continue;
        processGestureJson(doc.object());
    }
}

void Viewer3DDialog::processGestureJson(const QJsonObject &json)
{
    QString gesture = json.value("gesture").toString();
    double dx = json.value("dx").toDouble(0);
    double dy = json.value("dy").toDouble(0);
    double zoom = json.value("zoom").toDouble(0);
    (void)zoom;
    double nx = json.value("nx").toDouble(0);
    double ny = json.value("ny").toDouble(0);

    // End drawing if gesture changed
    if (gesture != "draw") {
        m_viewer->applyGestureDrawEnd();
    }

    QString gestureText;
    QColor gestureColor = Qt::white;
    const auto mode = m_viewer->interactionMode();

    if (gesture == "rotate") {
        if (mode == ModelViewerWidget::ModeRotate) {
            gestureText = QString::fromUtf8("\xf0\x9f\x91\x8c Rotation");
            gestureColor = QColor(167, 139, 250);
            m_viewer->applyGestureRotation(dx, dy);
        } else {
            gestureText = QString::fromUtf8("\xf0\x9f\x91\x8c Rotation disponible en mode Rotation");
            gestureColor = QColor(148, 163, 184);
        }
    } else if (gesture == "draw") {
        if (mode == ModelViewerWidget::ModeDraw) {
            gestureText = QString::fromUtf8("\xe2\x9c\x8f\xef\xb8\x8f Dessin");
            gestureColor = m_viewer->drawColor();
            m_viewer->applyGestureDraw(nx, ny);
        } else {
            gestureText = QString::fromUtf8("\xe2\x9c\x8f\xef\xb8\x8f Activez Dessiner");
            gestureColor = QColor(148, 163, 184);
        }
    } else if (gesture == "erase") {
        if (mode == ModelViewerWidget::ModeErase) {
            gestureText = QString::fromUtf8("\xf0\x9f\x92\xa8 Effacer");
            gestureColor = QColor(248, 113, 113);
            m_viewer->applyGestureErase(nx, ny);
        } else {
            gestureText = QString::fromUtf8("\xf0\x9f\x92\xa8 Activez Gomme");
            gestureColor = QColor(148, 163, 184);
        }
    } else if (gesture == "idle") {
        gestureText = QString::fromUtf8("\xe2\x9c\x8b Pret");
        gestureColor = QColor(148, 163, 184);
    }
    else {
        gestureText = gesture;
        gestureColor = QColor(148, 163, 184);
    }

    m_lblGesture->setText(gestureText);
    m_lblGesture->setStyleSheet(QString(
        "color: %1; font-size: 12px; font-weight: bold; border: none; padding: 4px;"
    ).arg(gestureColor.name()));

    // Handle webcam frame if included
    if (json.contains("frame")) {
        QByteArray frameData = QByteArray::fromBase64(
            json.value("frame").toString().toUtf8());
        QImage img;
        if (img.loadFromData(frameData, "JPEG")) {
            m_viewer->setWebcamFrame(img);
            if (m_webcamPreview) {
                const QPixmap pix = QPixmap::fromImage(img).scaled(
                    m_webcamPreview->size(),
                    Qt::KeepAspectRatioByExpanding,
                    Qt::SmoothTransformation
                );
                m_webcamPreview->setPixmap(pix);
                m_webcamPreview->setText(QString());
            }
        }
    }
}

void Viewer3DDialog::onTrackerError()
{
    if (!m_trackerProcess) return;
    QByteArray err = m_trackerProcess->readAllStandardError();
    if (!err.isEmpty()) {
        QString errText = QString::fromUtf8(err).trimmed();
        qDebug() << "Hand tracker stderr:" << errText;

        // Show meaningful errors in the UI
        if (errText.contains("ModuleNotFoundError") || errText.contains("No module named")) {
            m_lblGesture->setText("Erreur: Module Python manquant");
            m_lblGesture->setStyleSheet("color: #ef4444; font-size: 10px; border: none; padding: 4px;");
        } else if (errText.contains("Traceback") || errText.contains("Error")) {
            m_lblGesture->setText("Erreur Python");
            m_lblGesture->setStyleSheet("color: #ef4444; font-size: 10px; border: none; padding: 4px;");
        }
    }
}

void Viewer3DDialog::onTrackerFinished(int exitCode, QProcess::ExitStatus status)
{
    Q_UNUSED(status);
    m_webcamActive = false;
    m_viewer->setShowWebcam(false);
    m_viewer->setAutoSpinEnabled(true);
    if (m_webcamPreview) {
        m_webcamPreview->hide();
        m_webcamPreview->setText("Camera...");
        m_webcamPreview->setPixmap(QPixmap());
    }
    m_btnWebcam->setText(QString::fromUtf8("\xF0\x9F\x93\xB7  Activer Webcam"));

    if (exitCode != 0) {
        m_lblGesture->setText(QString("Erreur (code %1)\npip install mediapipe opencv-python").arg(exitCode));
        m_lblGesture->setStyleSheet("color: #ef4444; font-size: 10px; border: none; padding: 4px;");
    } else {
        m_lblGesture->setText("Deconnecte");
        m_lblGesture->setStyleSheet("color: #475569; font-size: 11px; border: none; padding: 4px;");
    }
}
