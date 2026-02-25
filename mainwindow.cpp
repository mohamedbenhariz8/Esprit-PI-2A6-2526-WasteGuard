#include "mainwindow.h"

#include "ui_mainwindow.h"
#include <QPdfWriter>
#include <QPainter>
#include <QPageSize>
#include <QMarginsF>
#include <QFileDialog>
#include <QEvent>




#include <QPushButton>

#include <QHBoxLayout>

#include <QWidget>

#include <QTime>

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
static constexpr int PROD_ROLE_CUR = Qt::UserRole;
static constexpr int PROD_ROLE_MAX = Qt::UserRole + 1;
static constexpr int PROD_ROLE_AISLE = Qt::UserRole + 2;
static constexpr int PROD_ROLE_ID = Qt::UserRole + 6;
static constexpr int PROD_ROLE_BATTERY = Qt::UserRole + 7;
static constexpr int PROD_ROLE_IMAGE   = Qt::UserRole + 8;



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


    , currentProduitRow(-1)

    , currentClientRow(-1)

    , currentMaintRow(-1)

    , globalStatsReturnPage(nullptr)

    , m_sidebarExpanded(true)

{

    ui->setupUi(this);



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

    connectToGlobalStats(findChild<QPushButton*>("btnGoStats_Client"));

    connectToGlobalStats(findChild<QPushButton*>("btnGoStats_Cmd"));




    connect(ui->btnAnnulerStats, &QPushButton::clicked, this, [this]() {

        if (auto *sw = mainStacked()) {

            QWidget *statsPage = sw->findChild<QWidget*>("statistiques", Qt::FindDirectChildrenOnly);

            if (globalStatsReturnPage && globalStatsReturnPage != statsPage) {

                sw->setCurrentWidget(globalStatsReturnPage);

                return;

            }

        }


    });









    // --- CLIENT INITIALIZATION ---

    if (ui->tableWidget_Client) {

        ui->tableWidget_Client->setColumnCount(7);

        QStringList headers = { "Matricule", "Nom", "Email", "Bacs", "Score", "Paiement", "Actions" };

        ui->tableWidget_Client->setHorizontalHeaderLabels(headers);

        ui->tableWidget_Client->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        

        // Client data
        struct ClientData { QString mat, nom, email, bacs, score, paiement; };
        QList<ClientData> clients = {
            {"CL-001", "Ahmed Ben Salah",   "ahmed.bensalah@gmail.com",  "5",  "92", "Mensuel"},
            {"CL-002", "Sarah Khalifa",     "sarah.khalifa@outlook.com", "3",  "78", "Trimestriel"},
            {"CL-003", "Mohamed Trabelsi",  "m.trabelsi@yahoo.fr",       "8",  "65", "Annuel"},
            {"CL-004", "Leila Mansouri",    "leila.mansouri@gmail.com",  "2",  "88", "Mensuel"},
            {"CL-005", "Karim Bouazizi",    "k.bouazizi@hotmail.com",    "12", "55", "Trimestriel"},
            {"CL-006", "Fatma Chaabane",    "fatma.chaabane@gmail.com",  "6",  "95", "Mensuel"},
            {"CL-007", "Youssef Hamdi",     "y.hamdi@entreprise.tn",     "4",  "72", "Annuel"},
            {"CL-008", "Nadia Belhaj",      "nadia.belhaj@gmail.com",    "9",  "81", "Mensuel"},
            {"CL-009", "Sami Jebali",       "sami.jebali@outlook.com",   "1",  "60", "Trimestriel"}
        };
        for (const auto &c : clients) {
            int row = ui->tableWidget_Client->rowCount();
            ui->tableWidget_Client->insertRow(row);
            ui->tableWidget_Client->setItem(row, 0, new QTableWidgetItem(c.mat));
            ui->tableWidget_Client->setItem(row, 1, new QTableWidgetItem(c.nom));
            ui->tableWidget_Client->setItem(row, 2, new QTableWidgetItem(c.email));
            ui->tableWidget_Client->setItem(row, 3, new QTableWidgetItem(c.bacs));
            ui->tableWidget_Client->setItem(row, 4, new QTableWidgetItem(c.score));
            ui->tableWidget_Client->setItem(row, 5, new QTableWidgetItem(c.paiement));
            addClientActionButtons(row);
        }
    }

    if (ui->btn_save_ajouter) connect(ui->btn_save_ajouter, &QPushButton::clicked, this, &MainWindow::on_btn_ajouter_client_clicked);

    if (ui->btn_annuler_ajouter) connect(ui->btn_annuler_ajouter, &QPushButton::clicked, this, &MainWindow::on_btn_annuler_client_clicked);

    if (ui->btn_save_modifier) connect(ui->btn_save_modifier, &QPushButton::clicked, this, &MainWindow::on_btn_modifier_client_clicked);

    if (ui->btn_annuler_modifier) connect(ui->btn_annuler_modifier, &QPushButton::clicked, this, &MainWindow::on_btn_annuler_client_clicked);

    if (ui->btnNouveau_Client) connect(ui->btnNouveau_Client, &QPushButton::clicked, this, &MainWindow::on_btnNouveau_client_clicked);





    setupProduitModule();


    setupStockModule();

    setupClientCardViewContainer();

    setupMaintenanceModule();

    setupMaintCardViewContainer();

    setupCmdCardViewContainer();

    setupCommandesModule();


    forceApplySidebarStyles();


}



