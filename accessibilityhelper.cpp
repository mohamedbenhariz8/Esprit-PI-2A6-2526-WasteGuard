#include "accessibilityhelper.h"
#include <QWidget>
#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QAccessibleWidget>
#include <QAccessibleObject>
#include <QSettings>
#include <QByteArray>
#include <QDebug>
#include <QAccessible>
#include <QList>
#include <QScreen>
#include <QWindow>

AccessibilityHelper::AccessibilityHelper(QObject *parent)
    : QObject(parent), m_highContrastEnabled(false), m_textZoomPercentage(100)
{
}

AccessibilityHelper::~AccessibilityHelper()
{
}

// ========== MODE CONTRASTE ÉLEVÉ ==========
void AccessibilityHelper::enableHighContrast(QWidget *widget, bool enable)
{
    if (!widget) return;
    if (enable == m_highContrastEnabled) return;

    if (enable) {
        m_highContrastEnabled = true;

        // Save the root widget stylesheet.
        m_savedStyleSheet = widget->styleSheet();

        // Walk the entire widget tree: save and CLEAR every inline stylesheet.
        // This is necessary because Qt's cascade gives widget-level stylesheets
        // higher priority than parent stylesheets, so without clearing them the
        // high-contrast theme would be silently overridden on most widgets.
        m_savedWidgetStyleSheets.clear();
        const auto children = widget->findChildren<QWidget*>();
        for (QWidget *w : children) {
            const QString ss = w->styleSheet();
            if (!ss.isEmpty()) {
                m_savedWidgetStyleSheets.insert(w, ss);
                w->setStyleSheet(QString());
            }
        }

        // Now apply the high-contrast sheet to the root: it cascades freely
        // because all descendants have been cleared.
        widget->setStyleSheet(getStyleSheetHighContrast());

    } else {
        m_highContrastEnabled = false;

        // Remove the high-contrast root stylesheet.
        widget->setStyleSheet(QString());

        // Restore every child widget's original stylesheet.
        for (auto it = m_savedWidgetStyleSheets.cbegin();
             it != m_savedWidgetStyleSheets.cend(); ++it) {
            QWidget *w = it.key();
            if (w) w->setStyleSheet(it.value());
        }
        m_savedWidgetStyleSheets.clear();

        // Restore the root widget's original stylesheet (may be empty).
        widget->setStyleSheet(m_savedStyleSheet);
    }

    saveContrastState();
    widget->update();
    widget->repaint();
}

bool AccessibilityHelper::isHighContrastEnabled() const
{
    return m_highContrastEnabled;
}

// ========== ZOOM TEXTE ==========
void AccessibilityHelper::setTextZoom(int zoomPercentage)
{
    if (zoomPercentage < 50) zoomPercentage = 50;   // Min 50%
    if (zoomPercentage > 300) zoomPercentage = 300; // Max 300%

    m_textZoomPercentage = zoomPercentage;
}

int AccessibilityHelper::getTextZoom() const
{
    return m_textZoomPercentage;
}

int AccessibilityHelper::adjustFontSize(int baseSize) const
{
    return (baseSize * m_textZoomPercentage) / 100;
}

QFont AccessibilityHelper::getScaledFont(const QFont &baseFont) const
{
    QFont font = baseFont;
    int scaledSize = adjustFontSize(baseFont.pointSize() > 0 ? baseFont.pointSize() : 10);
    font.setPointSize(scaledSize);
    return font;
}

// ========== ACCESSIBILITÉ DES WIDGETS ==========
void AccessibilityHelper::setAccessibleWidget(QWidget *widget, const QString &name, 
                                               const QString &description, const QString &role)
{
    if (!widget) return;

    widget->setAccessibleName(name);
    widget->setAccessibleDescription(description);

    if (!role.isEmpty()) {
        // Note: Custom roles not directly supported, using name and description only
    }
}

void AccessibilityHelper::setAccessibleName(QWidget *widget, const QString &name)
{
    if (!widget) return;
    widget->setAccessibleName(name);
}

void AccessibilityHelper::setAccessibleDescription(QWidget *widget, const QString &description)
{
    if (!widget) return;
    widget->setAccessibleDescription(description);
}

// ========== NAVIGATION CLAVIER ==========
void AccessibilityHelper::setupKeyboardNavigation(QWidget *widget)
{
    if (!widget) return;

    // Activer la navigation par Tab
    widget->setFocusPolicy(Qt::TabFocus);

    // Pour les QPushButton, les QLineEdit, etc.
    QPushButton *btn = qobject_cast<QPushButton *>(widget);
    if (btn) {
        btn->setFocusPolicy(Qt::StrongFocus);
        btn->setShortcut(Qt::Key_Return);
    }

    QLineEdit *edit = qobject_cast<QLineEdit *>(widget);
    if (edit) {
        edit->setFocusPolicy(Qt::StrongFocus);
    }

    QComboBox *combo = qobject_cast<QComboBox *>(widget);
    if (combo) {
        combo->setFocusPolicy(Qt::StrongFocus);
    }
}

