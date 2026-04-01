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

    if (enable == m_highContrastEnabled) {
        return;
    }

    if (enable) {
        // Preserve the active application stylesheet so disabling high contrast
        // restores the exact previous visual design.
        m_savedStyleSheet = widget->styleSheet();
        m_highContrastEnabled = true;
        applyHighContrastTheme(widget);
    } else {
        m_highContrastEnabled = false;
        applyDefaultTheme(widget);
    }
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
    return R"(
        QWidget {
            background-color: #000000;
            color: #FFFFFF;
            border: 2px solid #FFFF00;
        }
        QPushButton {
            background-color: #000000;
            color: #FFFFFF;
            border: 3px solid #FFFF00;
            border-radius: 5px;
            padding: 8px;
            font-weight: bold;
            font-size: 12pt;
        }
        QPushButton:hover {
            background-color: #1a1a1a;
            border: 3px solid #00FFFF;
        }
        QPushButton:pressed {
            background-color: #2a2a2a;
            border: 3px solid #FFFFFF;
        }
        QPushButton:focus {
            outline: 3px solid #00FF00;
        }
        QLineEdit, QTextEdit {
            background-color: #1a1a1a;
            color: #FFFFFF;
            border: 2px solid #FFFF00;
            border-radius: 3px;
            padding: 5px;
        }
        QLineEdit:focus, QTextEdit:focus {
            border: 2px solid #00FF00;
            outline: 3px dashed #00FF00;
        }
        QComboBox {
            background-color: #1a1a1a;
            color: #FFFFFF;
            border: 2px solid #FFFF00;
            padding: 5px;
        }
        QLabel {
            color: #FFFFFF;
            font-weight: bold;
        }
        QTableWidget {
            background-color: #1a1a1a;
            color: #FFFFFF;
            gridline-color: #FFFF00;
        }
        QTableWidget::item {
            padding: 5px;
            border: 1px solid #666666;
        }
        QTableWidget::item:selected {
            background-color: #00FFFF;
            color: #000000;
        }
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

    // Appliquer le zoom à tous les descendants
    QList<QWidget*> allWidgets = widget->findChildren<QWidget*>();
    allWidgets.append(widget);

    int zoomedCount = 0;
    for (QWidget *w : allWidgets) {
        if (!w) continue;
        
        QFont font = w->font();
        const QVariant baseProp = w->property("basePointSize");
        int baseSize = baseProp.isValid() ? baseProp.toInt() : font.pointSize();
        if (baseSize <= 0) baseSize = 10;  // Default fallback
        if (!baseProp.isValid()) {
            w->setProperty("basePointSize", baseSize);
        }
        
        int scaledSize = adjustFontSize(baseSize);
        
        // IMPORTANT: Toujours appliquer la nouvelle taille
        font.setPointSize(scaledSize);
        w->setFont(font);
        
        zoomedCount++;
    }
    
    qDebug() << "[ACCESSIBILITY] Zoomed" << zoomedCount << "widgets to size scale" << m_textZoomPercentage << "%";
    
    // Force la mise à jour complète du widget
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
    QByteArray geometry = settings.value("window/geometry", QByteArray()).toByteArray();
    Qt::WindowStates windowState = static_cast<Qt::WindowStates>(settings.value("window/windowState", static_cast<int>(Qt::WindowNoState)).toInt());
    
    if (!geometry.isEmpty()) {
        window->restoreGeometry(geometry);
    }
    
    window->setWindowState(windowState);
    qDebug() << "[ACCESSIBILITY] Window state loaded";
}