MainWindow::~MainWindow()

{

    delete ui;

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

    // NOTE: Do NOT manually connect prod_btnSave_Add / prod_btnSave_Mod here.
    // Their slots (on_prod_btnSave_Add_clicked / on_prod_btnSave_Mod_clicked)
    // follow Qt's auto-connect naming convention and are already connected
    // by connectSlotsByName() in setupUi(). Adding a manual connection causes
    // the slot to fire twice, leading to spurious validation warnings.

    // ── Photo upload buttons ──
    if (auto *b = root->findChild<QPushButton*>("prod_btnUpload_Add"))
        connect(b, &QPushButton::clicked, this, &MainWindow::onUploadImageAdd);

    // ── Stock Map 3D: make prod_l5 clickable ──
    if (auto *lblMap = root->findChild<QLabel*>("prod_l5")) {
        lblMap->setText("🗺️  Plan du Dépôt — Cliquer ici pour ouvrir");
        lblMap->setCursor(Qt::PointingHandCursor);
        lblMap->setStyleSheet(
            "color: #3B82F6;"
            "font-size: 13px;"
            "font-weight: 700;"
            "padding: 10px 16px;"
            "background: rgba(59,130,246,0.08);"
            "border: 1px solid rgba(59,130,246,0.25);"
            "border-radius: 10px;"
        );
        // Use event filter for click
        lblMap->installEventFilter(this);
        lblMap->setProperty("stockMapTrigger", true);
    }

    // ── Real-time slider → label updates ──
    // Battery slider (Ajout)
    if (ui->prod_slider_bat_add && ui->prod_l_bat) {
        auto updateBatAdd = [this](int val) {
            ui->prod_l_bat->setText(QString("🔋 Batterie : %1 mAh").arg(QString::number(val)));
        };
        disconnect(ui->prod_slider_bat_add, nullptr, ui->prod_l_bat, nullptr);
        connect(ui->prod_slider_bat_add, &QSlider::valueChanged, this, updateBatAdd);
        updateBatAdd(ui->prod_slider_bat_add->value());
    }
    // Capacity slider (Ajout)
    if (ui->prod_slider_cap_add && ui->prod_l_cap) {
        auto updateCapAdd = [this](int val) {
            ui->prod_l_cap->setText(QString("📦 Capacité : %1 L").arg(val));
        };
        disconnect(ui->prod_slider_cap_add, nullptr, ui->prod_l_cap, nullptr);
        connect(ui->prod_slider_cap_add, &QSlider::valueChanged, this, updateCapAdd);
        updateCapAdd(ui->prod_slider_cap_add->value());
    }
    // Battery slider (Modifier)
    if (ui->prod_slider_bat_mod && ui->prod_l_bat_m) {
        auto updateBatMod = [this](int val) {
            ui->prod_l_bat_m->setText(QString("🔋 Batterie : %1 mAh").arg(QString::number(val)));
        };
        disconnect(ui->prod_slider_bat_mod, nullptr, ui->prod_l_bat_m, nullptr);
        connect(ui->prod_slider_bat_mod, &QSlider::valueChanged, this, updateBatMod);
        updateBatMod(ui->prod_slider_bat_mod->value());
    }
    // Capacity slider (Modifier)
    if (ui->prod_slider_cap_mod && ui->prod_l_cap_m) {
        auto updateCapMod = [this](int val) {
            ui->prod_l_cap_m->setText(QString("📦 Capacité : %1 L").arg(val));
        };
        disconnect(ui->prod_slider_cap_mod, nullptr, ui->prod_l_cap_m, nullptr);
        connect(ui->prod_slider_cap_mod, &QSlider::valueChanged, this, updateCapMod);
        updateCapMod(ui->prod_slider_cap_mod->value());
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
        const QString imagePath = model->data(model->index(i, 7)).toString().trimmed();

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
            refItem->setData(PROD_ROLE_IMAGE, imagePath);
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
    const QString imagePath = t->item(row, 0) ? t->item(row, 0)->data(PROD_ROLE_IMAGE).toString() : QString();

    if (ui->prod_ln_ref_mod) ui->prod_ln_ref_mod->setText(reference);
    if (ui->prod_dsb_price_mod) ui->prod_dsb_price_mod->setValue(prix);
    if (ui->prod_sb_qty_mod) ui->prod_sb_qty_mod->setValue(quantite);
    if (ui->prod_slider_cap_mod) ui->prod_slider_cap_mod->setValue(qMax(ui->prod_slider_cap_mod->minimum(), qMin(ui->prod_slider_cap_mod->maximum(), capacite)));
    if (ui->prod_slider_bat_mod) ui->prod_slider_bat_mod->setValue(qMax(ui->prod_slider_bat_mod->minimum(), qMin(ui->prod_slider_bat_mod->maximum(), battery)));

    if (ui->prod_cb_model_mod) ui->prod_cb_model_mod->setText(modele);
    setComboTextSafe(ui->prod_cb_status_mod, status);
    if (ui->prod_lstCharacteristics_mod) ui->prod_lstCharacteristics_mod->clearSelection();

    // Load image for modification
    m_modImagePath = imagePath;
    if (auto *imgLabel = findChild<QLabel*>("prod_imgBin")) {
        if (!imagePath.isEmpty() && QFile::exists(imagePath)) {
            QPixmap pix(imagePath);
            imgLabel->setPixmap(pix.scaled(imgLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            imgLabel->setText("[ Image Actuelle ]");
        }
    }
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
    const QString modele = ui->prod_cb_model_add ? ui->prod_cb_model_add->text().trimmed() : QString();
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
    Ptmp.setImage(m_addImagePath);

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
                    statusItem->setText(productStatusFromQty(quantite));
                }
                t->selectRow(r);
                currentProduitRow = r;
                break;
            }
        }
    }

    if (m_isCardView) refreshCardView();

    if (ui->prod_ln_ref_add) ui->prod_ln_ref_add->clear();
    if (ui->prod_cb_model_add) ui->prod_cb_model_add->clear();
    m_addImagePath.clear();
    if (auto *imgLabel = findChild<QLabel*>("prod_lblImgPreview_Add")) {
        imgLabel->setPixmap(QPixmap());
        imgLabel->setText("Glisser Image Ici");
    }
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
    const QString modele = ui->prod_cb_model_mod ? ui->prod_cb_model_mod->text().trimmed() : QString();
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
    Ptmp.setImage(m_modImagePath);

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
                    statusItem->setText(productStatusFromQty(quantite));
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