void AccessibilityHelper::enableTabOrder(QWidget * /*widget*/, const QList<QWidget*> &order)
{
    for (int i = 0; i < order.length() - 1; ++i) {
        if (order[i] && order[i+1]) {
            QWidget::setTabOrder(order[i], order[i+1]);
        }
    }
}

// ========== THÈMES ==========
QString AccessibilityHelper::getStyleSheetHighContrast()
{
    // WCAG AAA compliant: black background (#000) / yellow text (#FFFF00)
    // contrast ratio ≈ 19.1:1 — far above the 7:1 AAA threshold.
    return R"(
        /* ── Base ─────────────────────────────────────────── */
        QWidget, QFrame, QGroupBox, QScrollArea, QAbstractScrollArea {
            background-color: #000000;
            color: #FFFF00;
            border-color: #FFFF00;
        }
        QStackedWidget, QSplitter, QMainWindow, QDialog {
            background-color: #000000;
        }

        /* ── Labels ────────────────────────────────────────── */
        QLabel {
            background-color: transparent;
            color: #FFFF00;
            font-weight: bold;
        }

        /* ── Buttons ───────────────────────────────────────── */
        QPushButton, QToolButton {
            background-color: #1a1a00;
            color: #FFFF00;
            border: 2px solid #FFFF00;
            border-radius: 6px;
            padding: 8px 14px;
            font-weight: bold;
            font-size: 11pt;
        }
        QPushButton:hover, QToolButton:hover {
            background-color: #2e2e00;
            border-color: #00FFFF;
            color: #00FFFF;
        }
        QPushButton:pressed, QToolButton:pressed {
            background-color: #3a3a00;
            border-color: #FFFFFF;
            color: #FFFFFF;
        }
        QPushButton:checked {
            background-color: #FFFF00;
            color: #000000;
            border-color: #FFFFFF;
        }
        QPushButton:focus, QToolButton:focus {
            border: 3px solid #00FF00;
        }

        /* ── Text inputs ───────────────────────────────────── */
        QLineEdit, QTextEdit, QPlainTextEdit, QSpinBox, QDoubleSpinBox, QDateEdit {
            background-color: #0d0d00;
            color: #FFFF00;
            border: 2px solid #FFFF00;
            border-radius: 4px;
            padding: 5px;
            selection-background-color: #FFFF00;
            selection-color: #000000;
        }
        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {
            border: 2px solid #00FF00;
        }

        /* ── ComboBox ──────────────────────────────────────── */
        QComboBox {
            background-color: #0d0d00;
            color: #FFFF00;
            border: 2px solid #FFFF00;
            border-radius: 4px;
            padding: 5px 8px;
        }
        QComboBox QAbstractItemView {
            background-color: #1a1a00;
            color: #FFFF00;
            selection-background-color: #FFFF00;
            selection-color: #000000;
            border: 2px solid #FFFF00;
        }
        QComboBox::drop-down { border: none; }

        /* ── Tables ────────────────────────────────────────── */
        QTableWidget, QTableView, QHeaderView {
            background-color: #0d0d00;
            color: #FFFF00;
            gridline-color: #FFFF00;
            border: 2px solid #FFFF00;
        }
        QHeaderView::section {
            background-color: #1a1a00;
            color: #FFFF00;
            border: 1px solid #FFFF00;
            padding: 6px;
            font-weight: bold;
        }
        QTableWidget::item, QTableView::item {
            padding: 5px;
            color: #FFFF00;
        }
        QTableWidget::item:selected, QTableView::item:selected {
            background-color: #FFFF00;
            color: #000000;
        }
        QTableWidget::item:alternate {
            background-color: #0a0a00;
        }

        /* ── Scrollbars ────────────────────────────────────── */
        QScrollBar:vertical, QScrollBar:horizontal {
            background-color: #1a1a00;
            border: 1px solid #FFFF00;
        }
        QScrollBar::handle:vertical, QScrollBar::handle:horizontal {
            background-color: #FFFF00;
            border-radius: 4px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover, QScrollBar::handle:horizontal:hover {
            background-color: #00FFFF;
        }
        QScrollBar::add-line, QScrollBar::sub-line { background: transparent; }

        /* ── Menu / Toolbar ────────────────────────────────── */
        QMenuBar, QMenu, QToolBar {
            background-color: #000000;
            color: #FFFF00;
            border: 1px solid #FFFF00;
        }
        QMenu::item:selected { background-color: #FFFF00; color: #000000; }

        /* ── Tab widget ────────────────────────────────────── */
        QTabWidget::pane { border: 2px solid #FFFF00; }
        QTabBar::tab {
            background-color: #1a1a00;
            color: #FFFF00;
            border: 2px solid #FFFF00;
            padding: 6px 14px;
        }
        QTabBar::tab:selected { background-color: #FFFF00; color: #000000; }
    )";
}

QString AccessibilityHelper::getStyleSheetDefault()
{
    return R"(
        QWidget {
            background-color: #FFFFFF;
            color: #000000;
        }
        QPushButton {
            background-color: #0078D4;
            color: #FFFFFF;
            border: 1px solid #0078D4;
            border-radius: 4px;
            padding: 6px;
            font-size: 10pt;
        }
        QPushButton:hover {
            background-color: #1084D8;
        }
        QLineEdit, QTextEdit {
            background-color: #FFFFFF;
            color: #000000;
            border: 1px solid #CCCCCC;
            border-radius: 3px;
        }
    )";
}

void AccessibilityHelper::applyHighContrastTheme(QWidget *widget)
{
    if (!widget) return;
    widget->setStyleSheet(getStyleSheetHighContrast());
    applyZoomToWidget(widget);
}

void AccessibilityHelper::applyDefaultTheme(QWidget *widget)
{
    if (!widget) return;
    if (!m_savedStyleSheet.isEmpty()) {
        widget->setStyleSheet(m_savedStyleSheet);
    } else {
        widget->setStyleSheet(getStyleSheetDefault());
    }
    applyZoomToWidget(widget);
}

void AccessibilityHelper::applyZoomToWidget(QWidget *widget)
{
    if (!widget) return;

    qDebug() << "[ACCESSIBILITY] Applying zoom" << m_textZoomPercentage << "% to widget";

    // Regex to find font-size in stylesheets (e.g. "font-size: 13px;" or "font-size: 10pt;")
    static const QRegularExpression fontSizeRx(
        R"(font-size\s*:\s*(\d+)\s*(px|pt))",
        QRegularExpression::CaseInsensitiveOption);

    // Apply zoom to all descendants
    QList<QWidget*> allWidgets = widget->findChildren<QWidget*>();
    allWidgets.append(widget);

    int zoomedCount = 0;
    for (QWidget *w : allWidgets) {
        if (!w) continue;

        // --- 1. Scale the programmatic font ---
        QFont font = w->font();
        const QVariant baseProp = w->property("basePointSize");
        int baseSize = baseProp.isValid() ? baseProp.toInt() : font.pointSize();
        if (baseSize <= 0) baseSize = 10;  // Default fallback
        if (!baseProp.isValid()) {
            w->setProperty("basePointSize", baseSize);
        }

        int scaledSize = adjustFontSize(baseSize);
        font.setPointSize(scaledSize);
        w->setFont(font);

        // --- 2. Patch stylesheet font-size so it doesn't override setFont ---
        QString ss = w->styleSheet();
        if (!ss.isEmpty() && fontSizeRx.match(ss).hasMatch()) {
            // Save original stylesheet once
            if (!w->property("baseStyleSheet").isValid()) {
                w->setProperty("baseStyleSheet", ss);
            }
            const QString baseSheet = w->property("baseStyleSheet").toString();

            // Replace every font-size in the *original* stylesheet with the scaled value
            QString patched = baseSheet;
            QRegularExpressionMatchIterator it = fontSizeRx.globalMatch(baseSheet);
            // Process matches in reverse order so string positions stay valid
            QList<QRegularExpressionMatch> matches;
            while (it.hasNext()) matches.append(it.next());

            for (int i = matches.size() - 1; i >= 0; --i) {
                const QRegularExpressionMatch &m = matches[i];
                int origSize = m.captured(1).toInt();
                const QString unit = m.captured(2);

                // Save per-occurrence base size
                int scaledFontSize = (origSize * m_textZoomPercentage) / 100;
                if (scaledFontSize < 1) scaledFontSize = 1;

                const QString replacement = QString("font-size: %1%2").arg(scaledFontSize).arg(unit);
                patched.replace(m.capturedStart(), m.capturedLength(), replacement);
            }

            if (patched != ss) {
                w->setStyleSheet(patched);
            }
        }

        zoomedCount++;
    }

    qDebug() << "[ACCESSIBILITY] Zoomed" << zoomedCount << "widgets to size scale" << m_textZoomPercentage << "%";

    // Force full repaint
    widget->update();
    widget->repaint();

    // Also recursively update parent
    QWidget *parent = widget->parentWidget();
    while (parent) {
        parent->update();
        parent = parent->parentWidget();
    }
}

void AccessibilityHelper::setupHighContrastColors()
{
    // Couleurs pour mode contraste élevé
    // Noir : #000000
    // Blanc : #FFFFFF
    // Jaune : #FFFF00 (pour les bordures)
    // Cyan : #00FFFF (pour le hover)
    // Vert : #00FF00 (pour le focus)
}

void AccessibilityHelper::saveContrastState()
{
    QSettings settings("WasteGuard", "WasteGuard");
    settings.setValue("accessibility/highContrast", m_highContrastEnabled);
    settings.sync();
    qDebug() << "[ACCESSIBILITY] Contrast state saved:" << m_highContrastEnabled;
}

void AccessibilityHelper::loadContrastState(QWidget *widget)
{
    if (!widget) return;
    QSettings settings("WasteGuard", "WasteGuard");
    const bool saved = settings.value("accessibility/highContrast", false).toBool();
    if (saved && !m_highContrastEnabled) {
        enableHighContrast(widget, true);
    }
}

void AccessibilityHelper::saveZoom()
{
    QSettings settings("WasteGuard", "WasteGuard");
    settings.setValue("accessibility/textZoom", m_textZoomPercentage);
    settings.sync();
    qDebug() << "[ACCESSIBILITY] Zoom saved:" << m_textZoomPercentage << "%";
}

void AccessibilityHelper::loadZoom()
{
    QSettings settings("WasteGuard", "WasteGuard");
    int savedZoom = settings.value("accessibility/textZoom", 100).toInt();
    
    // Vérifier que la valeur est valide
    if (savedZoom < 50) savedZoom = 50;
    if (savedZoom > 300) savedZoom = 300;
    
    m_textZoomPercentage = savedZoom;
    qDebug() << "[ACCESSIBILITY] Zoom loaded:" << m_textZoomPercentage << "%";
}

void AccessibilityHelper::saveWindowState(QWidget *window)
{
    if (!window) return;
    
    QSettings settings("WasteGuard", "WasteGuard");
    settings.setValue("window/geometry", window->saveGeometry());
    settings.setValue("window/windowState", static_cast<int>(window->windowState()));
    settings.sync();
    qDebug() << "[ACCESSIBILITY] Window state saved";
}

void AccessibilityHelper::loadWindowState(QWidget *window)
{
    if (!window) return;
    
    QSettings settings("WasteGuard", "WasteGuard");
    const QByteArray geometry = settings.value("window/geometry", QByteArray()).toByteArray();
    Qt::WindowStates storedState = static_cast<Qt::WindowStates>(
        settings.value("window/windowState", static_cast<int>(Qt::WindowNoState)).toInt());
    storedState &= (Qt::WindowMaximized | Qt::WindowFullScreen);

    // Restore raw geometry only for normal-window state.
    // Maximized/fullscreen states are handled separately below.
    if (!geometry.isEmpty() && storedState == Qt::WindowNoState) {
        window->restoreGeometry(geometry);
    }

    QScreen *screen = nullptr;
    if (window->windowHandle() && window->windowHandle()->screen()) {
        screen = window->windowHandle()->screen();
    }
    if (!screen) {
        screen = QApplication::screenAt(window->frameGeometry().center());
    }
    if (!screen) {
        screen = QApplication::primaryScreen();
    }

    const QRect avail = screen ? screen->availableGeometry() : QRect();
    if (avail.isValid() && storedState == Qt::WindowNoState) {
        QSize safeSize = window->size();
        if (safeSize.width() <= 0 || safeSize.height() <= 0) {
            safeSize = QSize(qMin(1200, avail.width()), qMin(800, avail.height()));
        }

        safeSize.setWidth(qBound(320, safeSize.width(), avail.width()));
        safeSize.setHeight(qBound(240, safeSize.height(), avail.height()));
        if (window->size() != safeSize) {
            window->resize(safeSize);
        }

        const QPoint p = window->pos();
        const int maxX = avail.right() - window->width() + 1;
        const int maxY = avail.bottom() - window->height() + 1;
        const int clampedX = qBound(avail.left(), p.x(), maxX);
        const int clampedY = qBound(avail.top(), p.y(), maxY);
        if (clampedX != p.x() || clampedY != p.y()) {
            window->move(clampedX, clampedY);
        }
    }

    if (storedState != Qt::WindowNoState) {
        window->setWindowState(window->windowState() | storedState);
    }
    qDebug() << "[ACCESSIBILITY] Window state loaded";
}
