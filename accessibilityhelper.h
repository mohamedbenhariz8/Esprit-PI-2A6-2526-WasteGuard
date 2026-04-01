#ifndef ACCESSIBILITYHELPER_H
#define ACCESSIBILITYHELPER_H

#include <QObject>
#include <QWidget>
#include <QFont>
#include <QString>

/**
 * @class AccessibilityHelper
 * @brief Gère les fonctionnalités d'accessibilité pour les personnes handicapées
 * 
 * Fonctionnalités :
 * - Mode contraste élevé (dark mode)
 * - Zoom texte configurable
 * - Navigation clavier améliorée
 * - Noms accessibles pour les widgets
 */
class AccessibilityHelper : public QObject
{
    Q_OBJECT

public:
    explicit AccessibilityHelper(QObject *parent = nullptr);
    ~AccessibilityHelper();

    // Mode Contraste Élevé
    void enableHighContrast(QWidget *widget, bool enable = true);
    bool isHighContrastEnabled() const;

    // Zoom Texte
    void setTextZoom(int zoomPercentage); // 50-200%
    int getTextZoom() const;
    int adjustFontSize(int baseSize) const;
    QFont getScaledFont(const QFont &baseFont) const;

    // Accessibilité des widgets
    void setAccessibleWidget(QWidget *widget, const QString &name, const QString &description, const QString &role = "");
    void setAccessibleName(QWidget *widget, const QString &name);
    void setAccessibleDescription(QWidget *widget, const QString &description);

    // Navigation clavier améliorée
    void setupKeyboardNavigation(QWidget *widget);
    void enableTabOrder(QWidget *widget, const QList<QWidget*> &order);

    // Appliquer les thèmes
    void applyHighContrastTheme(QWidget *widget);
    void applyDefaultTheme(QWidget *widget);

    // Utilitaires
    QString getStyleSheetHighContrast();
    QString getStyleSheetDefault();
    void applyZoomToWidget(QWidget *widget);
    
    // Sauvegarde/Chargement du zoom
    void saveZoom();
    void loadZoom();
    
    // Sauvegarde/Chargement de la géométrie de la fenêtre
    void saveWindowState(QWidget *window);
    void loadWindowState(QWidget *window);

private:
    bool m_highContrastEnabled;
    int m_textZoomPercentage;
    QString m_savedStyleSheet;

    void setupHighContrastColors();
};

#endif // ACCESSIBILITYHELPER_H
