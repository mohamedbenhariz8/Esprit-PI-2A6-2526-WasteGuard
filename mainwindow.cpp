#include "mainwindow.h"

#include "ui_mainwindow.h"
#include <QPdfWriter>
#include <QPainter>
#include <QPageSize>
#include <QMarginsF>
#include <QFileDialog>




#include <QPushButton>

#include <QHBoxLayout>

#include <QWidget>

#include <QTime>
#include <QDate>

#include <QMessageBox>

#include <QInputDialog>

#include <QTableWidget>

#include <QTableWidgetItem>

#include <QHeaderView>

#include <QBrush>

#include <QTimer>

#include <QFrame>
#include <QStackedWidget>
#include <QScrollArea>
#include <QGridLayout>
#include <QDebug>
#include <functional>

#include <QVBoxLayout>

#include <QApplication>

#include <QStyle>

#include <QStackedWidget>

#include <QLinearGradient>

#include <QPen>

#include <QPixmap>

#include <QLineEdit>

#include <QComboBox>

#include <QListWidget>

#include <QListWidgetItem>

#include <QLabel>

#include <QDialog>
#include <QEvent>
#include <QDesktopServices>
#include <QUrl>
#include <QStandardPaths>

#include <QtCharts/QPieSeries>

#include <QtCharts/QPieSlice>

#include <QtCharts/QChartView>

#include <QtCharts/QChart>

#include <QtCharts/QValueAxis>

#include <QtCharts/QLineSeries>

#include <QtCharts/QSplineSeries>

#include <QtCharts/QAreaSeries>

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

static constexpr int ACTIONS_COL = 6;
static constexpr int EMP_ROLE_ID = Qt::UserRole + 1;
static constexpr int EMP_ROLE_EMAIL = Qt::UserRole + 2;
static constexpr int EMP_ROLE_CIN = Qt::UserRole + 3;
static constexpr int EMP_ROLE_SALAIRE = Qt::UserRole + 4;
static constexpr int EMP_ROLE_PERF = Qt::UserRole + 5;
static constexpr int PROD_ROLE_CUR = Qt::UserRole;
static constexpr int PROD_ROLE_MAX = Qt::UserRole + 1;
static constexpr int PROD_ROLE_AISLE = Qt::UserRole + 2;
static constexpr int PROD_ROLE_ID = Qt::UserRole + 6;
static constexpr int PROD_ROLE_BATTERY = Qt::UserRole + 7;
static constexpr int STOCK_ROLE_ID = Qt::UserRole + 50;
static constexpr int STOCK_ROLE_SEUIL = Qt::UserRole + 51;
static constexpr int STOCK_ROLE_ID_FOUR = Qt::UserRole + 52;



namespace {

const QRegularExpression &emailRegex()
{
    static const QRegularExpression re(
        R"(^[A-Z0-9._%+\-]+@[A-Z0-9.\-]+\.[A-Z]{2,}$)",
        QRegularExpression::CaseInsensitiveOption
    );
    return re;
}

bool isValidEmail(const QString &email)
{
    return emailRegex().match(email.trimmed()).hasMatch();
}

QString productStatusFromQty(int qty)
{
    if (qty <= 0) return "Rupture";
    if (qty < 20) return "Stock Faible";
    return "En Stock";
}

QString resolvedProductStatus(const QString &statusUi, int qty)
{
    const QString s = statusUi.trimmed();
    return s.isEmpty() ? productStatusFromQty(qty) : s;
}

void setComboTextSafe(QComboBox *combo, const QString &value)
{
    if (!combo) return;
    int idx = combo->findText(value, Qt::MatchFixedString);
    if (idx < 0) idx = combo->findText(value, Qt::MatchContains);
    if (idx < 0) {
        combo->addItem(value);
        idx = combo->findText(value, Qt::MatchFixedString);
    }
    if (idx >= 0) combo->setCurrentIndex(idx);
}

QString firstNonEmptyLineEditText(QWidget *scope, const QStringList &preferredNames)
{
    if (!scope) return QString();

    auto byName = [scope](const QString &name) -> QString {
        if (auto *le = scope->findChild<QLineEdit*>(name)) {
            return le->text().trimmed();
        }
        return QString();
    };

    for (const QString &name : preferredNames) {
        const QString v = byName(name);
        if (!v.isEmpty()) return v;
    }

    const auto edits = scope->findChildren<QLineEdit*>();
    for (QLineEdit *le : edits) {
        if (!le || !le->isVisible()) continue;
        const QString v = le->text().trimmed();
        if (!v.isEmpty()) return v;
    }

    return QString();
}

double parseStockPrice(QString text, bool *okOut = nullptr)
{
    text = text.trimmed();
    text.remove("TND", Qt::CaseInsensitive);
    text.remove("DT", Qt::CaseInsensitive);
    text.remove(' ');
    text.replace(',', '.');
    bool ok = false;
    const double v = text.toDouble(&ok);
    if (okOut) *okOut = ok;
    return v;
}

QString stockSeuilLabel(int quantite, int seuilCritique)
{
    const int seuil = qMax(1, seuilCritique);
    if (quantite <= seuil) return "CRITIQUE";
    if (quantite <= (seuil * 2)) return "MOYEN";
    return "OK";
}

int defaultSeuilCritique(int quantite)
{
    return qMax(5, quantite / 5);
}

void setStockSeuilBadge(QTableWidget *table, int row, const QString &seuil)
{
    if (!table || row < 0 || row >= table->rowCount()) return;

    QLabel *badge = new QLabel(seuil);
    badge->setAlignment(Qt::AlignCenter);
    badge->setFixedSize(90, 25);

    QString style = "border-radius: 5px; font-weight: bold;";
    if (seuil == "CRITIQUE") style += "background-color: #dc3545; color: white;";
    else if (seuil == "MOYEN") style += "background-color: #ffc107; color: #333;";
    else style += "background-color: #28a745; color: white;";
    badge->setStyleSheet(style);

    QWidget *badgeWidget = new QWidget();
    auto *badgeLayout = new QHBoxLayout(badgeWidget);
    badgeLayout->addWidget(badge);
    badgeLayout->setAlignment(Qt::AlignCenter);
    badgeLayout->setContentsMargins(0, 0, 0, 0);
    table->setCellWidget(row, 3, badgeWidget);
}

QString normalizedStatus(QString value)
{
    return value.normalized(QString::NormalizationForm_D).toLower().trimmed();
}

QString toDbDisponibilite(const QString &statusUi)
{
    const QString s = normalizedStatus(statusUi);
    if (s.contains("indispon")) return "INDISPONIBLE";
    if (s.contains("intervention")) return "INDISPONIBLE";
    if (s.contains("conge")) return "INDISPONIBLE";
    if (s.contains("mission")) return "INDISPONIBLE";
    if (s.contains("disponible")) return "DISPONIBLE";
    return "INDISPONIBLE";
}

QString friendlySqlErrorMessage(const QString &rawError, const QString &action)
{
    const QString upper = rawError.toUpper();

    if (upper.contains("AUCUN FOURNISSEUR DISPONIBLE")) {
        return QString("Impossible de %1: aucun fournisseur n'existe dans la base.").arg(action);
    }
    if (upper.contains("ORA-00001")) {
        return QString("Impossible de %1: une valeur unique existe deja.").arg(action);
    }
    if (upper.contains("ORA-01400")) {
        if (upper.contains("ID_FOUR")) {
            return QString("Impossible de %1: fournisseur obligatoire manquant (ID_FOUR).").arg(action);
        }
        return QString("Impossible de %1: un champ obligatoire est vide.").arg(action);
    }
    if (upper.contains("ORA-02291")) {
        if (upper.contains("ID_FOUR")) {
            return QString("Impossible de %1: le fournisseur selectionne n'existe pas.").arg(action);
        }
        return QString("Impossible de %1: reference inexistante.").arg(action);
    }
    if (upper.contains("ORA-02292")) {
        return QString("Impossible de %1: element utilise dans d'autres donnees.").arg(action);
    }
    if (upper.contains("DRIVER NOT LOADED")) {
        return "Connexion base indisponible: driver SQL non charge.";
    }
    if (upper.contains("ORA-00942")) {
        return "Table introuvable dans la base de donnees.";
    }
    return QString("Impossible de %1. Verifiez les donnees saisies.").arg(action);
}

void showFriendlySqlError(QWidget *parent, const QString &action, const QString &rawError)
{
    QMessageBox msg(parent);
    msg.setIcon(QMessageBox::Critical);
    msg.setWindowTitle("Erreur SQL");
    msg.setText(friendlySqlErrorMessage(rawError, action));
    if (!rawError.trimmed().isEmpty()) {
        msg.setDetailedText(rawError);
    }
    msg.exec();
}

void enableStyledBgRecursive(QWidget *root)

{

    if (!root) return;



    root->setAttribute(Qt::WA_StyledBackground, true);



    const auto childs = root->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);

    for (QWidget *c : childs)

        enableStyledBgRecursive(c);

}



void repolishRecursive(QWidget *root)

{

    if (!root) return;



    root->style()->unpolish(root);

    root->style()->polish(root);

    root->update();



    const auto childs = root->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);

    for (QWidget *c : childs)

        repolishRecursive(c);

}



void styleTopTitle(QLabel *label)

{

    if (!label) return;

    label->setStyleSheet("color: #1f2d3d; font-size: 18px; font-weight: 700;");

    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

}



void styleTopUserName(QLabel *label)

{

    if (!label) return;

    label->setStyleSheet("color: #0f2b4c; font-size: 14px; font-weight: 700;");

    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

}



void styleTopUserRole(QLabel *label)

{

    if (!label) return;

    label->setStyleSheet("color: #16a34a; font-size: 12px; font-weight: 700;");

    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

}



void styleTopIconButton(QPushButton *button)

{

    if (!button) return;

    button->setMinimumSize(34, 34);

    button->setMaximumSize(34, 34);

    button->setCursor(Qt::PointingHandCursor);

    button->setStyleSheet(

        "QPushButton {"

        " background-color: #ffffff;"

        " color: #0f2b4c;"

        " border: 1px solid #d4dbe6;"

        " border-radius: 17px;"

        " font-size: 14px;"

        " font-weight: 700;"

        "}"

        "QPushButton:hover { background-color: #eff3f8; border-color: #1a4270; }"

    );

}



void styleModelAddButton(QPushButton *button)

{

    if (!button) return;

    button->setMinimumHeight(44);

    button->setMaximumHeight(44);

    if (button->minimumWidth() < 132)

        button->setMinimumWidth(132);

    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    button->setCursor(Qt::PointingHandCursor);

    button->setStyleSheet(

        "QPushButton {"

        " background-color: #0f2b4c;"

        " color: #ffffff;"

        " border: none;"

        " border-radius: 12px;"

        " padding: 8px 22px;"

        " font-size: 14px;"

        " font-weight: 700;"

        "}"

        "QPushButton:hover { background-color: #1a4270; }"

        "QPushButton:pressed { background-color: #0b223c; }"

    );

}



void moveWidgetBefore(QWidget *root, const char *widgetName, const char *layoutName, const char *beforeWidgetName)

{

    if (!root) return;



    QWidget *widget = root->findChild<QWidget*>(widgetName);

    QHBoxLayout *layout = root->findChild<QHBoxLayout*>(layoutName);

    QWidget *beforeWidget = root->findChild<QWidget*>(beforeWidgetName);

    if (!widget || !beforeWidget) return;



    if (!layout) {

        if (auto *parent = beforeWidget->parentWidget())

            layout = qobject_cast<QHBoxLayout*>(parent->layout());

    }

    if (!layout) return;



    int targetIdx = layout->indexOf(beforeWidget);

    if (targetIdx < 0) {

        if (auto *parent = beforeWidget->parentWidget()) {

            if (auto *fallbackLayout = qobject_cast<QHBoxLayout*>(parent->layout())) {

                layout = fallbackLayout;

                targetIdx = layout->indexOf(beforeWidget);

            }

        }

    }

    if (targetIdx < 0) return;



    if (layout->indexOf(widget) >= 0) {

        layout->removeWidget(widget);

    } else if (auto *parentLayout = widget->parentWidget() ? widget->parentWidget()->layout() : nullptr) {

        parentLayout->removeWidget(widget);

    }

    layout->insertWidget(targetIdx, widget);

}



void moveWidgetToStart(QWidget *root, const char *widgetName, const char *layoutName)

{

    if (!root) return;



    QWidget *widget = root->findChild<QWidget*>(widgetName);

    QHBoxLayout *layout = root->findChild<QHBoxLayout*>(layoutName);

    if (!widget || !layout) return;



    if (layout->indexOf(widget) >= 0) {

        layout->removeWidget(widget);

    } else if (auto *parentLayout = widget->parentWidget() ? widget->parentWidget()->layout() : nullptr) {

        parentLayout->removeWidget(widget);

    }

    layout->insertWidget(0, widget);

}



void styleTopCombinedUser(QLabel *label)

{

    if (!label) return;



    const QString raw = label->text().trimmed();

    if (raw.contains('\n')) {

        const QStringList lines = raw.split('\n');

        const QString firstLine = lines.value(0).trimmed().toHtmlEscaped();

        const QString secondLine = lines.mid(1).join(" ").trimmed().toHtmlEscaped();

        label->setTextFormat(Qt::RichText);

        label->setText(

            QString("<span style=\"color:#0f2b4c;font-size:14px;font-weight:700;\">%1</span>"

                    "<br/>"

                    "<span style=\"color:#16a34a;font-size:12px;font-weight:700;\">%2</span>")

                .arg(firstLine, secondLine));

    } else {

        label->setTextFormat(Qt::PlainText);

        label->setStyleSheet("color: #0f2b4c; font-size: 14px; font-weight: 700;");

    }

    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

}

}



MainWindow::MainWindow(QWidget *parent)

    : QMainWindow(parent)

    , ui(new Ui::MainWindow)

    , sidebarGroup(nullptr)

    , homeDashboardPage(nullptr)

    , currentEmployeRow(-1)

    , currentProduitRow(-1)

    , currentClientRow(-1)

    , currentMaintRow(-1)

    , globalStatsReturnPage(nullptr)

    , m_sidebarExpanded(true)

{

    ui->setupUi(this);

    // Initialize Add Commande Date Comboboxes
    for (int i = 1; i <= 31; ++i) {
        ui->comboBox_19->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
    }
    for (int i = 1; i <= 12; ++i) {
        ui->comboBox_20->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
    }
    for (int i = 2024; i <= 2035; ++i) {
        ui->comboBox_21->addItem(QString::number(i));
    }
    for (int i = 1; i <= 31; ++i) {
        ui->comboBox_22->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
    }
    for (int i = 1; i <= 12; ++i) {
        ui->comboBox_23->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
    }
    for (int i = 2024; i <= 2035; ++i) {
        ui->comboBox_24->addItem(QString::number(i));
    }
    
    // Initialize Modify Commande Date Comboboxes
    for (int i = 1; i <= 31; ++i) {
        ui->comboBox_7->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
    }
    for (int i = 1; i <= 12; ++i) {
        ui->comboBox_8->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
    }
    for (int i = 2024; i <= 2035; ++i) {
        ui->comboBox_9->addItem(QString::number(i));
    }
    for (int i = 1; i <= 31; ++i) {
        ui->comboBox_13->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
    }
    for (int i = 1; i <= 12; ++i) {
        ui->comboBox_14->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
    }
    for (int i = 2024; i <= 2035; ++i) {
        ui->comboBox_15->addItem(QString::number(i));
    }


    // --- AGGRESSIVE RELOAD OF STOCK SIDEBAR STYLES ---

    // We use a timer to ensure all widgets are fully initialized

    QTimer::singleShot(100, this, [this](){

        // 1. Force Find Widgets (Generic Search)

        QWidget *statsBox = this->findChild<QWidget*>("statsBox");

        QWidget *orderBox = this->findChild<QWidget*>("orderBox");

        

        // Debugging Feedback

        QString debugMsg = "Style Update Debug:\n";

        debugMsg += "statsBox found: " + QString(statsBox ? "YES" : "NO") + "\n";

        debugMsg += "orderBox found: " + QString(orderBox ? "YES" : "NO");

        

        // Uncomment the line below to see the debug check on startup

        // QMessageBox::information(this, "Debug Styles", debugMsg);



        if (statsBox) {

            // override previous stylesheet completely

            statsBox->setStyleSheet(

                "QWidget#statsBox {"

                "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0f2b4c, stop:1 #2a5298);" // Back to Dark Navy

                "   border-radius: 20px;"

                "   color: white;"

                "   border: none;"

                "}"

            );

            // Re-style children specifically

            auto *lblTitle = statsBox->findChild<QLabel*>("lblStatsTitle");

            if (lblTitle) lblTitle->setStyleSheet("color: #cfd8dc; font-size: 11px; font-weight: bold; background: transparent;");

            

            auto *lblVal = statsBox->findChild<QLabel*>("totalStock");

            if (lblVal) lblVal->setStyleSheet("color: #ffffff; font-size: 32px; font-weight: 800; background: transparent;");



            auto *lblCrit = statsBox->findChild<QLabel*>("lblCriticalStock");

            if (lblCrit) lblCrit->setStyleSheet("color: #ff6b6b; font-size: 13px; font-weight: 700; background: transparent; border: 1px solid #ff6b6b; border-radius: 6px; padding: 4px;");

        }



        if (orderBox) {

            orderBox->setStyleSheet(

                "QWidget#orderBox {"

                "   background-color: #ffffff;"

                "   border-radius: 20px;"

                "   border: 1px solid #f0f2f5;"

                "   border-left: 5px solid #eab308;" // Back to original Amber

                "}"

            );

            auto *lblTitle = orderBox->findChild<QLabel*>("lblOrderTitle");

            if (lblTitle) lblTitle->setStyleSheet("color: #1e293b; font-size: 12px; font-weight: 800; background: transparent;");

            

            auto *lblSum = orderBox->findChild<QLabel*>("lblOrderSummary");

            if (lblSum) lblSum->setStyleSheet("color: #64748b; font-size: 13px; background: transparent; font-style: italic;");

            

            auto *btnOrder = orderBox->findChild<QPushButton*>("btnOrder");

            QString btnStyle = 

                "QPushButton {"

                "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0f2b4c, stop:1 #2a5298);" // Back to Dark Navy Gradient

                "   color: white; border-radius: 12px; padding: 12px; font-weight: 700; font-size: 13px;"

                "   border: 1px solid #1e40af;"

                "}"

                "QPushButton:hover {"

                "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #1e40af, stop:1 #3b82f6);" // Original Hover

                "   border-color: #60a5fa;"

                "}";



            if (btnOrder) btnOrder->setStyleSheet(btnStyle);

            

            // Apply same style to other buttons

            if (auto *btnPred = this->findChild<QPushButton*>("btnPrediction")) btnPred->setStyleSheet(btnStyle);

            if (auto *btnAlert = this->findChild<QPushButton*>("btnAlerts")) btnAlert->setStyleSheet(btnStyle);

        }



        // --- APPLY STOCK STYLE TO PRODUCT SIDEBAR ---

        // 1. Main Stats Preview Card

        if (QWidget *prodStats = this->findChild<QWidget*>("prod_statsPreview")) {

            prodStats->setStyleSheet(

                "QWidget#prod_statsPreview {"

                "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0f2b4c, stop:1 #2a5298);"

                "   border-radius: 20px;"

                "   color: white;"

                "   border: none;"

                "}"

            );

            // Style inner labels

            const QStringList whiteLabels = {"prod_lblStatsTitle", "prod_lblMini1", "prod_lblMini2"};

            for (const QString &name : whiteLabels) {

                if (QLabel *lbl = prodStats->findChild<QLabel*>(name)) {

                    lbl->setStyleSheet("color: white; background: transparent; font-weight: bold;");

                }

            }

        }



        // 2. Creative Stat Cards (prod_stat1, prod_stat2, prod_stat3)

        // User requested "something creative" for the first 3 numbers

        auto styleCreativeCard = [this](const QString &name, const QString &startColor, const QString &endColor, const QString &textColor) {

            if (QWidget *card = this->findChild<QWidget*>(name)) {

                card->setStyleSheet(QString(

                    "QWidget#%1 {"

                    "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 %2, stop:1 %3);"

                    "   border-radius: 15px;"

                    "   border: 1px solid %3;"

                    "}"

                ).arg(name, startColor, endColor));

                

                // Ensure labels inside are styled

                QString valName = name;

                if (QLabel *lblVal = card->findChild<QLabel*>(valName.replace("stat", "st_val"))) {

                     lblVal->setStyleSheet(QString("color: %1; font-size: 20px; font-weight: 900; background: transparent;").arg(textColor));

                }

                QString lblName = name;

                if (QLabel *lblLbl = card->findChild<QLabel*>(lblName.replace("stat", "st_lbl"))) { // Corrected replace logic: stat -> st_lbl

                     lblLbl->setStyleSheet(QString("color: %1; font-size: 12px; font-weight: 600; opacity: 0.8; background: transparent;").arg(textColor));

                }

            }

        };



        // Light, airy gradients for the top cards

        styleCreativeCard("prod_stat1", "#e0f2fe", "#bae6fd", "#0c4a6e"); // Sky blue gradient

        styleCreativeCard("prod_stat2", "#e0f2fe", "#bae6fd", "#0c4a6e"); // Sky blue gradient

        styleCreativeCard("prod_stat3", "#fee2e2", "#fecaca", "#7f1d1d"); // Red gradient for alert



        // 3. Action Buttons (Map 3D, Video 3D, Export) -> Creative Gradient Style

        QString prodBtnStyle = 

            "QPushButton {"

            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0f2b4c, stop:1 #2a5298);"

            "   color: white; border-radius: 12px; padding: 12px; font-weight: 700; font-size: 13px;"

            "   border: 1px solid #1e40af;"

            "}"

            "QPushButton:hover {"

            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #1e40af, stop:1 #3b82f6);"

            "   border-color: #60a5fa;"

            "}";

        

        if (auto *btnMap = this->findChild<QPushButton*>("prod_btnMap3D")) btnMap->setStyleSheet(prodBtnStyle);

        if (auto *btnVideo = this->findChild<QPushButton*>("prod_btnVideo3D")) btnVideo->setStyleSheet(prodBtnStyle);

        

        // Export Button

        if (auto *btnPdf = this->findChild<QPushButton*>("prod_btnPdf")) btnPdf->setStyleSheet(prodBtnStyle);



        // 4. Compact "Voir Details" Button

        if (auto *btnOpen = this->findChild<QPushButton*>("prod_btnOpenStats")) {

             btnOpen->setStyleSheet(

                "QPushButton {"

                "   background: transparent;"

                "   color: white;"

                "   border: 1px solid rgba(255,255,255,0.5);"

                "   border-radius: 8px;"

                "   padding: 4px 8px;"

                "   font-size: 11px;"

                "   font-weight: 600;"

                "}"

                "QPushButton:hover {"

                "   background-color: rgba(255,255,255,0.1);"

                "   border-color: white;"

                "}"

             );

             btnOpen->setCursor(Qt::PointingHandCursor);

        }



        // Product Sidebar Fix (Robust - Cleanup)

        // We still run this loop to catch any other stray labels, but the card logic above handles most

        const QStringList prodLabelNames = {

             "prod_st_val1", "prod_st_lbl1",

             "prod_st_val2", "prod_st_lbl2",

             "prod_st_val3", "prod_st_lbl3",

             "prod_lblFunc"

        };

        for (const QString &name : prodLabelNames) {

             if (QLabel *lbl = this->findChild<QLabel*>(name)) {

                 QString current = lbl->styleSheet();

                 if (!current.contains("background: transparent")) {

                     lbl->setStyleSheet(current + " background: transparent;");

                 }

             }

        }

    });



    if (ui->btnStatistiques) ui->btnStatistiques->hide();

    if (ui->btnRetour) ui->btnRetour->hide();



    // --- SIDEBAR NAVIGATION LOGIC ---

    sidebarGroup = new QButtonGroup(this);

    sidebarGroup->setExclusive(true);



    QList<QPushButton*> sidebarButtons = {

        ui->btnAccueil, ui->btnClient, ui->btnStock, ui->btnProduits,

        ui->btnEmployes, ui->btnStatistiques, ui->btnMaintenance,

        this->findChild<QPushButton*>("btnCommandes")

    };



    for (QPushButton* btn : sidebarButtons) {

        if (!btn) continue;

        btn->setCheckable(true);

        sidebarGroup->addButton(btn);

    }



    if (ui->btnAccueil) {

        ui->btnAccueil->setChecked(true);

    } else if (ui->btnEmployes) {

        ui->btnEmployes->setChecked(true);

    }



    connect(ui->btnEmployes, &QPushButton::clicked, this, &MainWindow::showEmployesPage);

    connect(ui->btnAccueil, &QPushButton::clicked, this, &MainWindow::showDashboardHome);

    connect(ui->btnProduits, &QPushButton::clicked, this, [this](){

        if (auto *sw = mainStacked()) {

            if (auto *page = sw->findChild<QWidget*>("pageProduit", Qt::FindDirectChildrenOnly)) {

                sw->setCurrentWidget(page);

                ensureProduitModuleVisible();

                // Always open the product dashboard when entering the module

                goAffichage();

            }

        }

    });

    connect(ui->btnStock, &QPushButton::clicked, this, [this](){

        if (auto *sw = mainStacked()) {

            if (auto *page = sw->findChild<QWidget*>("pageStock", Qt::FindDirectChildrenOnly)) {

                sw->setCurrentWidget(page);

            }

        }

    });



    connect(ui->btnClient, &QPushButton::clicked, this, &MainWindow::on_btnClient_clicked);

    auto openGlobalStats = [this]() {

        if (auto *sw = mainStacked()) {

            if (auto *page = sw->findChild<QWidget*>("statistiques", Qt::FindDirectChildrenOnly)) {

                QWidget *current = sw->currentWidget();

                if (current && current != page)

                    globalStatsReturnPage = current;

                sw->setCurrentWidget(page);

            }

        }

    };



    connect(ui->btnStatistiques, &QPushButton::clicked, this, openGlobalStats);



    // --- MAINTENANCE INTEGRATION ---

    connect(ui->btnMaintenance, &QPushButton::clicked, this, [this](){

        if (auto *sw = mainStacked()) {

            if (auto *page = sw->findChild<QWidget*>("page_Maintenance_Tab", Qt::FindDirectChildrenOnly)) {

                sw->setCurrentWidget(page);

            }

        }



        // Apply styles with a small delay to ensure widgets are ready

        QTimer::singleShot(100, this, [this]() {

            forceApplySidebarStyles();

        });

    });



    // --- COMMANDES INTEGRATION ---

    if (QPushButton* btnCmd = this->findChild<QPushButton*>("btnCommandes")) {

        sidebarGroup->addButton(btnCmd);

        btnCmd->setCheckable(true);

        connect(btnCmd, &QPushButton::clicked, this, [this](){

            if (auto *sw = mainStacked()) {

                if (auto *page = sw->findChild<QWidget*>("pageCmdDashboard", Qt::FindDirectChildrenOnly)) {

                    sw->setCurrentWidget(page);

                }

            }

            // --- APPLY UNIFIED SIDEBAR STYLE (Longer Delay for Reliability) ---

            QTimer::singleShot(300, this, [this]() {

                this->forceApplySidebarStyles();

            });

        });

    }



    // Internal Navigation for Commandes

    auto safeConnectCmd = [this](const QString& btnName, const QString& targetPageName){

        QPushButton* btn = findChild<QPushButton*>(btnName); 

        QWidget* target = findChild<QWidget*>(targetPageName);

        if (btn && target && mainStacked()) {

             connect(btn, &QPushButton::clicked, this, [this, target](){

                 mainStacked()->setCurrentWidget(target);

             });

        }

    };

    

    safeConnectCmd("btnAddDashboard", "pageCmdAjout");

    safeConnectCmd("btnTempToModifier", "pageCmdModifier");

    safeConnectCmd("btnCancel_Mod", "pageCmdDashboard");

    safeConnectCmd("btnCancel_Mod_3", "pageCmdDashboard");



    // Commandes: in modifier page, "Annuler" must return to the Commandes list page.

    auto connectCmdModifierCancelToCommandes = [this](const QString &btnName) {

        QWidget *cmdModPage = findChild<QWidget*>("pageCmdModifier");

        if (!cmdModPage) return;

        QPushButton *btn = cmdModPage->findChild<QPushButton*>(btnName);

        if (!btn) return;



        connect(btn, &QPushButton::clicked, this, [this]() {

            if (auto *sw = mainStacked()) {

                if (auto *cmdDashboard = sw->findChild<QWidget*>("pageCmdDashboard", Qt::FindDirectChildrenOnly)) {

                    sw->setCurrentWidget(cmdDashboard);

                    if (auto *content = cmdDashboard->findChild<QStackedWidget*>("contentStack")) {

                        if (auto *pageCommandes = cmdDashboard->findChild<QWidget*>("pageCommandes")) {

                            content->setCurrentWidget(pageCommandes);

                        } else {

                            content->setCurrentIndex(1);

                        }

                    }

                }

            }

        });

    };

    connectCmdModifierCancelToCommandes("btnCancel_Mod1");

    connectCmdModifierCancelToCommandes("btnCancel_Mod");



    // Commandes dashboard table: add example row + actions (Modifier / Supprimer)

    if (ui->tableDashboard) {

        ui->tableDashboard->setColumnCount(10);

        ui->tableDashboard->setHorizontalHeaderLabels({

            "ID Commande", "Quantite", "Priorite", "Status", "Adresse",

            "Date", "Stock", "Date Livraison", "Prix Total", "Actions"

        });

        ui->tableDashboard->setRowCount(0);

        ui->tableDashboard->setWordWrap(false);

        ui->tableDashboard->verticalHeader()->setDefaultSectionSize(40);



        if (auto *header = ui->tableDashboard->horizontalHeader()) {

            header->setSectionResizeMode(QHeaderView::Stretch);

            header->setSectionResizeMode(9, QHeaderView::Fixed);

        }

        ui->tableDashboard->setColumnWidth(9, 190);



        auto reindexCmdActions = [this]() {

            if (!ui->tableDashboard) return;

            for (int r = 0; r < ui->tableDashboard->rowCount(); ++r) {

                if (QWidget *cell = ui->tableDashboard->cellWidget(r, 9)) {

                    const auto buttons = cell->findChildren<QPushButton*>();

                    for (QPushButton *b : buttons) {

                        b->setProperty("row", r);

                    }

                }

            }

        };



        auto installCmdActions = [this, &reindexCmdActions](int row) {

            if (!ui->tableDashboard) return;

            if (row < 0 || row >= ui->tableDashboard->rowCount()) return;



            QWidget *cell = new QWidget(ui->tableDashboard);

            auto *layout = new QHBoxLayout(cell);

            layout->setContentsMargins(4, 0, 4, 0);

            layout->setSpacing(6);

            layout->setAlignment(Qt::AlignCenter);



            auto *btnEdit = new QPushButton("Modifier", cell);

            auto *btnDelete = new QPushButton("Supprimer", cell);



            btnEdit->setStyleSheet("QPushButton { background-color: #3498db; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; }");

            btnDelete->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; }");

            btnEdit->setCursor(Qt::PointingHandCursor);

            btnDelete->setCursor(Qt::PointingHandCursor);

            btnEdit->setProperty("row", row);

            btnDelete->setProperty("row", row);



            layout->addWidget(btnEdit);

            layout->addWidget(btnDelete);



            ui->tableDashboard->setCellWidget(row, 9, cell);



            connect(btnEdit, &QPushButton::clicked, this, [this, btnEdit]() {

                if (auto *sw = mainStacked()) {

                    if (auto *page = sw->findChild<QWidget*>("pageCmdModifier", Qt::FindDirectChildrenOnly)) {

                        sw->setCurrentWidget(page);

                    }

                }

                if (ui->tableDashboard) {

                    const int currentRow = btnEdit->property("row").toInt();

                    if (currentRow >= 0 && currentRow < ui->tableDashboard->rowCount()) {

                        ui->tableDashboard->selectRow(currentRow);

                    }

                }

            });



            connect(btnDelete, &QPushButton::clicked, this, [this, btnDelete, reindexCmdActions]() {

                if (!ui->tableDashboard) return;

                const int currentRow = btnDelete->property("row").toInt();

                if (currentRow < 0 || currentRow >= ui->tableDashboard->rowCount()) return;



                if (QMessageBox::question(this, "Supprimer", "Voulez-vous supprimer cette commande ?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)

                    return;



                ui->tableDashboard->removeRow(currentRow);

                reindexCmdActions();

            });

        };



        const int row = ui->tableDashboard->rowCount();

        ui->tableDashboard->insertRow(row);

        ui->tableDashboard->setItem(row, 0, new QTableWidgetItem("CMD-001"));

        ui->tableDashboard->setItem(row, 1, new QTableWidgetItem("12"));

        ui->tableDashboard->setItem(row, 2, new QTableWidgetItem("Haute"));

        ui->tableDashboard->setItem(row, 3, new QTableWidgetItem("En preparation"));

        ui->tableDashboard->setItem(row, 4, new QTableWidgetItem("Zone Nord"));

        ui->tableDashboard->setItem(row, 5, new QTableWidgetItem("2026-02-12"));

        ui->tableDashboard->setItem(row, 6, new QTableWidgetItem("SmartBin X1"));

        ui->tableDashboard->setItem(row, 7, new QTableWidgetItem("2026-02-15"));

        ui->tableDashboard->setItem(row, 8, new QTableWidgetItem("5400 TND"));

        installCmdActions(row);

    }



    // Commandes table (pageCommandes): add example row + actions (Modifier / Supprimer)

    if (ui->tableProduits_2) {

        ui->tableProduits_2->setColumnCount(10);

        ui->tableProduits_2->setHorizontalHeaderLabels({

            "ID Commande", "Quantite", "Priorite", "Status", "Adresse",

            "Date", "Stock", "Date Livraison", "Prix Total", "Actions"

        });

        ui->tableProduits_2->setRowCount(0);

        ui->tableProduits_2->setWordWrap(false);

        ui->tableProduits_2->verticalHeader()->setDefaultSectionSize(40);



        if (auto *header = ui->tableProduits_2->horizontalHeader()) {

            header->setSectionResizeMode(QHeaderView::Stretch);

            header->setSectionResizeMode(9, QHeaderView::Fixed);

        }

        ui->tableProduits_2->setColumnWidth(9, 190);



        auto reindexCmdActions2 = [this]() {

            if (!ui->tableProduits_2) return;

            for (int r = 0; r < ui->tableProduits_2->rowCount(); ++r) {

                if (QWidget *cell = ui->tableProduits_2->cellWidget(r, 9)) {

                    const auto buttons = cell->findChildren<QPushButton*>();

                    for (QPushButton *b : buttons) {

                        b->setProperty("row", r);

                    }

                }

            }

        };



        auto installCmdActions2 = [this, &reindexCmdActions2](int row) {

            if (!ui->tableProduits_2) return;

            if (row < 0 || row >= ui->tableProduits_2->rowCount()) return;



            QWidget *cell = new QWidget(ui->tableProduits_2);

            auto *layout = new QHBoxLayout(cell);

            layout->setContentsMargins(4, 0, 4, 0);

            layout->setSpacing(6);

            layout->setAlignment(Qt::AlignCenter);



            auto *btnEdit = new QPushButton("Modifier", cell);

            auto *btnDelete = new QPushButton("Supprimer", cell);



            btnEdit->setStyleSheet("QPushButton { background-color: #3498db; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; }");

            btnDelete->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; }");

            btnEdit->setCursor(Qt::PointingHandCursor);

            btnDelete->setCursor(Qt::PointingHandCursor);

            btnEdit->setProperty("row", row);

            btnDelete->setProperty("row", row);



            layout->addWidget(btnEdit);

            layout->addWidget(btnDelete);



            ui->tableProduits_2->setCellWidget(row, 9, cell);



            connect(btnEdit, &QPushButton::clicked, this, [this, btnEdit]() {

                if (auto *sw = mainStacked()) {

                    if (auto *page = sw->findChild<QWidget*>("pageCmdModifier", Qt::FindDirectChildrenOnly)) {

                        sw->setCurrentWidget(page);

                    }

                }

                if (ui->tableProduits_2) {

                    const int currentRow = btnEdit->property("row").toInt();

                    if (currentRow >= 0 && currentRow < ui->tableProduits_2->rowCount()) {

                        ui->tableProduits_2->selectRow(currentRow);

                    }

                }

            });



            connect(btnDelete, &QPushButton::clicked, this, [this, btnDelete, reindexCmdActions2]() {

                if (!ui->tableProduits_2) return;

                const int currentRow = btnDelete->property("row").toInt();

                if (currentRow < 0 || currentRow >= ui->tableProduits_2->rowCount()) return;



                if (QMessageBox::question(this, "Supprimer", "Voulez-vous supprimer cette commande ?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)

                    return;



                ui->tableProduits_2->removeRow(currentRow);

                reindexCmdActions2();

            });

        };



        const int row = ui->tableProduits_2->rowCount();

        ui->tableProduits_2->insertRow(row);

        ui->tableProduits_2->setItem(row, 0, new QTableWidgetItem("CMD-002"));

        ui->tableProduits_2->setItem(row, 1, new QTableWidgetItem("8"));

        ui->tableProduits_2->setItem(row, 2, new QTableWidgetItem("Moyenne"));

        ui->tableProduits_2->setItem(row, 3, new QTableWidgetItem("Confirmation Pendante"));

        ui->tableProduits_2->setItem(row, 4, new QTableWidgetItem("Centre Ville"));

        ui->tableProduits_2->setItem(row, 5, new QTableWidgetItem("2026-02-12"));

        ui->tableProduits_2->setItem(row, 6, new QTableWidgetItem("Bac Intelligent"));

        ui->tableProduits_2->setItem(row, 7, new QTableWidgetItem("2026-02-16"));

        ui->tableProduits_2->setItem(row, 8, new QTableWidgetItem("3200 TND"));

        installCmdActions2(row);

        // --- Additional sample commandes ---
        struct CmdData {
            QString id, qty, priority, status, address, date, stock, delivDate, price;
        };
        QList<CmdData> extraCmds = {
            {"CMD-003", "5",  "Haute",   "En preparation",        "Zone Sud",       "2026-02-10", "SmartBin Pro",    "2026-02-14", "7800 TND"},
            {"CMD-004", "15", "Basse",   "Livraison en cours",    "Banlieue Est",   "2026-02-08", "EcoBac 200L",     "2026-02-18", "2100 TND"},
            {"CMD-005", "3",  "Haute",   "En preparation",        "Zone Industrielle","2026-02-13","BacSmart X2",    "2026-02-17", "9500 TND"},
            {"CMD-006", "20", "Moyenne", "Confirmation Pendante", "Centre Ville",   "2026-02-14", "Bac Intelligent", "2026-02-20", "4400 TND"},
            {"CMD-007", "7",  "Basse",   "Livree",                "Zone Nord",      "2026-01-28", "SmartBin X1",     "2026-02-02", "5600 TND"},
            {"CMD-008", "11", "Haute",   "Annulee",               "Quartier Menzah","2026-02-05", "EcoBac 200L",     "2026-02-09", "3300 TND"},
            {"CMD-009", "6",  "Moyenne", "En preparation",        "Zone Ouest",     "2026-02-15", "BacSmart X2",     "2026-02-19", "6200 TND"},
        };
        for (const auto &cmd : extraCmds) {
            const int r = ui->tableProduits_2->rowCount();
            ui->tableProduits_2->insertRow(r);
            ui->tableProduits_2->setItem(r, 0, new QTableWidgetItem(cmd.id));
            ui->tableProduits_2->setItem(r, 1, new QTableWidgetItem(cmd.qty));
            ui->tableProduits_2->setItem(r, 2, new QTableWidgetItem(cmd.priority));
            ui->tableProduits_2->setItem(r, 3, new QTableWidgetItem(cmd.status));
            ui->tableProduits_2->setItem(r, 4, new QTableWidgetItem(cmd.address));
            ui->tableProduits_2->setItem(r, 5, new QTableWidgetItem(cmd.date));
            ui->tableProduits_2->setItem(r, 6, new QTableWidgetItem(cmd.stock));
            ui->tableProduits_2->setItem(r, 7, new QTableWidgetItem(cmd.delivDate));
            ui->tableProduits_2->setItem(r, 8, new QTableWidgetItem(cmd.price));
            installCmdActions2(r);
        }

    }



    // Internal Navigation for Maintenance

    // Note: Assuming these objects exist in the UI now

    // If not, these lines should be commented out or verified after uic build.

    // Logic: Connect Dashboard buttons to Stack switching

    auto safeConnect = [this](QPushButton* btn, QWidget* targetPage){

        if (btn && targetPage && ui->stackedWidget_Maintenance) {

             connect(btn, &QPushButton::clicked, this, [this, targetPage](){

                 ui->stackedWidget_Maintenance->setCurrentWidget(targetPage);

             });

        }

    };



    safeConnect(ui->btnGotoAjout, ui->page_Maint_Ajout);

    if (ui->btnGotoModifier) {

        ui->btnGotoModifier->hide();

        ui->btnGotoModifier->setEnabled(false);

    }

    

    // Back / Cancel buttons -> Return to Dashboard

    QList<QPushButton*> backButtons = {

        ui->btnBack_Ajout, ui->btnBack_Modif, 

        ui->btnCancel_Add, ui->btnCancel_Mod

    };

    for(auto* btn : backButtons) {

        safeConnect(btn, ui->page_Maint_Dash);

    }

    // Clear photos when leaving the Add page
    auto clearPhotos = [this]() {
        m_photoAvantPath.clear();
        m_photoApresPath.clear();
        if (ui->lblImgPreview_Add) {
            ui->lblImgPreview_Add->setPixmap(QPixmap());
            ui->lblImgPreview_Add->setText("Cliquez ou déposez vos images ici pour les télécharger");
        }
        if (ui->lblImgPreview2_Add) {
            ui->lblImgPreview2_Add->setPixmap(QPixmap());
            ui->lblImgPreview2_Add->setText("Cliquez ou déposez vos images ici pour les télécharger");
        }
    };
    if (ui->btnCancel_Add) connect(ui->btnCancel_Add, &QPushButton::clicked, this, clearPhotos);
    if (ui->btnBack_Ajout) connect(ui->btnBack_Ajout, &QPushButton::clicked, this, clearPhotos);

    // Clear photo when leaving the Modify page
    auto clearModPhoto = [this]() {
        m_photoModPath.clear();
        if (ui->lblImgPreview_Mod) {
            ui->lblImgPreview_Mod->setPixmap(QPixmap());
            ui->lblImgPreview_Mod->setText(QString::fromUtf8("📷  Déposez vos images AVANT / APRÈS ici"));
        }
    };
    if (ui->btnCancel_Mod) connect(ui->btnCancel_Mod, &QPushButton::clicked, this, clearModPhoto);
    if (ui->btnBack_Modif) connect(ui->btnBack_Modif, &QPushButton::clicked, this, clearModPhoto);



    // --- REMPLISSAGE DU TABLEAU ---

    if (ui->tableEmployes) {

        ui->tableEmployes->setRowCount(0);

        ui->tableEmployes->setColumnCount(5);

        ui->tableEmployes->setHorizontalHeaderLabels({"MATRICULE", "NOM COMPLET", "SPECIALITE", "DISPONIBILITE", "ACTIONS"});

        ui->tableEmployes->setWordWrap(false);

        ui->tableEmployes->verticalHeader()->setDefaultSectionSize(40);



        if (auto *header = ui->tableEmployes->horizontalHeader()) {

            header->setStretchLastSection(false);

            header->setMinimumSectionSize(90);

            header->setSectionResizeMode(0, QHeaderView::ResizeToContents);

            header->setSectionResizeMode(1, QHeaderView::Stretch);

            header->setSectionResizeMode(2, QHeaderView::Stretch);

            header->setSectionResizeMode(3, QHeaderView::Stretch);

            header->setSectionResizeMode(4, QHeaderView::Fixed);

        }

        ui->tableEmployes->setColumnWidth(4, 220);


    refreshEmployes();
    }

    if (ui->txtEmailAjout) {
        ui->txtEmailAjout->setValidator(new QRegularExpressionValidator(emailRegex(), ui->txtEmailAjout));
    }
    if (ui->txtEmailModif) {
        ui->txtEmailModif->setValidator(new QRegularExpressionValidator(emailRegex(), ui->txtEmailModif));
    }



    // Connexions

    connect(ui->btnAnalyser, &QPushButton::clicked, this, &MainWindow::on_btnAnalyser_clicked);

    connect(ui->btnSimulerBadge, &QPushButton::clicked, this, &MainWindow::on_btnSimulerBadge_clicked);

    connect(ui->btnNouveau, &QPushButton::clicked, this, &MainWindow::on_btnNouveau_clicked);

    connect(ui->btnAnnuler_Ajout, &QPushButton::clicked, this, &MainWindow::on_btnAnnuler_Ajout_clicked);

    connect(ui->btnAnnuler, &QPushButton::clicked, this, &MainWindow::showEmployesPage);

    connect(ui->btnDelete, &QPushButton::clicked, this, &MainWindow::on_btnSupprimer_clicked);

    connect(ui->btnFichePaie, &QPushButton::clicked, this, &MainWindow::on_btnFichePaie_clicked);



    // --- NAVIGATION DASHBOARD (ACTIONS RAPIDES) ---

    connect(ui->btnGoMission, &QPushButton::clicked, this, [this](){

        if (auto *sw = mainStacked()) {

            if (auto *page = sw->findChild<QWidget*>("mission", Qt::FindDirectChildrenOnly)) {

                sw->setCurrentWidget(page);

            }

        }

    });

    connect(ui->btnGoPointage, &QPushButton::clicked, this, [this](){

        if (auto *sw = mainStacked()) {

            if (auto *page = sw->findChild<QWidget*>("pointage", Qt::FindDirectChildrenOnly)) {

                sw->setCurrentWidget(page);

            }

        }

    });

    connect(ui->btnGoStats, &QPushButton::clicked, this, openGlobalStats);

    auto connectToGlobalStats = [this](QPushButton *btn) {

        if (!btn) return;

        connect(btn, &QPushButton::clicked, this, [this]() {

            if (auto *sw = mainStacked()) {

                if (auto *page = sw->findChild<QWidget*>("statistiques", Qt::FindDirectChildrenOnly)) {

                    QWidget *current = sw->currentWidget();

                    if (current && current != page)

                        globalStatsReturnPage = current;

                    sw->setCurrentWidget(page);

                }

            }

        });

    };

    connectToGlobalStats(findChild<QPushButton*>("btnGoStats_Stock"));

    connectToGlobalStats(findChild<QPushButton*>("btnGoStats_Maint"));

    // btnGoStats_Client is connected separately to client-specific stats

    connectToGlobalStats(findChild<QPushButton*>("btnGoStats_Cmd"));



    // --- BOUTONS ANNULER / RETOUR ---

    connect(ui->btnAnnulerMission, &QPushButton::clicked, this, &MainWindow::showEmployesPage);

    connect(ui->btnAnnulerPointage, &QPushButton::clicked, this, &MainWindow::showEmployesPage);

    connect(ui->btnAnnulerStats, &QPushButton::clicked, this, [this]() {

        if (auto *sw = mainStacked()) {

            QWidget *statsPage = sw->findChild<QWidget*>("statistiques", Qt::FindDirectChildrenOnly);

            if (globalStatsReturnPage && globalStatsReturnPage != statsPage) {

                sw->setCurrentWidget(globalStatsReturnPage);

                return;

            }

        }

        showEmployesPage();

    });



    // Connexion ComboBox Projet Stats

    connect(ui->cbProjetStats, &QComboBox::currentTextChanged, this, &MainWindow::updateTaskChart);



    // --- SLIDERS DYNAMIQUES (SALAIRE) ---

    connect(ui->sliderSalaire_Ajout, &QSlider::valueChanged, [=](int value){

        ui->lblValSalaire_Ajout->setText(QString::number(value) + " DT");

    });

    ui->lblValSalaire_Ajout->setText(QString::number(ui->sliderSalaire_Ajout->value()) + " DT");



    connect(ui->sliderSalaire_Modif, &QSlider::valueChanged, [=](int value){

        ui->lblValSalaire_Modif->setText(QString::number(value) + " DT");

    });

    ui->lblValSalaire_Modif->setText(QString::number(ui->sliderSalaire_Modif->value()) + " DT");



    // --- CLIENT INITIALIZATION (from DB) ---
    if (ui->tableWidget_Client) {
        ui->tableWidget_Client->setColumnCount(7);
        QStringList headers = { "Matricule", "Nom", "Email", "Type Contrat", "Statut Paiement", "", "Actions" };
        ui->tableWidget_Client->setHorizontalHeaderLabels(headers);
        ui->tableWidget_Client->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableWidget_Client->setColumnHidden(5, true); // hidden column for internal ID
        refreshClients();
    }

    if (ui->btn_save_ajouter) connect(ui->btn_save_ajouter, &QPushButton::clicked, this, &MainWindow::on_btn_ajouter_client_clicked);

    if (ui->btn_annuler_ajouter) connect(ui->btn_annuler_ajouter, &QPushButton::clicked, this, &MainWindow::on_btn_annuler_client_clicked);

    if (ui->btn_save_modifier) connect(ui->btn_save_modifier, &QPushButton::clicked, this, &MainWindow::on_btn_modifier_client_clicked);

    if (ui->btn_annuler_modifier) connect(ui->btn_annuler_modifier, &QPushButton::clicked, this, &MainWindow::on_btn_annuler_client_clicked);

    if (ui->btnNouveau_Client) connect(ui->btnNouveau_Client, &QPushButton::clicked, this, &MainWindow::on_btnNouveau_client_clicked);

    // Search & Filter
    if (ui->recherche) connect(ui->recherche, &QLineEdit::textChanged, this, &MainWindow::filterClients);
    if (ui->cbTrier) connect(ui->cbTrier, &QComboBox::currentTextChanged, this, &MainWindow::filterClients);

    // Export PDF and Stats buttons
    if (ui->exportclient) connect(ui->exportclient, &QPushButton::clicked, this, &MainWindow::exportClientPdf);
    if (ui->btnGoStats_Client) connect(ui->btnGoStats_Client, &QPushButton::clicked, this, &MainWindow::showClientStats);
    if (ui->btnRetour_stats_client) connect(ui->btnRetour_stats_client, &QPushButton::clicked, this, [this]() {
        if (ui->stackedWidget_Client) ui->stackedWidget_Client->setCurrentIndex(0);
    });



    setupStatistics();

    setupAccueilDashboard();

    setupProduitModule();

    setupEmployeModule();

    setupStockModule();

    setupClientCardViewContainer();

    setupMaintenanceModule();

    setupMaintCardViewContainer();

    setupCmdCardViewContainer();

    setupCommandesModule();

    applyUnifiedTopBarStyle();

    forceApplySidebarStyles();

    showDashboardHome();

}



MainWindow::~MainWindow()

{

    delete ui;

}


void MainWindow::refreshEmployes()
{
    if (!ui->tableEmployes) return;

    ui->tableEmployes->setRowCount(0);

    QSqlQueryModel *model = Etmp.afficher();
    if (!model) {
        QMessageBox::critical(this, "Erreur SQL", "Impossible de charger les employes.");
        return;
    }

    if (model->lastError().isValid()) {
        showFriendlySqlError(this, "charger les employes", model->lastError().text());
        delete model;
        return;
    }

    for (int i = 0; i < model->rowCount(); ++i) {
        int id_emp = model->data(model->index(i, 0)).toInt();
        QString matricule = model->data(model->index(i, 1)).toString();
        QString cin = model->data(model->index(i, 2)).toString().trimmed();
        QString nom = model->data(model->index(i, 3)).toString();
        QString email = model->data(model->index(i, 4)).toString().trimmed();
        QString specialite = model->data(model->index(i, 5)).toString();
        QString dispoDb = model->data(model->index(i, 6)).toString().trimmed();

        QString dispoUi = dispoDb;
        if (dispoDb.compare("DISPONIBLE", Qt::CaseInsensitive) == 0) {
            dispoUi = "Disponible";
        } else if (dispoDb.compare("INDISPONIBLE", Qt::CaseInsensitive) == 0) {
            dispoUi = "Indisponible";
        }

        int row = ui->tableEmployes->rowCount();
        ui->tableEmployes->insertRow(row);

        auto *matItem = new QTableWidgetItem(matricule);
        matItem->setData(EMP_ROLE_ID, id_emp);
        matItem->setData(EMP_ROLE_EMAIL, email);
        matItem->setData(EMP_ROLE_CIN, cin);
        matItem->setData(EMP_ROLE_SALAIRE, 1200);
        matItem->setData(EMP_ROLE_PERF, 0);

        ui->tableEmployes->setItem(row, 0, matItem);
        ui->tableEmployes->setItem(row, 1, new QTableWidgetItem(nom));
        ui->tableEmployes->setItem(row, 2, new QTableWidgetItem(specialite));
        ui->tableEmployes->setItem(row, 3, new QTableWidgetItem(dispoUi));

        installEmployeActionButtonsForRow(row); // garde ton système de boutons
    }

    delete model;
}
// --- NAVIGATION ---

void MainWindow::on_btnNouveau_clicked()

{

    if (auto *sw = mainStacked()) {

        if (auto *page = sw->findChild<QWidget*>("ajout", Qt::FindDirectChildrenOnly)) {

            sw->setCurrentWidget(page);

        } else {

            sw->setCurrentIndex(2);

        }

    }



    // --- STYLE SIDEBAR & CLEAN LABELS (Employee Ajout) ---

    QTimer::singleShot(100, this, [this]() {

        if (QWidget *side = this->findChild<QWidget*>("sideAjout")) {

            side->setStyleSheet("QWidget#sideAjout { background-color: #ffffff; border-radius: 20px; border: 1px solid #e2e8f0; }");

            QList<QLabel*> labels = side->findChildren<QLabel*>();

            for (QLabel *lbl : labels) {

                lbl->setStyleSheet(lbl->styleSheet() + " background: transparent; color: #0f2b4c; border: none;");

            }

        }

        // Remove gray background from all labels in the page

        if (auto *page = mainStacked() ? mainStacked()->currentWidget() : nullptr) {

            QList<QLabel*> allLabels = page->findChildren<QLabel*>();

            for (QLabel *lbl : allLabels) {

                lbl->setStyleSheet(lbl->styleSheet() + " background: transparent; border: none;");

            }

        }

    });

}



void MainWindow::on_btnAnnuler_Ajout_clicked()

{

    showEmployesPage();

}



void MainWindow::installEmployeActionButtonsForRow(int row)

{

    if (!ui->tableEmployes) return;

    if (row < 0 || row >= ui->tableEmployes->rowCount()) return;



    if (QWidget *existing = ui->tableEmployes->cellWidget(row, 4)) {

        const auto buttons = existing->findChildren<QPushButton*>();

        for (auto *b : buttons)

            b->setProperty("row", row);

        return;

    }



    QWidget *pWidget = new QWidget(ui->tableEmployes);

    QPushButton *btnEdit = new QPushButton("Modifier", pWidget);

    btnEdit->setCursor(Qt::PointingHandCursor);

    btnEdit->setStyleSheet("QPushButton { background-color: #3498db; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; }");

    btnEdit->setMinimumWidth(84);

    btnEdit->setProperty("row", row);



    QPushButton *btnDelete = new QPushButton("Supprimer", pWidget);

    btnDelete->setCursor(Qt::PointingHandCursor);

    btnDelete->setStyleSheet(

        "QPushButton { background-color: #e74c3c; color: white; border-radius: 4px; padding: 4px 10px; font-weight: bold; }"

        "QPushButton:hover { background-color: #cf3e2e; }"

    );

    btnDelete->setMinimumWidth(96);

    btnDelete->setProperty("row", row);



    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::on_btnModifier_clicked);

    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::on_btnSupprimer_clicked);



    QHBoxLayout *pLayout = new QHBoxLayout(pWidget);

    pLayout->addWidget(btnEdit);

    pLayout->addWidget(btnDelete);

    pLayout->setSpacing(8);

    pLayout->setAlignment(Qt::AlignCenter);

    pLayout->setContentsMargins(0, 0, 0, 0);

    pWidget->setLayout(pLayout);



    ui->tableEmployes->setCellWidget(row, 4, pWidget);

}



void MainWindow::refreshEmployeActionButtons()

{

    if (!ui->tableEmployes) return;

    for (int row = 0; row < ui->tableEmployes->rowCount(); ++row)

        installEmployeActionButtonsForRow(row);

}



void MainWindow::on_btnAjouter_clicked()

{

    if (!ui->tableEmployes) return;



    const QString nom = ui->txtNom_Ajout ? ui->txtNom_Ajout->text().trimmed() : QString();

    if (nom.isEmpty()) {

        QMessageBox::warning(this, "Ajout", "Le nom est obligatoire.");

        return;

    }



    int maxId = 0;

    for (int r = 0; r < ui->tableEmployes->rowCount(); ++r) {

        if (auto *item = ui->tableEmployes->item(r, 0)) {

            const QString matricule = item->text().trimmed();

            const int dashPos = matricule.lastIndexOf('-');

            const QString numericPart = (dashPos >= 0) ? matricule.mid(dashPos + 1) : matricule;

            bool ok = false;

            const int id = numericPart.toInt(&ok);

            if (ok) maxId = qMax(maxId, id);

        }

    }

    const QString matricule = QString("EMP-%1").arg(maxId + 1, 3, 10, QChar('0'));



    const QString specialite = ui->cbSpecialite_Ajout ? ui->cbSpecialite_Ajout->currentText().trimmed() : QString();

    const QString statut = ui->cbStatut_Ajout ? ui->cbStatut_Ajout->currentText().trimmed() : QString();

    const QString email = ui->txtEmailAjout ? ui->txtEmailAjout->text().trimmed() : QString();

    if (email.isEmpty()) {
        QMessageBox::warning(this, "Ajout", "L'email est obligatoire.");
        return;
    }

    if (!isValidEmail(email)) {
        QMessageBox::warning(this, "Ajout", "Format d'email invalide. Exemple: nom@domaine.com");
        return;
    }

    const QString cin = ui->txtCIN_Ajout ? ui->txtCIN_Ajout->text().trimmed() : QString();

    const QString disponibiliteDb = toDbDisponibilite(statut);

    Etmp.setIdEmp(0);
    Etmp.setMatricule(matricule);
    Etmp.setCin(cin);
    Etmp.setNom(nom);
    Etmp.setEmail(email);
    Etmp.setSpecialite(specialite);
    Etmp.setDisponibilite(disponibiliteDb);
    if (!Etmp.ajouter()) {
        showFriendlySqlError(this, "ajouter l'employe", Etmp.lastError());
        return;
    }

    refreshEmployes();

    int insertedRow = -1;
    for (int r = 0; r < ui->tableEmployes->rowCount(); ++r) {
        if (auto *item = ui->tableEmployes->item(r, 0)) {
            if (item->text().trimmed() == matricule) {
                insertedRow = r;
                break;
            }
        }
    }

    if (insertedRow >= 0) {
        currentEmployeRow = insertedRow;
        ui->tableEmployes->selectRow(insertedRow);
    }



    if (ui->txtNom_Ajout) ui->txtNom_Ajout->clear();

    if (ui->txtEmailAjout) ui->txtEmailAjout->clear();

    if (ui->txtCIN_Ajout) ui->txtCIN_Ajout->clear();

    if (ui->cbSpecialite_Ajout && ui->cbSpecialite_Ajout->count() > 0) ui->cbSpecialite_Ajout->setCurrentIndex(0);

    if (ui->cbStatut_Ajout && ui->cbStatut_Ajout->count() > 0) ui->cbStatut_Ajout->setCurrentIndex(0);

    if (ui->sliderSalaire_Ajout) {

        const int resetSalaire = qMax(ui->sliderSalaire_Ajout->minimum(), qMin(ui->sliderSalaire_Ajout->maximum(), 1200));

        ui->sliderSalaire_Ajout->setValue(resetSalaire);

    }

    if (m_isEmpCardView)
        refreshEmpCardView();



    showEmployesPage();

}



void MainWindow::on_btnModifier_clicked()

{

    int row = -1;

    if (auto *btn = qobject_cast<QPushButton*>(sender())) {

        const QVariant rowProp = btn->property("row");

        if (rowProp.isValid())

            row = rowProp.toInt();

    }



    if (row < 0 && ui->tableEmployes)

        row = ui->tableEmployes->currentRow();



    if (row < 0 && ui->tableEmployes) {

        if (auto *btn = qobject_cast<QPushButton*>(sender())) {

            for (int r = 0; r < ui->tableEmployes->rowCount(); ++r) {

                QWidget *cell = ui->tableEmployes->cellWidget(r, 4);

                if (cell && (cell == btn->parentWidget() || cell->isAncestorOf(btn))) {

                    row = r;

                    break;

                }

            }

        }

    }



    auto cellText = [this, row](int col) -> QString {

        if (!ui->tableEmployes) return QString();

        if (row < 0 || row >= ui->tableEmployes->rowCount()) return QString();

        if (auto *item = ui->tableEmployes->item(row, col))

            return item->text().trimmed();

        return QString();

    };



    auto normalizeText = [](QString value) -> QString {

        value = value.normalized(QString::NormalizationForm_D).toLower().trimmed();

        QString normalized;

        normalized.reserve(value.size());

        for (QChar ch : value) {

            const QChar::Category category = ch.category();

            if (category == QChar::Mark_NonSpacing ||

                category == QChar::Mark_SpacingCombining ||

                category == QChar::Mark_Enclosing) {

                continue;

            }

            if (ch.isLetterOrNumber())

                normalized.append(ch);

        }

        return normalized;

    };



    auto setComboValue = [&](QComboBox *combo, const QString &value) {

        if (!combo || value.trimmed().isEmpty()) return;



        int idx = combo->findText(value, Qt::MatchFixedString);

        if (idx < 0)

            idx = combo->findText(value, Qt::MatchCaseSensitive | Qt::MatchContains);

        if (idx < 0)

            idx = combo->findText(value, Qt::MatchContains);



        if (idx < 0) {

            const QString wanted = normalizeText(value);

            for (int i = 0; i < combo->count(); ++i) {

                if (normalizeText(combo->itemText(i)) == wanted) {

                    idx = i;

                    break;

                }

            }

        }



        if (idx < 0) {

            const QString wanted = normalizeText(value);

            for (int i = 0; i < combo->count(); ++i) {

                const QString current = normalizeText(combo->itemText(i));

                if (current.contains(wanted) || wanted.contains(current)) {

                    idx = i;

                    break;

                }

            }

        }



        if (idx >= 0)

            combo->setCurrentIndex(idx);

    };



    if (row >= 0 && ui->tableEmployes) {

        currentEmployeRow = row;

        ui->tableEmployes->selectRow(row);



        const QString matricule = cellText(0);

        const QString nom = cellText(1);

        const QString specialite = cellText(2);

        const QString statut = cellText(3);



        if (ui->txtMatricule) ui->txtMatricule->setText(matricule);

        if (ui->txtNom) ui->txtNom->setText(nom);

        setComboValue(ui->cbSpecialite, specialite);

        if (ui->cbStatut) {
            const QString statusNorm = normalizedStatus(statut);
            int idx = -1;
            if (statusNorm.contains("indispon")) {
                idx = ui->cbStatut->findText("En intervention", Qt::MatchContains);
                if (idx < 0) idx = ui->cbStatut->findText("Conge", Qt::MatchContains);
            } else {
                idx = ui->cbStatut->findText("Disponible", Qt::MatchContains);
            }
            if (idx < 0 && ui->cbStatut->count() > 0) idx = 0;
            if (idx >= 0) ui->cbStatut->setCurrentIndex(idx);
        }



        if (auto *matItem = ui->tableEmployes->item(row, 0)) {

            const QString email = matItem->data(EMP_ROLE_EMAIL).toString().trimmed();

            const QString cin = matItem->data(EMP_ROLE_CIN).toString().trimmed();

            const QVariant salaireData = matItem->data(EMP_ROLE_SALAIRE);



            if (ui->txtEmailModif) ui->txtEmailModif->setText(email);

            if (ui->txtCIN_Modif) ui->txtCIN_Modif->setText(cin);



            if (ui->sliderSalaire_Modif) {

                int salaire = 1200;

                if (salaireData.isValid()) {

                    salaire = salaireData.toInt();

                }

                salaire = qMax(ui->sliderSalaire_Modif->minimum(),

                               qMin(ui->sliderSalaire_Modif->maximum(), salaire));

                ui->sliderSalaire_Modif->setValue(salaire);

            }

        }

    } else {

        currentEmployeRow = -1;

    }



    if (auto *sw = mainStacked()) {

        if (auto *page = sw->findChild<QWidget*>("modification", Qt::FindDirectChildrenOnly)) {

            sw->setCurrentWidget(page);

        } else {

            sw->setCurrentIndex(1);

        }

    }



    // --- STYLE SIDEBAR & CLEAN LABELS (Employee Modification) ---

    QTimer::singleShot(100, this, [this]() {

        if (QWidget *side = this->findChild<QWidget*>("sideMod")) {

            side->setStyleSheet("QWidget#sideMod { background-color: #ffffff; border-radius: 20px; border: 1px solid #e2e8f0; }");

            QList<QLabel*> labels = side->findChildren<QLabel*>();

            for (QLabel *lbl : labels) {

                lbl->setStyleSheet(lbl->styleSheet() + " background: transparent; color: #0f2b4c; border: none;");

            }

        }

        // Remove gray background from all labels in the page

        if (auto *page = mainStacked() ? mainStacked()->currentWidget() : nullptr) {

            QList<QLabel*> allLabels = page->findChildren<QLabel*>();

            for (QLabel *lbl : allLabels) {

                lbl->setStyleSheet(lbl->styleSheet() + " background: transparent; border: none;");

            }

        }

    });

}



void MainWindow::on_btnSave_clicked()

{

    if (!ui->tableEmployes)
        return;

    int row = currentEmployeRow;

    if (row < 0 || row >= ui->tableEmployes->rowCount()) {
        // Fallback: find row by matricule in case the tracked row was reset.
        const QString matriculeFromForm = ui->txtMatricule ? ui->txtMatricule->text().trimmed() : QString();
        if (!matriculeFromForm.isEmpty()) {
            for (int r = 0; r < ui->tableEmployes->rowCount(); ++r) {
                if (auto *item = ui->tableEmployes->item(r, 0)) {
                    if (item->text().trimmed() == matriculeFromForm) {
                        row = r;
                        break;
                    }
                }
            }
        }
    }

    if (row < 0 || row >= ui->tableEmployes->rowCount())
        return;

    const QString matricule = ui->txtMatricule ? ui->txtMatricule->text().trimmed() : QString();
    const QString nom = ui->txtNom ? ui->txtNom->text().trimmed() : QString();
    const QString specialite = ui->cbSpecialite ? ui->cbSpecialite->currentText().trimmed() : QString();
    const QString statut = ui->cbStatut ? ui->cbStatut->currentText().trimmed() : QString();
    const QString email = ui->txtEmailModif ? ui->txtEmailModif->text().trimmed() : QString();
    const QString cin = ui->txtCIN_Modif ? ui->txtCIN_Modif->text().trimmed() : QString();
    const int salaire = ui->sliderSalaire_Modif ? ui->sliderSalaire_Modif->value() : 1200;

    if (nom.isEmpty()) {
        QMessageBox::warning(this, "Modification", "Le nom est obligatoire.");
        return;
    }

    if (email.isEmpty()) {
        QMessageBox::warning(this, "Modification", "L'email est obligatoire.");
        return;
    }

    if (!isValidEmail(email)) {
        QMessageBox::warning(this, "Modification", "Format d'email invalide. Exemple: nom@domaine.com");
        return;
    }

    int idEmp = -1;
    int perf = 0;
    if (auto *matItem = ui->tableEmployes->item(row, 0)) {
        idEmp = matItem->data(EMP_ROLE_ID).toInt();
        perf = matItem->data(EMP_ROLE_PERF).toInt();
    }

    if (idEmp <= 0 && !matricule.isEmpty()) {
        int foundId = -1;
        if (Etmp.findIdByMatricule(matricule, foundId)) {
            idEmp = foundId;
        }
    }

    if (idEmp <= 0) {
        QMessageBox::warning(this, "Modification", "ID employe introuvable, modification annulee.");
        return;
    }

    const QString disponibiliteDb = toDbDisponibilite(statut);

    Etmp.setIdEmp(idEmp);
    Etmp.setMatricule(matricule);
    Etmp.setCin(cin);
    Etmp.setNom(nom);
    Etmp.setEmail(email);
    Etmp.setSpecialite(specialite);
    Etmp.setDisponibilite(disponibiliteDb);
    if (!Etmp.modifier()) {
        showFriendlySqlError(this, "modifier l'employe", Etmp.lastError());
        return;
    }

    refreshEmployes();

    int updatedRow = -1;
    for (int r = 0; r < ui->tableEmployes->rowCount(); ++r) {
        if (auto *item = ui->tableEmployes->item(r, 0)) {
            if (item->data(EMP_ROLE_ID).toInt() == idEmp) {
                updatedRow = r;
                item->setData(EMP_ROLE_SALAIRE, salaire);
                item->setData(EMP_ROLE_PERF, perf);
                break;
            }
        }
    }

    if (updatedRow >= 0) {
        ui->tableEmployes->selectRow(updatedRow);
        currentEmployeRow = updatedRow;
    } else {
        currentEmployeRow = -1;
    }

    if (m_isEmpCardView)
        refreshEmpCardView();

    showEmployesPage();

}



// --- SUPPRESSION ---

void MainWindow::on_btnSupprimer_clicked()

{

    if (!ui->tableEmployes) return;



    int row = -1;

    if (auto *btn = qobject_cast<QPushButton*>(sender())) {

        const QVariant rowProp = btn->property("row");

        if (rowProp.isValid())

            row = rowProp.toInt();

    }



    if ((row < 0 || row >= ui->tableEmployes->rowCount()) && currentEmployeRow >= 0)

        row = currentEmployeRow;



    if ((row < 0 || row >= ui->tableEmployes->rowCount()) && ui->tableEmployes->currentRow() >= 0)

        row = ui->tableEmployes->currentRow();



    if (row < 0 || row >= ui->tableEmployes->rowCount()) {

        const QString matriculeForm = ui->txtMatricule ? ui->txtMatricule->text().trimmed() : QString();

        if (!matriculeForm.isEmpty()) {

            for (int r = 0; r < ui->tableEmployes->rowCount(); ++r) {

                if (auto *it = ui->tableEmployes->item(r, 0)) {

                    if (it->text().trimmed() == matriculeForm) {

                        row = r;

                        break;

                    }

                }

            }

        }

    }



    if (row < 0 || row >= ui->tableEmployes->rowCount())

        return;



    if (QMessageBox::question(this, "Supprimer", "Voulez-vous vraiment supprimer cet employé ?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)

        return;

    int idEmp = -1;
    if (auto *matItem = ui->tableEmployes->item(row, 0)) {
        idEmp = matItem->data(EMP_ROLE_ID).toInt();
    }

    if (idEmp <= 0) {
        QMessageBox::warning(this, "Suppression", "ID employe introuvable, suppression annulee.");
        return;
    }

    if (!Etmp.supprimer(idEmp)) {
        showFriendlySqlError(this, "supprimer l'employe", Etmp.lastError());
        return;
    }

    refreshEmployes();

    currentEmployeRow = -1;

    if (ui->tableEmployes->rowCount() > 0) {
        const int rowToSelect = qMax(0, qMin(row, ui->tableEmployes->rowCount() - 1));
        ui->tableEmployes->selectRow(rowToSelect);
    }

    if (m_isEmpCardView)
        refreshEmpCardView();



    showEmployesPage();

}



// --- MISSION IA ---

void MainWindow::on_btnAnalyser_clicked()

{

    ui->tableResultat->setRowCount(0);

    ui->tableResultat->insertRow(0);

    ui->tableResultat->setItem(0, 0, new QTableWidgetItem("EMP-001"));

    ui->tableResultat->setItem(0, 1, new QTableWidgetItem("Ali Ben Salah"));

    ui->tableResultat->setItem(0, 2, new QTableWidgetItem("🔧 Moteur Diesel"));

    QTableWidgetItem* score1 = new QTableWidgetItem("99%");

    score1->setForeground(QBrush(QColor("#27ae60")));

    ui->tableResultat->setItem(0, 3, score1);

}



// --- POINTAGE ---

void MainWindow::on_btnSimulerBadge_clicked()

{

    ui->lblStatutRFID->setText("✅ BADGE ACCEPTÉ");

    ui->lblStatutRFID->setStyleSheet("background-color: #2ecc71; color: white; font-size: 24px; font-weight: bold; border-radius: 10px; padding: 20px; border: 2px solid #27ae60;");



    int row = ui->tablePointage->rowCount();

    ui->tablePointage->insertRow(row);

    ui->tablePointage->setItem(row, 0, new QTableWidgetItem(QTime::currentTime().toString("HH:mm:ss")));

    ui->tablePointage->setItem(row, 1, new QTableWidgetItem("BADGE-123"));

    ui->tablePointage->setItem(row, 2, new QTableWidgetItem("Ali Ben Salah"));



    QTableWidgetItem* status = new QTableWidgetItem("PRÉSENT");

    status->setForeground(QBrush(QColor("#27ae60")));

    ui->tablePointage->setItem(row, 3, status);

}



void MainWindow::on_btnFichePaie_clicked()

{

    QInputDialog dialog(this);

    dialog.setWindowTitle("Fiche de Paie");

    dialog.setLabelText("Veuillez entrer le matricule de l'employé :");

    dialog.setTextValue("");

    dialog.setInputMode(QInputDialog::TextInput);

    dialog.resize(400, 200);



    dialog.setStyleSheet(

        "QDialog { background-color: white; }"

        "QLabel { color: #333333; font-weight: bold; font-size: 14px; }"

        "QLineEdit { border: 1px solid #ccc; border-radius: 5px; padding: 5px; color: black; }"

        "QPushButton { "

        "   background-color: #0f2b4c; "

        "   color: white; "

        "   border: none; "

        "   border-radius: 5px; "

        "   padding: 8px 15px; "

        "   font-weight: bold; "

        "   min-width: 70px; "

        "}"

        "QPushButton:hover { background-color: #1a4270; }"

    );



    if (dialog.exec() == QDialog::Accepted) {

        QString matricule = dialog.textValue();

        if (!matricule.isEmpty()) {

            QMessageBox::information(this, "Génération en cours",

                                     "✅ Génération de la fiche de paie pour le matricule : " + matricule);

        }

    }

}



void MainWindow::on_btnCommsSend_clicked()

{

    QLineEdit *input = findChild<QLineEdit*>("txtCommsInput");

    QListWidget *list = findChild<QListWidget*>("listComms");



    if (!input || !list)

        return;



    const QString message = input->text().trimmed();

    if (message.isEmpty())

        return;



    QListWidgetItem *item = new QListWidgetItem("Vous : " + message);

    item->setForeground(QColor("#27ae60"));

    list->addItem(item);

    input->clear();

}



void MainWindow::setupStatistics()

{

    // --- GRAPHIQUE 1 : Répartition des interventions par STATUT (Pie Chart) ---

    QPieSeries *statutSeries = new QPieSeries();

    QSqlQuery queryStatut;

    queryStatut.exec("SELECT STATUT, COUNT(*) FROM INTERVENTION GROUP BY STATUT ORDER BY STATUT");

    QStringList pieColors;

    pieColors << "#27ae60" << "#e67e22" << "#c0392b" << "#3498db" << "#9b59b6";

    int colorIdx = 0;

    while (queryStatut.next()) {

        QString statut = queryStatut.value(0).toString();

        int count = queryStatut.value(1).toInt();

        statutSeries->append(statut, count);

    }

    for (auto *slice : statutSeries->slices()) {

        slice->setLabelVisible(true);

        slice->setBrush(QColor(pieColors.at(colorIdx % pieColors.size())));

        slice->setLabelColor(Qt::black);

        colorIdx++;

    }

    if (statutSeries->slices().size() == 0) {

        statutSeries->append("Aucune donnée", 1);

        statutSeries->slices().at(0)->setLabelVisible(true);

        statutSeries->slices().at(0)->setBrush(QColor("#bdc3c7"));

    }



    QChart *statutChart = new QChart();

    statutChart->addSeries(statutSeries);

    statutChart->setTitle("Répartition des Interventions par Statut");

    statutChart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));

    statutChart->legend()->setAlignment(Qt::AlignBottom);

    statutChart->setAnimationOptions(QChart::SeriesAnimations);



    ui->chartViewAbsence->setChart(statutChart);

    ui->chartViewAbsence->setRenderHint(QPainter::Antialiasing);



    // --- GRAPHIQUE 2 : Durée totale (heures) par TYPE d'intervention (Bar Chart) ---

    QBarSet *setDuree = new QBarSet("Durée totale (h)");

    setDuree->setColor(QColor("#3498db"));

    QStringList typeCategories;

    double maxDuree = 0;



    QSqlQuery queryType;

    queryType.exec("SELECT TYPE, SUM(DUREE) FROM INTERVENTION GROUP BY TYPE ORDER BY TYPE");

    while (queryType.next()) {

        QString type = queryType.value(0).toString();

        double totalDuree = queryType.value(1).toDouble();

        typeCategories << type;

        *setDuree << totalDuree;

        if (totalDuree > maxDuree) maxDuree = totalDuree;

    }

    if (typeCategories.isEmpty()) {

        typeCategories << "Aucune donnée";

        *setDuree << 0;

    }



    QBarSeries *workloadSeries = new QBarSeries();

    workloadSeries->append(setDuree);



    QChart *workloadChart = new QChart();

    workloadChart->addSeries(workloadSeries);

    workloadChart->setTitle("Charge de Travail par Type d'Intervention");

    workloadChart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));

    workloadChart->setAnimationOptions(QChart::SeriesAnimations);

    workloadChart->legend()->setVisible(false);



    QBarCategoryAxis *axisX = new QBarCategoryAxis();

    axisX->append(typeCategories);

    workloadChart->addAxis(axisX, Qt::AlignBottom);

    workloadSeries->attachAxis(axisX);



    QValueAxis *axisY = new QValueAxis();

    axisY->setRange(0, maxDuree > 0 ? maxDuree * 1.2 : 10);

    axisY->setTitleText("Durée totale (heures)");

    workloadChart->addAxis(axisY, Qt::AlignLeft);

    workloadSeries->attachAxis(axisY);



    ui->chartViewWorkload->setChart(workloadChart);

    ui->chartViewWorkload->setRenderHint(QPainter::Antialiasing);



    // --- Remplir le ComboBox dynamiquement depuis les TYPES d'interventions ---

    ui->cbProjetStats->blockSignals(true);

    ui->cbProjetStats->clear();

    QSqlQuery queryTypes;

    queryTypes.exec("SELECT DISTINCT TYPE FROM INTERVENTION ORDER BY TYPE");

    while (queryTypes.next()) {

        ui->cbProjetStats->addItem(queryTypes.value(0).toString());

    }

    if (ui->cbProjetStats->count() == 0) {

        ui->cbProjetStats->addItem("Aucun type");

    }

    ui->cbProjetStats->blockSignals(false);



    updateTaskChart(ui->cbProjetStats->currentText());

}



void MainWindow::setupAccueilDashboard()

{

    QStackedWidget *sw = mainStacked();

    if (!sw)

        return;



    QWidget *existing = sw->findChild<QWidget*>("dashboardAccueil", Qt::FindDirectChildrenOnly);

    if (existing) {

        homeDashboardPage = existing;

    } else {

        QWidget *page = new QWidget();

        page->setObjectName("dashboardAccueil");



        auto *mainLayout = new QHBoxLayout(page);

        mainLayout->setContentsMargins(20, 20, 20, 20);

        mainLayout->setSpacing(16);



        auto *leftLayout = new QVBoxLayout();

        leftLayout->setSpacing(20);



        auto *title = new QLabel("Tableau de Bord");

        title->setStyleSheet("font-size: 28px; font-weight: 700; color: #34495e;");

        leftLayout->addWidget(title);



        // --- WELCOME CARD (HERO) ---

        auto *welcomeCard = new QFrame();

        welcomeCard->setObjectName("welcomeCard");

        welcomeCard->setFrameShape(QFrame::StyledPanel);

        welcomeCard->setStyleSheet(

            "#welcomeCard {"

            "    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0f2b4c, stop:1 #3498db);"

            "    border-radius: 15px;"

            "    color: white;"

            "}"

        );

        auto *welcomeLayout = new QVBoxLayout(welcomeCard);

        welcomeLayout->setContentsMargins(30, 30, 30, 30);

        

        auto *welcomeTitle = new QLabel("WasteGuard : L'Innovation au Service de la Ville Durable.");

        welcomeTitle->setStyleSheet("font-size: 22px; font-weight: 700; color: #ffffff; background: transparent;");

        auto *welcomeMsg = new QLabel("Nous optimisons la collecte des déchets urbains grâce à l'intelligence artificielle (IA) et aux objets connectés (IoT).");

        welcomeMsg->setWordWrap(true);

        welcomeMsg->setStyleSheet("font-size: 15px; font-weight: 400; color: #ecf0f1; margin-top: 10px; background: transparent;");

        

        welcomeLayout->addWidget(welcomeTitle);

        welcomeLayout->addWidget(welcomeMsg);

        leftLayout->addWidget(welcomeCard);



        // --- FINANCE CARDS ---

        auto *financeRow = new QHBoxLayout();

        financeRow->setSpacing(20);



        // CARD 1: CA MENSIEL

        auto *financeCard1 = new QFrame();

        financeCard1->setObjectName("financeCard");

        financeCard1->setStyleSheet(

            "#financeCard {"

            "    background-color: #ffffff;"

            "    border-radius: 15px;"

            "    border-left: 10px solid #2980b9;"

            "    border-right: 1px solid #dce1e6;"

            "    border-top: 1px solid #dce1e6;"

            "    border-bottom: 2px solid #dce1e6;"

            "}"

        );

        auto *f1 = new QVBoxLayout(financeCard1);

        f1->setContentsMargins(25, 25, 25, 25);

        

        QLabel *lblTitle1 = new QLabel("CHIFFRE D'AFFAIRES MENSUEL (CA)");

        lblTitle1->setStyleSheet("color: #7f8c8d; font-size: 12px; font-weight: 700; letter-spacing: 1px; text-transform: uppercase; background: transparent;");

        

        QLabel *lblValue1 = new QLabel("125 k TND");

        lblValue1->setStyleSheet("color: #2c3e50; font-size: 48px; font-weight: 800; margin-top: 5px; margin-bottom: 5px; background: transparent;");

        

        QLabel *lblDesc1 = new QLabel("Montant total des ventes (bacs intelligents + abonnements) ce mois-ci.");

        lblDesc1->setWordWrap(true);

        lblDesc1->setStyleSheet("color: #95a5a6; font-size: 13px; font-style: italic; background: transparent;");

        

        f1->addWidget(lblTitle1);

        f1->addWidget(lblValue1);

        f1->addWidget(lblDesc1);



        // CARD 2: MARGE NETTE

        auto *financeCard2 = new QFrame();

        financeCard2->setObjectName("financeCard_2");

        financeCard2->setStyleSheet(

            "#financeCard_2 {"

            "    background-color: #ffffff;"

            "    border-radius: 15px;"

            "    border-left: 10px solid #27ae60;"

            "    border-right: 1px solid #dce1e6;"

            "    border-top: 1px solid #dce1e6;"

            "    border-bottom: 2px solid #dce1e6;"

            "}"

        );

        auto *f2 = new QVBoxLayout(financeCard2);

        f2->setContentsMargins(25, 25, 25, 25);

        

        QLabel *lblTitle2 = new QLabel("MARGE BÉNÉFICIAIRE NETTE");

        lblTitle2->setStyleSheet("color: #7f8c8d; font-size: 12px; font-weight: 700; letter-spacing: 1px; text-transform: uppercase; background: transparent;");

        

        QLabel *lblValue2 = new QLabel("+ 24 %");

        lblValue2->setStyleSheet("color: #27ae60; font-size: 48px; font-weight: 800; margin-top: 5px; margin-bottom: 5px; background: transparent;");

        

        QLabel *lblDesc2 = new QLabel("Pourcentage de profit réel après coûts de production et salaires.");

        lblDesc2->setWordWrap(true);

        lblDesc2->setStyleSheet("color: #95a5a6; font-size: 13px; font-style: italic; background: transparent;");

        

        f2->addWidget(lblTitle2);

        f2->addWidget(lblValue2);

        f2->addWidget(lblDesc2);



        financeRow->addWidget(financeCard1);

        financeRow->addWidget(financeCard2);

        leftLayout->addLayout(financeRow);



        // --- CHARTS ROW ---

        auto *chartsRow = new QHBoxLayout();

        chartsRow->setSpacing(20);



        auto *teamsCard = new QFrame();

        teamsCard->setObjectName("hrChartCard");

        teamsCard->setStyleSheet("background-color: white; border-radius: 12px; border: 1px solid #dce1e6;");

        auto *teamsLayout = new QVBoxLayout(teamsCard);

        

        QLabel *lblTeamTitle = new QLabel("Gestion des Ressources Humaines");

        lblTeamTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: #2c3e50; margin-bottom: 10px; background: transparent;");

        teamsLayout->addWidget(lblTeamTitle);

        

        auto *teamsView = new QChartView();

        teamsView->setObjectName("chartViewTeams");

        teamsView->setMinimumHeight(250);

        teamsLayout->addWidget(teamsView);



        auto *prodCard = new QFrame();

        prodCard->setObjectName("productionChartCard");

        prodCard->setStyleSheet("background-color: white; border-radius: 12px; border: 1px solid #dce1e6;");

        auto *prodLayout = new QVBoxLayout(prodCard);

        

        QLabel *lblProdTitle = new QLabel("Production & Logistique");

        lblProdTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: #2c3e50; margin-bottom: 10px; background: transparent;");

        prodLayout->addWidget(lblProdTitle);

        

        auto *prodView = new QChartView();

        prodView->setObjectName("chartViewProduction");

        prodView->setMinimumHeight(250);

        prodLayout->addWidget(prodView);



        chartsRow->addWidget(teamsCard);

        chartsRow->addWidget(prodCard);

        leftLayout->addLayout(chartsRow);

        

        leftLayout->addStretch();



        // --- RIGHT SIDEBAR (COMMS) ---

        auto *rightCard = new QFrame();

        rightCard->setObjectName("commsCard");

        rightCard->setMinimumWidth(360);

        rightCard->setMaximumWidth(420);

        auto *rightLayout = new QVBoxLayout(rightCard);

        rightLayout->setContentsMargins(20, 20, 20, 20);

        rightLayout->setSpacing(15);



        auto *commsTitle = new QLabel("Communication Interne");

        commsTitle->setObjectName("lblCommsTitle");

        rightLayout->addWidget(commsTitle);

        

        auto *list = new QListWidget();

        list->setObjectName("listComms");

        list->setMinimumHeight(220);

        list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        list->setWordWrap(true);

        list->setTextElideMode(Qt::ElideNone);

        rightLayout->addWidget(list);



        auto *inputRow = new QHBoxLayout();

        auto *input = new QLineEdit();

        input->setObjectName("txtCommsInput");

        input->setPlaceholderText("Écrire un message...");

        input->setMinimumHeight(40);

        

        auto *btnSend = new QPushButton("Envoyer");

        btnSend->setObjectName("btnCommsSend");

        btnSend->setMinimumWidth(80);

        btnSend->setMinimumHeight(40);

        btnSend->setCursor(Qt::PointingHandCursor);

        

        inputRow->addWidget(input);

        inputRow->addWidget(btnSend);

        rightLayout->addLayout(inputRow);



        rightCard->setStyleSheet(

            "QFrame#commsCard {"

            " background-color: #ffffff;"

            " border: 1px solid #dce1e6;"

            " border-radius: 12px;"

            "}"

            "QLabel#lblCommsTitle {"

            " color: #0f2b4c;"

            " font-size: 18px;"

            " font-weight: 700;"

            " border: none;"

            " background: transparent;"

            "}"

            "QListWidget#listComms {"

            " background-color: #f8f9fa;"

            " color: #2c3e50;"

            " border: 1px solid #e9ecef;"

            " border-radius: 10px;"

            " padding: 10px;"

            " font-size: 13px;"

            "}"

            "QListWidget#listComms::item {"

            " padding: 8px;"

            " margin-bottom: 5px;"

            " border-bottom: 1px solid #e9ecef;"

            "}"

            "QLineEdit#txtCommsInput {"

            " background-color: #ffffff;"

            " color: #2c3e50;"

            " border: 2px solid #e9ecef;"

            " border-radius: 8px;"

            " padding: 0px 12px;"

            " font-size: 13px;"

            "}"

            "QLineEdit#txtCommsInput:focus {"

            " border: 2px solid #3498db;"

            "}"

            "QPushButton#btnCommsSend {"

            " background-color: #3498db;"

            " color: white;"

            " border: none;"

            " border-radius: 8px;"

            " font-weight: bold;"

            " font-size: 13px;"

            "}"

            "QPushButton#btnCommsSend:hover {"

            " background-color: #2980b9;"

            "}"

        );



        mainLayout->addLayout(leftLayout, 3);

        mainLayout->addWidget(rightCard, 1);



        homeDashboardPage = page;

        sw->insertWidget(0, page);

    }

    QPushButton *btnSend = findChild<QPushButton*>("btnCommsSend");

    QLineEdit *input = findChild<QLineEdit*>("txtCommsInput");

    QListWidget *list = findChild<QListWidget*>("listComms");



    if (btnSend) {

        btnSend->setVisible(true);

        btnSend->setEnabled(true);

        btnSend->setCursor(Qt::PointingHandCursor);

        btnSend->setMinimumWidth(110);

        btnSend->setMinimumHeight(36);

        btnSend->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        btnSend->setStyleSheet(

            "QPushButton#btnCommsSend {"

            " background-color: #0f2b4c;"

            " color: white;"

            " border: 1px solid #0b223c;"

            " border-radius: 8px;"

            " font-weight: 700;"

            " padding: 6px 14px;"

            "}"

            "QPushButton#btnCommsSend:hover { background-color: #1a4270; }"

            "QPushButton#btnCommsSend:pressed { background-color: #0b223c; }"

        );

        connect(btnSend, &QPushButton::clicked, this, &MainWindow::on_btnCommsSend_clicked, Qt::UniqueConnection);

    }

    if (input) {

        input->setMinimumHeight(36);

        connect(input, &QLineEdit::returnPressed, this, &MainWindow::on_btnCommsSend_clicked, Qt::UniqueConnection);

    }



    if (list && list->count() == 0) {

        QListWidgetItem *msg1 = new QListWidgetItem("Ahmed (Technique) : Intervention prévue Zone Nord à 15h.");

        msg1->setForeground(QColor("#1f2d3d"));

        list->addItem(msg1);



        QListWidgetItem *msg2 = new QListWidgetItem("Sarah (RH) : Réunion RH demain à 10h.");

        msg2->setForeground(QColor("#1f2d3d"));

        list->addItem(msg2);



        QListWidgetItem *systemAlert = new QListWidgetItem("Alerte Système : Maintenance serveur ce soir à 22h.");

        systemAlert->setForeground(QColor("#e67e22"));

        list->addItem(systemAlert);

    }



    setupDashboardCharts();

}



void MainWindow::setupDashboardCharts()

{

    QChartView *teamsView = findChild<QChartView*>("chartViewTeams");

    QChartView *productionView = findChild<QChartView*>("chartViewProduction");



    if (!teamsView || !productionView)

        return;



    QPieSeries *teamSeries = new QPieSeries();

    teamSeries->append("Disponibles (12)", 12);

    teamSeries->append("En Mission (8)", 8);

    teamSeries->append("Absents (3)", 3);

    teamSeries->setHoleSize(0.45);



    teamSeries->slices().at(0)->setBrush(QColor("#27ae60"));

    teamSeries->slices().at(1)->setBrush(QColor("#f39c12"));

    teamSeries->slices().at(2)->setBrush(QColor("#c0392b"));

    for (QPieSlice *slice : teamSeries->slices()) {

        slice->setLabelVisible(true);

        slice->setLabelColor(Qt::black);

    }



    QChart *teamChart = new QChart();

    teamChart->addSeries(teamSeries);

    teamChart->legend()->setAlignment(Qt::AlignBottom);

    teamChart->setAnimationOptions(QChart::SeriesAnimations);

    teamChart->setTitle("Disponibilité des équipes techniques");

    teamsView->setChart(teamChart);

    teamsView->setRenderHint(QPainter::Antialiasing);



    QBarSet *setRaw = new QBarSet("Matière Première (t)");

    *setRaw << 40 << 38 << 42 << 36;

    setRaw->setColor(QColor("#95a5a6"));



    QBarSet *setBins = new QBarSet("Bacs Assemblés");

    *setBins << 28 << 32 << 35 << 30;

    setBins->setColor(QColor("#3498db"));



    QBarSeries *prodSeries = new QBarSeries();

    prodSeries->append(setRaw);

    prodSeries->append(setBins);



    QChart *prodChart = new QChart();

    prodChart->addSeries(prodSeries);

    prodChart->setTitle("Suivi de fabrication (hebdomadaire)");

    prodChart->legend()->setAlignment(Qt::AlignBottom);

    prodChart->setAnimationOptions(QChart::SeriesAnimations);



    QStringList weeks;

    weeks << "S1" << "S2" << "S3" << "S4";

    QBarCategoryAxis *axisX = new QBarCategoryAxis();

    axisX->append(weeks);

    prodChart->addAxis(axisX, Qt::AlignBottom);

    prodSeries->attachAxis(axisX);



    QValueAxis *axisY = new QValueAxis();

    axisY->setRange(0, 50);

    axisY->setTitleText("Volume / semaine");

    prodChart->addAxis(axisY, Qt::AlignLeft);

    prodSeries->attachAxis(axisY);



    productionView->setChart(prodChart);

    productionView->setRenderHint(QPainter::Antialiasing);

}



void MainWindow::applyUnifiedTopBarStyle()

{

    const QStringList headerContainers = {

        "headerBar", "topBar", "header_Client"

    };

    for (const QString &name : headerContainers) {

        if (auto *w = findChild<QWidget*>(name)) {

            w->setStyleSheet("background-color: #ffffff; border: none; border-bottom: 1px solid #dce1e6;");

        }

    }

    if (auto *stockHeader = findChild<QWidget*>("headerBar")) {

        stockHeader->setMinimumHeight(68);

        stockHeader->setMaximumHeight(68);

    }

    if (auto *clientHeader = findChild<QWidget*>("header_Client")) {

        clientHeader->setMinimumHeight(72);

        clientHeader->setMaximumHeight(72);

    }



    const QStringList titleLabels = {

        "label_Title", "prod_label_Title", "headerTitle",

        "wasteguardhead_Client", "lb_1", "lb_3", "lb_5", "titleLabel"

    };

    for (const QString &name : titleLabels)

        styleTopTitle(findChild<QLabel*>(name));



    const QStringList userNameLabels = {

        "lblUserName", "lblUserName_Modif", "lblUserName_Ajout", "prod_lblUserName"

    };

    for (const QString &name : userNameLabels)

        styleTopUserName(findChild<QLabel*>(name));



    const QStringList userRoleLabels = {

        "lblUserRole", "lblUserRole_Modif", "lblUserRole_Ajout", "prod_lblUserRole", "userLabel"

    };

    for (const QString &name : userRoleLabels)

        styleTopUserRole(findChild<QLabel*>(name));



    const QStringList combinedUserLabels = {

        "user_1", "user_3", "user_5", "user", "responsable_Client"

    };

    for (const QString &name : combinedUserLabels)

        styleTopCombinedUser(findChild<QLabel*>(name));



    const QStringList addButtons = {

        "btnNouveau", "prod_btnAddProduct", "btnNouveau_Client",

        "btnAddDashboard", "btnAddProduct_2", "btnNew", "btnGotoAjout"

    };

    for (const QString &name : addButtons)

        styleModelAddButton(findChild<QPushButton*>(name));



    const QStringList iconButtons = {

        "btnnotif_1", "btnprofil_1", "btnnotif_3", "btnprofil_3",

        "btnnotif_5", "btnprofil_5", "btnNotif", "btnProfil"

    };

    for (const QString &name : iconButtons)

        styleTopIconButton(findChild<QPushButton*>(name));



    const QStringList userFrames = {

        "frameUser", "frameUser_Modif", "frameUser_Ajout", "prod_frameUser"

    };

    for (const QString &name : userFrames) {

        if (auto *frame = findChild<QFrame*>(name))

            frame->setStyleSheet("background: transparent; border: none;");

    }



    // --- Maintenance top bar: align visual structure with other module headers ---

    if (auto *maintLayout = findChild<QHBoxLayout*>("topLayout_1")) {

        maintLayout->setContentsMargins(16, 8, 16, 8);

        maintLayout->setSpacing(10);

    }

    if (auto *maintTitle = findChild<QLabel*>("titleLabel")) {

        styleTopTitle(maintTitle);

        maintTitle->setText("Tableau de Bord");

    }

    if (auto *maintUser = findChild<QLabel*>("userLabel")) {

        maintUser->setTextFormat(Qt::RichText);

        maintUser->setText(

            "<span style=\"color:#0f2b4c;font-size:14px;font-weight:700;\">Admin System</span>"

            "<br/>"

            "<span style=\"color:#16a34a;font-size:12px;font-weight:700;\">Chef d'Atelier</span>"

        );

        maintUser->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    }

    if (auto *notif = findChild<QPushButton*>("btnNotif")) notif->hide();

    if (auto *profil = findChild<QPushButton*>("btnProfil")) profil->hide();

    moveWidgetBefore(this, "btnGotoAjout", "topLayout_1", "userLabel");



    // --- Client top bar: same visual language as global headers ---

    if (auto *clientLayout = findChild<QHBoxLayout*>("horizontalLayout_header_Client")) {

        clientLayout->setContentsMargins(16, 6, 16, 6);

        clientLayout->setSpacing(8);

    }

    if (auto *clientTitle = findChild<QLabel*>("wasteguardhead_Client")) {

        styleTopTitle(clientTitle);

        clientTitle->setText("Gestion de Clients");

    }

    if (auto *clientResp = findChild<QLabel*>("responsable_Client")) {

        clientResp->setTextFormat(Qt::RichText);

        clientResp->setText(

            "<span style=\"color:#0f2b4c;font-size:14px;font-weight:700;\">Admin System</span>"

            "<br/>"

            "<span style=\"color:#16a34a;font-size:12px;font-weight:700;\">Responsable Commercial</span>"

        );

        clientResp->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    }

    moveWidgetBefore(this, "btnNouveau_Client", "horizontalLayout_header_Client", "responsable_Client");



    // Employes top bar: keep + button directly before user block.

    moveWidgetToStart(this, "btnNouveau", "horizontalLayout_UserHeader");



    // Stock top bar: use "Tableau de Bord" strip as the main top strip.

    if (auto *stockTitle = findChild<QLabel*>("headerTitle")) {

        styleTopTitle(stockTitle);

        stockTitle->setText("Tableau de Bord");

    }

    if (auto *stockUser = findChild<QLabel*>("user")) {

        stockUser->setTextFormat(Qt::RichText);

        stockUser->setText(

            "<span style=\"color:#0f2b4c;font-size:14px;font-weight:700;\">Admin System</span>"

            "<br/>"

            "<span style=\"color:#16a34a;font-size:12px;font-weight:700;\">Responsable du stock</span>"

        );

        stockUser->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    }

    if (auto *headerLayout = findChild<QHBoxLayout*>("headerLayout")) {

        headerLayout->setContentsMargins(16, 4, 12, 4);

        headerLayout->setSpacing(4);

        QWidget *stockUserWidget = findChild<QWidget*>("user");

        QWidget *stockAddWidget = findChild<QWidget*>("btnNew");



        if (stockUserWidget) {

            if (auto *parentLayout = stockUserWidget->parentWidget() ? stockUserWidget->parentWidget()->layout() : nullptr)

                parentLayout->removeWidget(stockUserWidget);

        }

        if (stockAddWidget) {

            if (auto *parentLayout = stockAddWidget->parentWidget() ? stockAddWidget->parentWidget()->layout() : nullptr)

                parentLayout->removeWidget(stockAddWidget);

        }



        // Keep add button close to the user block: [+ Ajouter][Admin System]

        if (stockAddWidget) headerLayout->addWidget(stockAddWidget);

        if (stockUserWidget) headerLayout->addWidget(stockUserWidget);

    }

    if (auto *lblTableTitle = findChild<QLabel*>("lblTableTitle"))

        lblTableTitle->hide();

    if (auto *tableHeaderActions = findChild<QHBoxLayout*>("tableHeaderActions")) {

        tableHeaderActions->setContentsMargins(0, 0, 0, 0);

        tableHeaderActions->setSpacing(0);

        for (int i = 0; i < tableHeaderActions->count(); ++i) {

            if (auto *spacer = tableHeaderActions->itemAt(i)->spacerItem()) {

                spacer->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);

            }

        }

    }

    if (auto *stockPage = findChild<QWidget*>("pageStock")) {

        if (auto *dashLayout = stockPage->findChild<QHBoxLayout*>("dashboardLayout")) {

            dashLayout->setContentsMargins(25, 18, 25, 25);

        }

        if (auto *rightColumn = stockPage->findChild<QVBoxLayout*>("rightColumn")) {

            rightColumn->setContentsMargins(0, 0, 0, 0);

            rightColumn->setSpacing(14);

        }



        auto styleStockCard = [](QFrame *card) {

            if (!card) return;

            card->setStyleSheet(

                "QFrame {"

                " background-color: #ffffff;"

                " border: 1px solid #dce1e6;"

                " border-radius: 14px;"

                "}"

            );

        };

        styleStockCard(stockPage->findChild<QFrame*>("statsBox"));

        styleStockCard(stockPage->findChild<QFrame*>("orderBox"));



        if (auto *statsTitle = stockPage->findChild<QLabel*>("lblStatsTitle")) {

            statsTitle->setStyleSheet("color: #607080; font-size: 13px; font-weight: 600;");

        }

        if (auto *totalStock = stockPage->findChild<QLabel*>("totalStock")) {

            totalStock->setStyleSheet("color: #1f2d3d; font-size: 22px; font-weight: 700;");

        }

        if (auto *criticalStock = stockPage->findChild<QLabel*>("lblCriticalStock")) {

            criticalStock->setStyleSheet("color: #ef4444; font-size: 16px; font-weight: 700;");

        }

        if (auto *orderTitle = stockPage->findChild<QLabel*>("lblOrderTitle")) {

            orderTitle->setStyleSheet("color: #1f2d3d; font-size: 13px; font-weight: 700;");

        }

        if (auto *orderSummary = stockPage->findChild<QLabel*>("lblOrderSummary")) {

            orderSummary->setStyleSheet("color: #ef4444; font-size: 14px; font-weight: 700;");

        }



        if (auto *btnOrder = stockPage->findChild<QPushButton*>("btnOrder")) {

            btnOrder->setMinimumHeight(56);

            btnOrder->setCursor(Qt::PointingHandCursor);

        }

    }



    if (auto *maintRightLayout = findChild<QVBoxLayout*>("rightLayout_1")) {

        maintRightLayout->setContentsMargins(0, 0, 0, 0);

        maintRightLayout->setSpacing(10);

        if (auto *maintRight = qobject_cast<QFrame*>(maintRightLayout->parentWidget())) {

            maintRight->setMinimumWidth(300);

            maintRight->setMaximumWidth(350);

        }

    }

    if (auto *maintContent = findChild<QHBoxLayout*>("contentLayout_1")) {

        maintContent->setContentsMargins(0, 0, 0, 0);

        maintContent->setSpacing(16);

    }

    if (auto *maintTop = findChild<QHBoxLayout*>("topLayout_1")) {

        maintTop->setContentsMargins(16, 6, 12, 6);

        maintTop->setSpacing(4);

    }



    // Commandes dashboard top bar: put add button before user block.

    moveWidgetBefore(this, "btnAddDashboard", "horizontalLayout_1", "user_1");

    moveWidgetBefore(this, "btnAddProduct_2", "horizontalLayout_9", "user_5");

    if (auto *cmdAddDash = findChild<QWidget*>("btnAddDashboard")) {

        if (auto *cmdUserDash = findChild<QWidget*>("user_1")) {

            if (auto *targetLayout = qobject_cast<QHBoxLayout*>(cmdUserDash->parentWidget() ? cmdUserDash->parentWidget()->layout() : nullptr)) {

                if (auto *oldLayout = cmdAddDash->parentWidget() ? cmdAddDash->parentWidget()->layout() : nullptr)

                    oldLayout->removeWidget(cmdAddDash);

                int userIdx = targetLayout->indexOf(cmdUserDash);

                if (userIdx >= 0) targetLayout->insertWidget(userIdx, cmdAddDash);

            }

        }

    }

    if (auto *cmdAddAjout = findChild<QWidget*>("btnAddProduct_2")) {

        if (auto *cmdUserAjoutWidget = findChild<QWidget*>("user_5")) {

            if (auto *targetLayout = qobject_cast<QHBoxLayout*>(cmdUserAjoutWidget->parentWidget() ? cmdUserAjoutWidget->parentWidget()->layout() : nullptr)) {

                if (auto *oldLayout = cmdAddAjout->parentWidget() ? cmdAddAjout->parentWidget()->layout() : nullptr)

                    oldLayout->removeWidget(cmdAddAjout);

                int userIdx = targetLayout->indexOf(cmdUserAjoutWidget);

                if (userIdx >= 0) targetLayout->insertWidget(userIdx, cmdAddAjout);

            }

        }

    }

    if (auto *cmdUserDash = findChild<QLabel*>("user_1")) {

        cmdUserDash->setTextFormat(Qt::RichText);

        cmdUserDash->setText(

            "<span style=\"color:#0f2b4c;font-size:14px;font-weight:700;\">Admin System</span>"

            "<br/>"

            "<span style=\"color:#16a34a;font-size:12px;font-weight:700;\">Chef d'Atelier</span>"

        );

        cmdUserDash->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        cmdUserDash->setStyleSheet("background-color: #ffffff; border: none; border-bottom: 1px solid #dce1e6; padding: 0 6px;");

        cmdUserDash->setMinimumHeight(64);

        cmdUserDash->setMaximumHeight(64);

    }

    if (auto *cmdUserAjout = findChild<QLabel*>("user_5")) {

        cmdUserAjout->setTextFormat(Qt::RichText);

        cmdUserAjout->setText(

            "<span style=\"color:#0f2b4c;font-size:14px;font-weight:700;\">Admin System</span>"

            "<br/>"

            "<span style=\"color:#16a34a;font-size:12px;font-weight:700;\">Chef d'Atelier</span>"

        );

        cmdUserAjout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        cmdUserAjout->setStyleSheet("background-color: #ffffff; border: none; border-bottom: 1px solid #dce1e6; padding: 0 6px;");

        cmdUserAjout->setMinimumHeight(64);

        cmdUserAjout->setMaximumHeight(64);

    }

    if (auto *cmdTitleDash = findChild<QLabel*>("lb_1")) {

        cmdTitleDash->setText("Tableau de Bord");

        cmdTitleDash->setStyleSheet("color: #1f2d3d; font-size: 18px; font-weight: 700; background-color: #ffffff; border: none; border-bottom: 1px solid #dce1e6; padding: 0 6px;");

        cmdTitleDash->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        cmdTitleDash->setMinimumHeight(64);

        cmdTitleDash->setMaximumHeight(64);

    }

    if (auto *cmdTitleAjout = findChild<QLabel*>("lb_5")) {

        cmdTitleAjout->setStyleSheet("color: #1f2d3d; font-size: 18px; font-weight: 700; background-color: #ffffff; border: none; border-bottom: 1px solid #dce1e6; padding: 0 6px;");

        cmdTitleAjout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        cmdTitleAjout->setMinimumHeight(64);

        cmdTitleAjout->setMaximumHeight(64);

    }

    if (auto *layoutDash = findChild<QHBoxLayout*>("horizontalLayout_1")) {

        layoutDash->setContentsMargins(16, 0, 10, 0);

        layoutDash->setSpacing(4);

    }

    if (auto *layoutAjout = findChild<QHBoxLayout*>("horizontalLayout_9")) {

        layoutAjout->setContentsMargins(16, 0, 10, 0);

        layoutAjout->setSpacing(4);

    }

    if (auto *fillDash = findChild<QFrame*>("topFill_1")) {

        fillDash->setStyleSheet("background-color: #ffffff; border: none; border-bottom: 1px solid #dce1e6;");

        fillDash->setMinimumHeight(64);

        fillDash->setMaximumHeight(64);

    }

    if (auto *fillAjout = findChild<QFrame*>("topFill_5")) {

        fillAjout->setStyleSheet("background-color: #ffffff; border: none; border-bottom: 1px solid #dce1e6;");

        fillAjout->setMinimumHeight(64);

        fillAjout->setMaximumHeight(64);

    }

    if (auto *gapDash = findChild<QFrame*>("topGap_1")) {

        gapDash->setStyleSheet("background-color: #ffffff; border: none; border-bottom: 1px solid #dce1e6;");

        gapDash->setMinimumSize(2, 64);

        gapDash->setMaximumSize(2, 64);

    }

    if (auto *gapAjout = findChild<QFrame*>("topGap_5")) {

        gapAjout->setStyleSheet("background-color: #ffffff; border: none; border-bottom: 1px solid #dce1e6;");

        gapAjout->setMinimumSize(2, 64);

        gapAjout->setMaximumSize(2, 64);

    }

    if (auto *notif1 = findChild<QPushButton*>("btnnotif_1")) notif1->hide();

    if (auto *profil1 = findChild<QPushButton*>("btnprofil_1")) profil1->hide();

    if (auto *notif5 = findChild<QPushButton*>("btnnotif_5")) notif5->hide();

    if (auto *profil5 = findChild<QPushButton*>("btnprofil_5")) profil5->hide();



    // Produit top bar: ensure add button remains before user.

    moveWidgetBefore(this, "prod_btnAddProduct", "prod_topBarLayout", "prod_frameUser");



    if (auto *clientPage = findChild<QWidget*>("pageClient")) {

        if (auto *tableLayout = clientPage->findChild<QHBoxLayout*>("horizontalLayout_table")) {

            tableLayout->setContentsMargins(0, 0, 0, 0);

            tableLayout->setSpacing(10);

        }

        if (auto *sidepanel = clientPage->findChild<QWidget*>("sidepanel")) {

            sidepanel->setAttribute(Qt::WA_StyledBackground, true);

            sidepanel->setMinimumWidth(300);

            sidepanel->setMaximumWidth(350);

            sidepanel->setStyleSheet(

                "QWidget#sidepanel {"

                " background-color: #ffffff;"

                " border: 1px solid #dce1e6;"

                " border-radius: 14px;"

                "}"

            );

        }

        if (auto *sidepanelLayout = clientPage->findChild<QVBoxLayout*>("verticalLayout_sidepanel")) {

            sidepanelLayout->setContentsMargins(10, 2, 10, 10);

            sidepanelLayout->setSpacing(10);

        }

        if (auto *pilotage = clientPage->findChild<QLabel*>("pilotage")) {

            pilotage->setStyleSheet("color: #1f2d3d; font-size: 16px; font-weight: 700;");

        }

        if (auto *growth = clientPage->findChild<QWidget*>("growthWidget")) {

            growth->setAttribute(Qt::WA_StyledBackground, true);

            growth->setStyleSheet(

                "QWidget#growthWidget {"

                " background-color: #f4f6f9;"

                " border: 1px solid #dce1e6;"

                " border-radius: 10px;"

                "}"

            );

        }



        for (const QString &btnName : {QStringLiteral("facture"), QStringLiteral("score"), QStringLiteral("exportclient")}) {

            if (auto *btn = clientPage->findChild<QPushButton*>(btnName)) {

                btn->setMinimumHeight(56);

                btn->setCursor(Qt::PointingHandCursor);

            }

        }

    }



    if (auto *clientTop = findChild<QHBoxLayout*>("horizontalLayout_header_Client")) {

        clientTop->setContentsMargins(16, 6, 12, 6);

        clientTop->setSpacing(4);

    }

    if (auto *cmdPage = findChild<QWidget*>("pageCmdDashboard")) {

        if (auto *homeLayout = cmdPage->findChild<QHBoxLayout*>("pageHomeLayout")) {

            homeLayout->setContentsMargins(0, 0, 0, 0);

            homeLayout->setSpacing(12);

        }

        if (auto *cmdRight = cmdPage->findChild<QVBoxLayout*>("rightLayout")) {

            cmdRight->setContentsMargins(0, 0, 0, 0);

            cmdRight->setSpacing(10);

        }

    }



    auto styleRightSidebarButton = [](QPushButton *button) {

        if (!button) return;

        button->setEnabled(true);

        button->setFlat(false);

        button->setMinimumHeight(56);

        button->setCursor(Qt::PointingHandCursor);

        button->setStyleSheet(

            "QPushButton {"

            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0f2b4c, stop:1 #2a5298);"

            "   color: white; border-radius: 12px; padding: 12px; font-weight: 700; font-size: 13px;"

            "   border: 1px solid #1e40af;"

            "}"

            "QPushButton:hover {"

            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #1e40af, stop:1 #3b82f6);"

            "   border-color: #60a5fa;"

            "}"

        );

    };



    auto styleRightSidebarButtonCompact = [](QPushButton *button) {

        if (!button) return;

        button->setEnabled(true);

        button->setFlat(false);

        button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

        button->setMinimumHeight(42);

        button->setMaximumHeight(46);

        button->setCursor(Qt::PointingHandCursor);

        button->setStyleSheet(

            "QPushButton {"

            " background-color: #ffffff;"

            " color: #1f2d3d;"

            " border: 1px solid #dcdcdc;"

            " border-radius: 8px;"

            " padding: 8px 12px;"

            " font-size: 13px;"

            " font-weight: 700;"

            " text-align: center;"

            "}"

            "QPushButton:hover {"

            " background-color: #f0f8ff;"

            " border: 1px solid #3498db;"

            " color: #000000;"

            "}"

            "QPushButton:pressed {"

            " background-color: #e1ecf4;"

            " border: 1px solid #2980b9;"

            "}"

            "QPushButton:disabled {"

            " background-color: #ffffff;"

            " color: #1f2d3d;"

            " border: 1px solid #dcdcdc;"

            "}"

        );

    };



    auto styleButtonsIn = [&](QWidget *container) {

        if (!container) return;

        const auto buttons = container->findChildren<QPushButton*>();

        for (QPushButton *btn : buttons)

            styleRightSidebarButton(btn);

    };



    // All right-side panels across modules

    if (auto *empRight = findChild<QWidget*>("sidePanel_Employe")) styleButtonsIn(empRight);

    if (auto *clientRight = findChild<QWidget*>("sidepanel")) styleButtonsIn(clientRight);

    if (auto *prodRight = findChild<QWidget*>("prod_rightSidebar")) {

        prodRight->setMinimumWidth(300);

        prodRight->setMaximumWidth(350);

        styleButtonsIn(prodRight);



        // Product sidebar: keep the same visual style but with compact button size.

        if (auto *prodRightLayout = prodRight->findChild<QVBoxLayout*>("prod_rightLayout")) {

            prodRightLayout->setSpacing(8);

        }

        for (const QString &btnName : {

                 QStringLiteral("prod_btnOpenStats"),

                 QStringLiteral("prod_btnMap3D"),

                 QStringLiteral("prod_btnVideo3D"),

                 QStringLiteral("prod_btnPdf")

             }) {

            styleRightSidebarButtonCompact(prodRight->findChild<QPushButton*>(btnName));

        }

    }

    const auto rightPanels = findChildren<QWidget*>("rightSidebar");

    for (QWidget *panel : rightPanels) {

        panel->setMinimumWidth(300);

        panel->setMaximumWidth(350);

        

        // Force Inverted Style

        panel->setStyleSheet(

            "QWidget#rightSidebar {"

            "   background-color: #ffffff;"

            "   border-radius: 20px;"

            "   border: 1px solid #e2e8f0;"

            "}"

        );



        // Force clean labels

        QList<QLabel*> labels = panel->findChildren<QLabel*>();

        for (QLabel *lbl : labels) {

            lbl->setStyleSheet("background: transparent; color: #0f2b4c; border: none; padding: 2px;");

        }



        // Apply Premium Buttons

        styleButtonsIn(panel);

    }



    // Stock right-side buttons are not inside a dedicated rightSidebar frame

    styleRightSidebarButton(findChild<QPushButton*>("btnOrder"));

    styleRightSidebarButton(findChild<QPushButton*>("btnPrediction"));

    styleRightSidebarButton(findChild<QPushButton*>("btnAlerts"));

    styleRightSidebarButton(findChild<QPushButton*>("btnGoStats_Stock"));

}



void MainWindow::showDashboardHome()

{

    if (auto *sw = mainStacked()) {

        if (homeDashboardPage) {

            sw->setCurrentWidget(homeDashboardPage);

        } else if (auto *page = sw->findChild<QWidget*>("dashboardAccueil", Qt::FindDirectChildrenOnly)) {

            sw->setCurrentWidget(page);

        } else {

            sw->setCurrentIndex(0);

        }

    }



    // --- APPLY STYLE TO EMPLOYEE DASHBOARD SIDEBAR (Inverted) ---

    QTimer::singleShot(100, this, [this]() {

        // 1. Sidebar Container (White)

        if (QWidget *side = this->findChild<QWidget*>("sideDashboard")) {

            side->setStyleSheet(

                "QWidget#sideDashboard {"

                "   background-color: #ffffff;"

                "   border-radius: 20px;"

                "   border: 1px solid #e2e8f0;"

                "}"

            );

        }



        // 2. Buttons (Gradient)

        QString style = 

            "QPushButton {"

            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0f2b4c, stop:1 #2a5298);"

            "   color: white; border-radius: 12px; padding: 15px; font-weight: 700; font-size: 14px;"

            "   border: 1px solid #1e40af;"

            "}"

            "QPushButton:hover {"

            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #1e40af, stop:1 #3b82f6);"

            "   border-color: #60a5fa;"

            "}";

            

        if (auto *b = this->findChild<QPushButton*>("btnGoMission")) b->setStyleSheet(style);

        if (auto *b = this->findChild<QPushButton*>("btnGoPointage")) b->setStyleSheet(style);

        if (auto *b = this->findChild<QPushButton*>("btnGoStats")) b->setStyleSheet(style);

        if (auto *b = this->findChild<QPushButton*>("btnFichePaie")) b->setStyleSheet(style);

        if (auto *b = this->findChild<QPushButton*>("btnExport")) b->setStyleSheet(style);



        // 3. Clean Labels (Aggressive Fix)

        if (QWidget *side = this->findChild<QWidget*>("sideDashboard")) {

            QList<QLabel*> labels = side->findChildren<QLabel*>();

            for (QLabel *lbl : labels) {

                // Force background to transparent even if previously set

                lbl->setStyleSheet(lbl->styleSheet() + " background: transparent; color: #0f2b4c; border: none;");

            }

        }

    });

}



void MainWindow::showEmployesPage()

{

    if (auto *sw = mainStacked()) {

        if (auto *page = sw->findChild<QWidget*>("affichage", Qt::FindDirectChildrenOnly)) {

            sw->setCurrentWidget(page);

        }

    }



    // --- APPLY STYLE TO EMPLOYEE SIDEBAR (Inverted & Clean) ---

    QTimer::singleShot(100, this, [this]() {

        // 1. Sidebar Container (White)

        if (QWidget *side = this->findChild<QWidget*>("sidePanel_Employe")) {

            side->setStyleSheet(

                "QWidget#sidePanel_Employe {"

                "   background-color: #ffffff;"

                "   border-radius: 20px;"

                "   border: 1px solid #e2e8f0;"

                "}"

            );



            // 2. Stat Cards (White/Transparent)

            QList<QFrame*> cards = side->findChildren<QFrame*>();

            for (QFrame *card : cards) {

                if (card->objectName().startsWith("statCard") || card->objectName() == "actionCard") {

                    card->setStyleSheet("background-color: #ffffff; border: 1px solid #edf2f7; border-radius: 12px;");

                }

            }



            // 3. Labels (Aggressive Transparency Fix)

            QList<QLabel*> labels = side->findChildren<QLabel*>();

            for (QLabel *lbl : labels) {

                lbl->setStyleSheet(lbl->styleSheet() + " background: transparent; color: #0f2b4c; border: none;");

            }



            // 4. Buttons (Blue Gradient)

            QString btnStyle = 

                "QPushButton {"

                "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0f2b4c, stop:1 #2a5298);"

                "   color: white; border-radius: 12px; padding: 12px; font-weight: 700; font-size: 13px;"

                "   border: 1px solid #1e40af;"

                "}"

                "QPushButton:hover {"

                "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #1e40af, stop:1 #3b82f6);"

                "   border-color: #60a5fa;"

                "}";

            

            if (auto *b = side->findChild<QPushButton*>("btnFichePaie")) b->setStyleSheet(btnStyle);

            if (auto *b = side->findChild<QPushButton*>("btnGoMission")) b->setStyleSheet(btnStyle);

            if (auto *b = side->findChild<QPushButton*>("btnGoPointage")) b->setStyleSheet(btnStyle);

            if (auto *b = side->findChild<QPushButton*>("btnGoStats")) b->setStyleSheet(btnStyle);

            if (auto *b = side->findChild<QPushButton*>("btnExport")) b->setStyleSheet(btnStyle);

        }

    });

}



void MainWindow::updateTaskChart(const QString &projectName)

{

    QLineSeries *series = new QLineSeries();

    series->setName("Coût cumulé - " + projectName);



    // Query interventions of this TYPE ordered by date

    QSqlQuery query;

    query.prepare("SELECT DATE_INTER, COUT FROM INTERVENTION WHERE UPPER(TYPE) = UPPER(:type) ORDER BY DATE_INTER");

    query.bindValue(":type", projectName.trimmed());

    double cumCost = 0;

    int idx = 0;

    QStringList dateLabels;

    if (query.exec()) {

        while (query.next()) {

            double cout = query.value(1).toDouble();

            cumCost += cout;

            series->append(idx, cumCost);

            dateLabels << query.value(0).toDate().toString("dd/MM");

            idx++;

        }

    }

    if (idx == 0) {

        series->append(0, 0);

        dateLabels << "N/A";

        idx = 1;

    }



    QChart *chart = new QChart();

    chart->addSeries(series);

    chart->setTitle("Coût Cumulé : " + projectName);

    chart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));

    chart->setAnimationOptions(QChart::SeriesAnimations);

    chart->legend()->setAlignment(Qt::AlignBottom);



    QValueAxis *axisX = new QValueAxis();

    axisX->setTitleText("Interventions");

    axisX->setLabelFormat("%d");

    axisX->setRange(0, idx > 1 ? idx - 1 : 1);

    axisX->setTickCount(idx > 10 ? 10 : (idx > 1 ? idx : 2));

    chart->addAxis(axisX, Qt::AlignBottom);

    series->attachAxis(axisX);



    QValueAxis *axisY = new QValueAxis();

    axisY->setTitleText("Coût cumulé (DT)");

    axisY->setLabelFormat("%.0f");

    axisY->setRange(0, cumCost > 0 ? cumCost * 1.2 : 10);

    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisY);



    ui->chartViewTasks->setChart(chart);

    ui->chartViewTasks->setRenderHint(QPainter::Antialiasing);

}



void MainWindow::on_cbProjetStats_currentIndexChanged(const QString &arg1)

{

    updateTaskChart(arg1);

}



// ---------- Produit module helpers ----------

QStackedWidget *MainWindow::mainStacked() const

{

    if (!ui) return nullptr;



    QStackedWidget *candidate = ui->stackedWidget;

    if (candidate) {

        if (candidate->findChild<QWidget*>("pageProduit", Qt::FindDirectChildrenOnly))

            return candidate;

    }



    const auto stacks = this->findChildren<QStackedWidget*>();

    for (QStackedWidget *sw : stacks) {

        if (sw->findChild<QWidget*>("pageProduit", Qt::FindDirectChildrenOnly))

            return sw;

    }



    return candidate;

}



QWidget *MainWindow::produitRoot() const

{

    return ui ? ui->pageProduit : nullptr;

}



QWidget *MainWindow::produitCentral() const

{

    QWidget *root = produitRoot();

    return root ? root->findChild<QWidget*>("centralwidget") : nullptr;

}



QStackedWidget *MainWindow::produitStacked() const

{

    QWidget *root = produitRoot();

    if (!root) return nullptr;

    if (auto *sw = root->findChild<QStackedWidget*>("prod_stackedWidget"))

        return sw;

    return root->findChild<QStackedWidget*>("stackedWidget");

}



QWidget *MainWindow::produitPage(const char *objectName) const

{

    QWidget *root = produitRoot();

    return root ? root->findChild<QWidget*>(objectName) : nullptr;

}



QTableWidget *MainWindow::produitTable() const

{

    QWidget *root = produitRoot();

    if (!root) return nullptr;

    if (auto *t = root->findChild<QTableWidget*>("prod_tableProduits"))

        return t;

    return root->findChild<QTableWidget*>("tableProduits");

}



void MainWindow::setupProduitModule()

{

    QWidget *root = produitRoot();

    if (!root) return;



    ensureProduitModuleVisible();



    if (auto *left = root->findChild<QWidget*>("prod_leftSidebar"))

        left->setAttribute(Qt::WA_StyledBackground, true);

    else if (auto *left = root->findChild<QWidget*>("leftSidebar"))

        left->setAttribute(Qt::WA_StyledBackground, true);

    if (auto *right = root->findChild<QWidget*>("prod_rightSidebar"))

        right->setAttribute(Qt::WA_StyledBackground, true);

    if (auto *right = root->findChild<QFrame*>("prod_rightSidebar")) {

        right->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        right->setMinimumHeight(0);

    }

    else if (auto *right = root->findChild<QWidget*>("rightSidebar"))

        right->setAttribute(Qt::WA_StyledBackground, true);



    applyStyleFix();

    goAffichage();



    if (auto *btnAdd = root->findChild<QPushButton*>("prod_btnAddProduct"))

        connect(btnAdd, &QPushButton::clicked, this, &MainWindow::goAjout);

    else if (auto *btnAdd = root->findChild<QPushButton*>("btnAddProduct"))

        connect(btnAdd, &QPushButton::clicked, this, &MainWindow::goAjout);



    if (auto *btnStats = root->findChild<QPushButton*>("prod_btnOpenStats"))

        connect(btnStats, &QPushButton::clicked, this, &MainWindow::goStatistiques);

    else if (auto *btnStats = root->findChild<QPushButton*>("btnOpenStats"))

        connect(btnStats, &QPushButton::clicked, this, &MainWindow::goStatistiques);



    if (auto *b = root->findChild<QPushButton*>("prod_btnCancel_Add"))

        connect(b, &QPushButton::clicked, this, &MainWindow::goAffichage);

    else if (auto *b = root->findChild<QPushButton*>("btnCancel_Add"))

        connect(b, &QPushButton::clicked, this, &MainWindow::goAffichage);

    if (auto *b = root->findChild<QPushButton*>("prod_btnCancel_Mod"))

        connect(b, &QPushButton::clicked, this, &MainWindow::goAffichage);

    else if (auto *b = root->findChild<QPushButton*>("btnCancel_Mod"))

        connect(b, &QPushButton::clicked, this, &MainWindow::goAffichage);

    if (auto *b = root->findChild<QPushButton*>("prod_btnRetourStats"))

        connect(b, &QPushButton::clicked, this, &MainWindow::goAffichage);

    else if (auto *b = root->findChild<QPushButton*>("btnRetourStats"))

        connect(b, &QPushButton::clicked, this, &MainWindow::goAffichage);

    if (auto *b = root->findChild<QPushButton*>("prod_btnReturnStats"))

        connect(b, &QPushButton::clicked, this, &MainWindow::goAffichage);

    else if (auto *b = root->findChild<QPushButton*>("btnReturnStats"))

        connect(b, &QPushButton::clicked, this, &MainWindow::goAffichage);

    if (auto *b = root->findChild<QPushButton*>("prod_btnSave_Add")) {
        disconnect(b, &QPushButton::clicked, this, &MainWindow::on_prod_btnSave_Add_clicked);
        connect(b, &QPushButton::clicked, this, &MainWindow::on_prod_btnSave_Add_clicked);
    }

    if (auto *b = root->findChild<QPushButton*>("prod_btnSave_Mod")) {
        disconnect(b, &QPushButton::clicked, this, &MainWindow::on_prod_btnSave_Mod_clicked);
        connect(b, &QPushButton::clicked, this, &MainWindow::on_prod_btnSave_Mod_clicked);
    }



    if (QTableWidget *t = produitTable()) {
        QPushButton *btnToggle = root->findChild<QPushButton*>("prod_btnToggleView");
        if (!btnToggle && ui) btnToggle = ui->pageProduit->findChild<QPushButton*>("prod_btnToggleView");
        
        if (btnToggle) {
            disconnect(btnToggle, &QPushButton::clicked, this, &MainWindow::slot_toggleView);
            connect(btnToggle, &QPushButton::clicked, this, &MainWindow::slot_toggleView);
            btnToggle->setCursor(Qt::PointingHandCursor);
        }

        if (t->columnCount() != 7)

            t->setColumnCount(7);

        t->setHorizontalHeaderLabels({
            "Reference", "Modele", "Capacite", "Prix (TND)", "Stock", "Etat", "Actions"
        });



        t->verticalHeader()->setDefaultSectionSize(36);

        t->setWordWrap(false);



        // Make Actions column wider (180px) and fixed, others stretch

        t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        t->horizontalHeader()->setSectionResizeMode(ACTIONS_COL, QHeaderView::Fixed);

        t->setColumnWidth(ACTIONS_COL, 180);



        // Ensure headers are styled properly if not already

        t->horizontalHeader()->show(); 



        addExampleRow();
        refreshCardView();

        // --- INJECT PAGINATION BAR ---
        QTimer::singleShot(200, this, [this, root]() {
            QStackedWidget *stack = root->findChild<QStackedWidget*>("prod_viewStackedWidget");
            if (!stack || root->findChild<QWidget*>("paginationBar")) return;

            QVBoxLayout *vbox = qobject_cast<QVBoxLayout*>(stack->parentWidget()->layout());
            if (!vbox) return;

            QWidget *pBar = new QWidget();
            pBar->setObjectName("paginationBar");
            pBar->setFixedHeight(60);
            pBar->setStyleSheet("background: #f8fafc; border-top: 1px solid #e2e8f0; border-bottom-left-radius: 15px; border-bottom-right-radius: 15px;");
            
            QHBoxLayout *pLayout = new QHBoxLayout(pBar);
            pLayout->setContentsMargins(20, 0, 20, 0);

            QLabel *lblSize = new QLabel("Items par page:");
            lblSize->setStyleSheet("font-size: 13px; color: #64748b; font-weight: 600;");
            
            QComboBox *cbSize = new QComboBox();
            cbSize->setObjectName("pagination_cbSize");
            cbSize->addItems({"4", "6", "12", "24"});
            cbSize->setCurrentText("6");
            cbSize->setFixedWidth(70);
            cbSize->setStyleSheet("background: white; border: 1px solid #cbd5e1; border-radius: 8px; padding: 4px;");
            connect(cbSize, SIGNAL(currentIndexChanged(int)), this, SLOT(on_pagination_cbSize_currentIndexChanged(int)));

            QPushButton *btnPrev = new QPushButton("← Précédent");
            btnPrev->setObjectName("pagination_btnPrev");
            btnPrev->setFixedWidth(110);
            btnPrev->setCursor(Qt::PointingHandCursor);
            btnPrev->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
            connect(btnPrev, &QPushButton::clicked, this, &MainWindow::on_pagination_btnPrev_clicked);

            QLabel *lblPage = new QLabel("Page 1 sur 1");
            lblPage->setObjectName("pagination_lblPage");
            lblPage->setStyleSheet("font-size: 13px; color: #1e293b; font-weight: 700;");

            QPushButton *btnNext = new QPushButton("Suivant →");
            btnNext->setObjectName("pagination_btnNext");
            btnNext->setFixedWidth(110);
            btnNext->setCursor(Qt::PointingHandCursor);
            btnNext->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
            connect(btnNext, &QPushButton::clicked, this, &MainWindow::on_pagination_btnNext_clicked);

            pLayout->addWidget(lblSize);
            pLayout->addWidget(cbSize);
            pLayout->addStretch();
            pLayout->addWidget(btnPrev);
            pLayout->addWidget(lblPage);
            pLayout->addWidget(btnNext);

            vbox->addWidget(pBar);
            
            // Initial button state
            btnPrev->setEnabled(false);
        });



        QTimer::singleShot(0, this, [this]() {

            refreshActionButtons();

            applyStyleFix();

            

            // --- FIX LABEL BACKGROUNDS (AJOUT & MODIFIER) ---

            // Remove gray background from all labels in the add/edit pages

            auto fixLabelBg = [this](const QString &pageName) {

                if (QWidget *page = produitPage(qPrintable(pageName))) {

                    QList<QLabel*> labels = page->findChildren<QLabel*>();

                    for (QLabel *lbl : labels) {

                         // Skip image/icon placeholders if any special ones exists, 

                         // but generally we want text labels transparent.

                         QString current = lbl->styleSheet();

                         if (!current.contains("background: transparent")) {

                             lbl->setStyleSheet(current + " background: transparent;");

                         }

                    }

                }

            };

            

            fixLabelBg("prod_ajout");

            fixLabelBg("prod_modification");

            fixLabelBg("ajout");

            fixLabelBg("modification");

        });

    }



    buildStatsCharts();

}



void MainWindow::ensureProduitModuleVisible()

{

    QWidget *root = produitRoot();

    if (!root) return;



    if (auto *left = root->findChild<QWidget*>("prod_leftSidebar")) {

        left->setVisible(false);

        left->setMinimumWidth(0);

        left->setMaximumWidth(0);

    }



    if (auto *sw = root->findChild<QStackedWidget*>("SWSmartMarket")) {

        if (auto *page = root->findChild<QWidget*>("PModule2")) {

            sw->setCurrentWidget(page);

        } else if (auto *page = root->findChild<QWidget*>("PModule1")) {

            sw->setCurrentWidget(page);

        }

    }

}



void MainWindow::applyStyleFix()

{

    QWidget *root = produitCentral();

    if (!root)

        root = produitRoot();

    if (!root)

        return;



    enableStyledBgRecursive(root);



    const QString qss = productStyleSheet();

    if (!qss.isEmpty()) {

        if (auto *module = produitRoot() ? produitRoot()->findChild<QWidget*>("PModule2") : nullptr) {

            module->setStyleSheet(qss);

        } else if (auto *module = produitRoot() ? produitRoot()->findChild<QWidget*>("PModule1") : nullptr) {

            module->setStyleSheet(qss);

        } else {

            root->setStyleSheet(qss);

        }

    }



    repolishRecursive(root);

}



// ---------- Produit module navigation ----------

void MainWindow::goAffichage()

{

    if (auto *stack = produitStacked()) {

        if (auto *page = produitPage("prod_affichage"))

            stack->setCurrentWidget(page);

        else if (auto *page = produitPage("affichage"))

            stack->setCurrentWidget(page);

    }

    applyStyleFix();

}



void MainWindow::goAjout()

{

    if (auto *stack = produitStacked()) {

        if (auto *page = produitPage("prod_ajout"))

            stack->setCurrentWidget(page);

        else if (auto *page = produitPage("ajout"))

            stack->setCurrentWidget(page);

    }

    applyStyleFix();

}



void MainWindow::goModification()

{

    if (auto *stack = produitStacked()) {

        if (auto *page = produitPage("prod_modification"))

            stack->setCurrentWidget(page);

        else if (auto *page = produitPage("modification"))

            stack->setCurrentWidget(page);

    }

    applyStyleFix();

}



void MainWindow::goStatistiques()

{

    if (auto *stack = produitStacked()) {

        if (auto *page = produitPage("prod_statistiques"))

            stack->setCurrentWidget(page);

        else if (auto *page = produitPage("statistiques"))

            stack->setCurrentWidget(page);

    }



    buildStatsCharts();

    applyStyleFix();

}



// ---------- Produit module table ----------

void MainWindow::addExampleRow()
{
    QTableWidget *t = produitTable();
    if (!t) return;

    t->setRowCount(0);

    QSqlQueryModel *model = Ptmp.afficher();
    if (!model) {
        showFriendlySqlError(this, "charger les produits", "Modele SQL indisponible.");
        return;
    }

    if (model->lastError().isValid()) {
        showFriendlySqlError(this, "charger les produits", model->lastError().text());
        delete model;
        return;
    }

    for (int i = 0; i < model->rowCount(); ++i) {
        const int id = model->data(model->index(i, 0)).toInt();
        const QString reference = model->data(model->index(i, 1)).toString().trimmed();
        const QString modele = model->data(model->index(i, 2)).toString().trimmed();
        const int quantite = model->data(model->index(i, 3)).toInt();
        const int capacite = model->data(model->index(i, 4)).toInt();
        const double prix = model->data(model->index(i, 5)).toDouble();
        const int battery = model->data(model->index(i, 6)).toInt();

        const QString etat = productStatusFromQty(quantite);
        const QString aisle = QString("A%1").arg((i % 6) + 1);

        const int row = t->rowCount();
        t->insertRow(row);

        auto setCell = [&](int col, const QString &txt) {
            auto *it = new QTableWidgetItem(txt);
            it->setForeground(QBrush(Qt::black));
            t->setItem(row, col, it);
        };

        setCell(0, reference);
        setCell(1, modele);
        setCell(2, QString("%1 L").arg(capacite));
        setCell(3, QString::number(prix, 'f', 2));
        setCell(4, QString::number(quantite));
        setCell(5, etat);

        if (auto *refItem = t->item(row, 0)) {
            refItem->setData(PROD_ROLE_CUR, quantite);
            refItem->setData(PROD_ROLE_MAX, qMax(1, capacite));
            refItem->setData(PROD_ROLE_AISLE, aisle);
            refItem->setData(PROD_ROLE_ID, id);
            refItem->setData(PROD_ROLE_BATTERY, battery > 0 ? battery : 12000);
        }
    }

    delete model;
}



void MainWindow::installActionButtonsForRow(int row)

{

    QTableWidget *t = produitTable();

    if (!t) return;

    if (row < 0 || row >= t->rowCount()) return;

    if (t->columnCount() <= ACTIONS_COL) return;



    if (QWidget *existing = t->cellWidget(row, ACTIONS_COL)) {

        const auto buttons = existing->findChildren<QPushButton*>();

        for (auto *b : buttons) b->setProperty("row", row);

        return;

    }



    QWidget *cell = new QWidget(t);

    auto *layout = new QHBoxLayout(cell);

    layout->setContentsMargins(6, 0, 6, 0);

    layout->setSpacing(6);

    layout->setAlignment(Qt::AlignCenter);



    auto *btnEdit = new QPushButton("Modifier", cell);

    auto *btnDel  = new QPushButton("Supprimer", cell);



    btnEdit->setStyleSheet("QPushButton { background-color: #3498db; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; }");

    btnDel->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; }");



    btnEdit->setFixedHeight(24);

    btnDel->setFixedHeight(24);

    btnEdit->setFocusPolicy(Qt::NoFocus);

    btnDel->setFocusPolicy(Qt::NoFocus);

    btnEdit->setCursor(Qt::PointingHandCursor);

    btnDel->setCursor(Qt::PointingHandCursor);



    btnEdit->setToolTip("Modifier");

    btnDel->setToolTip("Supprimer");



    btnEdit->setProperty("row", row);

    btnDel->setProperty("row", row);



    layout->addWidget(btnEdit);

    layout->addWidget(btnDel);



    t->setCellWidget(row, ACTIONS_COL, cell);



    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::handleEditClicked);

    connect(btnDel,  &QPushButton::clicked, this, &MainWindow::handleDeleteClicked);

}



void MainWindow::refreshActionButtons()

{

    QTableWidget *t = produitTable();

    if (!t) return;



    for (int r = 0; r < t->rowCount(); ++r)

        installActionButtonsForRow(r);

}



void MainWindow::handleEditClicked()

{

    auto *btn = qobject_cast<QPushButton*>(sender());

    if (!btn) return;



    QTableWidget *t = produitTable();

    if (!t) return;



    int row = btn->property("row").toInt();

    if (row < 0 || row >= t->rowCount()) return;

    loadProduitToModificationForm(row);
    goModification();

}



void MainWindow::handleDeleteClicked()

{

    auto *btn = qobject_cast<QPushButton*>(sender());

    if (!btn) return;



    QTableWidget *t = produitTable();

    if (!t) return;



    int row = btn->property("row").toInt();

    if (row < 0 || row >= t->rowCount()) return;



    if (QMessageBox::question(this, "Supprimer", "Etes-vous sur de supprimer cet element ?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)

        return;

    int idProduit = -1;
    QString reference;
    if (auto *refItem = t->item(row, 0)) {
        idProduit = refItem->data(PROD_ROLE_ID).toInt();
        reference = refItem->text().trimmed();
    }

    if (idProduit <= 0 && !reference.isEmpty()) {
        int foundId = -1;
        if (Ptmp.findIdByReference(reference, foundId)) {
            idProduit = foundId;
        }
    }

    if (idProduit <= 0) {
        QMessageBox::warning(this, "Suppression", "ID produit introuvable, suppression annulee.");
        return;
    }

    if (!Ptmp.supprimer(idProduit)) {
        showFriendlySqlError(this, "supprimer le produit", Ptmp.lastError());
        return;
    }

    addExampleRow();
    refreshActionButtons();
    if (m_isCardView) refreshCardView();

}

void MainWindow::loadProduitToModificationForm(int row)
{
    QTableWidget *t = produitTable();
    if (!t || row < 0 || row >= t->rowCount()) return;

    currentProduitRow = row;
    t->selectRow(row);

    const QString reference = t->item(row, 0) ? t->item(row, 0)->text().trimmed() : QString();
    const QString modele = t->item(row, 1) ? t->item(row, 1)->text().trimmed() : QString();
    const QString status = t->item(row, 5) ? t->item(row, 5)->text().trimmed() : QString("En Stock");
    const int quantite = t->item(row, 4) ? t->item(row, 4)->text().trimmed().toInt() : 0;
    const double prix = t->item(row, 3) ? t->item(row, 3)->text().trimmed().toDouble() : 0.0;
    const int capacite = t->item(row, 0) ? t->item(row, 0)->data(PROD_ROLE_MAX).toInt() : 100;
    const int battery = t->item(row, 0) ? t->item(row, 0)->data(PROD_ROLE_BATTERY).toInt() : 12000;

    if (ui->prod_ln_ref_mod) ui->prod_ln_ref_mod->setText(reference);
    if (ui->prod_dsb_price_mod) ui->prod_dsb_price_mod->setValue(prix);
    if (ui->prod_sb_qty_mod) ui->prod_sb_qty_mod->setValue(quantite);
    if (ui->prod_slider_cap_mod) ui->prod_slider_cap_mod->setValue(qMax(ui->prod_slider_cap_mod->minimum(), qMin(ui->prod_slider_cap_mod->maximum(), capacite)));
    if (ui->prod_slider_bat_mod) ui->prod_slider_bat_mod->setValue(qMax(ui->prod_slider_bat_mod->minimum(), qMin(ui->prod_slider_bat_mod->maximum(), battery)));

    setComboTextSafe(ui->prod_cb_model_mod, modele);
    setComboTextSafe(ui->prod_cb_status_mod, status);
    if (ui->prod_lstCharacteristics_mod) ui->prod_lstCharacteristics_mod->clearSelection();
}

void MainWindow::on_prod_btnSave_Add_clicked()
{
    QTableWidget *t = produitTable();
    if (!t) return;

    QString reference = ui->prod_ln_ref_add ? ui->prod_ln_ref_add->text().trimmed() : QString();
    if (reference.isEmpty()) {
        QWidget *scope = produitRoot();
        if (auto *sw = produitStacked()) {
            if (QWidget *current = sw->currentWidget()) scope = current;
        }
        reference = firstNonEmptyLineEditText(scope, {"prod_ln_ref_add", "inputRef_add", "ln_ref_add_2", "ln_ref_add_4"});
    }
    const QString modele = ui->prod_cb_model_add ? ui->prod_cb_model_add->currentText().trimmed() : QString();
    const int quantite = ui->prod_sb_qty_add ? ui->prod_sb_qty_add->value() : 0;
    const int capacite = ui->prod_slider_cap_add ? ui->prod_slider_cap_add->value() : 100;
    const double prix = ui->prod_dsb_price_add ? ui->prod_dsb_price_add->value() : 0.0;
    const int battery = ui->prod_slider_bat_add ? ui->prod_slider_bat_add->value() : 12000;
    const QString status = ui->prod_cb_status_add ? ui->prod_cb_status_add->currentText().trimmed() : QString("En Stock");

    if (reference.isEmpty()) {
        QMessageBox::warning(this, "Ajout Produit", "La reference est obligatoire.");
        return;
    }
    if (modele.isEmpty()) {
        QMessageBox::warning(this, "Ajout Produit", "Le modele est obligatoire.");
        return;
    }
    if (prix <= 0.0) {
        QMessageBox::warning(this, "Ajout Produit", "Le prix doit etre strictement positif.");
        return;
    }

    Ptmp.setIdMp(0);
    Ptmp.setReference(reference);
    Ptmp.setNom(modele);
    Ptmp.setQuantite(quantite);
    Ptmp.setCapacite(capacite);
    Ptmp.setPrix(prix);
    Ptmp.setCapaciteBatterie(battery);

    if (!Ptmp.ajouter()) {
        showFriendlySqlError(this, "ajouter le produit", Ptmp.lastError());
        return;
    }

    addExampleRow();
    refreshActionButtons();

    for (int r = 0; r < t->rowCount(); ++r) {
        if (auto *it = t->item(r, 0)) {
            if (it->text().trimmed() == reference) {
                it->setData(PROD_ROLE_BATTERY, battery);
                setComboTextSafe(ui->prod_cb_status_add, status);
                if (auto *statusItem = t->item(r, 5)) {
                    statusItem->setText(resolvedProductStatus(status, quantite));
                }
                t->selectRow(r);
                currentProduitRow = r;
                break;
            }
        }
    }

    if (m_isCardView) refreshCardView();

    if (ui->prod_ln_ref_add) ui->prod_ln_ref_add->clear();
    if (ui->prod_dsb_price_add) ui->prod_dsb_price_add->setValue(0.0);
    if (ui->prod_sb_qty_add) ui->prod_sb_qty_add->setValue(0);
    if (ui->prod_slider_cap_add) ui->prod_slider_cap_add->setValue(qMax(0, ui->prod_slider_cap_add->maximum() / 2));
    if (ui->prod_slider_bat_add) ui->prod_slider_bat_add->setValue(10000);
    if (ui->prod_lstCharacteristics) ui->prod_lstCharacteristics->clearSelection();

    goAffichage();
}

void MainWindow::on_prod_btnSave_Mod_clicked()
{
    QTableWidget *t = produitTable();
    if (!t) return;

    int row = currentProduitRow;
    if (row < 0 || row >= t->rowCount()) {
        row = t->currentRow();
    }
    if (row < 0 || row >= t->rowCount()) {
        QMessageBox::warning(this, "Modification Produit", "Aucun produit selectionne.");
        return;
    }

    QString reference = ui->prod_ln_ref_mod ? ui->prod_ln_ref_mod->text().trimmed() : QString();
    if (reference.isEmpty()) {
        QWidget *scope = produitRoot();
        if (auto *sw = produitStacked()) {
            if (QWidget *current = sw->currentWidget()) scope = current;
        }
        reference = firstNonEmptyLineEditText(scope, {"prod_ln_ref_mod", "inputRef_mod"});
    }
    const QString modele = ui->prod_cb_model_mod ? ui->prod_cb_model_mod->currentText().trimmed() : QString();
    const int quantite = ui->prod_sb_qty_mod ? ui->prod_sb_qty_mod->value() : 0;
    const int capacite = ui->prod_slider_cap_mod ? ui->prod_slider_cap_mod->value() : 100;
    const double prix = ui->prod_dsb_price_mod ? ui->prod_dsb_price_mod->value() : 0.0;
    const int battery = ui->prod_slider_bat_mod ? ui->prod_slider_bat_mod->value() : 12000;
    const QString status = ui->prod_cb_status_mod ? ui->prod_cb_status_mod->currentText().trimmed() : QString("En Stock");

    if (reference.isEmpty()) {
        QMessageBox::warning(this, "Modification Produit", "La reference est obligatoire.");
        return;
    }
    if (modele.isEmpty()) {
        QMessageBox::warning(this, "Modification Produit", "Le modele est obligatoire.");
        return;
    }
    if (prix <= 0.0) {
        QMessageBox::warning(this, "Modification Produit", "Le prix doit etre strictement positif.");
        return;
    }

    int idProduit = -1;
    if (auto *it = t->item(row, 0)) {
        idProduit = it->data(PROD_ROLE_ID).toInt();
    }
    if (idProduit <= 0 && !reference.isEmpty()) {
        int foundId = -1;
        if (Ptmp.findIdByReference(reference, foundId)) {
            idProduit = foundId;
        }
    }
    if (idProduit <= 0) {
        QMessageBox::warning(this, "Modification Produit", "ID produit introuvable.");
        return;
    }

    Ptmp.setIdMp(idProduit);
    Ptmp.setReference(reference);
    Ptmp.setNom(modele);
    Ptmp.setQuantite(quantite);
    Ptmp.setCapacite(capacite);
    Ptmp.setPrix(prix);
    Ptmp.setCapaciteBatterie(battery);

    if (!Ptmp.modifier()) {
        showFriendlySqlError(this, "modifier le produit", Ptmp.lastError());
        return;
    }

    addExampleRow();
    refreshActionButtons();

    for (int r = 0; r < t->rowCount(); ++r) {
        if (auto *it = t->item(r, 0)) {
            if (it->data(PROD_ROLE_ID).toInt() == idProduit || it->text().trimmed() == reference) {
                it->setData(PROD_ROLE_BATTERY, battery);
                if (auto *statusItem = t->item(r, 5)) {
                    statusItem->setText(resolvedProductStatus(status, quantite));
                }
                t->selectRow(r);
                currentProduitRow = r;
                break;
            }
        }
    }

    if (m_isCardView) refreshCardView();
    setComboTextSafe(ui->prod_cb_status_mod, status);
    goAffichage();
}



// ---------- Maintenance Module Implementation ----------



QTableWidget* MainWindow::maintenanceTable() const {

    return ui->tableMaintenance;

}



void MainWindow::setupMaintenanceModule() {

    if (auto* table = maintenanceTable()) {

        table->setColumnCount(10);

        QStringList headers = { "", "Reference", "Date", "Statut", "Coût", "Durée", "Priorité", "Actions", "", "" };

        table->setHorizontalHeaderLabels(headers);

        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        table->setColumnWidth(0, 40);

        table->setColumnHidden(8, true); // hidden column for ID_INTER

        table->setColumnHidden(9, true); // hidden column for TYPE

        table->setRowCount(0);

        refreshInterventions();

    }

    

    if (ui->btnSave_Add) connect(ui->btnSave_Add, &QPushButton::clicked, this, &MainWindow::on_btnSave_Add_clicked);

    if (ui->btnSave_Mod) connect(ui->btnSave_Mod, &QPushButton::clicked, this, &MainWindow::on_btnSave_Mod_clicked);

    // Make photo labels clickable via event filter
    if (ui->lblImgPreview_Add) {
        ui->lblImgPreview_Add->setCursor(Qt::PointingHandCursor);
        ui->lblImgPreview_Add->installEventFilter(this);
    }
    if (ui->lblImgPreview2_Add) {
        ui->lblImgPreview2_Add->setCursor(Qt::PointingHandCursor);
        ui->lblImgPreview2_Add->installEventFilter(this);
    }
    if (ui->lblImgPreview_Mod) {
        ui->lblImgPreview_Mod->setCursor(Qt::PointingHandCursor);
        ui->lblImgPreview_Mod->installEventFilter(this);
    }

    // Connect Rapport PDF button
    if (ui->btnPdf) connect(ui->btnPdf, &QPushButton::clicked, this, &MainWindow::generateMaintenancePdf);

}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        // --- Photos Avant label ---
        if (obj == ui->lblImgPreview_Add) {
            if (!m_photoAvantPath.isEmpty()) {
                // Photo already loaded → show fullscreen preview dialog
                QDialog dlg(this);
                dlg.setWindowTitle("Photos Avant");
                dlg.setMinimumSize(600, 500);
                QVBoxLayout *lay = new QVBoxLayout(&dlg);
                QLabel *imgLbl = new QLabel(&dlg);
                QPixmap pix(m_photoAvantPath);
                imgLbl->setPixmap(pix.scaled(580, 460, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                imgLbl->setAlignment(Qt::AlignCenter);
                lay->addWidget(imgLbl);
                QPushButton *btnClose = new QPushButton("Fermer", &dlg);
                btnClose->setStyleSheet("background-color:#e74c3c; color:white; border-radius:6px; padding:8px 20px; font-weight:bold;");
                connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
                lay->addWidget(btnClose, 0, Qt::AlignCenter);
                QPushButton *btnChange = new QPushButton("Changer la photo", &dlg);
                btnChange->setStyleSheet("background-color:#3498db; color:white; border-radius:6px; padding:8px 20px; font-weight:bold;");
                connect(btnChange, &QPushButton::clicked, [&]() { dlg.done(2); });
                lay->addWidget(btnChange, 0, Qt::AlignCenter);
                int result = dlg.exec();
                if (result == 2) {
                    // User wants to change the photo
                    QString file = QFileDialog::getOpenFileName(this, "Sélectionner Photo Avant",
                        QString(), "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
                    if (!file.isEmpty()) {
                        m_photoAvantPath = file;
                        QPixmap pm(file);
                        ui->lblImgPreview_Add->setPixmap(pm.scaled(
                            ui->lblImgPreview_Add->width() - 10,
                            ui->lblImgPreview_Add->height() - 10,
                            Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    }
                }
            } else {
                // No photo yet → open file dialog
                QString file = QFileDialog::getOpenFileName(this, "Sélectionner Photo Avant",
                    QString(), "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
                if (!file.isEmpty()) {
                    m_photoAvantPath = file;
                    QPixmap pm(file);
                    ui->lblImgPreview_Add->setPixmap(pm.scaled(
                        ui->lblImgPreview_Add->width() - 10,
                        ui->lblImgPreview_Add->height() - 10,
                        Qt::KeepAspectRatio, Qt::SmoothTransformation));
                }
            }
            return true;
        }
        // --- Photos Après label ---
        if (obj == ui->lblImgPreview2_Add) {
            if (!m_photoApresPath.isEmpty()) {
                // Photo already loaded → show fullscreen preview dialog
                QDialog dlg(this);
                dlg.setWindowTitle("Photos Après");
                dlg.setMinimumSize(600, 500);
                QVBoxLayout *lay = new QVBoxLayout(&dlg);
                QLabel *imgLbl = new QLabel(&dlg);
                QPixmap pix(m_photoApresPath);
                imgLbl->setPixmap(pix.scaled(580, 460, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                imgLbl->setAlignment(Qt::AlignCenter);
                lay->addWidget(imgLbl);
                QPushButton *btnClose = new QPushButton("Fermer", &dlg);
                btnClose->setStyleSheet("background-color:#e74c3c; color:white; border-radius:6px; padding:8px 20px; font-weight:bold;");
                connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
                lay->addWidget(btnClose, 0, Qt::AlignCenter);
                QPushButton *btnChange = new QPushButton("Changer la photo", &dlg);
                btnChange->setStyleSheet("background-color:#3498db; color:white; border-radius:6px; padding:8px 20px; font-weight:bold;");
                connect(btnChange, &QPushButton::clicked, [&]() { dlg.done(2); });
                lay->addWidget(btnChange, 0, Qt::AlignCenter);
                int result = dlg.exec();
                if (result == 2) {
                    QString file = QFileDialog::getOpenFileName(this, "Sélectionner Photo Après",
                        QString(), "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
                    if (!file.isEmpty()) {
                        m_photoApresPath = file;
                        QPixmap pm(file);
                        ui->lblImgPreview2_Add->setPixmap(pm.scaled(
                            ui->lblImgPreview2_Add->width() - 10,
                            ui->lblImgPreview2_Add->height() - 10,
                            Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    }
                }
            } else {
                QString file = QFileDialog::getOpenFileName(this, "Sélectionner Photo Après",
                    QString(), "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
                if (!file.isEmpty()) {
                    m_photoApresPath = file;
                    QPixmap pm(file);
                    ui->lblImgPreview2_Add->setPixmap(pm.scaled(
                        ui->lblImgPreview2_Add->width() - 10,
                        ui->lblImgPreview2_Add->height() - 10,
                        Qt::KeepAspectRatio, Qt::SmoothTransformation));
                }
            }
            return true;
        }
        // --- Photos Avant / Après on Modify page ---
        if (obj == ui->lblImgPreview_Mod) {
            if (!m_photoModPath.isEmpty()) {
                QDialog dlg(this);
                dlg.setWindowTitle("Photos Avant / Après");
                dlg.setMinimumSize(600, 500);
                QVBoxLayout *lay = new QVBoxLayout(&dlg);
                QLabel *imgLbl = new QLabel(&dlg);
                QPixmap pix(m_photoModPath);
                imgLbl->setPixmap(pix.scaled(580, 460, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                imgLbl->setAlignment(Qt::AlignCenter);
                lay->addWidget(imgLbl);
                QPushButton *btnClose = new QPushButton("Fermer", &dlg);
                btnClose->setStyleSheet("background-color:#e74c3c; color:white; border-radius:6px; padding:8px 20px; font-weight:bold;");
                connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
                lay->addWidget(btnClose, 0, Qt::AlignCenter);
                QPushButton *btnChange = new QPushButton("Changer la photo", &dlg);
                btnChange->setStyleSheet("background-color:#3498db; color:white; border-radius:6px; padding:8px 20px; font-weight:bold;");
                connect(btnChange, &QPushButton::clicked, [&]() { dlg.done(2); });
                lay->addWidget(btnChange, 0, Qt::AlignCenter);
                int result = dlg.exec();
                if (result == 2) {
                    QString file = QFileDialog::getOpenFileName(this, "Sélectionner Photo",
                        QString(), "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
                    if (!file.isEmpty()) {
                        m_photoModPath = file;
                        QPixmap pm(file);
                        ui->lblImgPreview_Mod->setPixmap(pm.scaled(
                            ui->lblImgPreview_Mod->width() - 10,
                            ui->lblImgPreview_Mod->height() - 10,
                            Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    }
                }
            } else {
                QString file = QFileDialog::getOpenFileName(this, "Sélectionner Photo",
                    QString(), "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
                if (!file.isEmpty()) {
                    m_photoModPath = file;
                    QPixmap pm(file);
                    ui->lblImgPreview_Mod->setPixmap(pm.scaled(
                        ui->lblImgPreview_Mod->width() - 10,
                        ui->lblImgPreview_Mod->height() - 10,
                        Qt::KeepAspectRatio, Qt::SmoothTransformation));
                }
            }
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}



void MainWindow::refreshMaintActionButtons() {

    auto* table = maintenanceTable();

    if (!table) return;

    for (int i = 0; i < table->rowCount(); ++i) {

        installMaintActionButtonsForRow(i);

    }

}



void MainWindow::installMaintActionButtonsForRow(int row) {

    auto* table = maintenanceTable();

    if (!table) return;



    QWidget* pWidget = new QWidget();

    QPushButton* btnEdit = new QPushButton("Modifier");

    btnEdit->setCursor(Qt::PointingHandCursor);

    btnEdit->setStyleSheet("QPushButton { background-color: #3498db; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; }");



    QPushButton* btnDelete = new QPushButton("Supprimer");

    btnDelete->setCursor(Qt::PointingHandCursor);

    btnDelete->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; }");



    btnEdit->setProperty("row", row);

    btnDelete->setProperty("row", row);



    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::handleMaintEditClicked);

    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::handleMaintDeleteClicked);



    QHBoxLayout* pLayout = new QHBoxLayout(pWidget);

    pLayout->addWidget(btnEdit);

    pLayout->addWidget(btnDelete);

    pLayout->setAlignment(Qt::AlignCenter);

    pLayout->setContentsMargins(0, 0, 0, 0);

    pWidget->setLayout(pLayout);



    table->setCellWidget(row, 7, pWidget);

}



void MainWindow::on_btnSave_Add_clicked() {

    auto* table = maintenanceTable();

    if (!table) return;

    if (ui->editRefAdd->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Champ Manquant", "Veuillez remplir la référence.");
        return;
    }

    // Parse duration from combobox ("2 heures" → 2.0)
    QString durStr = ui->comboDurAdd->currentText();
    double duree = durStr.split(" ").first().toDouble();

    // Resolve id_bac: try to match address to a BAC localisation, otherwise use first available BAC
    int idBac = 0;
    QString addr = ui->editAddrAdd->text().trimmed();
    if (!addr.isEmpty()) {
        QSqlQuery qBac;
        qBac.prepare("SELECT ID_BAC FROM BAC_INTEL WHERE UPPER(LOCALISATION_STOCK) LIKE '%' || UPPER(:addr) || '%' AND ROWNUM = 1");
        qBac.bindValue(":addr", addr);
        if (qBac.exec() && qBac.next()) {
            idBac = qBac.value(0).toInt();
        }
    }
    if (idBac <= 0) {
        // Fallback: pick the first available BAC
        QSqlQuery qFirst;
        if (qFirst.exec("SELECT ID_BAC FROM BAC_INTEL WHERE ROWNUM = 1") && qFirst.next()) {
            idBac = qFirst.value(0).toInt();
        }
    }
    if (idBac <= 0) {
        QMessageBox::warning(this, "Erreur", "Aucun bac intelligent trouvé dans la base.\nVeuillez d'abord ajouter un produit (Bac).");
        return;
    }

    INTtmp.setIdInter(0); // auto-generate
    INTtmp.setReference(ui->editRefAdd->text().trimmed());
    INTtmp.setDateInter(ui->dateAdd->date());
    INTtmp.setStatut("En cours");
    INTtmp.setCout(ui->spinCoutAdd->value());
    INTtmp.setDuree(duree);
    INTtmp.setPriorite(ui->comboPrioAdd->currentText());
    INTtmp.setType(ui->editTechAdd->text().trimmed().isEmpty() ? "Maintenance" : ui->editTechAdd->text().trimmed());
    INTtmp.setIdBac(idBac);

    if (INTtmp.ajouter()) {
        QMessageBox::information(this, "Succès", "Intervention ajoutée avec succès.");
        refreshInterventions();
        setupStatistics();
    } else {
        QMessageBox::critical(this, "Erreur", "Echec ajout intervention:\n" + INTtmp.lastError());
    }

    ui->stackedWidget_Maintenance->setCurrentWidget(ui->page_Maint_Dash);

    

    // Clear fields

    ui->editRefAdd->clear();

    ui->editTechAdd->clear();

    ui->editAddrAdd->clear();

    ui->spinCoutAdd->setValue(0);

    // Reset photo labels
    m_photoAvantPath.clear();
    m_photoApresPath.clear();
    if (ui->lblImgPreview_Add) {
        ui->lblImgPreview_Add->setPixmap(QPixmap());
        ui->lblImgPreview_Add->setText("Cliquez ou déposez vos images ici pour les télécharger");
    }
    if (ui->lblImgPreview2_Add) {
        ui->lblImgPreview2_Add->setPixmap(QPixmap());
        ui->lblImgPreview2_Add->setText("Cliquez ou déposez vos images ici pour les télécharger");
    }

}



void MainWindow::on_btnSave_Mod_clicked() {

    auto* table = maintenanceTable();

    if (!table || currentMaintRow < 0) return;

    // Get stored ID from hidden column
    int idInter = 0;
    QTableWidgetItem *idItem = table->item(currentMaintRow, 8);
    if (idItem) idInter = idItem->text().toInt();

    // Parse duration
    QString durStr = ui->comboDurMod->currentText();
    double duree = durStr.split(" ").first().toDouble();

    // Resolve id_bac: try to match address, otherwise keep current or use first BAC
    int idBac = 0;
    QString addr = ui->editAddrMod->text().trimmed();
    if (!addr.isEmpty()) {
        QSqlQuery qBac;
        qBac.prepare("SELECT ID_BAC FROM BAC_INTEL WHERE UPPER(LOCALISATION_STOCK) LIKE '%' || UPPER(:addr) || '%' AND ROWNUM = 1");
        qBac.bindValue(":addr", addr);
        if (qBac.exec() && qBac.next()) {
            idBac = qBac.value(0).toInt();
        }
    }
    if (idBac <= 0) {
        // Try to keep the existing id_bac from the current intervention
        QSqlQuery qCur;
        qCur.prepare("SELECT ID_BAC FROM INTERVENTION WHERE ID_INTER = :id");
        qCur.bindValue(":id", idInter);
        if (qCur.exec() && qCur.next()) {
            idBac = qCur.value(0).toInt();
        }
    }
    if (idBac <= 0) {
        QSqlQuery qFirst;
        if (qFirst.exec("SELECT ID_BAC FROM BAC_INTEL WHERE ROWNUM = 1") && qFirst.next()) {
            idBac = qFirst.value(0).toInt();
        }
    }

    INTtmp.setIdInter(idInter);
    INTtmp.setReference(ui->editRefMod->text().trimmed());
    INTtmp.setDateInter(ui->dateMod->date());
    INTtmp.setStatut("En cours");
    INTtmp.setCout(ui->spinCoutMod->value());
    INTtmp.setDuree(duree);
    INTtmp.setPriorite(ui->comboPrioMod->currentText());
    INTtmp.setType(ui->editTechMod->text().trimmed().isEmpty() ? "Maintenance" : ui->editTechMod->text().trimmed());
    INTtmp.setIdBac(idBac);

    if (INTtmp.modifier()) {
        QMessageBox::information(this, "Succès", "Intervention modifiée avec succès.");
        refreshInterventions();
        setupStatistics();
    } else {
        QMessageBox::critical(this, "Erreur", "Echec modification intervention:\n" + INTtmp.lastError());
    }

    ui->stackedWidget_Maintenance->setCurrentWidget(ui->page_Maint_Dash);

    // Reset modify photo
    m_photoModPath.clear();
    if (ui->lblImgPreview_Mod) {
        ui->lblImgPreview_Mod->setPixmap(QPixmap());
        ui->lblImgPreview_Mod->setText(QString::fromUtf8("📷  Déposez vos images AVANT / APRÈS ici"));
    }

}



void MainWindow::handleMaintEditClicked() {

    QPushButton* btn = qobject_cast<QPushButton*>(sender());

    if (!btn) return;

    int row = btn->property("row").toInt();

    currentMaintRow = row;



    auto* table = maintenanceTable();

    ui->editRefMod->setText(table->item(row, 0)->text());

    ui->dateMod->setDate(QDate::fromString(table->item(row, 1)->text(), "yyyy-MM-dd"));

    ui->editTechMod->setText(table->item(row, 8) ? table->item(row, 8)->text() : ""); // TYPE in "Technicien" field

    

    QString coutStr = table->item(row, 3)->text().replace(" DT", "");

    ui->spinCoutMod->setValue(coutStr.toDouble());

    

    ui->comboDurMod->setCurrentText(table->item(row, 4)->text());

    ui->comboPrioMod->setCurrentText(table->item(row, 5)->text());



    ui->stackedWidget_Maintenance->setCurrentWidget(ui->page_Maint_Modif);

}



void MainWindow::handleMaintDeleteClicked() {

    QPushButton* btn = qobject_cast<QPushButton*>(sender());

    if (!btn) return;

    int row = btn->property("row").toInt();



    if (QMessageBox::question(this, "Supprimer", "Voulez-vous vraiment supprimer cette intervention ?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {

        // Get stored ID from hidden column
        int idInter = 0;
        auto* table = maintenanceTable();
        QTableWidgetItem *idItem = table->item(row, 8);
        if (idItem) idInter = idItem->text().toInt();

        if (idInter > 0 && INTtmp.supprimer(idInter)) {
            refreshInterventions();
            setupStatistics();
        } else {
            QMessageBox::critical(this, "Erreur", "Echec suppression intervention:\n" + INTtmp.lastError());
        }

    }

}


// ---------- refreshInterventions: load INTERVENTION table from DB ----------
void MainWindow::refreshInterventions() {
    auto* table = maintenanceTable();
    if (!table) return;
    table->setRowCount(0);

    QSqlQueryModel *model = INTtmp.afficher();
    // model columns: 0=ID_INTER, 1=REFERENCE, 2=DATE_INTER, 3=DUREE, 4=COUT, 5=STATUT, 6=TYPE, 7=PRIORITE, 8=ID_BAC
    for (int i = 0; i < model->rowCount(); ++i) {
        int row = table->rowCount();
        table->insertRow(row);
        int idInter = model->data(model->index(i, 0)).toInt();
        double duree = model->data(model->index(i, 3)).toDouble();
        double cout  = model->data(model->index(i, 4)).toDouble();
        QString dureeStr = QString::number(duree) + " heures";

        // Checkbox column 0
        QTableWidgetItem *chk = new QTableWidgetItem();
        chk->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        chk->setCheckState(Qt::Unchecked);
        table->setItem(row, 0, chk);

        table->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString())); // Reference
        table->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 2)).toDate().toString("yyyy-MM-dd"))); // Date
        table->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 5)).toString())); // Statut
        table->setItem(row, 4, new QTableWidgetItem(QString::number(cout, 'f', 2) + " DT"));      // Coût
        table->setItem(row, 5, new QTableWidgetItem(dureeStr));                                     // Durée
        table->setItem(row, 6, new QTableWidgetItem(model->data(model->index(i, 7)).toString())); // Priorité
        table->setItem(row, 8, new QTableWidgetItem(QString::number(idInter)));                    // Hidden ID
        table->setItem(row, 9, new QTableWidgetItem(model->data(model->index(i, 6)).toString()));  // Hidden TYPE
        installMaintActionButtonsForRow(row);
    }
    delete model;

    // --- Update INFOS GLOBALES labels from INTERVENTION table ---
    QSqlQuery statsQuery;

    // Total Interventions
    if (statsQuery.exec("SELECT COUNT(*) FROM INTERVENTION") && statsQuery.next()) {
        int totalInter = statsQuery.value(0).toInt();
        if (ui->st_val1) ui->st_val1->setText(QLocale(QLocale::French).toString(totalInter));
    }

    // Total Cout
    if (statsQuery.exec("SELECT NVL(SUM(COUT), 0) FROM INTERVENTION") && statsQuery.next()) {
        double totalCout = statsQuery.value(0).toDouble();
        if (ui->st_val2) ui->st_val2->setText(QLocale(QLocale::French).toString(totalCout, 'f', 0) + " TND");
    }

    // Total Heures de Maintenance
    if (statsQuery.exec("SELECT NVL(SUM(DUREE), 0) FROM INTERVENTION") && statsQuery.next()) {
        double totalHeures = statsQuery.value(0).toDouble();
        if (ui->st_val3) ui->st_val3->setText(QLocale(QLocale::French).toString(totalHeures, 'f', 1));
    }

    // --- Update Repartition par Priorite bars from INTERVENTION table ---
    {
        int totalCount = 0;
        QMap<QString, int> prioCounts;
        QSqlQuery prioQuery;
        if (prioQuery.exec("SELECT PRIORITE, COUNT(*) FROM INTERVENTION GROUP BY PRIORITE ORDER BY COUNT(*) DESC")) {
            while (prioQuery.next()) {
                QString prio = prioQuery.value(0).toString().trimmed();
                int cnt = prioQuery.value(1).toInt();
                prioCounts[prio] = cnt;
                totalCount += cnt;
            }
        }

        // Get top 3 priorities (or fewer)
        QList<QPair<QString, int>> sorted;
        for (auto it = prioCounts.begin(); it != prioCounts.end(); ++it)
            sorted.append(qMakePair(it.key(), it.value()));
        std::sort(sorted.begin(), sorted.end(), [](const QPair<QString,int> &a, const QPair<QString,int> &b){ return a.second > b.second; });

        QLabel* barLabels[] = { ui->lblMiniBar1, ui->lblMiniBar2, ui->lblMiniBar3 };
        QProgressBar* bars[] = { ui->pbMini1, ui->pbMini2, ui->pbMini3 };
        QLabel* pctLabels[] = { ui->lblMiniPct1, ui->lblMiniPct2, ui->lblMiniPct3 };

        for (int i = 0; i < 3; ++i) {
            if (i < sorted.size() && totalCount > 0) {
                int pct = qRound(100.0 * sorted[i].second / totalCount);
                if (barLabels[i]) barLabels[i]->setText(sorted[i].first);
                if (bars[i]) bars[i]->setValue(pct);
                if (pctLabels[i]) pctLabels[i]->setText(QString::number(pct) + "%");
            } else {
                if (barLabels[i]) barLabels[i]->setText("-");
                if (bars[i]) bars[i]->setValue(0);
                if (pctLabels[i]) pctLabels[i]->setText("0%");
            }
        }
    }
}



// ---------- generateMaintenancePdf: export checked rows as professional PDF ----------
void MainWindow::generateMaintenancePdf()
{
    auto* table = maintenanceTable();
    if (!table) return;

    // Collect checked rows
    QList<int> checkedRows;
    for (int i = 0; i < table->rowCount(); ++i) {
        QTableWidgetItem *chk = table->item(i, 0);
        if (chk && chk->checkState() == Qt::Checked)
            checkedRows.append(i);
    }
    if (checkedRows.isEmpty()) {
        QMessageBox::warning(this, "Rapport PDF", "Veuillez cocher au moins une intervention.");
        return;
    }

    // Generate one PDF per checked intervention
    int generated = 0;
    for (int row : checkedRows) {
        QString ref      = table->item(row, 1) ? table->item(row, 1)->text() : "UNKNOWN";
        QString date     = table->item(row, 2) ? table->item(row, 2)->text() : "";
        QString statut   = table->item(row, 3) ? table->item(row, 3)->text() : "";
        QString cout     = table->item(row, 4) ? table->item(row, 4)->text() : "";
        QString duree    = table->item(row, 5) ? table->item(row, 5)->text() : "";
        QString priorite = table->item(row, 6) ? table->item(row, 6)->text() : "";
        QString idStr    = table->item(row, 8) ? table->item(row, 8)->text() : "";
        QString type     = table->item(row, 9) ? table->item(row, 9)->text() : "";

        // Query additional info from DB
        QString description, adresse;
        int idBac = 0;
        {
            QSqlQuery q;
            q.prepare("SELECT ID_BAC FROM INTERVENTION WHERE ID_INTER = :id");
            q.bindValue(":id", idStr.toInt());
            if (q.exec() && q.next()) idBac = q.value(0).toInt();
        }
        if (idBac > 0) {
            QSqlQuery q;
            q.prepare("SELECT LOCALISATION_STOCK FROM BAC_INTEL WHERE ID_BAC = :id");
            q.bindValue(":id", idBac);
            if (q.exec() && q.next()) adresse = q.value(0).toString();
        }

        // File path
        QString safeRef = ref;
        safeRef.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
        QString dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QString filePath = dir + "/Maintenance_Request_" + safeRef + ".pdf";

        QPdfWriter writer(filePath);
        writer.setPageSize(QPageSize(QPageSize::A4));
        writer.setPageMargins(QMarginsF(25, 20, 25, 20), QPageLayout::Millimeter);
        writer.setResolution(300);

        QPainter p(&writer);
        int W = writer.width();
        int pageH = writer.height();

        // Colors
        QColor headerBg(31, 60, 90);       // #1f3c5a
        QColor sectionBg(241, 245, 249);    // #f1f5f9
        QColor textDark(30, 41, 59);        // #1e293b
        QColor textGray(100, 116, 139);     // #64748b
        QColor accent(59, 130, 246);        // #3b82f6
        QColor white(255, 255, 255);
        QColor borderColor(226, 232, 240);  // #e2e8f0
        QColor greenBg(220, 252, 231);      // #dcfce7
        QColor greenText(22, 163, 74);      // #16a34a
        QColor yellowBg(254, 243, 199);     // #fef3c7
        QColor yellowText(217, 119, 6);     // #d97706
        QColor redBg(254, 226, 226);        // #fee2e2
        QColor redText(220, 38, 38);        // #dc2626

        int y = 0;

        // ═══════════ HEADER BAR ═══════════
        int headerH = 420;
        p.fillRect(0, 0, W, headerH, headerBg);

        QFont titleFont("Segoe UI", 16, QFont::Bold);
        p.setFont(titleFont);
        p.setPen(white);
        p.drawText(120, 100, W - 240, 140, Qt::AlignLeft | Qt::AlignVCenter, "DEMANDE DE MAINTENANCE");

        QFont subtitleFont("Segoe UI", 8, QFont::Normal);
        p.setFont(subtitleFont);
        p.setPen(QColor(180, 200, 220));
        p.drawText(120, 240, W - 240, 80, Qt::AlignLeft | Qt::AlignVCenter,
                   "Document genere automatiquement | WasteGuard System");

        // Reference badge on right side
        QFont refFont("Segoe UI", 9, QFont::Bold);
        p.setFont(refFont);
        QFontMetrics fm(refFont, &writer);
        int refW = fm.horizontalAdvance(ref) + 120;
        int refH = 100;
        int refX = W - refW - 120;
        int refY = 160;
        p.setBrush(accent);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(refX, refY, refW, refH, 30, 30);
        p.setPen(white);
        p.drawText(refX, refY, refW, refH, Qt::AlignCenter, ref);

        y = headerH + 80;

        // ═══════════ DETAILS DE L'INTERVENTION ═══════════
        auto drawSectionTitle = [&](const QString &title) {
            QFont secFont("Segoe UI", 11, QFont::Bold);
            p.setFont(secFont);
            p.setPen(headerBg);
            p.drawText(120, y, W - 240, 100, Qt::AlignLeft | Qt::AlignVCenter, title);
            y += 120;
            // Accent line
            p.fillRect(120, y - 20, 200, 8, accent);
            y += 20;
        };

        auto drawInfoRow = [&](const QString &label, const QString &value, bool shaded = false) {
            int rowH = 120;
            if (shaded)
                p.fillRect(80, y, W - 160, rowH, sectionBg);
            else
                p.fillRect(80, y, W - 160, rowH, white);

            // Bottom border
            p.fillRect(80, y + rowH - 2, W - 160, 2, borderColor);

            QFont lblFont("Segoe UI", 8, QFont::Bold);
            p.setFont(lblFont);
            p.setPen(textGray);
            p.drawText(140, y, 600, rowH, Qt::AlignLeft | Qt::AlignVCenter, label);

            QFont valFont("Segoe UI", 8, QFont::Normal);
            p.setFont(valFont);
            p.setPen(textDark);
            p.drawText(750, y, W - 900, rowH, Qt::AlignLeft | Qt::AlignVCenter, value);

            y += rowH;
        };

        // Priority badge helper
        auto drawPrioBadge = [&](const QString &prio) {
            int rowH = 120;
            p.fillRect(80, y, W - 160, rowH, white);
            p.fillRect(80, y + rowH - 2, W - 160, 2, borderColor);

            QFont lblFont("Segoe UI", 8, QFont::Bold);
            p.setFont(lblFont);
            p.setPen(textGray);
            p.drawText(140, y, 600, rowH, Qt::AlignLeft | Qt::AlignVCenter, "Priorite");

            // Badge
            QColor bg = greenBg, fg = greenText;
            if (prio.contains("lev", Qt::CaseInsensitive) || prio == "Urgente")
            { bg = yellowBg; fg = yellowText; }
            else if (prio == "Critique") { bg = redBg; fg = redText; }

            QFont badgeFont("Segoe UI", 7, QFont::Bold);
            p.setFont(badgeFont);
            QFontMetrics bfm(badgeFont, &writer);
            int bw = bfm.horizontalAdvance(prio) + 80;
            int bh = 80;
            int bx = 750;
            int by = y + (rowH - bh) / 2;
            p.setBrush(bg);
            p.setPen(Qt::NoPen);
            p.drawRoundedRect(bx, by, bw, bh, 25, 25);
            p.setPen(fg);
            p.drawText(bx, by, bw, bh, Qt::AlignCenter, prio);

            y += rowH;
        };

        drawSectionTitle("DETAILS DE L'INTERVENTION");

        // Info card with rounded rect background
        int cardStart = y;
        drawInfoRow("Reference", ref, true);
        drawInfoRow("Date", date, false);
        drawInfoRow("Duree", duree, true);
        drawInfoRow("Cout", cout, false);
        drawInfoRow("Statut", statut, true);
        drawInfoRow("Technicien / Type", type.isEmpty() ? "Non specifie" : type, false);
        drawPrioBadge(priorite);

        y += 60;

        // ═══════════ INFORMATIONS COMPLEMENTAIRES ═══════════
        drawSectionTitle("INFORMATIONS COMPLEMENTAIRES");

        drawInfoRow("ID Intervention", idStr, true);
        drawInfoRow("Adresse / Localisation", adresse.isEmpty() ? "Non renseignee" : adresse, false);
        drawInfoRow("ID Bac", idBac > 0 ? QString::number(idBac) : "N/A", true);

        y += 60;

        // ═══════════ OBSERVATIONS ═══════════
        drawSectionTitle("OBSERVATIONS");

        int obsH = 300;
        p.setBrush(sectionBg);
        p.setPen(QPen(borderColor, 3));
        p.drawRoundedRect(80, y, W - 160, obsH, 20, 20);

        QFont obsFont("Segoe UI", 7, QFont::Normal);
        p.setFont(obsFont);
        p.setPen(textGray);
        p.drawText(140, y + 30, W - 300, obsH - 60, Qt::AlignLeft | Qt::TextWordWrap,
                   "Intervention " + ref + " planifiee le " + date + ".\n"
                   "Type: " + (type.isEmpty() ? "Maintenance" : type) + " | Priorite: " + priorite + "\n"
                   "Cout estime: " + cout + " | Duree estimee: " + duree);
        y += obsH + 40;

        // ═══════════ SIGNATURES ═══════════
        int sigY = pageH - 500;
        if (y > sigY - 100) sigY = y + 80;

        p.fillRect(0, sigY - 40, W, 3, borderColor);

        QFont sigLbl("Segoe UI", 7, QFont::Bold);
        p.setFont(sigLbl);
        p.setPen(textGray);

        int sigW = (W - 240) / 2;
        p.drawText(120, sigY, sigW, 80, Qt::AlignLeft | Qt::AlignVCenter, "Responsable Technique");
        p.drawText(120 + sigW + 40, sigY, sigW, 80, Qt::AlignLeft | Qt::AlignVCenter, "Directeur de Maintenance");

        // Signature lines
        p.setPen(QPen(textGray, 3));
        int lineY = sigY + 200;
        p.drawLine(120, lineY, 120 + sigW - 100, lineY);
        p.drawLine(120 + sigW + 40, lineY, W - 120, lineY);

        // ═══════════ FOOTER ═══════════
        int footerY = pageH - 150;
        p.fillRect(0, footerY, W, 150, headerBg);

        QFont footFont("Segoe UI", 6, QFont::Normal);
        p.setFont(footFont);
        p.setPen(QColor(180, 200, 220));
        p.drawText(120, footerY, W - 240, 150, Qt::AlignLeft | Qt::AlignVCenter,
                   "WasteGuard - Smart Waste Collection System | Document confidentiel | " +
                   QDate::currentDate().toString("dd/MM/yyyy"));

        // QR Code placeholder (simple styled square)
        int qrSize = 200;
        int qrX = W - qrSize - 120;
        int qrY = footerY - qrSize - 60;
        p.setBrush(white);
        p.setPen(QPen(borderColor, 4));
        p.drawRoundedRect(qrX, qrY, qrSize, qrSize, 12, 12);

        // Draw a mini QR-like pattern
        p.setBrush(headerBg);
        p.setPen(Qt::NoPen);
        int cellSz = qrSize / 8;
        int pattern[] = {0,0, 0,1, 0,2, 0,6, 0,7,
                         1,0, 1,2, 1,4, 1,7,
                         2,0, 2,1, 2,2, 2,5, 2,6, 2,7,
                         3,4, 3,5,
                         4,3, 4,5,
                         5,0, 5,1, 5,2, 5,4, 5,6,
                         6,0, 6,2, 6,3, 6,5, 6,7,
                         7,0, 7,1, 7,2, 7,4, 7,6, 7,7};
        int patLen = sizeof(pattern) / sizeof(pattern[0]);
        for (int pi = 0; pi < patLen; pi += 2) {
            p.drawRect(qrX + 8 + pattern[pi+1] * cellSz, qrY + 8 + pattern[pi] * cellSz, cellSz - 4, cellSz - 4);
        }

        p.end();
        generated++;

        // Open the generated PDF
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }

    // Uncheck all after generation
    for (int i = 0; i < table->rowCount(); ++i) {
        QTableWidgetItem *chk = table->item(i, 0);
        if (chk) chk->setCheckState(Qt::Unchecked);
    }

    QMessageBox::information(this, "Rapport PDF",
        QString::number(generated) + " rapport(s) PDF genere(s) sur le Bureau.");
}


// ---------- Produit module stats ----------

void MainWindow::buildStatsCharts()

{

    QWidget *root = produitRoot();

    if (!root) return;



    QFrame *chart1Host = root->findChild<QFrame*>("prod_graphFrame1Plot");

    if (!chart1Host)

        chart1Host = root->findChild<QFrame*>("graphFrame1Plot");

    QFrame *chart2Host = root->findChild<QFrame*>("prod_graphFrame2Plot");

    if (!chart2Host)

        chart2Host = root->findChild<QFrame*>("graphFrame2Plot");

    if (!chart1Host || !chart2Host) return;



    auto clearAndLayout = [](QWidget *w) {

        if (!w) return;

        if (w->layout()) {

            QLayoutItem *child;

            while ((child = w->layout()->takeAt(0)) != nullptr) {

                if (child->widget()) child->widget()->deleteLater();

                delete child;

            }

            delete w->layout();

        }

        auto *lay = new QVBoxLayout(w);

        lay->setContentsMargins(0,0,0,0);

        lay->setSpacing(0);

    };



    clearAndLayout(chart1Host);

    clearAndLayout(chart2Host);



    chart1Host->setMinimumHeight(260);

    chart2Host->setMinimumHeight(260);



    auto *donutSeries = new QPieSeries();

    donutSeries->setHoleSize(0.35);

    donutSeries->append("Produits", 45);

    donutSeries->append("Stock", 30);

    donutSeries->append("Maintenance", 15);

    donutSeries->append("Commandes", 10);



    for (auto *slice : donutSeries->slices()) {

        slice->setLabelVisible(true);

        slice->setLabelColor(Qt::white);

        slice->setLabelPosition(QPieSlice::LabelInsideHorizontal);

        slice->setBorderWidth(2);

        slice->setBorderColor(Qt::white);

    }

    if (!donutSeries->slices().isEmpty()) {

        auto *s = donutSeries->slices().at(0);

        s->setExploded(true);

        s->setLabelVisible(true);

        s->setLabelPosition(QPieSlice::LabelOutside);

        s->setLabelColor(Qt::black);

    }



    auto *donutChart = new QChart();

    donutChart->addSeries(donutSeries);

    donutChart->setTitle("Répartition Globale (Donut)");

    donutChart->setTheme(QChart::ChartThemeBlueCerulean);

    donutChart->setAnimationOptions(QChart::AllAnimations);

    donutChart->legend()->setAlignment(Qt::AlignRight);

    donutChart->legend()->setFont(QFont("Segoe UI", 9));

    donutChart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));



    auto *donutView = new QChartView(donutChart);

    donutView->setRenderHint(QPainter::Antialiasing);



    auto *splineSeries = new QSplineSeries();

    splineSeries->setName("Croissance");

    splineSeries->append(1, 10);

    splineSeries->append(2, 22);

    splineSeries->append(3, 18);

    splineSeries->append(4, 32);

    splineSeries->append(5, 28);

    splineSeries->append(6, 45);



    auto *areaSeries = new QAreaSeries(splineSeries);

    areaSeries->setName("Zone de Croissance");



    QLinearGradient gradient(QPointF(0, 0), QPointF(0, 1));

    gradient.setColorAt(0.0, QColor(0x3498db));

    gradient.setColorAt(1.0, QColor(0xecf0f1));

    areaSeries->setBrush(gradient);



    QPen pen(0x2980b9);

    pen.setWidth(3);

    areaSeries->setPen(pen);



    auto *areaChart = new QChart();

    areaChart->addSeries(areaSeries);

    areaChart->setTitle("Évolution Dynamique");

    areaChart->setTheme(QChart::ChartThemeLight);

    areaChart->setAnimationOptions(QChart::SeriesAnimations);

    areaChart->legend()->setAlignment(Qt::AlignBottom);

    areaChart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));



    auto *axisX = new QValueAxis();

    axisX->setRange(1, 6);

    axisX->setLabelFormat("%d");

    axisX->setTickCount(6);

    areaChart->addAxis(axisX, Qt::AlignBottom);

    areaSeries->attachAxis(axisX);



    auto *axisY = new QValueAxis();

    axisY->setRange(0, 50);

    areaChart->addAxis(axisY, Qt::AlignLeft);

    areaSeries->attachAxis(axisY);



    auto *areaView = new QChartView(areaChart);

    areaView->setRenderHint(QPainter::Antialiasing);



    if (!chart1Host->layout()) new QVBoxLayout(chart1Host);

    if (!chart2Host->layout()) new QVBoxLayout(chart2Host);



    chart1Host->layout()->addWidget(donutView);

    chart2Host->layout()->addWidget(areaView);

}



QString MainWindow::productStyleSheet() const

{

    QString qss = QStringLiteral(R"(

/* GLOBAL SETTINGS */



* { font-family: 'Segoe UI', Arial, sans-serif; }



QMainWindow, QWidget#centralwidget, QTabWidget::pane { background-color: #eff3f8; border: none; }



/* SIDEBAR */



QWidget#leftSidebar QPushButton {

    background-color: transparent;

    border: none;

    color: #bdc3c7;

    text-align: left;

    padding: 12px 20px;

    font-size: 14px;

    border-left: 3px solid transparent;

}



QWidget#leftSidebar QPushButton:hover, QWidget#leftSidebar QPushButton:checked {

    background-color: #1a4270;

    color: white;

    border-left: 3px solid #3498db;

}



/* TOP BAR */



QWidget#horizontalLayoutWidget_1, QWidget#horizontalLayoutWidget_2, QWidget#horizontalLayoutWidget_3 { background-color: #3b6998; }



QLabel#lb_1, QLabel#lb_2, QLabel#lb_3, QLabel#user_1, QLabel#user_2, QLabel#user_3 { color: white; font-weight: bold; font-size: 14px; }



QPushButton#btnnotif_1, QPushButton#btnprofil_1, QPushButton#btnnotif_2, QPushButton#btnprofil_2, QPushButton#btnnotif_3, QPushButton#btnprofil_3 { background-color: rgba(255, 255, 255, 0.2); border-radius: 4px; padding: 5px 15px; }



/* DASHBOARD TABLE & CONTROLS */



QTableWidget { background-color: white; border: 1px solid #dce1e8; border-radius: 8px; gridline-color: #f0f0f0; selection-background-color: #e3f2fd; selection-color: #333; }



QHeaderView::section { background-color: white; color: #333; font-weight: bold; border: none; border-bottom: 2px solid #3b6998; padding: 8px; }



QLineEdit#searchInput { background-color: white; border: 1px solid #ccc; padding: 5px; border-radius: 4px; }



QComboBox#cbSort { background-color: white; border: 1px solid #ccc; padding: 5px; border-radius: 4px; }



QPushButton#btnAddProduct { background-color: #3b6998; color: white; font-weight: bold; border-radius: 4px; padding: 5px 15px; }




/* CARD VIEW */
#prod_cardContainer {
    background-color: transparent;
}

QFrame#productCard {
    background: #ffffff;
    border: 1px solid #e2e8f0;
    border-radius: 16px;
}

QFrame#productCard:hover {
    border: 1px solid #3b82f6;
    background: #f8fafc;
}

QPushButton#btnProductAction {
    border: 1px solid #e2e8f0;
    background: #f1f5f9;
}

/* COMMON FORM STYLES */



QFrame#cardAdd, QFrame#cardMod, QFrame#rightSidebar { background-color: white; border-radius: 15px; border: 1px solid #dce1e8; }



QLabel { color: #37474f; font-weight: 700; font-size: 13px; margin-bottom: 2px; }



QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox { background-color: #f7f9fc; border: 1px solid #cfd8dc; border-radius: 6px; padding: 8px; color: #37474f; font-size: 13px; }



QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus { border: 1px solid #3b6998; background-color: white; }



/* STAT CARDS (Dashboard) */



QFrame.statCard { background-color: #e3f2fd; border-radius: 8px; border: 1px solid #bbdefb; }



QLabel.statValue { font-size: 20px; font-weight: bold; color: #1565c0; }



QLabel.statLabel { font-size: 12px; color: #546e7a; }



QFrame.alertCard { background-color: #ffebee; border-radius: 8px; border: 1px solid #ffcdd2; }



QLabel.alertValue { font-size: 20px; font-weight: bold; color: #c62828; }



/* SLIDERS */



QSlider::groove:horizontal { background: #e0e0e0; height: 8px; border-radius: 4px; }



QSlider::handle:horizontal { background: #3b6998; width: 16px; margin: -5px 0; border-radius: 8px; }



QSlider::sub-page:horizontal { background: #64b5f6; border-radius: 4px; }



/* CREATIVE WAREHOUSE MAP */



QFrame#mapFrame, QFrame#depotMapFrame_mod { background-color: #263238; border-radius: 8px; border: 2px solid #455a64; }



QLabel.zoneLabel { color: #cfd8dc; font-size: 11px; font-weight: bold; letter-spacing: 1px; margin-top: 10px; }



QPushButton.slotBtn { background-color: #4caf50; border: 2px solid #388e3c; border-radius: 4px; color: white; font-weight: bold; }



QPushButton.slotBtn:checked { background-color: #2196f3; border: 2px solid #1976d2; }



QPushButton.slotBtn:disabled { background-color: #ef5350; border: 2px solid #c62828; color: #ffebee; text-decoration: line-through; }



/* ACTION BUTTONS */



QPushButton#btnSave_Add, QPushButton#btnSave_Mod { background-color: #4caf50; color: white; font-weight: bold; border-radius: 6px; }



QPushButton#btnCancel_Add, QPushButton#btnCancel_Mod { background-color: #b0bec5; color: white; font-weight: bold; border-radius: 6px; }



QLabel#lblImgPreview_Add, QLabel#imgBin { background-color: #eceff1; border: 2px dashed #cfd8dc; border-radius: 12px; color: #90a4ae; }



/* --- Fix sidebar/background containers --- */

#leftSidebar{background-color:#0f2c4f;}

#rightSidebar{min-width:0px; max-width:16777215px;}



/* Stats preview colors */

#statCard QWidget#statMiniFrame QLabel{color:#ffffff;}

QPushButton#btnOpenStats{color:#000000;}



/* Plain text / no gray backgrounds */

QTableWidget { alternate-background-color: white; }

QTableWidget::item { color: #333; background-color: white; }

QTableWidget::item:selected { background-color: #e3f2fd; color: #333; }

QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox { color: #37474f; }



/* Force clean text colors in product module */

QWidget#PModule2 QLabel, QWidget#PModule1 QLabel { color: #37474f; background-color: transparent; }

QLabel#prod_lb_1, QLabel#prod_lb_2, QLabel#prod_lb_3, QLabel#prod_user_1, QLabel#prod_user_2, QLabel#prod_user_3 { color: #37474f; background-color: transparent; }

QLabel.statValue { color: #1565c0; }

QLabel.statLabel { color: #546e7a; }

QLabel.alertValue { color: #c62828; }



/* Product right sidebar visual (match reference) */

QLabel#prod_lblFunc, QLabel#prod_lblExport {

    color: #37474f;

    font-weight: 700;

    font-size: 13px;

}

QFrame#prod_stat1, QFrame#prod_stat2, QFrame#prod_statsPreview {

    background-color: #e3f2fd;

    border-radius: 10px;

    border: 1px solid #bbdefb;

}

QFrame#prod_stat3 {

    background-color: #ffebee;

    border-radius: 10px;

    border: 1px solid #ffcdd2;

}

QLabel#prod_st_val1, QLabel#prod_st_val2 {

    color: #1565c0;

    font-size: 20px;

    font-weight: bold;

}

QLabel#prod_st_val3 { color: #c62828; font-size: 20px; font-weight: bold; }

QLabel#prod_st_lbl1, QLabel#prod_st_lbl2, QLabel#prod_st_lbl3 { color: #546e7a; font-size: 12px; }

QFrame#prod_miniStatsFrame {

    background-color: #2f2f2f;

    border-radius: 8px;

    border: none;

}

QLabel#prod_lblMini1, QLabel#prod_lblMini2 { color: #ffffff; font-weight: 600; }

QProgressBar#prod_pbTrend, QProgressBar#prod_pbStock {

    background-color: #3e3e3e;

    border: none;

    border-radius: 3px;

    height: 6px;

}

QProgressBar#prod_pbTrend::chunk, QProgressBar#prod_pbStock::chunk {

    background-color: #1e88e5;

    border-radius: 3px;

}



/* Zone labels should be dark (not white) */

QLabel.zoneLabel { color: #37474f; }



/* Export buttons */

QPushButton#prod_btnPdf {

    background-color: #3a3a3a;

    color: #ffffff;

    border: none;

    border-radius: 6px;

    padding: 6px 10px;

}

QPushButton#prod_btnPdf:hover {

    background-color: #2f2f2f;

}



/* Employe-style top bar for product */

QLabel#prod_label_Title { color: #0f2b4c; font-size: 18px; font-weight: 600; }

 QLabel#prod_lblUserName { font-size: 14px; font-weight: bold; color: #0f2b4c; }

 QLabel#prod_lblUserRole { font-size: 12px; font-weight: bold; color: #28a745; margin-top: -2px; }



/* Match employe add button style in product top bar */

QPushButton#prod_btnAddProduct {

    background-color: #0f2b4c;

    color: white;

    border-radius: 5px;

    padding: 8px 15px;

    font-weight: bold;

    border: none;

}

QPushButton#prod_btnAddProduct:hover {

    background-color: #1a4270;

}



/* Match employe control sizing on product search/filter/add row */

QLineEdit#prod_searchInput { min-height: 40px; }

QComboBox#prod_cbSort { min-height: 40px; min-width: 180px; }

QPushButton#prod_btnAddProduct { min-height: 40px; }



/* Match employe search/filter visual style */

QLineEdit#prod_searchInput {

    background-color: #ffffff;

    color: #000000;

    border: 1px solid #cccccc;

    border-radius: 5px;

    padding: 6px;

}

QLineEdit#prod_searchInput:focus { border: 1px solid #3498db; }

QComboBox#prod_cbSort {

    background-color: #ffffff;

    color: #333333;

    border: 1px solid #cccccc;

    border-radius: 5px;

    padding: 6px;

}

QComboBox#prod_cbSort:focus { border: 1px solid #3498db; }



/* Force Product + Stock sidebar button text to black */

QPushButton#prod_btnproduit_1, QPushButton#prod_btnstock_1 { color: #000000; }



/* Ensure product left sidebar buttons have no frames/borders */

QFrame#prod_leftSidebar QPushButton {

    background-color: transparent;

    border: none;

    text-align: left;

    padding: 12px 20px;

    font-size: 14px;

    border-left: 3px solid transparent;

}

QFrame#prod_leftSidebar QPushButton:hover, QFrame#prod_leftSidebar QPushButton:checked {

    background-color: #1a4270;

    color: white;

    border-left: 3px solid #3498db;

}

QFrame#prod_leftSidebar QPushButton:focus { outline: none; }



/* Product table styled like employe */

QTableWidget#prod_tableProduits {

    background-color: #f4f6f9;

    alternate-background-color: #e8ebf0;

    color: #000000;

    gridline-color: transparent;

    border: 1px solid #dce1e6;

    border-radius: 8px;

    font-size: 13px;

}

QTableWidget#prod_tableProduits::item {

    color: #000000;

    padding: 5px;

    border-bottom: 1px solid #e0e6ed;

}

QTableWidget#prod_tableProduits::item:selected {

    background-color: #e3f2fd;

    color: #0f2b4c;

}

QTableWidget#prod_tableProduits QHeaderView::section {

    background-color: #0f2b4c;

    color: white;

    padding: 8px;

    border: none;

    font-weight: bold;

    font-size: 13px;

}



)");



    qss.replace("QMainWindow, QWidget#centralwidget", "QWidget#PModule2, QWidget#PModule1, QWidget#pageProduit");

    qss.replace("QWidget#leftSidebar", "QWidget#prod_leftSidebar");

    qss.replace("QWidget#rightSidebar", "QWidget#prod_rightSidebar");

    qss.replace("QWidget#horizontalLayoutWidget_1", "QWidget#prod_horizontalLayoutWidget_1");

    qss.replace("QWidget#horizontalLayoutWidget_2", "QWidget#prod_horizontalLayoutWidget_2");

    qss.replace("QWidget#horizontalLayoutWidget_3", "QWidget#prod_horizontalLayoutWidget_3");

    qss.replace("QLabel#lb_1", "QLabel#prod_lb_1");

    qss.replace("QLabel#lb_2", "QLabel#prod_lb_2");

    qss.replace("QLabel#lb_3", "QLabel#prod_lb_3");

    qss.replace("QLabel#user_1", "QLabel#prod_user_1");

    qss.replace("QLabel#user_2", "QLabel#prod_user_2");

    qss.replace("QLabel#user_3", "QLabel#prod_user_3");

    qss.replace("QPushButton#btnnotif_1", "QPushButton#prod_btnnotif_1");

    qss.replace("QPushButton#btnprofil_1", "QPushButton#prod_btnprofil_1");

    qss.replace("QPushButton#btnnotif_2", "QPushButton#prod_btnnotif_2");

    qss.replace("QPushButton#btnprofil_2", "QPushButton#prod_btnprofil_2");

    qss.replace("QPushButton#btnnotif_3", "QPushButton#prod_btnnotif_3");

    qss.replace("QPushButton#btnprofil_3", "QPushButton#prod_btnprofil_3");

    qss.replace("QLineEdit#searchInput", "QLineEdit#prod_searchInput");

    qss.replace("QComboBox#cbSort", "QComboBox#prod_cbSort");

    qss.replace("QPushButton#btnAddProduct", "QPushButton#prod_btnAddProduct");

    qss.replace("QFrame#cardAdd", "QFrame#prod_cardAdd");

    qss.replace("QFrame#cardMod", "QFrame#prod_cardMod");

    qss.replace("QFrame#rightSidebar", "QFrame#prod_rightSidebar");

    qss.replace("QFrame#mapFrame", "QFrame#prod_mapFrame");

    qss.replace("QFrame#depotMapFrame_mod", "QFrame#prod_depotMapFrame_mod");

    qss.replace("QPushButton#btnSave_Add", "QPushButton#prod_btnSave_Add");

    qss.replace("QPushButton#btnSave_Mod", "QPushButton#prod_btnSave_Mod");

    qss.replace("QPushButton#btnCancel_Add", "QPushButton#prod_btnCancel_Add");

    qss.replace("QPushButton#btnCancel_Mod", "QPushButton#prod_btnCancel_Mod");

    qss.replace("QLabel#lblImgPreview_Add", "QLabel#prod_lblImgPreview_Add");

    qss.replace("QLabel#imgBin", "QLabel#prod_imgBin");

    qss.replace("QPushButton#btnOpenStats", "QPushButton#prod_btnOpenStats");

    qss.replace("#leftSidebar", "#prod_leftSidebar");

    qss.replace("#rightSidebar", "#prod_rightSidebar");



    return qss;

}



void MainWindow::setupStockModule()

{

    if (ui->stock_stackedWidget)

        ui->stock_stackedWidget->setCurrentIndex(0);



    setupStockTableData();



    if (ui->stock_searchInput) {

        ui->stock_searchInput->setClearButtonEnabled(true);

        ui->stock_searchInput->setStyleSheet(

            "QLineEdit#stock_searchInput {"

            " background-color: #f8fafc;"

            " border: 1px solid #dce1e6;"

            " border-radius: 10px;"

            " padding: 0 14px;"

            " color: #1f2d3d;"

            " font-size: 14px;"

            "}"

            "QLineEdit#stock_searchInput:focus {"

            " background-color: #ffffff;"

            " border: 1px solid #2563eb;"

            "}"

        );

    }

    if (ui->stock_sortCombo) {

        ui->stock_sortCombo->setStyleSheet(

            "QComboBox#stock_sortCombo {"

            " background-color: #f8fafc;"

            " border: 1px solid #dce1e6;"

            " border-radius: 10px;"

            " padding: 0 34px 0 12px;"

            " color: #1f2d3d;"

            " font-size: 14px;"

            "}"

            "QComboBox#stock_sortCombo:focus {"

            " background-color: #ffffff;"

            " border: 1px solid #2563eb;"

            "}"

            "QComboBox#stock_sortCombo::drop-down {"

            " border: none;"

            " width: 30px;"

            "}"

            "QComboBox#stock_sortCombo QAbstractItemView {"

            " background-color: #ffffff;"

            " border: 1px solid #dce1e6;"

            " selection-background-color: #e7f0ff;"

            " selection-color: #1f2d3d;"

            "}"

        );

    }



    if (ui->stock_searchInput && !ui->stock_searchInput->property("stockFilterConnected").toBool()) {

        connect(ui->stock_searchInput, &QLineEdit::textChanged, this, [this]() {

            applyStockFilterAndSort();

        });

        ui->stock_searchInput->setProperty("stockFilterConnected", true);

    }

    if (ui->stock_sortCombo && !ui->stock_sortCombo->property("stockFilterConnected").toBool()) {

        connect(ui->stock_sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {

            applyStockFilterAndSort();

        });

        ui->stock_sortCombo->setProperty("stockFilterConnected", true);

    }



    // Connexions Sliders -> Inputs (Ajouter)

    if (ui->sliderStock_add && ui->inputStock_add)

        connect(ui->sliderStock_add, &QSlider::valueChanged, this, [=](int val){ ui->inputStock_add->setText(QString::number(val)); });

    if (ui->sliderPrix_add && ui->inputPrix_add)

        connect(ui->sliderPrix_add, &QSlider::valueChanged, this, [=](int val){ ui->inputPrix_add->setText(QString::number(val)); });



    // Connexions Sliders -> Inputs (Modifier)

    if (ui->sliderStock_mod && ui->inputStock_mod)

        connect(ui->sliderStock_mod, &QSlider::valueChanged, this, [=](int val){ ui->inputStock_mod->setText(QString::number(val)); });

    if (ui->sliderPrix_mod && ui->inputPrix_mod)

        connect(ui->sliderPrix_mod, &QSlider::valueChanged, this, [=](int val){ ui->inputPrix_mod->setText(QString::number(val) + " TND"); });



    // Navigation buttons

    if (ui->btnNew) {

        connect(ui->btnNew, &QPushButton::clicked, this, [this](){

            if (ui->stock_stackedWidget) ui->stock_stackedWidget->setCurrentIndex(1);

        });

    }

    if (ui->btnCancel_add) {

        connect(ui->btnCancel_add, &QPushButton::clicked, this, [this](){

            if (ui->stock_stackedWidget) ui->stock_stackedWidget->setCurrentIndex(0);

        });

    }

    if (ui->btnCancel_mod) {

        connect(ui->btnCancel_mod, &QPushButton::clicked, this, [this](){

            if (ui->stock_stackedWidget) ui->stock_stackedWidget->setCurrentIndex(0);

        });

    }

    if (ui->btnSave_add && !ui->btnSave_add->property("stockCrudConnected").toBool()) {
        connect(ui->btnSave_add, &QPushButton::clicked, this, [this]() {
            const QString reference = ui->inputRef_add ? ui->inputRef_add->text().trimmed() : QString();
            const QString nom = ui->inputNom_add ? ui->inputNom_add->text().trimmed() : QString();
            const int quantite = ui->inputStock_add ? ui->inputStock_add->text().trimmed().toInt() : 0;
            bool prixOk = false;
            const double prix = ui->inputPrix_add ? parseStockPrice(ui->inputPrix_add->text(), &prixOk) : 0.0;
            const QString fournisseurInput = ui->inputFournisseur_add ? ui->inputFournisseur_add->text().trimmed() : QString();

            if (reference.isEmpty()) {
                QMessageBox::warning(this, "Ajout Stock", "La reference est obligatoire.");
                return;
            }
            if (nom.isEmpty()) {
                QMessageBox::warning(this, "Ajout Stock", "Le nom est obligatoire.");
                return;
            }
            if (quantite < 0) {
                QMessageBox::warning(this, "Ajout Stock", "La quantite doit etre positive.");
                return;
            }
            if (!prixOk || prix <= 0.0) {
                QMessageBox::warning(this, "Ajout Stock", "Le prix doit etre un nombre strictement positif.");
                return;
            }

            Stmp.setIdMp(0);
            Stmp.setReference(reference);
            Stmp.setNom(nom);
            Stmp.setQuantite(quantite);
            Stmp.setSeuilCritique(defaultSeuilCritique(quantite));
            Stmp.setPrix(prix);
            Stmp.setFournisseurInput(fournisseurInput);

            if (!Stmp.ajouter()) {
                showFriendlySqlError(this, "ajouter le composant stock", Stmp.lastError());
                return;
            }

            if (ui->inputRef_add) ui->inputRef_add->clear();
            if (ui->inputNom_add) ui->inputNom_add->clear();
            if (ui->inputStock_add) ui->inputStock_add->clear();
            if (ui->inputPrix_add) ui->inputPrix_add->clear();
            if (ui->inputFournisseur_add) ui->inputFournisseur_add->clear();
            if (ui->sliderStock_add) ui->sliderStock_add->setValue(0);
            if (ui->sliderPrix_add) ui->sliderPrix_add->setValue(0);

            setupStockTableData();
            applyStockFilterAndSort();
            if (m_isStockCardView) refreshStockCardView();

            if (ui->stock_stackedWidget) ui->stock_stackedWidget->setCurrentIndex(0);
        });
        ui->btnSave_add->setProperty("stockCrudConnected", true);
    }

    if (ui->btnSave_mod && !ui->btnSave_mod->property("stockCrudConnected").toBool()) {
        connect(ui->btnSave_mod, &QPushButton::clicked, this, [this]() {
            const QString reference = ui->inputRef_mod ? ui->inputRef_mod->text().trimmed() : QString();
            const QString nom = ui->inputNom_mod ? ui->inputNom_mod->text().trimmed() : QString();
            const int quantite = ui->inputStock_mod ? ui->inputStock_mod->text().trimmed().toInt() : 0;
            bool prixOk = false;
            const double prix = ui->inputPrix_mod ? parseStockPrice(ui->inputPrix_mod->text(), &prixOk) : 0.0;
            const QString fournisseurInput = ui->inputFournisseur_mod ? ui->inputFournisseur_mod->text().trimmed() : QString();

            if (reference.isEmpty()) {
                QMessageBox::warning(this, "Modification Stock", "La reference est obligatoire.");
                return;
            }
            if (nom.isEmpty()) {
                QMessageBox::warning(this, "Modification Stock", "Le nom est obligatoire.");
                return;
            }
            if (quantite < 0) {
                QMessageBox::warning(this, "Modification Stock", "La quantite doit etre positive.");
                return;
            }
            if (!prixOk || prix <= 0.0) {
                QMessageBox::warning(this, "Modification Stock", "Le prix doit etre un nombre strictement positif.");
                return;
            }

            int idMp = ui->stock_stackedWidget ? ui->stock_stackedWidget->property("stockEditId").toInt() : -1;
            if (idMp <= 0) {
                if (!Stmp.findIdByReference(reference, idMp)) {
                    QMessageBox::warning(this, "Modification Stock", "Composant introuvable pour cette reference.");
                    return;
                }
            }

            int seuilCritique = ui->stock_stackedWidget ? ui->stock_stackedWidget->property("stockEditSeuil").toInt() : 0;
            if (seuilCritique <= 0) seuilCritique = defaultSeuilCritique(quantite);

            Stmp.setIdMp(idMp);
            Stmp.setReference(reference);
            Stmp.setNom(nom);
            Stmp.setQuantite(quantite);
            Stmp.setSeuilCritique(seuilCritique);
            Stmp.setPrix(prix);
            Stmp.setFournisseurInput(fournisseurInput);

            if (!Stmp.modifier()) {
                showFriendlySqlError(this, "modifier le composant stock", Stmp.lastError());
                return;
            }

            setupStockTableData();
            applyStockFilterAndSort();
            if (m_isStockCardView) refreshStockCardView();

            if (ui->stock_stackedWidget) {
                ui->stock_stackedWidget->setProperty("stockEditId", -1);
                ui->stock_stackedWidget->setProperty("stockEditSeuil", 0);
                ui->stock_stackedWidget->setCurrentIndex(0);
            }
        });
        ui->btnSave_mod->setProperty("stockCrudConnected", true);
    }



    applyStockFilterAndSort();

    // Setup card view toggle
    setupStockCardViewContainer();

}



void MainWindow::applyStockFilterAndSort()

{

    if (!ui->tableWidget) return;



    // Reset visibility first, then sort, then filter.

    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)

        ui->tableWidget->setRowHidden(row, false);



    if (ui->stock_sortCombo) {

        if (ui->stock_sortCombo->currentIndex() == 1) {

            ui->tableWidget->sortItems(2, Qt::AscendingOrder);

        } else if (ui->stock_sortCombo->currentIndex() == 2) {

            ui->tableWidget->sortItems(2, Qt::DescendingOrder);

        }

    }



    const QString keyword = ui->stock_searchInput ? ui->stock_searchInput->text().trimmed() : QString();

    if (keyword.isEmpty()) return;



    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {

        const QString ref = ui->tableWidget->item(row, 0) ? ui->tableWidget->item(row, 0)->text() : QString();

        const QString nom = ui->tableWidget->item(row, 1) ? ui->tableWidget->item(row, 1)->text() : QString();

        const QString stock = ui->tableWidget->item(row, 2) ? ui->tableWidget->item(row, 2)->text() : QString();

        const QString fournisseur = ui->tableWidget->item(row, 5) ? ui->tableWidget->item(row, 5)->text() : QString();



        const bool match =

            ref.contains(keyword, Qt::CaseInsensitive) ||

            nom.contains(keyword, Qt::CaseInsensitive) ||

            stock.contains(keyword, Qt::CaseInsensitive) ||

            fournisseur.contains(keyword, Qt::CaseInsensitive);



        ui->tableWidget->setRowHidden(row, !match);

    }

}



void MainWindow::setupStockTableData()

{

    if (!ui->tableWidget) return;



    ui->tableWidget->setColumnCount(7);

    ui->tableWidget->setRowCount(0);

    ui->tableWidget->setHorizontalHeaderLabels({"REF", "NOM", "STOCK", "SEUIL", "PRIX", "FOURNISSEUR", "ACTIONS"});

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);



    ui->tableWidget->verticalHeader()->setDefaultSectionSize(50);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);

    ui->tableWidget->setColumnWidth(6, 180);



    double totalValue = 0;

    int criticalCount = 0;

    QSqlQueryModel *model = Stmp.afficher();
    if (!model) {
        showFriendlySqlError(this, "charger le stock", "Modele SQL indisponible.");
        return;
    }
    if (model->lastError().isValid()) {
        showFriendlySqlError(this, "charger le stock", model->lastError().text());
        delete model;
        return;
    }

    for (int i = 0; i < model->rowCount(); ++i) {
        const int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        const int idMp = model->data(model->index(i, 0)).toInt();
        const QString ref = model->data(model->index(i, 1)).toString().trimmed();
        const QString nom = model->data(model->index(i, 2)).toString().trimmed();
        const int quantite = model->data(model->index(i, 3)).toInt();
        const int seuilCritique = model->data(model->index(i, 4)).toInt();
        const double prix = model->data(model->index(i, 5)).toDouble();
        const int idFour = model->data(model->index(i, 6)).toInt();
        const QString fournisseurNom = model->data(model->index(i, 7)).toString().trimmed();

        auto *refItem = new QTableWidgetItem(ref);
        refItem->setData(STOCK_ROLE_ID, idMp);
        refItem->setData(STOCK_ROLE_SEUIL, seuilCritique);
        refItem->setData(STOCK_ROLE_ID_FOUR, idFour);
        ui->tableWidget->setItem(row, 0, refItem);

        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(nom));

        auto *stockItem = new QTableWidgetItem();
        stockItem->setData(Qt::DisplayRole, quantite);
        ui->tableWidget->setItem(row, 2, stockItem);

        const QString seuilLabel = stockSeuilLabel(quantite, seuilCritique);
        setStockSeuilBadge(ui->tableWidget, row, seuilLabel);
        if (seuilLabel == "CRITIQUE") criticalCount++;

        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(QString("%1 TND").arg(QString::number(prix, 'f', 3))));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(fournisseurNom));

        totalValue += (quantite * prix);

        QWidget *container = new QWidget();
        auto *layout = new QHBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);

        auto *editBtn = new QPushButton("Modifier");
        auto *delBtn = new QPushButton("Supprimer");
        editBtn->setStyleSheet("color: #3182CE; border: none; font-weight: bold;");
        delBtn->setStyleSheet("color: #E53E3E; border: none; font-weight: bold;");

        connect(editBtn, &QPushButton::clicked, this, [this, container]() {
            if (!ui->tableWidget) return;
            int row = -1;
            for (int r = 0; r < ui->tableWidget->rowCount(); ++r) {
                if (ui->tableWidget->cellWidget(r, 6) == container) { row = r; break; }
            }
            if (row < 0) return;

            QTableWidgetItem *refItem = ui->tableWidget->item(row, 0);
            const int idMp = refItem ? refItem->data(STOCK_ROLE_ID).toInt() : -1;
            const int seuil = refItem ? refItem->data(STOCK_ROLE_SEUIL).toInt() : 0;

            if (ui->inputRef_mod) ui->inputRef_mod->setText(refItem ? refItem->text() : QString());
            if (ui->inputNom_mod) ui->inputNom_mod->setText(ui->tableWidget->item(row, 1) ? ui->tableWidget->item(row, 1)->text() : QString());
            if (ui->inputStock_mod) ui->inputStock_mod->setText(ui->tableWidget->item(row, 2) ? ui->tableWidget->item(row, 2)->text() : QString());
            if (ui->inputPrix_mod) ui->inputPrix_mod->setText(ui->tableWidget->item(row, 4) ? ui->tableWidget->item(row, 4)->text() : QString());
            if (ui->inputFournisseur_mod) ui->inputFournisseur_mod->setText(ui->tableWidget->item(row, 5) ? ui->tableWidget->item(row, 5)->text() : QString());

            if (ui->sliderStock_mod && ui->tableWidget->item(row, 2))
                ui->sliderStock_mod->setValue(ui->tableWidget->item(row, 2)->text().toInt());
            if (ui->sliderPrix_mod && ui->tableWidget->item(row, 4))
                ui->sliderPrix_mod->setValue(static_cast<int>(parseStockPrice(ui->tableWidget->item(row, 4)->text())));

            if (ui->stock_stackedWidget) {
                ui->stock_stackedWidget->setProperty("stockEditId", idMp);
                ui->stock_stackedWidget->setProperty("stockEditSeuil", seuil);
                ui->stock_stackedWidget->setCurrentIndex(2);
            }
        });

        connect(delBtn, &QPushButton::clicked, this, [this, container]() {
            if (!ui->tableWidget) return;
            int row = -1;
            for (int r = 0; r < ui->tableWidget->rowCount(); ++r) {
                if (ui->tableWidget->cellWidget(r, 6) == container) { row = r; break; }
            }
            if (row < 0) return;

            if (QMessageBox::question(this, "Supprimer", "Etes-vous sur de supprimer cet element ?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
                return;

            QTableWidgetItem *refItem = ui->tableWidget->item(row, 0);
            int idMp = refItem ? refItem->data(STOCK_ROLE_ID).toInt() : -1;
            if (idMp <= 0 && refItem) {
                Stmp.findIdByReference(refItem->text(), idMp);
            }
            if (idMp <= 0) {
                QMessageBox::warning(this, "Suppression Stock", "ID composant introuvable.");
                return;
            }

            if (!Stmp.supprimer(idMp)) {
                showFriendlySqlError(this, "supprimer le composant stock", Stmp.lastError());
                return;
            }

            setupStockTableData();
            applyStockFilterAndSort();
            if (m_isStockCardView) refreshStockCardView();
        });

        layout->addWidget(editBtn);
        layout->addWidget(delBtn);
        ui->tableWidget->setCellWidget(row, 6, container);
    }

    delete model;



    if (ui->totalStock) ui->totalStock->setText(QString::number(totalValue, 'f', 3) + " TND");

    if (ui->lblCriticalStock) ui->lblCriticalStock->setText(QString("⚠ %1 Produits Critiques").arg(criticalCount));



    if (ui->lblOrderSummary) {

        if(criticalCount > 0) {

            ui->lblOrderSummary->setText(QString("Urgent : %1 articles à commander").arg(criticalCount));

            ui->lblOrderSummary->setStyleSheet("font-size: 13px; color: #E53E3E; font-weight: bold; margin-bottom: 5px;");

        } else {

            ui->lblOrderSummary->setText("Stock suffisant");

            ui->lblOrderSummary->setStyleSheet("font-size: 13px; color: #28a745; margin-bottom: 5px;");

        }

    }



    applyStockFilterAndSort();

}



// --- CLIENT MODULE (Integrated from mainwindowcl) ---

void MainWindow::on_btnClient_clicked() {

    if (auto *sw = mainStacked()) {

        if (auto *page = sw->findChild<QWidget*>("pageClient", Qt::FindDirectChildrenOnly)) {

            sw->setCurrentWidget(page);

        } else if (auto *page = sw->findChild<QWidget*>("client", Qt::FindDirectChildrenOnly)) {

            sw->setCurrentWidget(page);

        } else {

             // Fallback index if named widgets fail, assuming Client is index 1

             sw->setCurrentIndex(1);

        }

    }



    // --- APPLY CREATIVE STYLING TO CLIENT SIDEBAR ---

    QTimer::singleShot(100, this, [this]() {

        // 1. Main Sidebar Container (White & Clean)

        if (QWidget *sidePanel = this->findChild<QWidget*>("sidepanel")) {

            sidePanel->setStyleSheet(

                "QWidget#sidepanel {"

                "   background-color: #ffffff;"

                "   border-radius: 20px;"

                "   border: 1px solid #e2e8f0;"

                "}"

            );

            

            // Style Title "Pilotage Commercial" -> Dark Blue Text

            if (QLabel *lblTitle = sidePanel->findChild<QLabel*>("pilotage")) {

                lblTitle->setStyleSheet("color: #0f2b4c; font-size: 16px; font-weight: bold; background: transparent; margin-bottom: 10px;");

                lblTitle->setAlignment(Qt::AlignCenter);

            }

        }



        // 2. Creative Stat Card (Growth Widget)

        if (QWidget *growthWidget = this->findChild<QWidget*>("growthWidget")) {

             growthWidget->setStyleSheet(

                "QWidget#growthWidget {"

                "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #e0f2fe, stop:1 #bae6fd);"

                "   border-radius: 15px;"

                "   border: 1px solid #bae6fd;"

                "}"

             );

             

             // Style inner labels

             QList<QLabel*> labels = growthWidget->findChildren<QLabel*>();

             for (QLabel *lbl : labels) {

                 QString text = lbl->text();

                 if (text.contains("%") || text.contains("+")) {

                     lbl->setStyleSheet("color: #0c4a6e; font-size: 20px; font-weight: 900; background: transparent;");

                 } else {

                     lbl->setStyleSheet("color: #0c4a6e; font-size: 12px; font-weight: 600; opacity: 0.8; background: transparent;");

                 }

             }

        }



        // 3. Action Buttons (Export, Stats) -> Blue Gradient Style
        QString clientBtnStyle = 
            "QPushButton {"
            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0f2b4c, stop:1 #2a5298);"
            "   color: white; border-radius: 12px; padding: 14px; font-weight: 700; font-size: 13px;"
            "   border: none;"
            "}"
            "QPushButton:hover {"
            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #1e40af, stop:1 #3b82f6);"
            "}";
        if (auto *btn = this->findChild<QPushButton*>("exportclient")) btn->setStyleSheet(clientBtnStyle);
        if (auto *btn = this->findChild<QPushButton*>("btnGoStats_Client")) btn->setStyleSheet(clientBtnStyle);



        // Ensure Table Actions Column is wide enough AND Rows are taller

        if (ui->tableWidget_Client) {

             ui->tableWidget_Client->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed); // 6 is Actions

             ui->tableWidget_Client->setColumnWidth(6, 180);

             

             // Taller rows to show "Modifier" better

             ui->tableWidget_Client->verticalHeader()->setDefaultSectionSize(50);

        }



        // 4. Ensure all other labels are transparent

        if (QWidget *sidePanel = this->findChild<QWidget*>("sidepanel")) {

             QList<QLabel*> allLabels = sidePanel->findChildren<QLabel*>();

             for(QLabel* lbl : allLabels) {

                 // Force transparency to override global styles

                 lbl->setStyleSheet(lbl->styleSheet() + " background: transparent; border: none;");

             }

        }

    });

}



void MainWindow::on_btnNouveau_client_clicked() {

    if (ui->stackedWidget_Client) {

        ui->input_matricule_ajouter->clear();

        ui->input_nom_ajouter->clear();

        ui->input_email_ajouter->clear();

        ui->stackedWidget_Client->setCurrentIndex(1);

    }

}



void MainWindow::on_btn_annuler_client_clicked() {

    if (ui->stackedWidget_Client) ui->stackedWidget_Client->setCurrentIndex(0);

}



void MainWindow::on_btn_ajouter_client_clicked() {

    if(ui->input_matricule_ajouter->text().isEmpty() || ui->input_nom_ajouter->text().isEmpty()) {

         QMessageBox::warning(this, "Champs Manquants", "Veuillez remplir au moins le Matricule et le Nom.");

         return;

    }

    CLtmp.setMatricule(ui->input_matricule_ajouter->text().trimmed());
    CLtmp.setNom(ui->input_nom_ajouter->text().trimmed());
    CLtmp.setEmail(ui->input_email_ajouter->text().trimmed());
    CLtmp.setTypeContrat(ui->input_contrat_ajouter->currentText());
    CLtmp.setStatutPaiement(ui->input_paiement_ajouter->currentText());
    CLtmp.setIdClient(0); // auto-generate

    if (CLtmp.ajouter()) {
        QMessageBox::information(this, "Succès", "Client ajouté avec succès.");
        refreshClients();
    } else {
        QMessageBox::critical(this, "Erreur", "Echec ajout client:\n" + CLtmp.lastError());
    }

    if (ui->stackedWidget_Client) ui->stackedWidget_Client->setCurrentIndex(0);

}



void MainWindow::on_btn_modifier_client_clicked() {

    if(ui->input_matricule_modifier->text().isEmpty() || ui->input_nom_modifier->text().isEmpty()) {

         QMessageBox::warning(this, "Champs Manquants", "Veuillez remplir au moins le Matricule et le Nom.");

         return;

    }

    // Retrieve stored ID from hidden column
    int idClient = 0;
    if (currentClientRow >= 0 && currentClientRow < ui->tableWidget_Client->rowCount()) {
        QTableWidgetItem *idItem = ui->tableWidget_Client->item(currentClientRow, 5);
        if (idItem) idClient = idItem->text().toInt();
    }

    CLtmp.setIdClient(idClient);
    CLtmp.setMatricule(ui->input_matricule_modifier->text().trimmed());
    CLtmp.setNom(ui->input_nom_modifier->text().trimmed());
    CLtmp.setEmail(ui->input_email_modifier->text().trimmed());
    CLtmp.setTypeContrat(ui->input_contrat_modifier->currentText());
    CLtmp.setStatutPaiement(ui->input_paiement_modifier->currentText());

    if (CLtmp.modifier()) {
        QMessageBox::information(this, "Succès", "Informations du client modifiées avec succès.");
        refreshClients();
    } else {
        QMessageBox::critical(this, "Erreur", "Echec modification client:\n" + CLtmp.lastError());
    }

    if (ui->stackedWidget_Client) ui->stackedWidget_Client->setCurrentIndex(0);

}



void MainWindow::onClientAdded(QString matricule, QString nom, QString email, QString bacs, QString score, QString paiement) {
    Q_UNUSED(bacs); Q_UNUSED(score);
    // Now handled by DB - just refresh
    Q_UNUSED(matricule); Q_UNUSED(nom); Q_UNUSED(email); Q_UNUSED(paiement);
    refreshClients();
}



void MainWindow::onClientModified(int row, QString matricule, QString nom, QString email, QString bacs, QString score, QString paiement) {
    Q_UNUSED(row); Q_UNUSED(matricule); Q_UNUSED(nom); Q_UNUSED(email);
    Q_UNUSED(bacs); Q_UNUSED(score); Q_UNUSED(paiement);
    // Now handled by DB - just refresh
    refreshClients();
}



void MainWindow::addClientActionButtons(int row) {

    QWidget* actionWidget = new QWidget();

    QHBoxLayout* layout = new QHBoxLayout(actionWidget);

    layout->setContentsMargins(4, 2, 4, 2);

    layout->setSpacing(6);

    layout->setAlignment(Qt::AlignCenter);



    QPushButton* btnEdit = new QPushButton("Modifier");

    btnEdit->setCursor(Qt::PointingHandCursor);

    btnEdit->setStyleSheet("QPushButton { background-color: #3498db; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; }");

    

    QPushButton* btnDelete = new QPushButton("Supprimer");

    btnDelete->setCursor(Qt::PointingHandCursor);

    btnDelete->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; }");

    

    layout->addWidget(btnEdit);

    layout->addWidget(btnDelete);

    

    ui->tableWidget_Client->setCellWidget(row, 6, actionWidget);

    connect(btnEdit, &QPushButton::clicked, this, [this, actionWidget](){

        int r = getRowForClientWidget(actionWidget);

        if (r < 0) return;

        currentClientRow = r;

        ui->input_matricule_modifier->setText(ui->tableWidget_Client->item(r, 0)->text());

        ui->input_nom_modifier->setText(ui->tableWidget_Client->item(r, 1)->text());

        ui->input_email_modifier->setText(ui->tableWidget_Client->item(r, 2)->text());

        ui->input_contrat_modifier->setCurrentText(ui->tableWidget_Client->item(r, 3)->text());

        ui->input_paiement_modifier->setCurrentText(ui->tableWidget_Client->item(r, 4)->text());

        if (ui->stackedWidget_Client) ui->stackedWidget_Client->setCurrentIndex(2);

    });

    connect(btnDelete, &QPushButton::clicked, this, [this, actionWidget](){

        int r = getRowForClientWidget(actionWidget);

        if (r < 0) return;

        if (QMessageBox::question(this, "Supprimer", "Voulez-vous supprimer ce client ?") == QMessageBox::Yes) {

            // Get the stored ID from hidden column
            int idClient = 0;
            QTableWidgetItem *idItem = ui->tableWidget_Client->item(r, 5);
            if (idItem) idClient = idItem->text().toInt();
            if (idClient > 0 && CLtmp.supprimer(idClient)) {
                refreshClients();
            } else {
                QMessageBox::critical(this, "Erreur", "Echec suppression client:\n" + CLtmp.lastError());
            }

        }

    });

}



int MainWindow::getRowForClientWidget(QWidget *widget) {

    for (int i = 0; i < ui->tableWidget_Client->rowCount(); i++) {

        if (ui->tableWidget_Client->cellWidget(i, 6) == widget) return i;

    }

    return -1;

}


// ---------- refreshClients: load CLIENT table from DB ----------
void MainWindow::refreshClients() {
    if (!ui->tableWidget_Client) return;
    ui->tableWidget_Client->setRowCount(0);

    QSqlQueryModel *model = CLtmp.afficher();
    // model columns: 0=ID_CLIENT, 1=MATRICULE, 2=NOM, 3=EMAIL, 4=TYPE_CONTRAT, 5=STATUT_PAIEMENT
    for (int i = 0; i < model->rowCount(); ++i) {
        int row = ui->tableWidget_Client->rowCount();
        ui->tableWidget_Client->insertRow(row);
        int idClient = model->data(model->index(i, 0)).toInt();
        ui->tableWidget_Client->setItem(row, 0, new QTableWidgetItem(model->data(model->index(i, 1)).toString())); // Matricule
        ui->tableWidget_Client->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 2)).toString())); // Nom
        ui->tableWidget_Client->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 3)).toString())); // Email
        ui->tableWidget_Client->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 4)).toString())); // Type Contrat
        ui->tableWidget_Client->setItem(row, 4, new QTableWidgetItem(model->data(model->index(i, 5)).toString())); // Statut Paiement
        ui->tableWidget_Client->setItem(row, 5, new QTableWidgetItem(QString::number(idClient)));                  // Hidden ID
        addClientActionButtons(row);
    }
    delete model;
}


// ---------- filterClients: advanced search + filter by Type de Contrat ----------
void MainWindow::filterClients() {
    if (!ui->tableWidget_Client) return;
    QString searchText = ui->recherche ? ui->recherche->text().trimmed().toLower() : QString();
    QString selectedContrat = ui->cbTrier ? ui->cbTrier->currentText() : "Tous";

    for (int i = 0; i < ui->tableWidget_Client->rowCount(); ++i) {
        bool matchSearch = true;
        bool matchContrat = true;

        // Search across Matricule (0), Nom (1), Email (2)
        if (!searchText.isEmpty()) {
            matchSearch = false;
            for (int col : {0, 1, 2}) {
                QTableWidgetItem *item = ui->tableWidget_Client->item(i, col);
                if (item && item->text().toLower().contains(searchText)) {
                    matchSearch = true;
                    break;
                }
            }
        }

        // Filter by Type de Contrat (column 3)
        if (selectedContrat != "Tous") {
            QTableWidgetItem *contratItem = ui->tableWidget_Client->item(i, 3);
            if (contratItem) {
                matchContrat = contratItem->text().compare(selectedContrat, Qt::CaseInsensitive) == 0;
            } else {
                matchContrat = false;
            }
        }

        ui->tableWidget_Client->setRowHidden(i, !(matchSearch && matchContrat));
    }
}


// ---------- exportClientPdf: PDF export filtered by selected Type de Contrat ----------
void MainWindow::exportClientPdf() {
    if (!ui->tableWidget_Client) return;

    // Determine which contrat type to export
    QString selectedContrat = ui->cbTrier ? ui->cbTrier->currentText() : "Tous";

    // Collect visible rows matching the filter
    QList<int> rowsToExport;
    for (int i = 0; i < ui->tableWidget_Client->rowCount(); ++i) {
        if (!ui->tableWidget_Client->isRowHidden(i)) {
            rowsToExport.append(i);
        }
    }

    if (rowsToExport.isEmpty()) {
        QMessageBox::warning(this, "Export PDF", "Aucun client à exporter pour le filtre sélectionné.");
        return;
    }

    QString defaultName = (selectedContrat == "Tous")
        ? "Rapport_Clients.pdf"
        : "Rapport_Clients_" + selectedContrat + ".pdf";
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter PDF Clients", defaultName, "PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;

    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize::A4);
    writer.setPageMargins(QMarginsF(25, 25, 25, 25));
    writer.setResolution(300);

    QPainter painter(&writer);
    int pageW = writer.width();

    // ── Colors ──
    QColor headerBg(15, 43, 76);
    QColor lightGray(245, 247, 250);
    QColor borderGray(220, 225, 230);
    QColor textDark(30, 30, 30);
    QColor white(255, 255, 255);
    QColor green(39, 174, 96);
    QColor red(231, 76, 60);

    int y = 0;

    // ── Header banner ──
    painter.fillRect(0, 0, pageW, 520, headerBg);
    painter.setPen(white);
    painter.setFont(QFont("Segoe UI", 24, QFont::Bold));
    painter.drawText(QRect(200, 60, pageW - 400, 180), Qt::AlignLeft | Qt::AlignVCenter, "WasteGuard");
    painter.setFont(QFont("Segoe UI", 12));
    QString subtitle = (selectedContrat == "Tous")
        ? "Rapport des Clients — Tous les contrats"
        : "Rapport des Clients — Contrat " + selectedContrat;
    painter.drawText(QRect(200, 240, pageW - 400, 120), Qt::AlignLeft | Qt::AlignVCenter, subtitle);
    painter.setFont(QFont("Segoe UI", 9));
    painter.drawText(QRect(200, 380, pageW - 400, 100), Qt::AlignLeft | Qt::AlignVCenter,
        "Date : " + QDate::currentDate().toString("dd MMMM yyyy") + "   |   Clients : " + QString::number(rowsToExport.size()));
    y = 660;

    // ── Summary cards ──
    int paidCount = 0, lateCount = 0;
    for (int row : rowsToExport) {
        QTableWidgetItem *item = ui->tableWidget_Client->item(row, 4);
        if (item && item->text().contains("Retard", Qt::CaseInsensitive)) lateCount++;
        else paidCount++;
    }
    int totalFiltered = rowsToExport.size();

    int cardW = (pageW - 300) / 3;
    int cardH = 300;
    int cardY = y;

    auto drawCard = [&](int idx, int value, const QString &label, QColor valueColor) {
        int cx = 100 + idx * (cardW + 50);
        painter.fillRect(cx, cardY, cardW, cardH, white);
        painter.setPen(QPen(borderGray, 3));
        painter.drawRoundedRect(cx, cardY, cardW, cardH, 20, 20);
        painter.setPen(valueColor);
        painter.setFont(QFont("Segoe UI", 20, QFont::Bold));
        painter.drawText(QRect(cx, cardY + 30, cardW, 140), Qt::AlignCenter, QString::number(value));
        painter.setFont(QFont("Segoe UI", 9));
        painter.setPen(textDark);
        painter.drawText(QRect(cx, cardY + 170, cardW, 100), Qt::AlignCenter, label);
    };
    drawCard(0, totalFiltered, "Total Clients", headerBg);
    drawCard(1, paidCount, "Payé", green);
    drawCard(2, lateCount, "En Retard", red);

    y = cardY + cardH + 160;

    // ── Table header ──
    QStringList headers = {"Matricule", "Nom", "Email", "Type Contrat", "Statut Paiement"};
    int colCount = headers.size();
    int colW = (pageW - 200) / colCount;
    int rowH = 240;
    int xStart = 100;

    auto drawTableHeader = [&]() {
        painter.fillRect(xStart, y, pageW - 200, rowH, headerBg);
        painter.setPen(white);
        painter.setFont(QFont("Segoe UI", 9, QFont::Bold));
        for (int c = 0; c < colCount; ++c)
            painter.drawText(QRect(xStart + c * colW + 40, y, colW - 80, rowH), Qt::AlignVCenter | Qt::AlignLeft, headers[c]);
        y += rowH;
    };
    drawTableHeader();

    // ── Table rows ──
    int visibleIdx = 0;
    for (int row : rowsToExport) {
        if (y + rowH > writer.height() - 400) {
            painter.setPen(borderGray);
            painter.drawLine(100, writer.height() - 300, pageW - 100, writer.height() - 300);
            painter.setPen(QColor(150, 150, 150));
            painter.setFont(QFont("Segoe UI", 7));
            painter.drawText(QRect(100, writer.height() - 280, pageW - 200, 200), Qt::AlignCenter,
                "WasteGuard — Page " + QString::number(writer.logicalDpiX())); // placeholder
            writer.newPage();
            y = 200;
            drawTableHeader();
        }

        QColor rowBg = (visibleIdx % 2 == 0) ? white : lightGray;
        painter.fillRect(xStart, y, pageW - 200, rowH, rowBg);
        painter.setPen(QPen(borderGray, 1));
        painter.drawRect(xStart, y, pageW - 200, rowH);
        painter.setPen(textDark);
        painter.setFont(QFont("Segoe UI", 8));

        for (int c = 0; c < 5; ++c) {
            QString txt;
            QTableWidgetItem *item = ui->tableWidget_Client->item(row, c);
            if (item) txt = item->text();

            if (c == 4) {
                painter.setPen(txt.contains("Retard", Qt::CaseInsensitive) ? red : green);
                painter.setFont(QFont("Segoe UI", 8, QFont::Bold));
            }
            painter.drawText(QRect(xStart + c * colW + 40, y, colW - 80, rowH), Qt::AlignVCenter | Qt::AlignLeft, txt);
            if (c == 4) {
                painter.setPen(textDark);
                painter.setFont(QFont("Segoe UI", 8));
            }
        }
        y += rowH;
        visibleIdx++;
    }

    // ── Footer ──
    painter.setPen(borderGray);
    painter.drawLine(100, writer.height() - 300, pageW - 100, writer.height() - 300);
    painter.setPen(QColor(150, 150, 150));
    painter.setFont(QFont("Segoe UI", 7));
    QString footerText = "WasteGuard — Généré le " + QDate::currentDate().toString("dd/MM/yyyy");
    if (selectedContrat != "Tous") footerText += " — Filtre : " + selectedContrat;
    painter.drawText(QRect(100, writer.height() - 280, pageW - 200, 200), Qt::AlignCenter, footerText);

    painter.end();
    QMessageBox::information(this, "Export PDF",
        "Le rapport PDF (" + QString::number(rowsToExport.size()) + " clients) a été généré avec succès.");
}


// ---------- showClientStats: EcoScore stats based on correct sorting rate ----------
void MainWindow::showClientStats() {
    if (!ui->stackedWidget_Client) return;

    // ── Query data from DB ──
    QSqlQuery query;

    // 1. Pie chart: Statut Paiement distribution
    query.exec("SELECT STATUT_PAIEMENT, COUNT(*) FROM CLIENT GROUP BY STATUT_PAIEMENT");
    QPieSeries *pieSeries = new QPieSeries();
    while (query.next()) {
        QString statut = query.value(0).toString();
        int count = query.value(1).toInt();
        pieSeries->append(statut + " (" + QString::number(count) + ")", count);
    }
    // Style pie slices
    QList<QColor> pieColors = {QColor("#27ae60"), QColor("#e74c3c"), QColor("#f39c12"), QColor("#3498db")};
    for (int i = 0; i < pieSeries->slices().size(); ++i) {
        QPieSlice *slice = pieSeries->slices().at(i);
        slice->setLabelVisible(true);
        slice->setBrush(pieColors[i % pieColors.size()]);
        slice->setLabelFont(QFont("Segoe UI", 9, QFont::Bold));
        if (i == 0) slice->setExploded(true);
    }

    QChart *pieChart = new QChart();
    pieChart->addSeries(pieSeries);
    pieChart->setTitle("Répartition Statut de Paiement");
    pieChart->setTitleFont(QFont("Segoe UI", 13, QFont::Bold));
    pieChart->setTitleBrush(QColor("#0f2b4c"));
    pieChart->legend()->setAlignment(Qt::AlignBottom);
    pieChart->legend()->setFont(QFont("Segoe UI", 9));
    pieChart->setAnimationOptions(QChart::SeriesAnimations);
    pieChart->setBackgroundBrush(Qt::white);

    // Replace old chart
    if (ui->chartEcoScorePie->chart()) delete ui->chartEcoScorePie->chart();
    ui->chartEcoScorePie->setChart(pieChart);
    ui->chartEcoScorePie->setRenderHint(QPainter::Antialiasing);

    // 2. Bar chart: EcoScore per client (simulated from TYPE_CONTRAT + STATUT_PAIEMENT)
    //    EcoScore = base score per contract type * payment multiplier
    //    Annuel=90, Trimestriel=70, Mensuel=50; Payé=1.0, En Retard=0.6
    query.exec("SELECT NOM, TYPE_CONTRAT, STATUT_PAIEMENT FROM CLIENT ORDER BY NOM");
    QBarSet *ecoScoreSet = new QBarSet("EcoScore");
    ecoScoreSet->setColor(QColor("#2a5298"));
    QStringList clientNames;
    while (query.next()) {
        QString nom = query.value(0).toString();
        QString contrat = query.value(1).toString();
        QString statut = query.value(2).toString();

        double baseScore = 50;
        if (contrat.contains("Annuel", Qt::CaseInsensitive)) baseScore = 90;
        else if (contrat.contains("Trimestriel", Qt::CaseInsensitive)) baseScore = 70;

        double multiplier = statut.contains("Retard", Qt::CaseInsensitive) ? 0.6 : 1.0;
        double score = baseScore * multiplier;

        *ecoScoreSet << score;
        clientNames << (nom.length() > 10 ? nom.left(10) + ".." : nom);
    }

    QBarSeries *barSeries = new QBarSeries();
    barSeries->append(ecoScoreSet);
    barSeries->setLabelsVisible(true);
    barSeries->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);

    QChart *barChart = new QChart();
    barChart->addSeries(barSeries);
    barChart->setTitle("EcoScore par Client (Taux de Tri Correct)");
    barChart->setTitleFont(QFont("Segoe UI", 13, QFont::Bold));
    barChart->setTitleBrush(QColor("#0f2b4c"));
    barChart->setAnimationOptions(QChart::SeriesAnimations);
    barChart->setBackgroundBrush(Qt::white);
    barChart->legend()->setVisible(false);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(clientNames);
    axisX->setLabelsFont(QFont("Segoe UI", 8));
    barChart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setTitleText("Score");
    axisY->setTitleFont(QFont("Segoe UI", 10, QFont::Bold));
    axisY->setLabelFormat("%d");
    barChart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);

    if (ui->chartEcoScoreBar->chart()) delete ui->chartEcoScoreBar->chart();
    ui->chartEcoScoreBar->setChart(barChart);
    ui->chartEcoScoreBar->setRenderHint(QPainter::Antialiasing);

    // 3. Bottom chart: Distribution by Type de Contrat
    query.exec("SELECT TYPE_CONTRAT, COUNT(*) FROM CLIENT GROUP BY TYPE_CONTRAT");
    QPieSeries *contratSeries = new QPieSeries();
    QList<QColor> contratColors = {QColor("#0f2b4c"), QColor("#2a5298"), QColor("#3498db"), QColor("#85c1e9")};
    int ci = 0;
    while (query.next()) {
        QString type = query.value(0).toString();
        int count = query.value(1).toInt();
        QPieSlice *slice = contratSeries->append(type + " (" + QString::number(count) + ")", count);
        slice->setLabelVisible(true);
        slice->setBrush(contratColors[ci % contratColors.size()]);
        slice->setLabelFont(QFont("Segoe UI", 9, QFont::Bold));
        ci++;
    }

    QChart *contratChart = new QChart();
    contratChart->addSeries(contratSeries);
    contratChart->setTitle("Distribution par Type de Contrat");
    contratChart->setTitleFont(QFont("Segoe UI", 13, QFont::Bold));
    contratChart->setTitleBrush(QColor("#0f2b4c"));
    contratChart->legend()->setAlignment(Qt::AlignRight);
    contratChart->legend()->setFont(QFont("Segoe UI", 9));
    contratChart->setAnimationOptions(QChart::SeriesAnimations);
    contratChart->setBackgroundBrush(Qt::white);

    if (ui->chartContratDist->chart()) delete ui->chartContratDist->chart();
    ui->chartContratDist->setChart(contratChart);
    ui->chartContratDist->setRenderHint(QPainter::Antialiasing);

    // Navigate to stats page (index 3)
    ui->stackedWidget_Client->setCurrentWidget(ui->page_stats_client);
}


void MainWindow::forceApplySidebarStyles()

{

    // Premium Styles - Simplified for maximum reliability

    QString btnStyle = 

        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0f2b4c, stop:1 #2a5298);"

        "color: #ffffff;"

        "border-radius: 12px;"

        "padding: 12px;"

        "font-weight: bold;"

        "font-size: 13px;"

        "border: 1px solid #1e40af;"

        "min-height: 40px;";



    QString sideStyle = 

        "background-color: #ffffff;"

        "border-radius: 20px;"

        "border: 1px solid #e2e8f0;";



    QString labelStyle = 

        "background: transparent;"

        "color: #0f2b4c;"

        "font-weight: 600;"

        "border: none;";



    QString cardStyle = 

        "background-color: #ffffff;"

        "border: 1px solid #edf2f7;"

        "border-radius: 12px;";



    QStringList sidebarNames = {"rightSidebar", "rightSidebar_Commande", "sidePanel_Employe", "sidePanel_Client", "sideDashboard", "sidepanel"};

    

    for (const QString &name : sidebarNames) {

        QList<QWidget*> panels = this->findChildren<QWidget*>(name);

        for (QWidget *panel : panels) {

            // 1. STYLE THE PANEL ITSELF

            panel->setStyleSheet(sideStyle);

            panel->setAttribute(Qt::WA_StyledBackground, true);



            // 2. STYLE ALL BUTTONS INSIDE (DIRECTLY)

            QList<QPushButton*> buttons = panel->findChildren<QPushButton*>();

            for (QPushButton *btn : buttons) {

                btn->setStyleSheet(btnStyle);

                btn->setCursor(Qt::PointingHandCursor);

                // Also handle hover state directly on the object to ensure it works

                btn->setStyleSheet(btn->styleSheet() + " QPushButton:hover { background: #1e40af; }");

            }



            // 3. STYLE ALL LABELS INSIDE (DIRECTLY)

            QList<QLabel*> labels = panel->findChildren<QLabel*>();

            for (QLabel *lbl : labels) {

                lbl->setStyleSheet(labelStyle);

            }



            // 4. STYLE ALL CARDS INSIDE (DIRECTLY)

            QList<QFrame*> cards = panel->findChildren<QFrame*>();

            for (QFrame *card : cards) {

                if (card->objectName().startsWith("stat") || card->objectName().startsWith("st_")) {

                    card->setStyleSheet(cardStyle);

                }

            }

        }

    }



    // --- CATCH-ALL FOR GLOBAL BUTTONS BY NAME ---

    QStringList globalBtnNames = {

        "btnPdf", "btnPdf_Cmd", "btnExport", "btnOrder", "btnFichePaie",

        "btnGoStats_Stock", "btnGoStats_Maint", "btnGoStats_Client", "btnGoStats_Cmd"

    };

    for (const QString &btnName : globalBtnNames) {

        if (QPushButton *btn = this->findChild<QPushButton*>(btnName)) {

            btn->setStyleSheet(btnStyle);

            btn->setCursor(Qt::PointingHandCursor);

        }

    }

    // Force update sidebar state to fix any overrides
    updateSidebarState();
}


void MainWindow::on_btnToggleSidebar_clicked()
{
    m_sidebarExpanded = !m_sidebarExpanded;
    updateSidebarState();
}

void MainWindow::updateSidebarState()
{
    int targetWidth = m_sidebarExpanded ? 250 : 70;
    
    // Animate minimum width for smooth transition
    QPropertyAnimation *animation = new QPropertyAnimation(ui->sidebar, "minimumWidth");
    animation->setDuration(300);
    animation->setStartValue(ui->sidebar->width());
    animation->setEndValue(targetWidth);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    ui->sidebar->setMaximumWidth(targetWidth);

    // Toggle Logo Visibility
    if (ui->label_logo) {
         ui->label_logo->setVisible(m_sidebarExpanded);
    }

    // Toggle Button Text
    struct ButtonInfo {
        QPushButton* btn;
        QString fullText;
        QString emoji;
    };

    QList<ButtonInfo> buttons;
    // Append \uFE0E (Effectively \xe2\x96\x8e in utf8 literal? No, use u string or just the char)
    // Actually, in C++ source with UTF-8, we can just embed it if we are careful, or use QString properties.
    // \uFE0E is the "Text Presentation Selector".
    const QString vs = QChar(0xFE0E);

    if (ui->btnAccueil) buttons.append({ui->btnAccueil, "🏠" + vs + " Accueils", "🏠" + vs});
    if (ui->btnStock) buttons.append({ui->btnStock, "📦" + vs + " Stock", "📦" + vs});
    if (ui->btnProduits) buttons.append({ui->btnProduits, "🛒" + vs + " Produits", "🛒" + vs});
    if (ui->btnClient) buttons.append({ui->btnClient, "👥" + vs + " Clients", "👥" + vs});
    if (ui->btnEmployes) buttons.append({ui->btnEmployes, "👷" + vs + " Employes", "👷" + vs});
    if (ui->btnStatistiques) buttons.append({ui->btnStatistiques, "📊" + vs + " Statistiques", "📊" + vs});
    if (ui->btnMaintenance) buttons.append({ui->btnMaintenance, "🔧" + vs + " Maintenances", "🔧" + vs});
    
    if (QPushButton* btnCmd = this->findChild<QPushButton*>("btnCommandes")) {
        buttons.append({btnCmd, "📝" + vs + " Commandes", "📝" + vs});
    }

    QString commonStyle = 
        "QPushButton {"
        "   color: white !important;"
        "   background-color: transparent;"
        "   border: none;"
        "   font-size: 14px;"
        "   font-family: 'Segoe UI Symbol', 'Segoe UI', sans-serif;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1a4270;"
        "   border-left: 3px solid #3498db;"
        "}"
        "QPushButton:checked {"
        "   background-color: #1a4270;"
        "   border-left: 3px solid #3498db;"
        "}";

    for (const auto& info : buttons) {
        if (!info.btn) continue;
        info.btn->setText(m_sidebarExpanded ? info.fullText : info.emoji);
        info.btn->setIcon(QIcon()); // Force remove any existing icon to rely on emoji text
        
        if (m_sidebarExpanded) {
            // Expanded: Left aligned, padding to look nice
            info.btn->setStyleSheet(commonStyle + 
                "QPushButton { text-align: left; padding-left: 20px; }");
        } else {
            // Collapsed: Center aligned, no padding (to center emoji)
            info.btn->setStyleSheet(commonStyle + 
                "QPushButton { text-align: center; padding-left: 0px; padding-right: 0px; font-size: 18px; }");
        }
    }
    
    // Toggle button icon
    if (QPushButton* btnToggle = ui->sidebar->findChild<QPushButton*>("btnToggleSidebar")) {
        btnToggle->setText(m_sidebarExpanded ? "☰" : "➡");
    }
}

// ---------- Card View Implementation ----------
void MainWindow::setupCardViewContainer()
{
    if (ui && ui->prod_cardLayout) {
        m_cardLayout = ui->prod_cardLayout;
    } else {
        // Fallback search if UI pointer is limited or renamed in merge
        m_cardLayout = findChild<QGridLayout*>("prod_cardLayout");
    }
}

void MainWindow::slot_toggleView()
{
    if (!ui || !ui->prod_viewStackedWidget) return;
    
    m_isCardView = !m_isCardView;
    ui->prod_viewStackedWidget->setCurrentIndex(m_isCardView ? 1 : 0);
    
    if (m_isCardView) {
        refreshCardView();
    }
    
    if (ui->prod_btnToggleView) {
        ui->prod_btnToggleView->setText(m_isCardView ? "≣" : "⊞");
    }
}

void MainWindow::refreshCardView()
{
    if (!m_isCardView) return;
    if (!m_cardLayout) setupCardViewContainer();
    if (!m_cardLayout) return;

    // Clear existing layout items
    QLayoutItem *child;
    while ((child = m_cardLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }

    QTableWidget *t = produitTable();
    if (!t) return;

    int totalItems = t->rowCount();
    int startIdx = m_currentPage * m_itemsPerPage;
    int endIdx = qMin(startIdx + m_itemsPerPage, totalItems);

    for (int i = startIdx; i < endIdx; ++i) {
        QWidget *card = createProductCard(i);
        if (card) {
            int row = (i - startIdx) / 3;
            int col = (i - startIdx) % 3;
            m_cardLayout->addWidget(card, row, col);
        }
    }

    // Update Table visibility (Synchronized Pagination)
    for (int i = 0; i < totalItems; ++i) {
        t->setRowHidden(i, (i < startIdx || i >= endIdx));
    }

    // Update Pagination UI
    QWidget *root = produitRoot();
    if (root) {
        QLabel *lblPage = root->findChild<QLabel*>("pagination_lblPage");
        QPushButton *btnPrev = root->findChild<QPushButton*>("pagination_btnPrev");
        QPushButton *btnNext = root->findChild<QPushButton*>("pagination_btnNext");

        int maxPages = qMax(1, (totalItems + m_itemsPerPage - 1) / m_itemsPerPage);
        if (lblPage) lblPage->setText(QString("Page %1 sur %2").arg(m_currentPage + 1).arg(maxPages));
        if (btnPrev) btnPrev->setEnabled(m_currentPage > 0);
        if (btnNext) btnNext->setEnabled(m_currentPage < maxPages - 1);
    }
}

QWidget* MainWindow::createProductCard(int row)
{
    QTableWidget *t = produitTable();
    if (!t || row >= t->rowCount()) return nullptr;

    QString model = t->item(row, 1) ? t->item(row, 1)->text() : "";
    QString price = t->item(row, 3) ? t->item(row, 3)->text() : "";
    QString stock = t->item(row, 4) ? t->item(row, 4)->text() : "";

    int cur = t->item(row, 0)->data(PROD_ROLE_CUR).toInt();
    int max = t->item(row, 0)->data(PROD_ROLE_MAX).toInt();
    QString aisle = t->item(row, 0)->data(PROD_ROLE_AISLE).toString();
    if (max == 0) max = 100;
    int stockVal = stock.toInt();

    // ── Outer card frame ──────────────────────────────────────────────────────
    QFrame *card = new QFrame();
    card->setObjectName("productCard");
    card->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    card->setMinimumWidth(280);
    card->setMaximumWidth(420);
    card->setStyleSheet(
        "QFrame#productCard {"
        "  background-color: white;"
        "  border-radius: 18px;"
        "  border: 1px solid #e8edf2;"
        "}"
    );
    auto *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 20));
    shadow->setOffset(0, 6);
    card->setGraphicsEffect(shadow);

    QVBoxLayout *outerLayout = new QVBoxLayout(card);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    // ── GLOSS IMAGE AREA ──────────────────────────────────────────────────────
    QWidget *imgArea = new QWidget();
    imgArea->setFixedHeight(160);
    imgArea->setStyleSheet(
        "background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "  stop:0 #eef4fb, stop:0.6 #ddeaf7, stop:1 #ccddf0);"
        "border-top-left-radius: 18px;"
        "border-top-right-radius: 18px;"
    );

    QHBoxLayout *imgAreaLayout = new QHBoxLayout(imgArea);
    imgAreaLayout->setContentsMargins(12, 8, 12, 8);

    QLabel *imgLabel = new QLabel();
    imgLabel->setAlignment(Qt::AlignCenter);
    imgLabel->setStyleSheet("background: transparent;");

    QString resPath = ":/assets/bin.png";

    QPixmap pix(resPath);
    if (!pix.isNull())
        imgLabel->setPixmap(pix.scaled(130, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Info column inside image area (right side)
    QVBoxLayout *infoCol = new QVBoxLayout();
    infoCol->setSpacing(4);
    infoCol->setContentsMargins(8, 0, 0, 0);

    QLabel *lblTitle = new QLabel(model.toUpper());
    lblTitle->setWordWrap(true);
    lblTitle->setStyleSheet("font-size: 15px; font-weight: 800; color: #1e293b; background: transparent;");

    QLabel *lblSub = new QLabel("Smart Waste");
    lblSub->setStyleSheet("font-size: 12px; color: #64748b; background: transparent;");

    // Progress bar row
    QWidget *progRow = new QWidget();
    progRow->setStyleSheet("background: transparent;");
    QHBoxLayout *progLayout = new QHBoxLayout(progRow);
    progLayout->setContentsMargins(0, 0, 0, 0);
    progLayout->setSpacing(6);

    QProgressBar *bar = new QProgressBar();
    bar->setRange(0, max);
    bar->setValue(cur);
    bar->setTextVisible(false);
    bar->setFixedHeight(7);
    bar->setStyleSheet(
        "QProgressBar { background: #d1dce8; border-radius: 3px; border: none; }"
        "QProgressBar::chunk { background: #4ade80; border-radius: 3px; }"
    );
    QLabel *lblProg = new QLabel(QString("%1 / %2").arg(cur).arg(max));
    lblProg->setStyleSheet("font-size: 11px; color: #475569; font-weight: 600; background: transparent;");
    progLayout->addWidget(bar, 1);
    progLayout->addWidget(lblProg);

    // Status pill
    QWidget *pill = new QWidget();
    QHBoxLayout *pillL = new QHBoxLayout(pill);
    pillL->setContentsMargins(8, 3, 8, 3);
    pillL->setSpacing(4);
    QLabel *pIco = new QLabel();
    QLabel *pTxt = new QLabel();
    pIco->setStyleSheet("background: transparent;");
    if (stockVal == 0) {
        pIco->setText("🚨"); pTxt->setText("Rupture de Stock");
        pill->setStyleSheet("background: #fee2e2; border-radius: 10px;");
        pTxt->setStyleSheet("color: #b91c1c; font-size: 11px; font-weight: 700; background: transparent;");
    } else if (stockVal < 20) {
        pIco->setText("⚠️"); pTxt->setText("Stock Faible");
        pill->setStyleSheet("background: #fef3c7; border-radius: 10px;");
        pTxt->setStyleSheet("color: #92400e; font-size: 11px; font-weight: 700; background: transparent;");
    } else {
        pIco->setText("✅"); pTxt->setText("En Stock");
        pill->setStyleSheet("background: #dcfce7; border-radius: 10px;");
        pTxt->setStyleSheet("color: #15803d; font-size: 11px; font-weight: 700; background: transparent;");
    }
    pillL->addWidget(pIco);
    pillL->addWidget(pTxt);

    // Aisle badge (replaces stock circle)
    QLabel *lblAisle = new QLabel(QString("📍 Allée %1").arg(aisle));
    lblAisle->setStyleSheet("font-size: 11px; color: #475569; font-weight: 600; background: transparent;");

    QHBoxLayout *statusRow = new QHBoxLayout();
    statusRow->setContentsMargins(0,0,0,0);
    statusRow->addWidget(pill);
    statusRow->addStretch();
    statusRow->addWidget(lblAisle);

    infoCol->addWidget(lblTitle);
    infoCol->addWidget(lblSub);
    infoCol->addStretch();
    infoCol->addWidget(progRow);
    infoCol->addLayout(statusRow);

    imgAreaLayout->addWidget(imgLabel, 0, Qt::AlignCenter);
    imgAreaLayout->addLayout(infoCol, 1);

    // ── BOTTOM: Price + Buttons on ONE ROW ───────────────────────────────────
    QWidget *bottomArea = new QWidget();
    bottomArea->setStyleSheet("background: transparent;");
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomArea);
    bottomLayout->setContentsMargins(14, 10, 14, 12);
    bottomLayout->setSpacing(0);

    // Separator
    QFrame *sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #f1f5f9; background: #f1f5f9; border: none; max-height: 1px;");

    // Single row: Price badge + Modifier + Supprimer
    QHBoxLayout *actionRow = new QHBoxLayout();
    actionRow->setContentsMargins(0, 10, 0, 0);
    actionRow->setSpacing(8);

    // Price badge
    QWidget *priceBadge = new QWidget();
    priceBadge->setStyleSheet("background: #f0f9ff; border-radius: 10px; border: 1.5px solid #bae6fd;");
    QHBoxLayout *priceInner = new QHBoxLayout(priceBadge);
    priceInner->setContentsMargins(8, 5, 8, 5);
    priceInner->setSpacing(4);
    QLabel *priceIcon = new QLabel("🏷");
    priceIcon->setStyleSheet("background: transparent; font-size: 14px;");
    QLabel *priceVal = new QLabel(QString("%1 TND").arg(price));
    priceVal->setStyleSheet(
        "font-size: 15px; font-weight: 900; color: #0f172a;"
        "background: transparent;"
    );
    priceVal->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    priceInner->addWidget(priceIcon);
    priceInner->addWidget(priceVal);

    // Modifier button
    QPushButton *btnEdit = new QPushButton("▷ Modifier");
    btnEdit->setFixedSize(100, 36);
    btnEdit->setProperty("row", row);
    btnEdit->setCursor(Qt::PointingHandCursor);
    btnEdit->setStyleSheet(
        "QPushButton { background: #2563eb; color: white; border-radius: 9px;"
        "  font-weight: 700; font-size: 12px; border: none; }"
        "QPushButton:hover { background: #1d4ed8; }"
    );
    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::handleEditClicked);

    // Supprimer button
    QPushButton *btnDelete = new QPushButton("Supprimer");
    btnDelete->setFixedSize(95, 36);
    btnDelete->setProperty("row", row);
    btnDelete->setCursor(Qt::PointingHandCursor);
    btnDelete->setStyleSheet(
        "QPushButton { background: #ef4444; color: white; border-radius: 9px;"
        "  font-weight: 700; font-size: 12px; border: none; }"
        "QPushButton:hover { background: #dc2626; }"
    );
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::handleDeleteClicked);

    actionRow->addWidget(priceBadge);
    actionRow->addStretch(1);
    actionRow->addWidget(btnEdit);
    actionRow->addWidget(btnDelete);

    bottomLayout->addWidget(sep);
    bottomLayout->addLayout(actionRow);

    outerLayout->addWidget(imgArea);
    outerLayout->addWidget(bottomArea);

    return card;
}

// --- PAGINATION LOGIC ---

void MainWindow::on_pagination_cbSize_currentIndexChanged(int index)
{
    QComboBox *cb = qobject_cast<QComboBox*>(sender());
    if (!cb) return;

    m_itemsPerPage = cb->currentText().toInt();
    m_currentPage = 0; // Reset to first page
    refreshCardView();
}

void MainWindow::on_pagination_btnPrev_clicked()
{
    if (m_currentPage > 0) {
        m_currentPage--;
        refreshCardView();
    }
}

void MainWindow::on_pagination_btnNext_clicked()
{
    QTableWidget *t = produitTable();
    if (!t) return;

    int totalItems = t->rowCount();
    int maxPages = (totalItems + m_itemsPerPage - 1) / m_itemsPerPage;

    if (m_currentPage < maxPages - 1) {
        m_currentPage++;
        refreshCardView();
    }
}

// ============================================================
// EMPLOYEE CARD VIEW MODULE
// ============================================================

void MainWindow::setupEmployeModule()
{
    QTimer::singleShot(200, this, [this]() {
        if (findChild<QPushButton*>("emp_btnToggleView")) return;

        QTableWidget *t = ui->tableEmployes;
        if (!t) return;

        // The table lives inside a widget whose layout is verticalLayout_Left
        // Walk up to find the widget that has horizontalLayout_Search as a child layout
        QWidget *pageWidget = t->parentWidget();
        if (!pageWidget) return;

        // Find the search row layout by name within the page widget
        QHBoxLayout *searchLayout = pageWidget->findChild<QHBoxLayout*>("horizontalLayout_Search");
        QVBoxLayout *leftLayout   = pageWidget->findChild<QVBoxLayout*>("verticalLayout_Left");

        // If not found directly, try the grandparent
        if (!searchLayout || !leftLayout) {
            pageWidget = pageWidget->parentWidget();
            if (!pageWidget) return;
            searchLayout = pageWidget->findChild<QHBoxLayout*>("horizontalLayout_Search");
            leftLayout   = pageWidget->findChild<QVBoxLayout*>("verticalLayout_Left");
        }

        if (!searchLayout || !leftLayout) return;

        // ── Toggle button → add to search row (next to cbSort) ────────────
        QPushButton *btnToggle = new QPushButton("⊞");
        btnToggle->setObjectName("emp_btnToggleView");
        btnToggle->setFixedSize(40, 40);
        btnToggle->setCursor(Qt::PointingHandCursor);
        btnToggle->setToolTip("Vue cartes / tableau");
        btnToggle->setStyleSheet(
            "QPushButton { background: #2563eb; color: white; border-radius: 10px;"
            "  font-size: 18px; border: none; font-weight: 700; }"
            "QPushButton:hover { background: #1d4ed8; }"
        );
        connect(btnToggle, &QPushButton::clicked, this, &MainWindow::slot_toggleEmpView);
        searchLayout->addWidget(btnToggle);

        // ── Card scroll area → add to verticalLayout_Left ─────────────────
        QScrollArea *cardScroll = new QScrollArea();
        cardScroll->setObjectName("emp_cardScrollArea");
        cardScroll->setWidgetResizable(true);
        cardScroll->setFrameShape(QFrame::NoFrame);
        cardScroll->setStyleSheet("background: transparent; border: none;");
        cardScroll->setVisible(false);

        QWidget *cardContainer = new QWidget();
        cardContainer->setObjectName("emp_cardContainer");
        cardContainer->setStyleSheet("background: transparent;");
        QGridLayout *gridLayout = new QGridLayout(cardContainer);
        gridLayout->setObjectName("emp_cardLayout");
        gridLayout->setSpacing(16);
        gridLayout->setContentsMargins(8, 8, 8, 8);
        cardScroll->setWidget(cardContainer);
        m_empCardLayout = gridLayout;

        leftLayout->addWidget(cardScroll);

        // ── Pagination bar ─────────────────────────────────────────────────
        QWidget *pBar = new QWidget();
        pBar->setObjectName("emp_paginationBar");
        pBar->setFixedHeight(60);
        pBar->setVisible(false);
        pBar->setStyleSheet("background: #f8fafc; border-top: 1px solid #e2e8f0; border-radius: 12px;");

        QHBoxLayout *pLayout = new QHBoxLayout(pBar);
        pLayout->setContentsMargins(20, 0, 20, 0);

        QLabel *lblSize = new QLabel("Items par page:");
        lblSize->setStyleSheet("font-size: 13px; color: #64748b; font-weight: 600;");

        QComboBox *cbSize = new QComboBox();
        cbSize->setObjectName("emp_pagination_cbSize");
        cbSize->addItems({"4", "6", "12"});
        cbSize->setCurrentText("6");
        cbSize->setFixedWidth(70);
        cbSize->setStyleSheet("background: white; border: 1px solid #cbd5e1; border-radius: 8px; padding: 4px;");
        connect(cbSize, SIGNAL(currentIndexChanged(int)), this, SLOT(on_emp_pagination_cbSize_currentIndexChanged(int)));

        QPushButton *btnPrev = new QPushButton("← Précédent");
        btnPrev->setObjectName("emp_pagination_btnPrev");
        btnPrev->setFixedWidth(110);
        btnPrev->setCursor(Qt::PointingHandCursor);
        btnPrev->setEnabled(false);
        btnPrev->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnPrev, &QPushButton::clicked, this, &MainWindow::on_emp_pagination_btnPrev_clicked);

        QLabel *lblPage = new QLabel("Page 1 sur 1");
        lblPage->setObjectName("emp_pagination_lblPage");
        lblPage->setStyleSheet("font-size: 13px; color: #1e293b; font-weight: 700;");

        QPushButton *btnNext = new QPushButton("Suivant →");
        btnNext->setObjectName("emp_pagination_btnNext");
        btnNext->setFixedWidth(110);
        btnNext->setCursor(Qt::PointingHandCursor);
        btnNext->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnNext, &QPushButton::clicked, this, &MainWindow::on_emp_pagination_btnNext_clicked);

        pLayout->addWidget(lblSize);
        pLayout->addWidget(cbSize);
        pLayout->addStretch();
        pLayout->addWidget(btnPrev);
        pLayout->addWidget(lblPage);
        pLayout->addWidget(btnNext);

        leftLayout->addWidget(pBar);
    });
}

void MainWindow::setupEmpCardViewContainer()
{
    m_empCardLayout = findChild<QGridLayout*>("emp_cardLayout");
}

void MainWindow::slot_toggleEmpView()
{
    m_isEmpCardView = !m_isEmpCardView;

    if (ui->tableEmployes)
        ui->tableEmployes->setVisible(!m_isEmpCardView);

    QScrollArea *cardScroll = findChild<QScrollArea*>("emp_cardScrollArea");
    if (cardScroll) cardScroll->setVisible(m_isEmpCardView);

    QWidget *pBar = findChild<QWidget*>("emp_paginationBar");
    if (pBar) pBar->setVisible(m_isEmpCardView);

    QPushButton *btnToggle = findChild<QPushButton*>("emp_btnToggleView");
    if (btnToggle) btnToggle->setText(m_isEmpCardView ? "≣" : "⊞");

    if (m_isEmpCardView) {
        if (!m_empCardLayout) setupEmpCardViewContainer();
        refreshEmpCardView();
    }
}

void MainWindow::refreshEmpCardView()
{
    if (!m_isEmpCardView) return;
    if (!m_empCardLayout) setupEmpCardViewContainer();
    if (!m_empCardLayout) return;

    QLayoutItem *child;
    while ((child = m_empCardLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }

    QTableWidget *t = ui->tableEmployes;
    if (!t) return;

    int totalItems = t->rowCount();
    int startIdx = m_empCurrentPage * m_empItemsPerPage;
    int endIdx = qMin(startIdx + m_empItemsPerPage, totalItems);

    int col = 0, row = 0;
    const int COLS = 2;

    for (int i = startIdx; i < endIdx; ++i) {
        QWidget *card = createEmployeeCard(i);
        if (card) {
            m_empCardLayout->addWidget(card, row, col);
            ++col;
            if (col >= COLS) { col = 0; ++row; }
        }
    }
    m_empCardLayout->setRowStretch(row + 1, 1);

    QWidget *pBarRoot = findChild<QWidget*>("emp_paginationBar");
    if (pBarRoot) {
        QLabel *lblPage = pBarRoot->findChild<QLabel*>("emp_pagination_lblPage");
        QPushButton *btnPrev = pBarRoot->findChild<QPushButton*>("emp_pagination_btnPrev");
        QPushButton *btnNext = pBarRoot->findChild<QPushButton*>("emp_pagination_btnNext");
        int maxPages = qMax(1, (totalItems + m_empItemsPerPage - 1) / m_empItemsPerPage);
        if (lblPage) lblPage->setText(QString("Page %1 sur %2").arg(m_empCurrentPage + 1).arg(maxPages));
        if (btnPrev) btnPrev->setEnabled(m_empCurrentPage > 0);
        if (btnNext) btnNext->setEnabled(m_empCurrentPage < maxPages - 1);
    }
}

QWidget* MainWindow::createEmployeeCard(int row)
{
    QTableWidget *t = ui->tableEmployes;
    if (!t || row >= t->rowCount()) return nullptr;

    QString nom    = t->item(row, 1) ? t->item(row, 1)->text() : "";
    QString role   = t->item(row, 2) ? t->item(row, 2)->text() : "";
    QString statut = t->item(row, 3) ? t->item(row, 3)->text() : "";
    int salaire = t->item(row, 0) ? t->item(row, 0)->data(EMP_ROLE_SALAIRE).toInt() : 0;
    int perf    = t->item(row, 0) ? t->item(row, 0)->data(EMP_ROLE_PERF).toInt() : 0;

    QFrame *card = new QFrame();
    card->setObjectName("empCard");
    card->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    card->setMinimumWidth(280);
    card->setMaximumWidth(460);
    card->setStyleSheet(
        "QFrame#empCard { background-color: white; border-radius: 18px; border: 1px solid #e8edf2; }"
    );
    auto *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 18));
    shadow->setOffset(0, 5);
    card->setGraphicsEffect(shadow);

    QVBoxLayout *outerLayout = new QVBoxLayout(card);
    outerLayout->setContentsMargins(16, 16, 16, 14);
    outerLayout->setSpacing(12);

    // TOP ROW: Avatar + Name + Status + dots
    QHBoxLayout *topRow = new QHBoxLayout();
    topRow->setSpacing(14);

    QLabel *avatar = new QLabel();
    avatar->setFixedSize(64, 64);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setStyleSheet(
        "border-radius: 32px;"
        "background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #dbeafe, stop:1 #bfdbfe);"
        "border: 3px solid #93c5fd; font-size: 24px; font-weight: 700; color: #1e40af;"
    );
    QStringList parts = nom.split(' ');
    QString initials;
    for (const QString &p : parts) if (!p.isEmpty()) initials += p[0].toUpper();
    if (initials.length() > 2) initials = initials.left(2);
    avatar->setText(initials);

    QVBoxLayout *nameCol = new QVBoxLayout();
    nameCol->setSpacing(3);

    QLabel *lblName = new QLabel(nom);
    lblName->setStyleSheet("font-size: 15px; font-weight: 800; color: #1e293b; background: transparent;");
    QLabel *lblRole = new QLabel(role);
    lblRole->setStyleSheet("font-size: 12px; color: #64748b; background: transparent;");

    QWidget *statusPill = new QWidget();
    QHBoxLayout *pillL = new QHBoxLayout(statusPill);
    pillL->setContentsMargins(8, 3, 8, 3);
    pillL->setSpacing(4);
    QLabel *pillIco = new QLabel();
    QLabel *pillTxt = new QLabel(statut);
    pillIco->setStyleSheet("background: transparent;");
    if (statut == "Disponible") {
        pillIco->setText("✅");
        statusPill->setStyleSheet("background: #dcfce7; border-radius: 10px;");
        pillTxt->setStyleSheet("color: #15803d; font-size: 11px; font-weight: 700; background: transparent;");
    } else if (statut == "En mission") {
        pillIco->setText("🔵");
        statusPill->setStyleSheet("background: #dbeafe; border-radius: 10px;");
        pillTxt->setStyleSheet("color: #1d4ed8; font-size: 11px; font-weight: 700; background: transparent;");
    } else {
        pillIco->setText("🟠");
        statusPill->setStyleSheet("background: #fef3c7; border-radius: 10px;");
        pillTxt->setStyleSheet("color: #92400e; font-size: 11px; font-weight: 700; background: transparent;");
    }
    pillL->addWidget(pillIco);
    pillL->addWidget(pillTxt);

    nameCol->addWidget(lblName);
    nameCol->addWidget(lblRole);
    nameCol->addWidget(statusPill);

    QLabel *dots = new QLabel("•••");
    dots->setStyleSheet("color: #94a3b8; font-size: 16px; background: transparent;");

    topRow->addWidget(avatar);
    topRow->addLayout(nameCol, 1);
    topRow->addWidget(dots, 0, Qt::AlignTop);

    // SEPARATOR
    QFrame *sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #f1f5f9; background: #f1f5f9; border: none; max-height: 1px;");

    // STATS ROW
    QHBoxLayout *statsRow = new QHBoxLayout();
    statsRow->setSpacing(8);

    QWidget *perfWidget = new QWidget();
    perfWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *perfLayout = new QHBoxLayout(perfWidget);
    perfLayout->setContentsMargins(0,0,0,0);
    perfLayout->setSpacing(5);
    QLabel *perfIcon = new QLabel("⚙");
    perfIcon->setStyleSheet("font-size: 15px; background: transparent;");
    QLabel *perfLbl = new QLabel("Performance");
    perfLbl->setStyleSheet("font-size: 12px; color: #64748b; background: transparent;");
    QLabel *perfVal = new QLabel(QString("%1%").arg(perf));
    perfVal->setStyleSheet("font-size: 14px; font-weight: 800; color: #16a34a; background: transparent;");
    perfLayout->addWidget(perfIcon);
    perfLayout->addWidget(perfLbl);
    perfLayout->addWidget(perfVal);
    perfLayout->addStretch();

    QWidget *intWidget = new QWidget();
    intWidget->setStyleSheet("background: transparent;");
    QVBoxLayout *intLayout = new QVBoxLayout(intWidget);
    intLayout->setContentsMargins(0,0,0,0);
    intLayout->setSpacing(1);
    QLabel *intLbl = new QLabel("Interventions");
    intLbl->setStyleSheet("font-size: 10px; color: #94a3b8; background: transparent;");
    QLabel *intVal = new QLabel(QString("ce mois: %1%").arg(qMin(perf + 5, 100)));
    intVal->setStyleSheet("font-size: 11px; font-weight: 700; color: #475569; background: transparent;");
    intLayout->addWidget(intLbl);
    intLayout->addWidget(intVal);

    statsRow->addWidget(perfWidget, 1);
    statsRow->addWidget(intWidget);

    // BOTTOM ROW: Salary + Buttons
    QHBoxLayout *bottomRow = new QHBoxLayout();
    bottomRow->setSpacing(8);

    QWidget *salBadge = new QWidget();
    salBadge->setStyleSheet("background: #f0f9ff; border-radius: 10px; border: 1.5px solid #bae6fd;");
    QHBoxLayout *salInner = new QHBoxLayout(salBadge);
    salInner->setContentsMargins(8, 5, 8, 5);
    salInner->setSpacing(4);
    QLabel *salIcon = new QLabel("🏦");
    salIcon->setStyleSheet("background: transparent; font-size: 14px;");
    QLabel *salVal = new QLabel(QString("%1 TND").arg(salaire));
    salVal->setStyleSheet("font-size: 15px; font-weight: 900; color: #0f172a; background: transparent;");
    salInner->addWidget(salIcon);
    salInner->addWidget(salVal);

    QPushButton *btnProfil = new QPushButton("▷ Voir Profil");
    btnProfil->setFixedSize(105, 36);
    btnProfil->setProperty("row", row);
    btnProfil->setCursor(Qt::PointingHandCursor);
    btnProfil->setStyleSheet(
        "QPushButton { background: #2563eb; color: white; border-radius: 9px;"
        "  font-weight: 700; font-size: 12px; border: none; }"
        "QPushButton:hover { background: #1d4ed8; }"
    );

    QPushButton *btnEdit = new QPushButton("Modifier");
    btnEdit->setFixedSize(85, 36);
    btnEdit->setProperty("row", row);
    btnEdit->setCursor(Qt::PointingHandCursor);
    btnEdit->setStyleSheet(
        "QPushButton { background: #f97316; color: white; border-radius: 9px;"
        "  font-weight: 700; font-size: 12px; border: none; }"
        "QPushButton:hover { background: #ea6c0a; }"
    );
    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::on_btnModifier_clicked);

    bottomRow->addWidget(salBadge);
    bottomRow->addStretch(1);
    bottomRow->addWidget(btnProfil);
    bottomRow->addWidget(btnEdit);

    outerLayout->addLayout(topRow);
    outerLayout->addWidget(sep);
    outerLayout->addLayout(statsRow);
    outerLayout->addLayout(bottomRow);

    return card;
}

void MainWindow::on_emp_pagination_cbSize_currentIndexChanged(int index)
{
    QComboBox *cb = qobject_cast<QComboBox*>(sender());
    if (!cb) return;
    m_empItemsPerPage = cb->currentText().toInt();
    m_empCurrentPage = 0;
    refreshEmpCardView();
}

void MainWindow::on_emp_pagination_btnPrev_clicked()
{
    if (m_empCurrentPage > 0) {
        m_empCurrentPage--;
        refreshEmpCardView();
    }
}

void MainWindow::on_emp_pagination_btnNext_clicked()
{
    if (!ui->tableEmployes) return;
    int totalItems = ui->tableEmployes->rowCount();
    int maxPages = (totalItems + m_empItemsPerPage - 1) / m_empItemsPerPage;
    if (m_empCurrentPage < maxPages - 1) {
        m_empCurrentPage++;
        refreshEmpCardView();
    }
}


// ============================================================
// STOCK CARD VIEW MODULE
// ============================================================

void MainWindow::setupStockCardViewContainer()
{
    QTimer::singleShot(200, this, [this]() {
        if (findChild<QPushButton*>("stock_btnToggleView")) return;

        QTableWidget *t = ui->tableWidget;
        if (!t) return;

        QWidget *pageWidget = t->parentWidget();
        if (!pageWidget) return;

        QHBoxLayout *searchLayout = pageWidget->findChild<QHBoxLayout*>("stockSearchSortRow");
        QVBoxLayout *tableLayout  = pageWidget->findChild<QVBoxLayout*>("tableCardLayout");

        if (!searchLayout || !tableLayout) {
            pageWidget = pageWidget->parentWidget();
            if (!pageWidget) return;
            searchLayout = pageWidget->findChild<QHBoxLayout*>("stockSearchSortRow");
            tableLayout  = pageWidget->findChild<QVBoxLayout*>("tableCardLayout");
        }
        if (!searchLayout || !tableLayout) return;

        // Toggle button
        QPushButton *btnToggle = new QPushButton(QString::fromUtf8("\xe2\x8a\x9e"));
        btnToggle->setObjectName("stock_btnToggleView");
        btnToggle->setFixedSize(40, 40);
        btnToggle->setCursor(Qt::PointingHandCursor);
        btnToggle->setToolTip("Vue cartes / tableau");
        btnToggle->setStyleSheet(
            "QPushButton { background: #2563eb; color: white; border-radius: 10px;"
            "  font-size: 18px; border: none; font-weight: 700; }"
            "QPushButton:hover { background: #1d4ed8; }"
        );
        connect(btnToggle, &QPushButton::clicked, this, &MainWindow::slot_toggleStockView);
        searchLayout->addWidget(btnToggle);

        // Card scroll area
        QScrollArea *cardScroll = new QScrollArea();
        cardScroll->setObjectName("stock_cardScrollArea");
        cardScroll->setWidgetResizable(true);
        cardScroll->setFrameShape(QFrame::NoFrame);
        cardScroll->setStyleSheet("background: transparent; border: none;");
        cardScroll->setVisible(false);

        QWidget *cardContainer = new QWidget();
        cardContainer->setObjectName("stock_cardContainer");
        cardContainer->setStyleSheet("background: transparent;");
        QGridLayout *gridLayout = new QGridLayout(cardContainer);
        gridLayout->setObjectName("stock_cardLayout");
        gridLayout->setSpacing(16);
        gridLayout->setContentsMargins(8, 8, 8, 8);
        cardScroll->setWidget(cardContainer);
        m_stockCardLayout = gridLayout;

        tableLayout->addWidget(cardScroll);

        // Pagination bar
        QWidget *pBar = new QWidget();
        pBar->setObjectName("stock_paginationBar");
        pBar->setFixedHeight(60);
        pBar->setVisible(false);
        pBar->setStyleSheet("background: #f8fafc; border-top: 1px solid #e2e8f0; border-radius: 12px;");

        QHBoxLayout *pLayout = new QHBoxLayout(pBar);
        pLayout->setContentsMargins(20, 0, 20, 0);

        QLabel *lblSize = new QLabel("Items par page:");
        lblSize->setStyleSheet("font-size: 13px; color: #64748b; font-weight: 600;");

        QComboBox *cbSize = new QComboBox();
        cbSize->setObjectName("stock_pagination_cbSize");
        cbSize->addItems({"4", "6", "12"});
        cbSize->setCurrentText("6");
        cbSize->setFixedWidth(70);
        cbSize->setStyleSheet("background: white; border: 1px solid #cbd5e1; border-radius: 8px; padding: 4px;");
        connect(cbSize, SIGNAL(currentIndexChanged(int)), this, SLOT(on_stock_pagination_cbSize_currentIndexChanged(int)));

        QPushButton *btnPrev = new QPushButton(QString::fromUtf8("\xe2\x86\x90") + " Precedent");
        btnPrev->setObjectName("stock_pagination_btnPrev");
        btnPrev->setFixedWidth(120);
        btnPrev->setCursor(Qt::PointingHandCursor);
        btnPrev->setEnabled(false);
        btnPrev->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnPrev, &QPushButton::clicked, this, &MainWindow::on_stock_pagination_btnPrev_clicked);

        QLabel *lblPage = new QLabel("Page 1 sur 1");
        lblPage->setObjectName("stock_pagination_lblPage");
        lblPage->setStyleSheet("font-size: 13px; color: #1e293b; font-weight: 700;");

        QPushButton *btnNext = new QPushButton("Suivant " + QString::fromUtf8("\xe2\x86\x92"));
        btnNext->setObjectName("stock_pagination_btnNext");
        btnNext->setFixedWidth(120);
        btnNext->setCursor(Qt::PointingHandCursor);
        btnNext->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnNext, &QPushButton::clicked, this, &MainWindow::on_stock_pagination_btnNext_clicked);

        pLayout->addWidget(lblSize);
        pLayout->addWidget(cbSize);
        pLayout->addStretch();
        pLayout->addWidget(btnPrev);
        pLayout->addWidget(lblPage);
        pLayout->addWidget(btnNext);

        tableLayout->addWidget(pBar);
    });
}

void MainWindow::slot_toggleStockView()
{
    m_isStockCardView = !m_isStockCardView;

    if (ui->tableWidget)
        ui->tableWidget->setVisible(!m_isStockCardView);

    QScrollArea *cardScroll = findChild<QScrollArea*>("stock_cardScrollArea");
    if (cardScroll) cardScroll->setVisible(m_isStockCardView);

    QWidget *pBar = findChild<QWidget*>("stock_paginationBar");
    if (pBar) pBar->setVisible(m_isStockCardView);

    if (m_isStockCardView) {
        if (!m_stockCardLayout) m_stockCardLayout = findChild<QGridLayout*>("stock_cardLayout");
        refreshStockCardView();
    }
}

void MainWindow::refreshStockCardView()
{
    if (!m_stockCardLayout) return;
    QTableWidget *t = ui->tableWidget;
    if (!t) return;

    // Clear existing cards
    while (QLayoutItem *item = m_stockCardLayout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    int totalItems = t->rowCount();
    int startIdx = m_stockCurrentPage * m_stockItemsPerPage;
    int endIdx = qMin(startIdx + m_stockItemsPerPage, totalItems);

    int col = 0, row = 0;
    const int cols = 3;
    for (int i = startIdx; i < endIdx; ++i) {
        QWidget *card = createStockCard(i);
        if (card) {
            m_stockCardLayout->addWidget(card, row, col);
            col++;
            if (col >= cols) { col = 0; row++; }
        }
    }

    // Update pagination
    int totalPages = qMax(1, (totalItems + m_stockItemsPerPage - 1) / m_stockItemsPerPage);
    QLabel *lblPage = findChild<QLabel*>("stock_pagination_lblPage");
    if (lblPage) lblPage->setText(QString("Page %1 sur %2").arg(m_stockCurrentPage + 1).arg(totalPages));

    QPushButton *btnPrev = findChild<QPushButton*>("stock_pagination_btnPrev");
    if (btnPrev) btnPrev->setEnabled(m_stockCurrentPage > 0);

    QPushButton *btnNext = findChild<QPushButton*>("stock_pagination_btnNext");
    if (btnNext) btnNext->setEnabled(m_stockCurrentPage < totalPages - 1);
}

QWidget* MainWindow::createStockCard(int row)
{
    QTableWidget *t = ui->tableWidget;
    if (!t || row < 0 || row >= t->rowCount()) return nullptr;

    auto getText = [&](int col) -> QString {
        QTableWidgetItem *item = t->item(row, col);
        return item ? item->text() : QString();
    };

    QString ref         = getText(0);
    QString nom         = getText(1);
    QString stockQty    = getText(2);
    // stockQty may be stored as int via DisplayRole
    if (stockQty.isEmpty()) {
        QTableWidgetItem *si = t->item(row, 2);
        if (si) stockQty = si->data(Qt::DisplayRole).toString();
    }
    QString prix        = getText(4);
    QString fournisseur = getText(5);

    // Get seuil from badge widget
    QString seuil;
    QWidget *badgeW = t->cellWidget(row, 3);
    if (badgeW) {
        QLabel *lbl = badgeW->findChild<QLabel*>();
        if (lbl) seuil = lbl->text();
    }
    if (seuil.isEmpty()) seuil = getText(3);

    // Seuil colors
    QString seuilColor = "#16a34a";
    QString seuilBg    = "#dcfce7";
    QString seuilText  = seuil.isEmpty() ? "OK" : seuil;
    if (seuil == "CRITIQUE") { seuilColor = "#dc2626"; seuilBg = "#fee2e2"; }
    else if (seuil == "MOYEN") { seuilColor = "#d97706"; seuilBg = "#fef3c7"; }

    // Avatar color based on row
    QStringList avatarColors = {"#2563eb","#7c3aed","#0891b2","#059669","#d97706","#dc2626","#0f766e","#9333ea","#c2410c"};
    QString avatarColor = avatarColors[row % avatarColors.size()];
    QString initials = nom.isEmpty() ? "?" : QString(nom[0]).toUpper();

    // Card widget
    QFrame *card = new QFrame();
    card->setFrameShape(QFrame::StyledPanel);
    card->setMinimumWidth(240);
    card->setMaximumWidth(380);
    card->setStyleSheet(
        "QFrame { background: white; border-radius: 16px; border: 1px solid #e2e8f0; }"
    );
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(18);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 28));
    card->setGraphicsEffect(shadow);

    QVBoxLayout *vl = new QVBoxLayout(card);
    vl->setContentsMargins(18, 18, 18, 18);
    vl->setSpacing(12);

    // ── Header: avatar + ref/name + seuil pill ─────────────────────────────
    QHBoxLayout *headerRow = new QHBoxLayout();
    headerRow->setSpacing(12);

    // Circular avatar
    QLabel *avatar = new QLabel(initials);
    avatar->setFixedSize(52, 52);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setStyleSheet(QString(
        "background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 %1,stop:1 %2);"
        "border-radius: 26px; color: white; font-size: 22px; font-weight: 700;"
    ).arg(avatarColor).arg(avatarColor));
    headerRow->addWidget(avatar);

    QVBoxLayout *nameCol = new QVBoxLayout();
    nameCol->setSpacing(2);
    QLabel *lblRef = new QLabel(ref);
    lblRef->setStyleSheet("font-size: 11px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *lblNom = new QLabel(nom);
    lblNom->setStyleSheet("font-size: 14px; font-weight: 700; color: #1e293b; background: transparent; border: none;");
    lblNom->setWordWrap(true);
    nameCol->addWidget(lblRef);
    nameCol->addWidget(lblNom);
    headerRow->addLayout(nameCol, 1);

    // Seuil pill
    QLabel *seuilPill = new QLabel(seuilText);
    seuilPill->setAlignment(Qt::AlignCenter);
    seuilPill->setFixedHeight(26);
    seuilPill->setMinimumWidth(60);
    seuilPill->setContentsMargins(10, 0, 10, 0);
    seuilPill->setStyleSheet(QString(
        "background: %1; color: %2; border-radius: 13px; font-size: 11px; font-weight: 700; border: none;"
    ).arg(seuilBg).arg(seuilColor));
    headerRow->addWidget(seuilPill);
    vl->addLayout(headerRow);

    // ── Divider ────────────────────────────────────────────────────────────
    QFrame *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setFixedHeight(1);
    divider->setStyleSheet("background: #f1f5f9; border: none;");
    vl->addWidget(divider);

    // ── Stats: stock qty + prix ────────────────────────────────────────────
    QHBoxLayout *statsRow = new QHBoxLayout();
    statsRow->setSpacing(10);

    // Stock stat box
    QWidget *stockBox = new QWidget();
    stockBox->setStyleSheet("background: #f8fafc; border-radius: 10px; border: none;");
    QVBoxLayout *stockBoxL = new QVBoxLayout(stockBox);
    stockBoxL->setContentsMargins(12, 8, 12, 8);
    stockBoxL->setSpacing(3);
    QLabel *stockLbl = new QLabel("Stock");
    stockLbl->setStyleSheet("font-size: 10px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *stockVal = new QLabel(stockQty + " " + QString::fromUtf8("unit\xc3\xa9s"));
    stockVal->setStyleSheet("font-size: 14px; font-weight: 700; color: #1e293b; background: transparent; border: none;");
    stockBoxL->addWidget(stockLbl);
    stockBoxL->addWidget(stockVal);
    statsRow->addWidget(stockBox, 1);

    // Prix stat box
    QWidget *prixBox = new QWidget();
    prixBox->setStyleSheet("background: #eff6ff; border-radius: 10px; border: none;");
    QVBoxLayout *prixBoxL = new QVBoxLayout(prixBox);
    prixBoxL->setContentsMargins(12, 8, 12, 8);
    prixBoxL->setSpacing(3);
    QLabel *prixLbl = new QLabel("Prix unitaire");
    prixLbl->setStyleSheet("font-size: 10px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *prixVal = new QLabel(prix.isEmpty() ? "---" : prix);
    prixVal->setStyleSheet("font-size: 14px; font-weight: 700; color: #2563eb; background: transparent; border: none;");
    prixBoxL->addWidget(prixLbl);
    prixBoxL->addWidget(prixVal);
    statsRow->addWidget(prixBox, 1);

    vl->addLayout(statsRow);

    // ── Fournisseur badge ──────────────────────────────────────────────────
    QLabel *fournBadge = new QLabel("[F] " + (fournisseur.isEmpty() ? "---" : fournisseur));
    fournBadge->setStyleSheet(
        "background: #f0fdf4; color: #15803d; border-radius: 8px; padding: 6px 12px;"
        "font-size: 12px; font-weight: 600; border: none;"
    );
    vl->addWidget(fournBadge);

    // ── Action buttons ─────────────────────────────────────────────────────
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->setSpacing(8);

    QPushButton *btnView = new QPushButton("Voir " + QString::fromUtf8("D\xc3\xa9tails"));
    btnView->setCursor(Qt::PointingHandCursor);
    btnView->setStyleSheet(
        "QPushButton { background: #2563eb; color: white; border-radius: 10px;"
        "  font-size: 12px; font-weight: 700; padding: 9px 0; border: none; }"
        "QPushButton:hover { background: #1d4ed8; }"
    );

    QPushButton *btnEdit = new QPushButton("Modifier");
    btnEdit->setCursor(Qt::PointingHandCursor);
    btnEdit->setStyleSheet(
        "QPushButton { background: #fff7ed; color: #ea580c; border-radius: 10px;"
        "  font-size: 12px; font-weight: 700; padding: 9px 0; border: 1px solid #fed7aa; }"
        "QPushButton:hover { background: #ffedd5; }"
    );

    btnRow->addWidget(btnView, 1);
    btnRow->addWidget(btnEdit, 1);
    vl->addLayout(btnRow);

    return card;
}

void MainWindow::on_stock_pagination_cbSize_currentIndexChanged(int)
{
    QComboBox *cb = findChild<QComboBox*>("stock_pagination_cbSize");
    if (!cb) return;
    m_stockItemsPerPage = cb->currentText().toInt();
    m_stockCurrentPage = 0;
    refreshStockCardView();
}

void MainWindow::on_stock_pagination_btnPrev_clicked()
{
    if (m_stockCurrentPage > 0) {
        m_stockCurrentPage--;
        refreshStockCardView();
    }
}

void MainWindow::on_stock_pagination_btnNext_clicked()
{
    if (!ui->tableWidget) return;
    int totalItems = ui->tableWidget->rowCount();
    int maxPages = (totalItems + m_stockItemsPerPage - 1) / m_stockItemsPerPage;
    if (m_stockCurrentPage < maxPages - 1) {
        m_stockCurrentPage++;
        refreshStockCardView();
    }
}

// ============================================================
// CLIENT CARD VIEW MODULE
// ============================================================

void MainWindow::setupClientCardViewContainer()
{
    QTimer::singleShot(200, this, [this]() {
        if (findChild<QPushButton*>("client_btnToggleView")) return;

        QTableWidget *t = ui->tableWidget_Client;
        if (!t) return;

        QWidget *pageWidget = t->parentWidget();
        if (!pageWidget) return;

        // Walk up to find the layouts by name
        QHBoxLayout *searchLayout = nullptr;
        QVBoxLayout *tableLayout  = nullptr;
        QWidget *w = pageWidget;
        for (int attempt = 0; attempt < 5 && w; attempt++) {
            searchLayout = w->findChild<QHBoxLayout*>("horizontalLayout_filters");
            tableLayout  = w->findChild<QVBoxLayout*>("verticalLayout_repertoire");
            if (searchLayout && tableLayout) break;
            w = w->parentWidget();
        }
        if (!searchLayout || !tableLayout) return;

        // Toggle button
        QPushButton *btnToggle = new QPushButton(QString::fromUtf8("\xe2\x8a\x9e"));
        btnToggle->setObjectName("client_btnToggleView");
        btnToggle->setFixedSize(40, 40);
        btnToggle->setCursor(Qt::PointingHandCursor);
        btnToggle->setToolTip("Vue cartes / tableau");
        btnToggle->setStyleSheet(
            "QPushButton { background: #7c3aed; color: white; border-radius: 10px;"
            "  font-size: 18px; border: none; font-weight: 700; }"
            "QPushButton:hover { background: #6d28d9; }"
        );
        connect(btnToggle, &QPushButton::clicked, this, &MainWindow::slot_toggleClientView);
        searchLayout->addWidget(btnToggle);

        // Card scroll area
        QScrollArea *cardScroll = new QScrollArea();
        cardScroll->setObjectName("client_cardScrollArea");
        cardScroll->setWidgetResizable(true);
        cardScroll->setFrameShape(QFrame::NoFrame);
        cardScroll->setStyleSheet("background: transparent; border: none;");
        cardScroll->setVisible(false);

        QWidget *cardContainer = new QWidget();
        cardContainer->setObjectName("client_cardContainer");
        cardContainer->setStyleSheet("background: transparent;");
        QGridLayout *gridLayout = new QGridLayout(cardContainer);
        gridLayout->setObjectName("client_cardLayout");
        gridLayout->setSpacing(16);
        gridLayout->setContentsMargins(8, 8, 8, 8);
        cardScroll->setWidget(cardContainer);
        m_clientCardLayout = gridLayout;

        tableLayout->addWidget(cardScroll);

        // Pagination bar
        QWidget *pBar = new QWidget();
        pBar->setObjectName("client_paginationBar");
        pBar->setFixedHeight(60);
        pBar->setVisible(false);
        pBar->setStyleSheet("background: #f8fafc; border-top: 1px solid #e2e8f0; border-radius: 12px;");

        QHBoxLayout *pLayout = new QHBoxLayout(pBar);
        pLayout->setContentsMargins(20, 0, 20, 0);

        QLabel *lblSize = new QLabel("Items par page:");
        lblSize->setStyleSheet("font-size: 13px; color: #64748b; font-weight: 600;");

        QComboBox *cbSize = new QComboBox();
        cbSize->setObjectName("client_pagination_cbSize");
        cbSize->addItems({"4", "6", "12"});
        cbSize->setCurrentText("6");
        cbSize->setFixedWidth(70);
        cbSize->setStyleSheet("background: white; border: 1px solid #cbd5e1; border-radius: 8px; padding: 4px;");
        connect(cbSize, SIGNAL(currentIndexChanged(int)), this, SLOT(on_client_pagination_cbSize_currentIndexChanged(int)));

        QPushButton *btnPrev = new QPushButton(QString::fromUtf8("\xe2\x86\x90") + " Precedent");
        btnPrev->setObjectName("client_pagination_btnPrev");
        btnPrev->setFixedWidth(120);
        btnPrev->setCursor(Qt::PointingHandCursor);
        btnPrev->setEnabled(false);
        btnPrev->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnPrev, &QPushButton::clicked, this, &MainWindow::on_client_pagination_btnPrev_clicked);

        QLabel *lblPage = new QLabel("Page 1 sur 1");
        lblPage->setObjectName("client_pagination_lblPage");
        lblPage->setStyleSheet("font-size: 13px; color: #1e293b; font-weight: 700;");

        QPushButton *btnNext = new QPushButton("Suivant " + QString::fromUtf8("\xe2\x86\x92"));
        btnNext->setObjectName("client_pagination_btnNext");
        btnNext->setFixedWidth(120);
        btnNext->setCursor(Qt::PointingHandCursor);
        btnNext->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnNext, &QPushButton::clicked, this, &MainWindow::on_client_pagination_btnNext_clicked);

        pLayout->addWidget(lblSize);
        pLayout->addWidget(cbSize);
        pLayout->addStretch();
        pLayout->addWidget(btnPrev);
        pLayout->addWidget(lblPage);
        pLayout->addWidget(btnNext);

        tableLayout->addWidget(pBar);
    });
}

void MainWindow::slot_toggleClientView()
{
    m_isClientCardView = !m_isClientCardView;

    if (ui->tableWidget_Client)
        ui->tableWidget_Client->setVisible(!m_isClientCardView);

    // Hide the Pilotage Commercial side panel when in card view
    QWidget *sidepanel = findChild<QWidget*>("sidepanel");
    if (sidepanel) sidepanel->setVisible(!m_isClientCardView);

    QScrollArea *cardScroll = findChild<QScrollArea*>("client_cardScrollArea");
    if (cardScroll) cardScroll->setVisible(m_isClientCardView);

    QWidget *pBar = findChild<QWidget*>("client_paginationBar");
    if (pBar) pBar->setVisible(m_isClientCardView);

    if (m_isClientCardView) {
        if (!m_clientCardLayout) m_clientCardLayout = findChild<QGridLayout*>("client_cardLayout");
        refreshClientCardView();
    }
}

void MainWindow::refreshClientCardView()
{
    if (!m_clientCardLayout) return;
    QTableWidget *t = ui->tableWidget_Client;
    if (!t) return;

    // Clear existing cards
    while (QLayoutItem *item = m_clientCardLayout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    int totalItems = t->rowCount();
    int startIdx = m_clientCurrentPage * m_clientItemsPerPage;
    int endIdx = qMin(startIdx + m_clientItemsPerPage, totalItems);

    int col = 0, row = 0;
    const int cols = 3;
    for (int i = startIdx; i < endIdx; ++i) {
        QWidget *card = createClientCard(i);
        if (card) {
            m_clientCardLayout->addWidget(card, row, col);
            col++;
            if (col >= cols) { col = 0; row++; }
        }
    }

    // Update pagination
    int totalPages = qMax(1, (totalItems + m_clientItemsPerPage - 1) / m_clientItemsPerPage);
    QLabel *lblPage = findChild<QLabel*>("client_pagination_lblPage");
    if (lblPage) lblPage->setText(QString("Page %1 sur %2").arg(m_clientCurrentPage + 1).arg(totalPages));

    QPushButton *btnPrev = findChild<QPushButton*>("client_pagination_btnPrev");
    if (btnPrev) btnPrev->setEnabled(m_clientCurrentPage > 0);

    QPushButton *btnNext = findChild<QPushButton*>("client_pagination_btnNext");
    if (btnNext) btnNext->setEnabled(m_clientCurrentPage < totalPages - 1);
}

QWidget* MainWindow::createClientCard(int row)
{
    QTableWidget *t = ui->tableWidget_Client;
    if (!t || row < 0 || row >= t->rowCount()) return nullptr;

    auto getText = [&](int col) -> QString {
        QTableWidgetItem *item = t->item(row, col);
        return item ? item->text() : QString();
    };

    QString matricule = getText(0);
    QString nom       = getText(1);
    QString email     = getText(2);
    QString bacs      = getText(3);
    QString score     = getText(4);
    QString paiement  = getText(5);

    // Score color
    int scoreVal = score.toInt();
    QString scoreColor = scoreVal >= 80 ? "#16a34a" : (scoreVal >= 60 ? "#d97706" : "#dc2626");
    QString scoreBg    = scoreVal >= 80 ? "#dcfce7" : (scoreVal >= 60 ? "#fef3c7" : "#fee2e2");

    // Paiement color
    QString paiColor = "#2563eb";
    QString paiBg    = "#eff6ff";
    if (paiement == "Trimestriel") { paiColor = "#7c3aed"; paiBg = "#f5f3ff"; }
    else if (paiement == "Annuel") { paiColor = "#0891b2"; paiBg = "#ecfeff"; }

    // Avatar color
    QStringList avatarColors = {"#7c3aed","#2563eb","#0891b2","#059669","#d97706","#dc2626","#0f766e","#9333ea","#c2410c"};
    QString avatarColor = avatarColors[row % avatarColors.size()];

    // Initials (first + last name)
    QStringList parts = nom.split(' ', Qt::SkipEmptyParts);
    QString initials = parts.size() >= 2
        ? QString(parts[0][0]).toUpper() + QString(parts[1][0]).toUpper()
        : (nom.isEmpty() ? "?" : QString(nom[0]).toUpper());

    // Card widget
    QFrame *card = new QFrame();
    card->setFrameShape(QFrame::StyledPanel);
    card->setMinimumWidth(240);
    card->setMaximumWidth(380);
    card->setStyleSheet(
        "QFrame { background: white; border-radius: 16px; border: 1px solid #e2e8f0; }"
    );
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(18);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 28));
    card->setGraphicsEffect(shadow);

    QVBoxLayout *vl = new QVBoxLayout(card);
    vl->setContentsMargins(18, 18, 18, 18);
    vl->setSpacing(12);

    // ── Header: avatar + matricule/name + score pill ───────────────────────
    QHBoxLayout *headerRow = new QHBoxLayout();
    headerRow->setSpacing(12);

    QLabel *avatar = new QLabel(initials);
    avatar->setFixedSize(56, 56);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setStyleSheet(QString(
        "background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 %1,stop:1 %2);"
        "border-radius: 28px; color: white; font-size: 18px; font-weight: 700; border: none;"
    ).arg(avatarColor).arg(avatarColor));
    headerRow->addWidget(avatar);

    QVBoxLayout *nameCol = new QVBoxLayout();
    nameCol->setSpacing(2);
    QLabel *lblMat = new QLabel(matricule);
    lblMat->setStyleSheet("font-size: 11px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *lblNom = new QLabel(nom);
    lblNom->setStyleSheet("font-size: 15px; font-weight: 700; color: #1e293b; background: transparent; border: none;");
    lblNom->setWordWrap(true);
    nameCol->addWidget(lblMat);
    nameCol->addWidget(lblNom);
    headerRow->addLayout(nameCol, 1);

    // Score pill
    QLabel *scorePill = new QLabel(score + "%");
    scorePill->setAlignment(Qt::AlignCenter);
    scorePill->setFixedHeight(28);
    scorePill->setMinimumWidth(52);
    scorePill->setContentsMargins(10, 0, 10, 0);
    scorePill->setStyleSheet(QString(
        "background: %1; color: %2; border-radius: 14px; font-size: 12px; font-weight: 700; border: none;"
    ).arg(scoreBg).arg(scoreColor));
    headerRow->addWidget(scorePill);
    vl->addLayout(headerRow);

    // ── Email ──────────────────────────────────────────────────────────────
    QLabel *emailLbl = new QLabel(email);
    emailLbl->setStyleSheet(
        "font-size: 12px; color: #64748b; background: #f8fafc; border-radius: 8px;"
        "padding: 6px 10px; border: none;"
    );
    emailLbl->setWordWrap(true);
    vl->addWidget(emailLbl);

    // ── Divider ────────────────────────────────────────────────────────────
    QFrame *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setFixedHeight(1);
    divider->setStyleSheet("background: #f1f5f9; border: none;");
    vl->addWidget(divider);

    // ── Stats: bacs + paiement ─────────────────────────────────────────────
    QHBoxLayout *statsRow = new QHBoxLayout();
    statsRow->setSpacing(10);

    // Bacs box
    QWidget *bacsBox = new QWidget();
    bacsBox->setStyleSheet("background: #f8fafc; border-radius: 10px; border: none;");
    QVBoxLayout *bacsL = new QVBoxLayout(bacsBox);
    bacsL->setContentsMargins(12, 8, 12, 8);
    bacsL->setSpacing(3);
    QLabel *bacsLbl = new QLabel("Bacs");
    bacsLbl->setStyleSheet("font-size: 10px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *bacsVal = new QLabel(bacs + " bacs");
    bacsVal->setStyleSheet("font-size: 15px; font-weight: 700; color: #1e293b; background: transparent; border: none;");
    bacsL->addWidget(bacsLbl);
    bacsL->addWidget(bacsVal);
    statsRow->addWidget(bacsBox, 1);

    // Paiement box
    QWidget *paiBox = new QWidget();
    paiBox->setStyleSheet(QString("background: %1; border-radius: 10px; border: none;").arg(paiBg));
    QVBoxLayout *paiL = new QVBoxLayout(paiBox);
    paiL->setContentsMargins(12, 8, 12, 8);
    paiL->setSpacing(3);
    QLabel *paiLbl = new QLabel("Paiement");
    paiLbl->setStyleSheet("font-size: 10px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *paiVal = new QLabel(paiement);
    paiVal->setStyleSheet(QString("font-size: 13px; font-weight: 700; color: %1; background: transparent; border: none;").arg(paiColor));
    paiL->addWidget(paiLbl);
    paiL->addWidget(paiVal);
    statsRow->addWidget(paiBox, 1);

    vl->addLayout(statsRow);

    // ── Action buttons ─────────────────────────────────────────────────────
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->setSpacing(8);

    QPushButton *btnView = new QPushButton("Voir Profil");
    btnView->setCursor(Qt::PointingHandCursor);
    btnView->setStyleSheet(
        "QPushButton { background: #7c3aed; color: white; border-radius: 10px;"
        "  font-size: 12px; font-weight: 700; padding: 9px 0; border: none; }"
        "QPushButton:hover { background: #6d28d9; }"
    );

    QPushButton *btnEdit = new QPushButton("Modifier");
    btnEdit->setCursor(Qt::PointingHandCursor);
    btnEdit->setStyleSheet(
        "QPushButton { background: #fff7ed; color: #ea580c; border-radius: 10px;"
        "  font-size: 12px; font-weight: 700; padding: 9px 0; border: 1px solid #fed7aa; }"
        "QPushButton:hover { background: #ffedd5; }"
    );

    btnRow->addWidget(btnView, 1);
    btnRow->addWidget(btnEdit, 1);
    vl->addLayout(btnRow);

    return card;
}

void MainWindow::on_client_pagination_cbSize_currentIndexChanged(int)
{
    QComboBox *cb = findChild<QComboBox*>("client_pagination_cbSize");
    if (!cb) return;
    m_clientItemsPerPage = cb->currentText().toInt();
    m_clientCurrentPage = 0;
    refreshClientCardView();
}

void MainWindow::on_client_pagination_btnPrev_clicked()
{
    if (m_clientCurrentPage > 0) {
        m_clientCurrentPage--;
        refreshClientCardView();
    }
}

void MainWindow::on_client_pagination_btnNext_clicked()
{
    if (!ui->tableWidget_Client) return;
    int totalItems = ui->tableWidget_Client->rowCount();
    int maxPages = (totalItems + m_clientItemsPerPage - 1) / m_clientItemsPerPage;
    if (m_clientCurrentPage < maxPages - 1) {
        m_clientCurrentPage++;
        refreshClientCardView();
    }
}

// ============================================================
// MAINTENANCE CARD VIEW MODULE
// ============================================================

void MainWindow::setupMaintCardViewContainer()
{
    QTimer::singleShot(200, this, [this]() {
        if (findChild<QPushButton*>("maint_btnToggleView")) return;

        QTableWidget *t = ui->tableMaintenance;
        if (!t) return;

        QWidget *pageWidget = t->parentWidget();
        if (!pageWidget) return;

        // Walk up to find the layouts by name
        QHBoxLayout *searchLayout = nullptr;
        QVBoxLayout *tableLayout  = nullptr;
        QWidget *w = pageWidget;
        for (int attempt = 0; attempt < 6 && w; attempt++) {
            searchLayout = w->findChild<QHBoxLayout*>("searchRow");
            tableLayout  = w->findChild<QVBoxLayout*>("tableLayout");
            if (searchLayout && tableLayout) break;
            w = w->parentWidget();
        }
        if (!searchLayout || !tableLayout) return;

        // Toggle button
        QPushButton *btnToggle = new QPushButton(QString::fromUtf8("\xe2\x8a\x9e"));
        btnToggle->setObjectName("maint_btnToggleView");
        btnToggle->setFixedSize(40, 40);
        btnToggle->setCursor(Qt::PointingHandCursor);
        btnToggle->setToolTip("Vue cartes / tableau");
        btnToggle->setStyleSheet(
            "QPushButton { background: #0f766e; color: white; border-radius: 10px;"
            "  font-size: 18px; border: none; font-weight: 700; }"
            "QPushButton:hover { background: #0d9488; }"
        );
        connect(btnToggle, &QPushButton::clicked, this, &MainWindow::slot_toggleMaintView);
        searchLayout->addWidget(btnToggle);

        // Card scroll area
        QScrollArea *cardScroll = new QScrollArea();
        cardScroll->setObjectName("maint_cardScrollArea");
        cardScroll->setWidgetResizable(true);
        cardScroll->setFrameShape(QFrame::NoFrame);
        cardScroll->setStyleSheet("background: transparent; border: none;");
        cardScroll->setVisible(false);

        QWidget *cardContainer = new QWidget();
        cardContainer->setObjectName("maint_cardContainer");
        cardContainer->setStyleSheet("background: transparent;");
        QGridLayout *gridLayout = new QGridLayout(cardContainer);
        gridLayout->setObjectName("maint_cardLayout");
        gridLayout->setSpacing(16);
        gridLayout->setContentsMargins(8, 8, 8, 8);
        cardScroll->setWidget(cardContainer);
        m_maintCardLayout = gridLayout;

        tableLayout->addWidget(cardScroll);

        // Pagination bar
        QWidget *pBar = new QWidget();
        pBar->setObjectName("maint_paginationBar");
        pBar->setFixedHeight(60);
        pBar->setVisible(false);
        pBar->setStyleSheet("background: #f8fafc; border-top: 1px solid #e2e8f0; border-radius: 12px;");

        QHBoxLayout *pLayout = new QHBoxLayout(pBar);
        pLayout->setContentsMargins(20, 0, 20, 0);

        QLabel *lblSize = new QLabel("Items par page:");
        lblSize->setStyleSheet("font-size: 13px; color: #64748b; font-weight: 600;");

        QComboBox *cbSize = new QComboBox();
        cbSize->setObjectName("maint_pagination_cbSize");
        cbSize->addItems({"4", "6", "12"});
        cbSize->setCurrentText("6");
        cbSize->setFixedWidth(70);
        cbSize->setStyleSheet("background: white; border: 1px solid #cbd5e1; border-radius: 8px; padding: 4px;");
        connect(cbSize, SIGNAL(currentIndexChanged(int)), this, SLOT(on_maint_pagination_cbSize_currentIndexChanged(int)));

        QPushButton *btnPrev = new QPushButton(QString::fromUtf8("\xe2\x86\x90") + " Precedent");
        btnPrev->setObjectName("maint_pagination_btnPrev");
        btnPrev->setFixedWidth(120);
        btnPrev->setCursor(Qt::PointingHandCursor);
        btnPrev->setEnabled(false);
        btnPrev->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnPrev, &QPushButton::clicked, this, &MainWindow::on_maint_pagination_btnPrev_clicked);

        QLabel *lblPage = new QLabel("Page 1 sur 1");
        lblPage->setObjectName("maint_pagination_lblPage");
        lblPage->setStyleSheet("font-size: 13px; color: #1e293b; font-weight: 700;");

        QPushButton *btnNext = new QPushButton("Suivant " + QString::fromUtf8("\xe2\x86\x92"));
        btnNext->setObjectName("maint_pagination_btnNext");
        btnNext->setFixedWidth(120);
        btnNext->setCursor(Qt::PointingHandCursor);
        btnNext->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnNext, &QPushButton::clicked, this, &MainWindow::on_maint_pagination_btnNext_clicked);

        pLayout->addWidget(lblSize);
        pLayout->addWidget(cbSize);
        pLayout->addStretch();
        pLayout->addWidget(btnPrev);
        pLayout->addWidget(lblPage);
        pLayout->addWidget(btnNext);

        tableLayout->addWidget(pBar);
    });
}

void MainWindow::slot_toggleMaintView()
{
    m_isMaintCardView = !m_isMaintCardView;

    // Hide/show the card frame that wraps the table
    QFrame *cardFrame = findChild<QFrame*>("card");
    if (cardFrame) cardFrame->setVisible(!m_isMaintCardView);

    QScrollArea *cardScroll = findChild<QScrollArea*>("maint_cardScrollArea");
    if (cardScroll) cardScroll->setVisible(m_isMaintCardView);

    QWidget *pBar = findChild<QWidget*>("maint_paginationBar");
    if (pBar) pBar->setVisible(m_isMaintCardView);

    if (m_isMaintCardView) {
        if (!m_maintCardLayout) m_maintCardLayout = findChild<QGridLayout*>("maint_cardLayout");
        refreshMaintCardView();
    }
}

void MainWindow::refreshMaintCardView()
{
    if (!m_maintCardLayout) return;
    QTableWidget *t = ui->tableMaintenance;
    if (!t) return;

    // Clear existing cards
    while (QLayoutItem *item = m_maintCardLayout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    int totalItems = t->rowCount();
    int startIdx = m_maintCurrentPage * m_maintItemsPerPage;
    int endIdx = qMin(startIdx + m_maintItemsPerPage, totalItems);

    int col = 0, row = 0;
    const int cols = 3;
    for (int i = startIdx; i < endIdx; ++i) {
        QWidget *card = createMaintCard(i);
        if (card) {
            m_maintCardLayout->addWidget(card, row, col);
            col++;
            if (col >= cols) { col = 0; row++; }
        }
    }

    // Update pagination
    int totalPages = qMax(1, (totalItems + m_maintItemsPerPage - 1) / m_maintItemsPerPage);
    QLabel *lblPage = findChild<QLabel*>("maint_pagination_lblPage");
    if (lblPage) lblPage->setText(QString("Page %1 sur %2").arg(m_maintCurrentPage + 1).arg(totalPages));

    QPushButton *btnPrev = findChild<QPushButton*>("maint_pagination_btnPrev");
    if (btnPrev) btnPrev->setEnabled(m_maintCurrentPage > 0);

    QPushButton *btnNext = findChild<QPushButton*>("maint_pagination_btnNext");
    if (btnNext) btnNext->setEnabled(m_maintCurrentPage < totalPages - 1);
}

QWidget* MainWindow::createMaintCard(int row)
{
    QTableWidget *t = ui->tableMaintenance;
    if (!t || row < 0 || row >= t->rowCount()) return nullptr;

    auto getText = [&](int col) -> QString {
        QTableWidgetItem *item = t->item(row, col);
        return item ? item->text() : QString();
    };

    QString ref      = getText(1);
    QString date     = getText(2);
    QString tech     = getText(3);
    QString cout_    = getText(4);
    QString duree    = getText(5);
    QString priorite = getText(6);

    // Priority colors
    QString prioColor = "#16a34a";
    QString prioBg    = "#dcfce7";
    QString prioText  = priorite.isEmpty() ? "Normale" : priorite;
    if (priorite == "Critique") { prioColor = "#dc2626"; prioBg = "#fee2e2"; }
    else if (priorite == "Urgente") { prioColor = "#d97706"; prioBg = "#fef3c7"; }

    // Avatar color based on row
    QStringList avatarColors = {"#0f766e","#2563eb","#7c3aed","#059669","#d97706","#dc2626","#0891b2","#9333ea","#c2410c"};
    QString avatarColor = avatarColors[row % avatarColors.size()];

    // Initials from ref (e.g. INT-001 -> I)
    QString initials = ref.isEmpty() ? "?" : QString(ref[0]).toUpper();
    // Or from tech name
    QStringList parts = tech.split(' ', Qt::SkipEmptyParts);
    if (parts.size() >= 2)
        initials = QString(parts[0][0]).toUpper() + QString(parts[1][0]).toUpper();
    else if (!tech.isEmpty())
        initials = QString(tech[0]).toUpper();

    // Card widget
    QFrame *card = new QFrame();
    card->setFrameShape(QFrame::StyledPanel);
    card->setMinimumWidth(240);
    card->setMaximumWidth(380);
    card->setStyleSheet(
        "QFrame { background: white; border-radius: 16px; border: 1px solid #e2e8f0; }"
    );
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(18);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 28));
    card->setGraphicsEffect(shadow);

    QVBoxLayout *vl = new QVBoxLayout(card);
    vl->setContentsMargins(18, 18, 18, 18);
    vl->setSpacing(12);

    // ── Header: avatar + ref/tech + priority pill ──────────────────────────
    QHBoxLayout *headerRow = new QHBoxLayout();
    headerRow->setSpacing(12);

    QLabel *avatar = new QLabel(initials);
    avatar->setFixedSize(52, 52);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setStyleSheet(QString(
        "background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 %1,stop:1 %2);"
        "border-radius: 26px; color: white; font-size: 18px; font-weight: 700; border: none;"
    ).arg(avatarColor).arg(avatarColor));
    headerRow->addWidget(avatar);

    QVBoxLayout *nameCol = new QVBoxLayout();
    nameCol->setSpacing(2);
    QLabel *lblRef = new QLabel(ref);
    lblRef->setStyleSheet("font-size: 11px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *lblTech = new QLabel(tech);
    lblTech->setStyleSheet("font-size: 14px; font-weight: 700; color: #1e293b; background: transparent; border: none;");
    lblTech->setWordWrap(true);
    nameCol->addWidget(lblRef);
    nameCol->addWidget(lblTech);
    headerRow->addLayout(nameCol, 1);

    // Priority pill
    QLabel *prioPill = new QLabel(prioText);
    prioPill->setAlignment(Qt::AlignCenter);
    prioPill->setFixedHeight(26);
    prioPill->setMinimumWidth(65);
    prioPill->setContentsMargins(10, 0, 10, 0);
    prioPill->setStyleSheet(QString(
        "background: %1; color: %2; border-radius: 13px; font-size: 11px; font-weight: 700; border: none;"
    ).arg(prioBg).arg(prioColor));
    headerRow->addWidget(prioPill);
    vl->addLayout(headerRow);

    // ── Date badge ─────────────────────────────────────────────────────────
    QLabel *dateLbl = new QLabel("[Date] " + date);
    dateLbl->setStyleSheet(
        "font-size: 12px; color: #475569; background: #f1f5f9; border-radius: 8px;"
        "padding: 6px 10px; border: none; font-weight: 600;"
    );
    vl->addWidget(dateLbl);

    // ── Divider ────────────────────────────────────────────────────────────
    QFrame *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setFixedHeight(1);
    divider->setStyleSheet("background: #f1f5f9; border: none;");
    vl->addWidget(divider);

    // ── Stats: cout + duree ────────────────────────────────────────────────
    QHBoxLayout *statsRow = new QHBoxLayout();
    statsRow->setSpacing(10);

    // Cout box
    QWidget *coutBox = new QWidget();
    coutBox->setStyleSheet("background: #fff7ed; border-radius: 10px; border: none;");
    QVBoxLayout *coutL = new QVBoxLayout(coutBox);
    coutL->setContentsMargins(12, 8, 12, 8);
    coutL->setSpacing(3);
    QLabel *coutLbl = new QLabel(QString::fromUtf8("Co\xc3\xbbt"));
    coutLbl->setStyleSheet("font-size: 10px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *coutVal = new QLabel(cout_.isEmpty() ? "---" : cout_);
    coutVal->setStyleSheet("font-size: 14px; font-weight: 700; color: #ea580c; background: transparent; border: none;");
    coutL->addWidget(coutLbl);
    coutL->addWidget(coutVal);
    statsRow->addWidget(coutBox, 1);

    // Duree box
    QWidget *dureeBox = new QWidget();
    dureeBox->setStyleSheet("background: #f0fdf4; border-radius: 10px; border: none;");
    QVBoxLayout *dureeL = new QVBoxLayout(dureeBox);
    dureeL->setContentsMargins(12, 8, 12, 8);
    dureeL->setSpacing(3);
    QLabel *dureeLbl = new QLabel(QString::fromUtf8("Dur\xc3\xa9e"));
    dureeLbl->setStyleSheet("font-size: 10px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *dureeVal = new QLabel(duree.isEmpty() ? "---" : duree);
    dureeVal->setStyleSheet("font-size: 14px; font-weight: 700; color: #16a34a; background: transparent; border: none;");
    dureeL->addWidget(dureeLbl);
    dureeL->addWidget(dureeVal);
    statsRow->addWidget(dureeBox, 1);

    vl->addLayout(statsRow);

    // ── Action buttons ─────────────────────────────────────────────────────
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->setSpacing(8);

    QPushButton *btnView = new QPushButton(QString::fromUtf8("D\xc3\xa9tails"));
    btnView->setCursor(Qt::PointingHandCursor);
    btnView->setStyleSheet(
        "QPushButton { background: #0f766e; color: white; border-radius: 10px;"
        "  font-size: 12px; font-weight: 700; padding: 9px 0; border: none; }"
        "QPushButton:hover { background: #0d9488; }"
    );

    QPushButton *btnEdit = new QPushButton("Modifier");
    btnEdit->setCursor(Qt::PointingHandCursor);
    btnEdit->setStyleSheet(
        "QPushButton { background: #fff7ed; color: #ea580c; border-radius: 10px;"
        "  font-size: 12px; font-weight: 700; padding: 9px 0; border: 1px solid #fed7aa; }"
        "QPushButton:hover { background: #ffedd5; }"
    );

    btnRow->addWidget(btnView, 1);
    btnRow->addWidget(btnEdit, 1);
    vl->addLayout(btnRow);

    return card;
}

void MainWindow::on_maint_pagination_cbSize_currentIndexChanged(int)
{
    QComboBox *cb = findChild<QComboBox*>("maint_pagination_cbSize");
    if (!cb) return;
    m_maintItemsPerPage = cb->currentText().toInt();
    m_maintCurrentPage = 0;
    refreshMaintCardView();
}

void MainWindow::on_maint_pagination_btnPrev_clicked()
{
    if (m_maintCurrentPage > 0) {
        m_maintCurrentPage--;
        refreshMaintCardView();
    }
}

void MainWindow::on_maint_pagination_btnNext_clicked()
{
    if (!ui->tableMaintenance) return;
    int totalItems = ui->tableMaintenance->rowCount();
    int maxPages = (totalItems + m_maintItemsPerPage - 1) / m_maintItemsPerPage;
    if (m_maintCurrentPage < maxPages - 1) {
        m_maintCurrentPage++;
        refreshMaintCardView();
    }
}


// ============================================================
// COMMANDE CARD VIEW MODULE
// ============================================================

void MainWindow::setupCmdCardViewContainer()
{
    QTimer::singleShot(200, this, [this]() {
        if (findChild<QPushButton*>("cmd_btnToggleView")) return;

        QTableWidget *t = ui->tableProduits_2;
        if (!t) return;

        // The UI structure is:
        //   pageCommandes (QWidget, layout: pageCommandesLayout QVBoxLayout)
        //     tableLayout_2 (QVBoxLayout)
        //       tableProduits_2 (QTableWidget)
        //       searchRow_2 (QHBoxLayout) — search bar row
        //
        // We inject the toggle button into searchRow_2,
        // and add the card scroll area + pagination into tableLayout_2.

        QWidget *cmdPage = findChild<QWidget*>("pageCommandes");
        if (!cmdPage) return;

        // Find the outer VBoxLayout that holds the table
        QVBoxLayout *tableLayout = cmdPage->findChild<QVBoxLayout*>("tableLayout_2");
        if (!tableLayout) {
            // fallback: use the page's own layout
            tableLayout = qobject_cast<QVBoxLayout*>(cmdPage->layout());
        }
        if (!tableLayout) return;

        // Find the search row HBoxLayout to inject the toggle button
        QHBoxLayout *searchLayout = cmdPage->findChild<QHBoxLayout*>("searchRow_2");

        // Toggle button
        QPushButton *btnToggle = new QPushButton(QString::fromUtf8("\xe2\x8a\x9e"));
        btnToggle->setObjectName("cmd_btnToggleView");
        btnToggle->setFixedSize(40, 40);
        btnToggle->setCursor(Qt::PointingHandCursor);
        btnToggle->setToolTip("Vue cartes / tableau");
        btnToggle->setStyleSheet(
            "QPushButton { background: #0f2b4c; color: white; border-radius: 10px;"
            "  font-size: 18px; border: none; font-weight: 700; }"
            "QPushButton:hover { background: #1a4270; }"
        );
        connect(btnToggle, &QPushButton::clicked, this, &MainWindow::slot_toggleCmdView);

        if (searchLayout) {
            searchLayout->addWidget(btnToggle);
        } else {
            // Place button at top-right as a new row
            QHBoxLayout *btnRow = new QHBoxLayout();
            btnRow->addStretch();
            btnRow->addWidget(btnToggle);
            tableLayout->insertLayout(0, btnRow);
        }

        // Card scroll area
        QScrollArea *cardScroll = new QScrollArea();
        cardScroll->setObjectName("cmd_cardScrollArea");
        cardScroll->setWidgetResizable(true);
        cardScroll->setFrameShape(QFrame::NoFrame);
        cardScroll->setStyleSheet("background: transparent; border: none;");
        cardScroll->setVisible(false);

        QWidget *cardContainer = new QWidget();
        cardContainer->setObjectName("cmd_cardContainer");
        cardContainer->setStyleSheet("background: transparent;");
        QGridLayout *gridLayout = new QGridLayout(cardContainer);
        gridLayout->setObjectName("cmd_cardLayout");
        gridLayout->setSpacing(16);
        gridLayout->setContentsMargins(8, 8, 8, 8);
        cardScroll->setWidget(cardContainer);
        m_cmdCardLayout = gridLayout;

        tableLayout->addWidget(cardScroll);

        // Pagination bar
        QWidget *pBar = new QWidget();
        pBar->setObjectName("cmd_paginationBar");
        pBar->setFixedHeight(60);
        pBar->setVisible(false);
        pBar->setStyleSheet("background: #f8fafc; border-top: 1px solid #e2e8f0; border-radius: 12px;");

        QHBoxLayout *pLayout = new QHBoxLayout(pBar);
        pLayout->setContentsMargins(20, 0, 20, 0);

        QLabel *lblSize = new QLabel("Items par page:");
        lblSize->setStyleSheet("font-size: 13px; color: #64748b; font-weight: 600;");

        QComboBox *cbSize = new QComboBox();
        cbSize->setObjectName("cmd_pagination_cbSize");
        cbSize->addItems({"4", "6", "12"});
        cbSize->setCurrentText("6");
        cbSize->setFixedWidth(70);
        cbSize->setStyleSheet("background: white; border: 1px solid #cbd5e1; border-radius: 8px; padding: 4px;");
        connect(cbSize, SIGNAL(currentIndexChanged(int)), this, SLOT(on_cmd_pagination_cbSize_currentIndexChanged(int)));

        QPushButton *btnPrev = new QPushButton(QString::fromUtf8("\xe2\x86\x90") + " Precedent");
        btnPrev->setObjectName("cmd_pagination_btnPrev");
        btnPrev->setFixedWidth(120);
        btnPrev->setCursor(Qt::PointingHandCursor);
        btnPrev->setEnabled(false);
        btnPrev->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnPrev, &QPushButton::clicked, this, &MainWindow::on_cmd_pagination_btnPrev_clicked);

        QLabel *lblPage = new QLabel("Page 1 sur 1");
        lblPage->setObjectName("cmd_pagination_lblPage");
        lblPage->setStyleSheet("font-size: 13px; color: #1e293b; font-weight: 700;");

        QPushButton *btnNext = new QPushButton("Suivant " + QString::fromUtf8("\xe2\x86\x92"));
        btnNext->setObjectName("cmd_pagination_btnNext");
        btnNext->setFixedWidth(120);
        btnNext->setCursor(Qt::PointingHandCursor);
        btnNext->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnNext, &QPushButton::clicked, this, &MainWindow::on_cmd_pagination_btnNext_clicked);

        pLayout->addWidget(lblSize);
        pLayout->addWidget(cbSize);
        pLayout->addStretch();
        pLayout->addWidget(btnPrev);
        pLayout->addWidget(lblPage);
        pLayout->addWidget(btnNext);

        tableLayout->addWidget(pBar);
    });
}

void MainWindow::slot_toggleCmdView()
{
    m_isCmdCardView = !m_isCmdCardView;

    if (ui->tableProduits_2)
        ui->tableProduits_2->setVisible(!m_isCmdCardView);

    QScrollArea *cardScroll = findChild<QScrollArea*>("cmd_cardScrollArea");
    if (cardScroll) cardScroll->setVisible(m_isCmdCardView);

    QWidget *pBar = findChild<QWidget*>("cmd_paginationBar");
    if (pBar) pBar->setVisible(m_isCmdCardView);

    if (m_isCmdCardView) {
        if (!m_cmdCardLayout) m_cmdCardLayout = findChild<QGridLayout*>("cmd_cardLayout");
        refreshCmdCardView();
    }
}

void MainWindow::refreshCmdCardView()
{
    if (!m_cmdCardLayout) return;
    QTableWidget *t = ui->tableProduits_2;
    if (!t) return;

    // Clear existing cards
    while (QLayoutItem *item = m_cmdCardLayout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    int totalItems = t->rowCount();
    int startIdx = m_cmdCurrentPage * m_cmdItemsPerPage;
    int endIdx = qMin(startIdx + m_cmdItemsPerPage, totalItems);

    int col = 0, row = 0;
    const int cols = 3;
    for (int i = startIdx; i < endIdx; ++i) {
        QWidget *card = createCmdCard(i);
        if (card) {
            m_cmdCardLayout->addWidget(card, row, col);
            col++;
            if (col >= cols) { col = 0; row++; }
        }
    }

    // Update pagination
    int totalPages = qMax(1, (totalItems + m_cmdItemsPerPage - 1) / m_cmdItemsPerPage);
    QLabel *lblPage = findChild<QLabel*>("cmd_pagination_lblPage");
    if (lblPage) lblPage->setText(QString("Page %1 sur %2").arg(m_cmdCurrentPage + 1).arg(totalPages));

    QPushButton *btnPrev = findChild<QPushButton*>("cmd_pagination_btnPrev");
    if (btnPrev) btnPrev->setEnabled(m_cmdCurrentPage > 0);

    QPushButton *btnNext = findChild<QPushButton*>("cmd_pagination_btnNext");
    if (btnNext) btnNext->setEnabled(m_cmdCurrentPage < totalPages - 1);
}

QWidget* MainWindow::createCmdCard(int row)
{
    QTableWidget *t = ui->tableProduits_2;
    if (!t || row < 0 || row >= t->rowCount()) return nullptr;

    auto getText = [&](int col) -> QString {
        QTableWidgetItem *item = t->item(row, col);
        return item ? item->text() : QString();
    };

    QString id          = getText(0);
    QString qty         = getText(1);
    QString priority    = getText(2);
    QString status      = getText(3);
    QString address     = getText(4);
    QString date        = getText(5);
    QString stock       = getText(6);
    QString delivDate   = getText(7);
    QString price       = getText(8);

    // Priority color mapping
    QString prioColor = "#64748b";
    QString prioBg    = "#f1f5f9";
    QString prioText  = priority.isEmpty() ? "Normale" : priority;
    if (priority.contains("Haute", Qt::CaseInsensitive) || priority.contains("High", Qt::CaseInsensitive)) {
        prioColor = "#dc2626"; prioBg = "#fee2e2";
    } else if (priority.contains("Moyenne", Qt::CaseInsensitive) || priority.contains("Medium", Qt::CaseInsensitive)) {
        prioColor = "#d97706"; prioBg = "#fef3c7";
    } else if (priority.contains("Basse", Qt::CaseInsensitive) || priority.contains("Low", Qt::CaseInsensitive)) {
        prioColor = "#16a34a"; prioBg = "#dcfce7";
    }

    // Status color mapping
    QString statusColor = "#64748b";
    QString statusBg    = "#f1f5f9";
    if (status.contains("preparation", Qt::CaseInsensitive) || status.contains("En cours", Qt::CaseInsensitive)) {
        statusColor = "#2563eb"; statusBg = "#eff6ff";
    } else if (status.contains("Livr", Qt::CaseInsensitive) || status.contains("Complet", Qt::CaseInsensitive)) {
        statusColor = "#16a34a"; statusBg = "#dcfce7";
    } else if (status.contains("Annul", Qt::CaseInsensitive)) {
        statusColor = "#dc2626"; statusBg = "#fee2e2";
    } else if (status.contains("Pending", Qt::CaseInsensitive) || status.contains("Pendante", Qt::CaseInsensitive)) {
        statusColor = "#d97706"; statusBg = "#fef3c7";
    }

    // Avatar color based on row
    QStringList avatarColors = {"#0f2b4c","#2563eb","#7c3aed","#059669","#d97706","#dc2626","#0891b2","#9333ea","#c2410c"};
    QString avatarColor = avatarColors[row % avatarColors.size()];

    // Initials from ID
    QString initials = id.isEmpty() ? "?" : QString(id[0]).toUpper();
    if (id.contains("-") && id.split("-").size() >= 2)
        initials = id.split("-").last().left(3);

    // Card widget
    QFrame *card = new QFrame();
    card->setFrameShape(QFrame::StyledPanel);
    card->setMinimumWidth(240);
    card->setMaximumWidth(380);
    card->setStyleSheet(
        "QFrame { background: white; border-radius: 16px; border: 1px solid #e2e8f0; }"
    );
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(18);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 28));
    card->setGraphicsEffect(shadow);

    QVBoxLayout *vl = new QVBoxLayout(card);
    vl->setContentsMargins(18, 18, 18, 18);
    vl->setSpacing(12);

    // ── Header: avatar + ID/stock + priority pill ──────────────────────────
    QHBoxLayout *headerRow = new QHBoxLayout();
    headerRow->setSpacing(12);

    QLabel *avatar = new QLabel(initials);
    avatar->setFixedSize(52, 52);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setStyleSheet(QString(
        "background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 %1,stop:1 %2);"
        "border-radius: 26px; color: white; font-size: 14px; font-weight: 700; border: none;"
    ).arg(avatarColor).arg(avatarColor));
    headerRow->addWidget(avatar);

    QVBoxLayout *nameCol = new QVBoxLayout();
    nameCol->setSpacing(2);
    QLabel *lblId = new QLabel(id);
    lblId->setStyleSheet("font-size: 11px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *lblStock = new QLabel(stock);
    lblStock->setStyleSheet("font-size: 14px; font-weight: 700; color: #1e293b; background: transparent; border: none;");
    lblStock->setWordWrap(true);
    nameCol->addWidget(lblId);
    nameCol->addWidget(lblStock);
    headerRow->addLayout(nameCol, 1);

    // Priority pill
    QLabel *prioPill = new QLabel(prioText);
    prioPill->setAlignment(Qt::AlignCenter);
    prioPill->setFixedHeight(26);
    prioPill->setMinimumWidth(65);
    prioPill->setContentsMargins(10, 0, 10, 0);
    prioPill->setStyleSheet(QString(
        "background: %1; color: %2; border-radius: 13px; font-size: 11px; font-weight: 700; border: none;"
    ).arg(prioBg).arg(prioColor));
    headerRow->addWidget(prioPill);
    vl->addLayout(headerRow);

    // ── Status + address badge ─────────────────────────────────────────────
    QLabel *statusLbl = new QLabel(status + "  |  " + QString::fromUtf8("\xf0\x9f\x93\x8d") + " " + address);
    statusLbl->setStyleSheet(QString(
        "font-size: 12px; color: %1; background: %2; border-radius: 8px;"
        "padding: 6px 10px; border: none; font-weight: 600;"
    ).arg(statusColor).arg(statusBg));
    statusLbl->setWordWrap(true);
    vl->addWidget(statusLbl);

    // ── Divider ────────────────────────────────────────────────────────────
    QFrame *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setFixedHeight(1);
    divider->setStyleSheet("background: #f1f5f9; border: none;");
    vl->addWidget(divider);

    // ── Stats: qty + price ─────────────────────────────────────────────────
    QHBoxLayout *statsRow = new QHBoxLayout();
    statsRow->setSpacing(10);

    // Qty box
    QWidget *qtyBox = new QWidget();
    qtyBox->setStyleSheet("background: #eff6ff; border-radius: 10px; border: none;");
    QVBoxLayout *qtyL = new QVBoxLayout(qtyBox);
    qtyL->setContentsMargins(12, 8, 12, 8);
    qtyL->setSpacing(3);
    QLabel *qtyLbl = new QLabel(QString::fromUtf8("Quantit\xc3\xa9"));
    qtyLbl->setStyleSheet("font-size: 10px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *qtyVal = new QLabel(qty);
    qtyVal->setStyleSheet("font-size: 18px; font-weight: 700; color: #2563eb; background: transparent; border: none;");
    qtyL->addWidget(qtyLbl);
    qtyL->addWidget(qtyVal);
    statsRow->addWidget(qtyBox, 1);

    // Price box
    QWidget *priceBox = new QWidget();
    priceBox->setStyleSheet("background: #fff7ed; border-radius: 10px; border: none;");
    QVBoxLayout *priceL = new QVBoxLayout(priceBox);
    priceL->setContentsMargins(12, 8, 12, 8);
    priceL->setSpacing(3);
    QLabel *priceLbl = new QLabel("Prix Total");
    priceLbl->setStyleSheet("font-size: 10px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *priceVal = new QLabel(price.isEmpty() ? "---" : price);
    priceVal->setStyleSheet("font-size: 13px; font-weight: 700; color: #ea580c; background: transparent; border: none;");
    priceL->addWidget(priceLbl);
    priceL->addWidget(priceVal);
    statsRow->addWidget(priceBox, 1);

    vl->addLayout(statsRow);

    // ── Dates row ──────────────────────────────────────────────────────────
    QHBoxLayout *datesRow = new QHBoxLayout();
    datesRow->setSpacing(6);

    QLabel *dateLbl = new QLabel(QString::fromUtf8("\xf0\x9f\x97\x93") + " " + date);
    dateLbl->setStyleSheet(
        "font-size: 11px; color: #475569; background: #f1f5f9; border-radius: 6px;"
        "padding: 4px 8px; border: none; font-weight: 600;"
    );

    QLabel *delivLbl = new QLabel(QString::fromUtf8("\xf0\x9f\x9a\x9a") + " " + delivDate);
    delivLbl->setStyleSheet(
        "font-size: 11px; color: #475569; background: #f0fdf4; border-radius: 6px;"
        "padding: 4px 8px; border: none; font-weight: 600;"
    );

    datesRow->addWidget(dateLbl, 1);
    datesRow->addWidget(delivLbl, 1);
    vl->addLayout(datesRow);

    // ── Action buttons ─────────────────────────────────────────────────────
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->setSpacing(8);

    QPushButton *btnView = new QPushButton(QString::fromUtf8("D\xc3\xa9tails"));
    btnView->setCursor(Qt::PointingHandCursor);
    btnView->setStyleSheet(
        "QPushButton { background: #0f2b4c; color: white; border-radius: 10px;"
        "  font-size: 12px; font-weight: 700; padding: 9px 0; border: none; }"
        "QPushButton:hover { background: #1a4270; }"
    );

    QPushButton *btnEdit = new QPushButton("Modifier");
    btnEdit->setCursor(Qt::PointingHandCursor);
    btnEdit->setStyleSheet(
        "QPushButton { background: #fff7ed; color: #ea580c; border-radius: 10px;"
        "  font-size: 12px; font-weight: 700; padding: 9px 0; border: 1px solid #fed7aa; }"
        "QPushButton:hover { background: #ffedd5; }"
    );

    btnRow->addWidget(btnView, 1);
    btnRow->addWidget(btnEdit, 1);
    vl->addLayout(btnRow);

    return card;
}

void MainWindow::on_cmd_pagination_cbSize_currentIndexChanged(int)
{
    QComboBox *cb = findChild<QComboBox*>("cmd_pagination_cbSize");
    if (!cb) return;
    m_cmdItemsPerPage = cb->currentText().toInt();
    m_cmdCurrentPage = 0;
    refreshCmdCardView();
}

void MainWindow::on_cmd_pagination_btnPrev_clicked()
{
    if (m_cmdCurrentPage > 0) {
        m_cmdCurrentPage--;
        refreshCmdCardView();
    }
}

void MainWindow::on_cmd_pagination_btnNext_clicked()
{
    if (!ui->tableProduits_2) return;
    int totalItems = ui->tableProduits_2->rowCount();
    int maxPages = (totalItems + m_cmdItemsPerPage - 1) / m_cmdItemsPerPage;
    if (m_cmdCurrentPage < maxPages - 1) {
        m_cmdCurrentPage++;
        refreshCmdCardView();
    }
}

// ============================================================
// COMMANDE MODULE (Integration 24)
// ============================================================

void MainWindow::setupCommandesModule()
{
    // Connect "Statistiques" button
    // Note: btnGoStats_Cmd is already connected in setupStatistics() to the global stats page.
    // If a specific inner page is needed later, we can add it here.

    // Connect PDF button
    QPushButton *btnPdf = findChild<QPushButton*>("btnPdf_Cmd");
    if (btnPdf) {
        connect(btnPdf, &QPushButton::clicked, this, &MainWindow::on_btnPdf_Cmd_clicked);
    }

    // Initial stats refresh
    refreshCmdStats();
    
    updateClientCombos();
    refreshCommandes();
}

void MainWindow::refreshCmdStats()
{
    QTableWidget *t = ui->tableProduits_2;
    if (!t) return;

    int totalCmds = t->rowCount();
    double totalCA = 0.0;
    int totalBacs = 0.0;

    for(int i=0; i<t->rowCount(); i++) {
        // Col 8: "Prix Total" (e.g. "3200 TND")
        QTableWidgetItem *itPrice = t->item(i, 8);
        if (itPrice) {
            QString txt = itPrice->text().remove(" TND").remove(" ").trimmed();
            totalCA += txt.toDouble();
        }

        // Col 1: "Quantité" (e.g. "8")
        QTableWidgetItem *itQty = t->item(i, 1);
        if (itQty) {
            totalBacs += itQty->text().toInt();
        }
    }

    // Update labels
    QLabel *l1 = findChild<QLabel*>("st_val1_Cmd"); // Total Commandes
    if(l1) l1->setText(QString::number(totalCmds));

    QLabel *l2 = findChild<QLabel*>("st_val2_Cmd"); // CA
    if(l2) l2->setText(QString::number(totalCA, 'f', 2) + " TND");

    QLabel *l3 = findChild<QLabel*>("st_val3_Cmd"); // Bacs Vendus
    if(l3) l3->setText(QString::number(totalBacs));
}

void MainWindow::on_btnPdf_Cmd_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter PDF Commandes", "", "PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;

    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize::A4);
    writer.setPageMargins(QMarginsF(30, 30, 30, 30));

    QPainter painter(&writer);
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 10));

    // Title
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(3000, 1000, "Rapport des Commandes");

    // Date
    painter.setFont(QFont("Arial", 10));
    painter.drawText(300, 1600, "Date: " + QDate::currentDate().toString("dd/MM/yyyy"));

    // Table Content
    int y = 2500;
    int cellHeight = 400;
    int xStart = 300;
    
    // Header
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    QStringList headers = {"ID", "Quantité", "Priorité", "Status", "Date", "Prix Total"};
    int x = xStart;
    int colWidth = 1300;
    
    for(const QString &h : headers) {
        painter.drawRect(x, y, colWidth, cellHeight);
        painter.drawText(x+100, y+250, h);
        x += colWidth;
    }
    y += cellHeight;

    // Rows
    painter.setFont(QFont("Arial", 10));
    QTableWidget *t = ui->tableProduits_2;
    if(t) {
        for(int i=0; i<t->rowCount(); i++) {
            x = xStart;
            // Map our chosen columns to table columns: 0, 1, 2, 3, 5, 8
            int colIndices[] = {0, 1, 2, 3, 5, 8};
            
            for(int j=0; j<6; j++) {
                QString txt = "";
                QTableWidgetItem *it = t->item(i, colIndices[j]);
                if(it) txt = it->text();
                
                painter.drawRect(x, y, colWidth, cellHeight);
                painter.drawText(x+100, y+250, txt);
                x += colWidth;
            }
            y += cellHeight;
        }
    }

    painter.end();
    QMessageBox::information(this, "Export PDF", "Le rapport PDF a été généré avec succès.");
}

void MainWindow::updateClientCombos()
{
    QSqlQuery query("SELECT ID_CLIENT, MATRICULE, NOM FROM CLIENT ORDER BY NOM");
    if (ui->cb_client_add) ui->cb_client_add->clear();
    if (ui->cb_client_mod) ui->cb_client_mod->clear();

    while (query.next()) {
        QString displayName = query.value(1).toString() + " - " + query.value(2).toString();
        int id = query.value(0).toInt();
        
        if (ui->cb_client_add) ui->cb_client_add->addItem(displayName, id); 
        if (ui->cb_client_mod) ui->cb_client_mod->addItem(displayName, id);
    }
}

void MainWindow::refreshCommandes()
{
    if (!ui->tableProduits_2) return;
    ui->tableProduits_2->setRowCount(0);
    
    QSqlQueryModel *model = Ctmp.afficher();
    if (!model) return;
    
    // Change the first header implicitly to 'Reference' dynamically if needed
    if (ui->tableProduits_2->horizontalHeaderItem(0)) {
        ui->tableProduits_2->horizontalHeaderItem(0)->setText("Reference");
    }

    for(int i=0; i<model->rowCount(); ++i) {
        int row = ui->tableProduits_2->rowCount();
        ui->tableProduits_2->insertRow(row);
        
        QTableWidgetItem *idItem = new QTableWidgetItem(model->data(model->index(i, 0)).toString());
        // Store ID_CLIENT (index 8) in UserRole for easy retrieval
        int idClient = model->data(model->index(i, 8)).toInt();
        idItem->setData(Qt::UserRole, idClient);
        
        // Fetch Client Address from new model column (index 9)
        QString adresse = model->data(model->index(i, 9)).toString();

        // Stock in dashboard is just QTE directly? Or just model->data(model->index(i, 2)) representing QTE
        QString stockTxt = model->data(model->index(i, 2)).toString();

        ui->tableProduits_2->setItem(row, 0, idItem); // ID / Reference
        ui->tableProduits_2->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 2)).toString())); // Qte
        ui->tableProduits_2->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 3)).toString())); // Prio
        ui->tableProduits_2->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 4)).toString())); // Statut
        
        // Populate actual values for Adresse and Stock
        ui->tableProduits_2->setItem(row, 4, new QTableWidgetItem(adresse.isEmpty() ? "---" : adresse)); 
        ui->tableProduits_2->setItem(row, 5, new QTableWidgetItem(model->data(model->index(i, 5)).toDate().toString("yyyy-MM-dd"))); // Date Cmd
        ui->tableProduits_2->setItem(row, 6, new QTableWidgetItem(stockTxt.isEmpty() ? "0" : stockTxt)); 
        ui->tableProduits_2->setItem(row, 7, new QTableWidgetItem(model->data(model->index(i, 6)).toDate().toString("yyyy-MM-dd"))); // Date Liv
        ui->tableProduits_2->setItem(row, 8, new QTableWidgetItem(model->data(model->index(i, 7)).toString() + " TND")); // Prix
        
        installCmdActions2(row);
    }
    delete model;
    refreshCmdStats();
}

void MainWindow::on_btnSave_Mod_3_clicked()
{
    if (!ui->cb_client_add) return;
    int idClient = ui->cb_client_add->currentData().toInt();
    int qte = ui->sb_qty_add_4->value();
    QString prio = ui->cb_model_add_4->currentText();
    QString statut = ui->cb_status_add_4->currentText();
    double prix = ui->dsb_price_add_4->value();
    
    // Read dates from the combo boxes instead of defaulting to currentDate
    QDate dCmd(ui->comboBox_21->currentText().toInt(), ui->comboBox_20->currentText().toInt(), ui->comboBox_19->currentText().toInt());
    QDate dLiv(ui->comboBox_24->currentText().toInt(), ui->comboBox_23->currentText().toInt(), ui->comboBox_22->currentText().toInt());

    QString adresseUI = ui->textEdit ? ui->textEdit->toPlainText() : "";

    Ctmp.setId(0);
    Ctmp.setIdClient(idClient);
    Ctmp.setQte(qte);
    Ctmp.setPriorite(prio);
    Ctmp.setStatut(statut);
    Ctmp.setDateCommande(dCmd);
    Ctmp.setDateLivraison(dLiv);
    Ctmp.setPrixTotal(prix);
    Ctmp.setAdresse(adresseUI);

    if(Ctmp.ajouter()) {
        QMessageBox::information(this, "Succès", "Commande ajoutée.");
        refreshCommandes();
        if (auto *sw = mainStacked()) {
            if (auto *page = sw->findChild<QWidget*>("pageCmdDashboard", Qt::FindDirectChildrenOnly)) {
                sw->setCurrentWidget(page);
            }
        }
    } else {
        QMessageBox::critical(this, "Erreur", "Erreur d'ajout: " + Ctmp.lastError());
    }
}

void MainWindow::loadCmdToModificationForm(int row)
{
    if (!ui->tableProduits_2) return;
    currentCmdRow = row;
    ui->tableProduits_2->selectRow(row);

    // Retrieve data from table
    int idClient = ui->tableProduits_2->item(row, 0)->data(Qt::UserRole).toInt();
    int qte = ui->tableProduits_2->item(row, 1)->text().toInt();
    QString prio = ui->tableProduits_2->item(row, 2)->text();
    QString statut = ui->tableProduits_2->item(row, 3)->text();
    QString adresse = ui->tableProduits_2->item(row, 4)->text();
    // Price string "3200 TND" -> 3200
    double prix = ui->tableProduits_2->item(row, 8)->text().split(" ").first().toDouble();

    // Populate UI
    if (ui->cb_client_mod) {
        int idx = ui->cb_client_mod->findData(idClient);
        if (idx >= 0) ui->cb_client_mod->setCurrentIndex(idx);
    }
    if (ui->sb_qty_add_2) ui->sb_qty_add_2->setValue(qte);
    if (ui->cb_model_add_2) ui->cb_model_add_2->setCurrentText(prio);
    if (ui->cb_status_add_2) ui->cb_status_add_2->setCurrentText(statut);
    if (ui->dsb_price_add_2) ui->dsb_price_add_2->setValue(prix);
    if (ui->textEdit_2) ui->textEdit_2->setPlainText(adresse == "---" ? "" : adresse);

    QDate dCmd = QDate::fromString(ui->tableProduits_2->item(row, 5)->text(), "yyyy-MM-dd");
    if (dCmd.isValid()) {
        ui->comboBox_7->setCurrentText(QString("%1").arg(dCmd.day(), 2, 10, QChar('0')));
        ui->comboBox_8->setCurrentText(QString("%1").arg(dCmd.month(), 2, 10, QChar('0')));
        ui->comboBox_9->setCurrentText(QString::number(dCmd.year()));
    }
    
    QDate dLiv = QDate::fromString(ui->tableProduits_2->item(row, 7)->text(), "yyyy-MM-dd");
    if (dLiv.isValid()) {
        ui->comboBox_13->setCurrentText(QString("%1").arg(dLiv.day(), 2, 10, QChar('0')));
        ui->comboBox_14->setCurrentText(QString("%1").arg(dLiv.month(), 2, 10, QChar('0')));
        ui->comboBox_15->setCurrentText(QString::number(dLiv.year()));
    }
}

void MainWindow::on_btnSave_CmdMod_clicked()
{
    if (!ui->cb_client_mod || currentCmdRow < 0) return;
    
    // Get ID from the currently selected row
    int idCmd = ui->tableProduits_2->item(currentCmdRow, 0)->text().toInt();
    int idClient = ui->cb_client_mod->currentData().toInt();
    int qte = ui->sb_qty_add_2->value();
    QString prio = ui->cb_model_add_2->currentText();
    QString statut = ui->cb_status_add_2->currentText();
    double prix = ui->dsb_price_add_2->value();

    // Reconstruct selected date
    QDate dCmd(ui->comboBox_9->currentText().toInt(), ui->comboBox_8->currentText().toInt(), ui->comboBox_7->currentText().toInt());
    QDate dLiv(ui->comboBox_15->currentText().toInt(), ui->comboBox_14->currentText().toInt(), ui->comboBox_13->currentText().toInt());
    QString adresseUI = ui->textEdit_2 ? ui->textEdit_2->toPlainText() : "";
    
    Ctmp.setId(idCmd);
    Ctmp.setIdClient(idClient);
    Ctmp.setQte(qte);
    Ctmp.setPriorite(prio);
    Ctmp.setStatut(statut);
    Ctmp.setDateCommande(dCmd);
    Ctmp.setDateLivraison(dLiv);
    Ctmp.setPrixTotal(prix);
    Ctmp.setAdresse(adresseUI);
    
    if(Ctmp.modifier()) {
        QMessageBox::information(this, "Succès", "Commande modifiée.");
        refreshCommandes();
        if (auto *sw = mainStacked()) {
            if (auto *page = sw->findChild<QWidget*>("pageCmdDashboard", Qt::FindDirectChildrenOnly)) {
                sw->setCurrentWidget(page);
            }
        }
    } else {
        QMessageBox::critical(this, "Erreur", "Erreur de modification: " + Ctmp.lastError());
    }
}

void MainWindow::on_btnCancel_Mod_3_clicked()
{
    if (auto *sw = mainStacked()) {
        if (auto *page = sw->findChild<QWidget*>("pageCmdDashboard", Qt::FindDirectChildrenOnly)) {
            sw->setCurrentWidget(page);
        }
    }
}

void MainWindow::on_btnCancel_CmdMod_clicked()
{
    if (auto *sw = mainStacked()) {
        if (auto *page = sw->findChild<QWidget*>("pageCmdDashboard", Qt::FindDirectChildrenOnly)) {
            sw->setCurrentWidget(page);
        }
    }
}

void MainWindow::installCmdActions2(int row)
{
    if (!ui->tableProduits_2) return;
    if (row < 0 || row >= ui->tableProduits_2->rowCount()) return;

    QWidget *cell = new QWidget(ui->tableProduits_2);
    auto *layout = new QHBoxLayout(cell);
    layout->setContentsMargins(4, 0, 4, 0);
    layout->setSpacing(6);
    layout->setAlignment(Qt::AlignCenter);

    auto *btnEdit = new QPushButton("Modifier", cell);
    auto *btnDelete = new QPushButton("Supprimer", cell);

    btnEdit->setStyleSheet("QPushButton { background-color: #3498db; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; }");
    btnDelete->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; }");
    btnEdit->setCursor(Qt::PointingHandCursor);
    btnDelete->setCursor(Qt::PointingHandCursor);
    btnEdit->setProperty("row", row);
    btnDelete->setProperty("row", row);

    layout->addWidget(btnEdit);
    layout->addWidget(btnDelete);

    ui->tableProduits_2->setCellWidget(row, 9, cell);

    connect(btnEdit, &QPushButton::clicked, this, [this, btnEdit]() {
        if (auto *sw = mainStacked()) {
            if (auto *page = sw->findChild<QWidget*>("pageCmdModifier", Qt::FindDirectChildrenOnly)) {
                sw->setCurrentWidget(page);
            }
        }
        if (ui->tableProduits_2) {
            const int currentRow = btnEdit->property("row").toInt();
            if (currentRow >= 0 && currentRow < ui->tableProduits_2->rowCount()) {
                loadCmdToModificationForm(currentRow);
            }
        }
    });

    connect(btnDelete, &QPushButton::clicked, this, [this, btnDelete]() {
        if (!ui->tableProduits_2) return;
        const int currentRow = btnDelete->property("row").toInt();
        if (currentRow < 0 || currentRow >= ui->tableProduits_2->rowCount()) return;

        if (QMessageBox::question(this, "Supprimer", "Voulez-vous supprimer cette commande ?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
            return;

        // Try to delete from DB if it's a real ID
        QString idStr = ui->tableProduits_2->item(currentRow, 0)->text();
        bool ok;
        int id = idStr.toInt(&ok);
        if(ok && id > 0) {
             Ctmp.supprimer(id);
        }

        ui->tableProduits_2->removeRow(currentRow);
        
        // Reindex
        for (int r = 0; r < ui->tableProduits_2->rowCount(); ++r) {
            if (QWidget *c = ui->tableProduits_2->cellWidget(r, 9)) {
                const auto buttons = c->findChildren<QPushButton*>();
                for (QPushButton *b : buttons) {
                    b->setProperty("row", r);
                }
            }
        }
        refreshCmdStats();
    });
}
