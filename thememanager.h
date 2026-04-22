#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QMap>

struct ThemePalette
{
    // Backgrounds
    QString bgApp;
    QString bgCard;
    QString bgCardAlt;
    // Borders
    QString border;
    QString borderFocus;
    // Text
    QString textTitle;
    QString textBody;
    QString textMuted;
    QString textWhite;
    // Primary
    QString primary;
    QString primaryDk;
    QString primaryLt;
    QString primaryBorder;
    // Sidebar
    QString sidebarBg;
    QString sidebarBg2;
    QString sidebarText;
    // Danger / Accent
    QString danger;
    QString accent;      // "Ajouter" buttons accent (gold-like)
    QString accentDk;
};

class ThemeManager : public QObject
{
    Q_OBJECT
public:
    enum Mode { Default = 0, Dark = 1, Custom = 2 };
    Q_ENUM(Mode)

    static ThemeManager* instance();

    Mode mode() const { return m_mode; }
    ThemePalette palette() const;

    static ThemePalette defaultPalette();
    static ThemePalette darkPalette();
    ThemePalette customPalette() const { return m_custom; }

    // Returns the list of customizable color role ids (stable keys).
    static QStringList customRoleKeys();
    // Human label for a role key.
    static QString roleLabel(const QString &key);
    // Read / write a single custom color (in #RRGGBB form).
    QString customColor(const QString &key) const;
    void setCustomColor(const QString &key, const QString &hex);

    void setMode(Mode m);
    void resetCustomToDefault();      // reset custom palette back to Default preset
    void resetCustomToDark();         // reset custom palette to Dark preset (starting point)
    void applyToApplication();        // applies palette + global QSS to qApp

signals:
    void themeChanged();

private:
    explicit ThemeManager(QObject *parent = nullptr);
    void load();
    void save();

    static QString toHex(const QColor &c);
    static QString setRoleOnPalette(ThemePalette &p, const QString &key, const QString &hex);
    static QString getRoleFromPalette(const ThemePalette &p, const QString &key);

    Mode m_mode;
    ThemePalette m_custom;
};

#endif // THEMEMANAGER_H