// ── Photo Upload Handlers ──
void MainWindow::onUploadImageAdd()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Choisir une image", QString(),
        "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
    if (filePath.isEmpty()) return;

    m_addImagePath = filePath;
    if (auto *imgLabel = findChild<QLabel*>("prod_lblImgPreview_Add")) {
        QPixmap pix(filePath);
        imgLabel->setPixmap(pix.scaled(imgLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void MainWindow::onUploadImageMod()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Choisir une image", QString(),
        "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
    if (filePath.isEmpty()) return;

    m_modImagePath = filePath;
    if (auto *imgLabel = findChild<QLabel*>("prod_imgBin")) {
        QPixmap pix(filePath);
        imgLabel->setPixmap(pix.scaled(imgLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}



// ---------- Maintenance Module Implementation ----------



QTableWidget* MainWindow::maintenanceTable() const {

    return ui->tableMaintenance;

}



void MainWindow::setupMaintenanceModule() {

    if (auto* table = maintenanceTable()) {

        table->setColumnCount(7);

        QStringList headers = { "Reference", "Date", "Technicien", "Coût", "Durée", "Priorité", "Actions" };

        table->setHorizontalHeaderLabels(headers);

        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        table->setRowCount(0);

        

        // Maintenance interventions data
        struct MaintData { QString ref, date, tech, cout, duree, priorite; };
        QList<MaintData> items = {
            {"INT-001", "2024-01-15", "Ahmed Ali",       "150 TND", "2h",   "Normale"},
            {"INT-002", "2024-01-22", "Sami Jebali",     "320 TND", "5h",   "Urgente"},
            {"INT-003", "2024-02-03", "Karim Bouazizi",  "85 TND",  "1h30", "Normale"},
            {"INT-004", "2024-02-10", "Mohamed Trabelsi","500 TND", "8h",   "Critique"},
            {"INT-005", "2024-02-18", "Ahmed Ali",       "200 TND", "3h",   "Urgente"},
            {"INT-006", "2024-03-01", "Youssef Hamdi",   "120 TND", "2h",   "Normale"},
            {"INT-007", "2024-03-12", "Sami Jebali",     "450 TND", "6h",   "Critique"},
            {"INT-008", "2024-03-20", "Karim Bouazizi",  "75 TND",  "1h",   "Normale"},
            {"INT-009", "2024-04-05", "Mohamed Trabelsi","280 TND", "4h",   "Urgente"}
        };
        for (const auto &m : items) {
            int row = table->rowCount();
            table->insertRow(row);
            table->setItem(row, 0, new QTableWidgetItem(m.ref));
            table->setItem(row, 1, new QTableWidgetItem(m.date));
            table->setItem(row, 2, new QTableWidgetItem(m.tech));
            table->setItem(row, 3, new QTableWidgetItem(m.cout));
            table->setItem(row, 4, new QTableWidgetItem(m.duree));
            table->setItem(row, 5, new QTableWidgetItem(m.priorite));
        }
        refreshMaintActionButtons();

    }

    

    if (ui->btnSave_Add) connect(ui->btnSave_Add, &QPushButton::clicked, this, &MainWindow::on_btnSave_Add_clicked);

    if (ui->btnSave_Mod) connect(ui->btnSave_Mod, &QPushButton::clicked, this, &MainWindow::on_btnSave_Mod_clicked);

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



    table->setCellWidget(row, 6, pWidget);

}



void MainWindow::on_btnSave_Add_clicked() {

    auto* table = maintenanceTable();

    if (!table) return;



    int row = table->rowCount();

    table->insertRow(row);

    table->setItem(row, 0, new QTableWidgetItem(ui->editRefAdd->text()));

    table->setItem(row, 1, new QTableWidgetItem(ui->dateAdd->date().toString("yyyy-MM-dd")));

    table->setItem(row, 2, new QTableWidgetItem(ui->editTechAdd->text()));

    table->setItem(row, 3, new QTableWidgetItem(QString::number(ui->spinCoutAdd->value()) + " DT"));

    table->setItem(row, 4, new QTableWidgetItem(ui->comboDurAdd->currentText()));

    table->setItem(row, 5, new QTableWidgetItem(ui->comboPrioAdd->currentText()));



    installMaintActionButtonsForRow(row);

    ui->stackedWidget_Maintenance->setCurrentWidget(ui->page_Maint_Dash);

    

    // Clear fields

    ui->editRefAdd->clear();

    ui->editTechAdd->clear();

    ui->spinCoutAdd->setValue(0);

}



void MainWindow::on_btnSave_Mod_clicked() {

    auto* table = maintenanceTable();

    if (!table || currentMaintRow < 0) return;



    table->item(currentMaintRow, 0)->setText(ui->editRefMod->text());

    table->item(currentMaintRow, 1)->setText(ui->dateMod->date().toString("yyyy-MM-dd"));

    table->item(currentMaintRow, 2)->setText(ui->editTechMod->text());

    table->item(currentMaintRow, 3)->setText(QString::number(ui->spinCoutMod->value()) + " DT");

    table->item(currentMaintRow, 4)->setText(ui->comboDurMod->currentText());

    table->item(currentMaintRow, 5)->setText(ui->comboPrioMod->currentText());



    ui->stackedWidget_Maintenance->setCurrentWidget(ui->page_Maint_Dash);

}



void MainWindow::handleMaintEditClicked() {

    QPushButton* btn = qobject_cast<QPushButton*>(sender());

    if (!btn) return;

    int row = btn->property("row").toInt();

    currentMaintRow = row;



    auto* table = maintenanceTable();

    ui->editRefMod->setText(table->item(row, 0)->text());

    ui->dateMod->setDate(QDate::fromString(table->item(row, 1)->text(), "yyyy-MM-dd"));

    ui->editTechMod->setText(table->item(row, 2)->text());

    

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

        maintenanceTable()->removeRow(row);

        refreshMaintActionButtons();

    }

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



    struct Product { QString ref, nom, stock, seuil, prix, fournisseur; };

    QList<Product> items = {
        {"REF-001", "Capteur Ultrason HC-SR04",   "150", "OK",       "25 TND",  "TechSupply"},
        {"REF-002", "Batterie Lithium 18650",      "15",  "CRITIQUE", "45 TND",  "PowerPack"},
        {"REF-003", "Module GPS NEO-6M",           "80",  "MOYEN",    "30 TND",  "GeoTrack"},
        {"REF-004", "Moteur DC 12V",               "60",  "OK",       "55 TND",  "MotorTech"},
        {"REF-005", "Capteur Infrarouge IR",       "200", "OK",       "12 TND",  "SensorHub"},
        {"REF-006", "Raspberry Pi 4 Model B",      "8",   "CRITIQUE", "320 TND", "DigiComp"},
        {"REF-007", "Servo Moteur SG90",           "45",  "MOYEN",    "18 TND",  "RoboTech"},
        {"REF-008", "Module WiFi ESP8266",         "120", "OK",       "22 TND",  "IoTStore"},
        {"REF-009", "Capteur Temp DS18B20",        "35",  "MOYEN",    "15 TND",  "TechSupply"}
    };



    double totalValue = 0;

    int criticalCount = 0;



    ui->tableWidget->setRowCount(items.size());



    for(int i = 0; i < items.size(); ++i) {

        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(items[i].ref));

        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(items[i].nom));

        QTableWidgetItem *stockItem = new QTableWidgetItem();

        stockItem->setData(Qt::DisplayRole, items[i].stock.toInt());

        ui->tableWidget->setItem(i, 2, stockItem);



        QLabel* badge = new QLabel(items[i].seuil);

        badge->setAlignment(Qt::AlignCenter);

        badge->setFixedSize(90, 25);

        QString style = "border-radius: 5px; font-weight: bold;";



        if(items[i].seuil == "CRITIQUE") {

            style += "background-color: #dc3545; color: white;";

            criticalCount++;

        }

        else if(items[i].seuil == "MOYEN") style += "background-color: #ffc107; color: #333;";

        else style += "background-color: #28a745; color: white;";



        badge->setStyleSheet(style);



        QWidget* badgeWidget = new QWidget();

        QHBoxLayout* badgeLayout = new QHBoxLayout(badgeWidget);

        badgeLayout->addWidget(badge);

        badgeLayout->setAlignment(Qt::AlignCenter);

        badgeLayout->setContentsMargins(0,0,0,0);

        ui->tableWidget->setCellWidget(i, 3, badgeWidget);



        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(items[i].prix));

        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(items[i].fournisseur));



        double price = items[i].prix.split(" ")[0].toDouble();

        totalValue += items[i].stock.toInt() * price;



        QWidget* container = new QWidget();

        QHBoxLayout* layout = new QHBoxLayout(container);

        QPushButton* editBtn = new QPushButton("Modifier");

        QPushButton* delBtn = new QPushButton("Supprimer");



        editBtn->setStyleSheet("color: #3182CE; border: none; font-weight: bold;");

        delBtn->setStyleSheet("color: #E53E3E; border: none; font-weight: bold;");



        connect(editBtn, &QPushButton::clicked, this, [this, container]() {

            if (!ui->tableWidget) return;



            int row = -1;

            for (int r = 0; r < ui->tableWidget->rowCount(); ++r) {

                if (ui->tableWidget->cellWidget(r, 6) == container) {

                    row = r;

                    break;

                }

            }

            if (row < 0) return;



            if (ui->inputRef_mod) ui->inputRef_mod->setText(ui->tableWidget->item(row, 0)->text());

            if (ui->inputNom_mod) ui->inputNom_mod->setText(ui->tableWidget->item(row, 1)->text());

            if (ui->inputStock_mod) ui->inputStock_mod->setText(ui->tableWidget->item(row, 2)->text());

            if (ui->inputPrix_mod) ui->inputPrix_mod->setText(ui->tableWidget->item(row, 4)->text());

            if (ui->inputFournisseur_mod) ui->inputFournisseur_mod->setText(ui->tableWidget->item(row, 5)->text());



            if (ui->sliderStock_mod) ui->sliderStock_mod->setValue(ui->tableWidget->item(row, 2)->text().toInt());

            if (ui->sliderPrix_mod) ui->sliderPrix_mod->setValue(ui->tableWidget->item(row, 4)->text().split(" ")[0].toInt());



            if (ui->stock_stackedWidget) ui->stock_stackedWidget->setCurrentIndex(2);

        });

        connect(delBtn, &QPushButton::clicked, this, [this, container]() {

            if (!ui->tableWidget) return;



            int row = -1;

            for (int r = 0; r < ui->tableWidget->rowCount(); ++r) {

                if (ui->tableWidget->cellWidget(r, 6) == container) {

                    row = r;

                    break;

                }

            }

            if (row < 0) return;



            if (QMessageBox::question(this, "Supprimer", "Etes-vous sur de supprimer cet element ?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)

                return;



            ui->tableWidget->removeRow(row);

            applyStockFilterAndSort();

        });



        layout->addWidget(editBtn);

        layout->addWidget(delBtn);

        layout->setContentsMargins(0,0,0,0);

        ui->tableWidget->setCellWidget(i, 6, container);

    }



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



        // 3. Action Buttons (Facture, Score, Export) -> Blue Gradient Style (White Text)

        QString clientBtnStyle = 

            "QPushButton {"

            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0f2b4c, stop:1 #2a5298);"

            "   color: white; border-radius: 12px; padding: 12px; font-weight: 700; font-size: 13px;"

            "   border: 1px solid #1e40af;"

            "}"

            "QPushButton:hover {"

            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #1e40af, stop:1 #3b82f6);"

            "   border-color: #60a5fa;"

            "}";

            

        if (auto *btn = this->findChild<QPushButton*>("facture")) btn->setStyleSheet(clientBtnStyle);

        if (auto *btn = this->findChild<QPushButton*>("score")) btn->setStyleSheet(clientBtnStyle);

        if (auto *btn = this->findChild<QPushButton*>("exportclient")) btn->setStyleSheet(clientBtnStyle);



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

        ui->input_bacs_ajouter->setValue(0);

        ui->input_score_ajouter->setValue(0);

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

    onClientAdded(ui->input_matricule_ajouter->text(), 

                  ui->input_nom_ajouter->text(), 

                  ui->input_email_ajouter->text(), 

                  QString::number(ui->input_bacs_ajouter->value()),

                  QString::number(ui->input_score_ajouter->value()), 

                  ui->input_paiement_ajouter->currentText());

    QMessageBox::information(this, "Succès", "Client ajouté avec succès.");

    if (ui->stackedWidget_Client) ui->stackedWidget_Client->setCurrentIndex(0);

}



void MainWindow::on_btn_modifier_client_clicked() {

    if(ui->input_matricule_modifier->text().isEmpty() || ui->input_nom_modifier->text().isEmpty()) {

         QMessageBox::warning(this, "Champs Manquants", "Veuillez remplir au moins le Matricule et le Nom.");

         return;

    }

    onClientModified(currentClientRow,

                     ui->input_matricule_modifier->text(), 

                     ui->input_nom_modifier->text(), 

                     ui->input_email_modifier->text(), 

                     QString::number(ui->input_bacs_modifier->value()),

                     QString::number(ui->input_score_modifier->value()), 

                     ui->input_paiement_modifier->currentText());

    QMessageBox::information(this, "Succès", "Informations du client modifiées avec succès.");

    if (ui->stackedWidget_Client) ui->stackedWidget_Client->setCurrentIndex(0);

}



void MainWindow::onClientAdded(QString matricule, QString nom, QString email, QString bacs, QString score, QString paiement) {

    int row = ui->tableWidget_Client->rowCount();

    ui->tableWidget_Client->insertRow(row);

    ui->tableWidget_Client->setItem(row, 0, new QTableWidgetItem(matricule));

    ui->tableWidget_Client->setItem(row, 1, new QTableWidgetItem(nom));

    ui->tableWidget_Client->setItem(row, 2, new QTableWidgetItem(email));

    ui->tableWidget_Client->setItem(row, 3, new QTableWidgetItem(bacs));

    ui->tableWidget_Client->setItem(row, 4, new QTableWidgetItem(score));

    ui->tableWidget_Client->setItem(row, 5, new QTableWidgetItem(paiement));

    addClientActionButtons(row);

}



void MainWindow::onClientModified(int row, QString matricule, QString nom, QString email, QString bacs, QString score, QString paiement) {

    if(row < ui->tableWidget_Client->rowCount()) {

        ui->tableWidget_Client->item(row, 0)->setText(matricule);

        ui->tableWidget_Client->item(row, 1)->setText(nom);

        ui->tableWidget_Client->item(row, 2)->setText(email);

        ui->tableWidget_Client->item(row, 3)->setText(bacs);

        ui->tableWidget_Client->item(row, 4)->setText(score);

        ui->tableWidget_Client->item(row, 5)->setText(paiement);

    }

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

        ui->input_bacs_modifier->setValue(ui->tableWidget_Client->item(r, 3)->text().toInt());

        ui->input_score_modifier->setValue(ui->tableWidget_Client->item(r, 4)->text().toInt());

        ui->input_paiement_modifier->setCurrentText(ui->tableWidget_Client->item(r, 5)->text());

        if (ui->stackedWidget_Client) ui->stackedWidget_Client->setCurrentIndex(2);

    });

    connect(btnDelete, &QPushButton::clicked, this, [this, actionWidget](){

        int r = getRowForClientWidget(actionWidget);

        if (r < 0) return;

        if (QMessageBox::question(this, "Supprimer", "Voulez-vous supprimer ce client ?") == QMessageBox::Yes) {

            ui->tableWidget_Client->removeRow(r);

        }

    });

}



