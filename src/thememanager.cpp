#include "thememanager.h"

#include <QApplication>
#include <QPalette>
#include <QSettings>
#include <QColor>

namespace {
constexpr const char *kSettingsGroup = "Theme";
constexpr const char *kKeyMode       = "mode";
constexpr const char *kKeyCustomPfx  = "custom/";
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_mode(Default)
    , m_custom(defaultPalette())
{
    load();
}

ThemeManager* ThemeManager::instance()
{
    static ThemeManager *s_instance = nullptr;
    if (!s_instance) {
        s_instance = new ThemeManager(qApp);
    }
    return s_instance;
}

ThemePalette ThemeManager::defaultPalette()
{
    ThemePalette p;
    p.bgApp        = "#f0f2f5";
    p.bgCard       = "#ffffff";
    p.bgCardAlt    = "#f8fafb";
    p.border       = "#e8ecf0";
    p.borderFocus  = "#1e40af";
    p.textTitle    = "#1a1a2e";
    p.textBody     = "#374151";
    p.textMuted    = "#6c757d";
    p.textWhite    = "#ffffff";
    p.primary      = "#1e40af";
    p.primaryDk    = "#1e3a8a";
    p.primaryLt    = "#dbeafe";
    p.primaryBorder= "#bfdbfe";
    p.sidebarBg    = "#1a3a2e";
    p.sidebarBg2   = "#2d5a42";
    p.sidebarText  = "#a8d5b5";
    p.danger       = "#e74c3c";
    p.accent       = "#f0c343";
    p.accentDk     = "#d39c1e";
    return p;
}

ThemePalette ThemeManager::darkPalette()
{
    ThemePalette p;
    p.bgApp        = "#0f172a"; // slate-900
    p.bgCard       = "#1e293b"; // slate-800
    p.bgCardAlt    = "#273449"; // slightly lighter
    p.border       = "#334155"; // slate-700
    p.borderFocus  = "#60a5fa"; // blue-400
    p.textTitle    = "#f8fafc"; // slate-50
    p.textBody     = "#e2e8f0"; // slate-200
    p.textMuted    = "#94a3b8"; // slate-400
    p.textWhite    = "#ffffff";
    p.primary      = "#3b82f6"; // blue-500
    p.primaryDk    = "#2563eb"; // blue-600
    p.primaryLt    = "#1e3a8a"; // dark tinted
    p.primaryBorder= "#1e40af";
    p.sidebarBg    = "#0b1220";
    p.sidebarBg2   = "#1f2937";
    p.sidebarText  = "#cbd5e1";
    p.danger       = "#ef4444";
    p.accent       = "#facc15"; // yellow-400
    p.accentDk     = "#ca8a04";
    return p;
}

ThemePalette ThemeManager::palette() const
{
    switch (m_mode) {
    case Dark:   return darkPalette();
    case Custom: return m_custom;
    case Default:
    default:     return defaultPalette();
    }
}

QStringList ThemeManager::customRoleKeys()
{
    return {
        "bgApp", "bgCard", "bgCardAlt",
        "border", "borderFocus",
        "textTitle", "textBody", "textMuted",
        "primary", "primaryDk", "primaryLt", "primaryBorder",
        "sidebarBg", "sidebarBg2", "sidebarText",
        "danger", "accent", "accentDk"
    };
}

QString ThemeManager::roleLabel(const QString &key)
{
    static const QMap<QString, QString> labels = {
        {"bgApp",         "Arriere-plan application"},
        {"bgCard",        "Arriere-plan carte"},
        {"bgCardAlt",     "Arriere-plan secondaire"},
        {"border",        "Bordure"},
        {"borderFocus",   "Bordure focus"},
        {"textTitle",     "Texte titre"},
        {"textBody",      "Texte corps"},
        {"textMuted",     "Texte attenue"},
        {"primary",       "Couleur principale"},
        {"primaryDk",     "Principale foncee"},
        {"primaryLt",     "Principale claire"},
        {"primaryBorder", "Bordure principale"},
        {"sidebarBg",     "Barre laterale"},
        {"sidebarBg2",    "Barre laterale (accent)"},
        {"sidebarText",   "Texte barre laterale"},
        {"danger",        "Couleur danger"},
        {"accent",        "Accent (boutons Ajouter)"},
        {"accentDk",      "Accent fonce"}
    };
    return labels.value(key, key);
}

QString ThemeManager::getRoleFromPalette(const ThemePalette &p, const QString &key)
{
    if (key == "bgApp")         return p.bgApp;
    if (key == "bgCard")        return p.bgCard;
    if (key == "bgCardAlt")     return p.bgCardAlt;
    if (key == "border")        return p.border;
    if (key == "borderFocus")   return p.borderFocus;
    if (key == "textTitle")     return p.textTitle;
    if (key == "textBody")      return p.textBody;
    if (key == "textMuted")     return p.textMuted;
    if (key == "textWhite")     return p.textWhite;
    if (key == "primary")       return p.primary;
    if (key == "primaryDk")     return p.primaryDk;
    if (key == "primaryLt")     return p.primaryLt;
    if (key == "primaryBorder") return p.primaryBorder;
    if (key == "sidebarBg")     return p.sidebarBg;
    if (key == "sidebarBg2")    return p.sidebarBg2;
    if (key == "sidebarText")   return p.sidebarText;
    if (key == "danger")        return p.danger;
    if (key == "accent")        return p.accent;
    if (key == "accentDk")      return p.accentDk;
    return QString();
}

QString ThemeManager::setRoleOnPalette(ThemePalette &p, const QString &key, const QString &hex)
{
    const QString v = hex;
    if (key == "bgApp")         p.bgApp = v;
    else if (key == "bgCard")   p.bgCard = v;
    else if (key == "bgCardAlt")p.bgCardAlt = v;
    else if (key == "border")   p.border = v;
    else if (key == "borderFocus") p.borderFocus = v;
    else if (key == "textTitle")   p.textTitle = v;
    else if (key == "textBody")    p.textBody = v;
    else if (key == "textMuted")   p.textMuted = v;
    else if (key == "textWhite")   p.textWhite = v;
    else if (key == "primary")     p.primary = v;
    else if (key == "primaryDk")   p.primaryDk = v;
    else if (key == "primaryLt")   p.primaryLt = v;
    else if (key == "primaryBorder") p.primaryBorder = v;
    else if (key == "sidebarBg")    p.sidebarBg = v;
    else if (key == "sidebarBg2")   p.sidebarBg2 = v;
    else if (key == "sidebarText")  p.sidebarText = v;
    else if (key == "danger")       p.danger = v;
    else if (key == "accent")       p.accent = v;
    else if (key == "accentDk")     p.accentDk = v;
    return v;
}

QString ThemeManager::customColor(const QString &key) const
{
    return getRoleFromPalette(m_custom, key);
}

void ThemeManager::setCustomColor(const QString &key, const QString &hex)
{
    QColor c(hex);
    if (!c.isValid()) return;
    const QString normalized = c.name(QColor::HexRgb);
    setRoleOnPalette(m_custom, key, normalized);
    save();
    if (m_mode == Custom) {
        applyToApplication();
        emit themeChanged();
    }
}

void ThemeManager::setMode(Mode m)
{
    if (m_mode == m) return;
    m_mode = m;
    save();
    applyToApplication();
    emit themeChanged();
}

void ThemeManager::resetCustomToDefault()
{
    m_custom = defaultPalette();
    save();
    if (m_mode == Custom) {
        applyToApplication();
        emit themeChanged();
    }
}

void ThemeManager::resetCustomToDark()
{
    m_custom = darkPalette();
    save();
    if (m_mode == Custom) {
        applyToApplication();
        emit themeChanged();
    }
}

QString ThemeManager::toHex(const QColor &c)
{
    return c.name(QColor::HexRgb);
}

void ThemeManager::load()
{
    QSettings s;
    s.beginGroup(kSettingsGroup);
    int raw = s.value(kKeyMode, int(Default)).toInt();
    if (raw < 0 || raw > 2) raw = 0;
    m_mode = static_cast<Mode>(raw);

    ThemePalette base = defaultPalette();
    const QStringList keys = customRoleKeys();
    for (const QString &k : keys) {
        const QString v = s.value(QString(kKeyCustomPfx) + k,
                                  getRoleFromPalette(base, k)).toString();
        QColor c(v);
        if (c.isValid()) {
            setRoleOnPalette(m_custom, k, c.name(QColor::HexRgb));
        } else {
            setRoleOnPalette(m_custom, k, getRoleFromPalette(base, k));
        }
    }
    // Make sure derivable colors exist
    m_custom.textWhite = "#ffffff";
    s.endGroup();
}

void ThemeManager::save()
{
    QSettings s;
    s.beginGroup(kSettingsGroup);
    s.setValue(kKeyMode, int(m_mode));
    const QStringList keys = customRoleKeys();
    for (const QString &k : keys) {
        s.setValue(QString(kKeyCustomPfx) + k, getRoleFromPalette(m_custom, k));
    }
    s.endGroup();
    s.sync();
}

void ThemeManager::applyToApplication()
{
    if (!qApp) return;
    const ThemePalette p = palette();

    // QPalette for native widgets and menus.
    QPalette pal = qApp->palette();
    pal.setColor(QPalette::Window,          QColor(p.bgApp));
    pal.setColor(QPalette::WindowText,      QColor(p.textBody));
    pal.setColor(QPalette::Base,            QColor(p.bgCard));
    pal.setColor(QPalette::AlternateBase,   QColor(p.bgCardAlt));
    pal.setColor(QPalette::Text,            QColor(p.textBody));
    pal.setColor(QPalette::Button,          QColor(p.bgCard));
    pal.setColor(QPalette::ButtonText,      QColor(p.textTitle));
    pal.setColor(QPalette::Highlight,       QColor(p.primary));
    pal.setColor(QPalette::HighlightedText, QColor(p.textWhite));
    pal.setColor(QPalette::ToolTipBase,     QColor(p.bgCard));
    pal.setColor(QPalette::ToolTipText,     QColor(p.textBody));
    pal.setColor(QPalette::PlaceholderText, QColor(p.textMuted));
    qApp->setPalette(pal);

    // Global stylesheet that acts as a safety net for widgets not covered by
    // per-widget stylesheets in MainWindow::applyHomogeneousTheme().
    // Per-widget styles always take precedence (higher specificity).
    const QString qss = QString(
        // Base: all widgets get correct bg + text so dark mode works globally
        "QWidget{ background-color:%1; color:%2; }"
        "QFrame{ background-color:%1; color:%2; }"
        "QLabel{ background:transparent; color:%2; }"
        // Tooltips
        "QToolTip{ background:%3; color:%2; border:1px solid %4; padding:4px 8px; }"
        // Menus
        "QMenu{ background:%3; color:%2; border:1px solid %4; }"
        "QMenu::item:selected{ background:%5; color:%6; }"
        // ScrollBars
        "QScrollBar:vertical{ background:%1; width:8px; border-radius:4px; margin:0; }"
        "QScrollBar::handle:vertical{ background:%4; border-radius:4px; min-height:30px; }"
        "QScrollBar::handle:vertical:hover{ background:%5; }"
        "QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{ height:0px; }"
        "QScrollBar:horizontal{ background:%1; height:8px; border-radius:4px; margin:0; }"
        "QScrollBar::handle:horizontal{ background:%4; border-radius:4px; min-width:30px; }"
        "QScrollBar::handle:horizontal:hover{ background:%5; }"
        "QScrollBar::add-line:horizontal,QScrollBar::sub-line:horizontal{ width:0px; }"
        // Status bar
        "QStatusBar{ background:%3; color:%2; border-top:1px solid %4; }"
    ).arg(p.bgApp,       // %1  app bg
          p.textBody,    // %2  body text
          p.bgCard,      // %3  card bg
          p.border,      // %4  border (doubles as scrollbar handle)
          p.primary,     // %5  primary (highlight / scrollbar hover)
          p.textWhite    // %6  highlighted text
    );
    qApp->setStyleSheet(qss);
}