int MainWindow::getRowForClientWidget(QWidget *widget) {

    for (int i = 0; i < ui->tableWidget_Client->rowCount(); i++) {

        if (ui->tableWidget_Client->cellWidget(i, 6) == widget) return i;

    }

    return -1;

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

    QString ref      = getText(0);
    QString date     = getText(1);
    QString tech     = getText(2);
    QString cout_    = getText(3);
    QString duree    = getText(4);
    QString priorite = getText(5);

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
    
    // Refresh stats when table changes (if needed, or just rely on manual refresh/module load)
    // For now, we call it here.
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

// ════════════════════════════════════════════════
//  3D Stock Map – Event Filter & Dialog Launcher
// ════════════════════════════════════════════════

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // Intercept mouse-press on the "prod_l5" label (stockMapTrigger)
    if (event->type() == QEvent::MouseButtonPress) {
        QWidget *w = qobject_cast<QWidget*>(obj);
        if (w && w->property("stockMapTrigger").toBool()) {
            openStockMap3D();
            return true;  // event consumed
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::openStockMap3D()
{
    StockMapDialog dlg(this);

    // Mock occupied slots (temporary – will be replaced by SQL later)
    QStringList occupied;
    occupied << "A1-N1" << "A2-N2" << "B1-N1" << "B3-N3"
             << "C2-N1" << "C4-N2" << "D1-N3" << "D3-N1"
             << "E2-N2" << "E4-N1" << "F1-N1" << "F3-N2"
             << "A3-N1" << "B2-N3" << "C1-N2" << "D4-N1"
             << "E1-N3" << "F2-N1" << "A4-N3" << "B4-N2"
             << "C3-N3" << "D2-N2" << "E3-N1" << "F4-N3";
    dlg.setOccupiedSlots(occupied);

    if (dlg.exec() == QDialog::Accepted) {
        m_selectedEmplacement = dlg.selectedCode();

        // Update the label to show the selected location
        QWidget *root = produitRoot();
        if (root) {
            if (auto *lbl = root->findChild<QLabel*>("prod_l5")) {
                lbl->setText(QString("📍  Emplacement sélectionné : %1  (cliquer pour changer)")
                                 .arg(m_selectedEmplacement));
                lbl->setStyleSheet(
                    "color: #FFD700;"
                    "font-size: 13px;"
                    "font-weight: 700;"
                    "padding: 10px 16px;"
                    "background: rgba(255,215,0,0.08);"
                    "border: 1px solid rgba(255,215,0,0.3);"
                    "border-radius: 10px;"
                );
            }
        }
    }
}
