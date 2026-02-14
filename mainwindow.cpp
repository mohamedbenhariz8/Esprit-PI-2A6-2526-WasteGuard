#include "mainwindow.h"
#include "ui_mainwindow.h"

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

static constexpr int ACTIONS_COL = 7;

namespace {
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
    , currentClientRow(-1)
    , currentMaintRow(-1)
    , globalStatsReturnPage(nullptr)
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

        for (int i = 1; i <= 5; ++i) {
            int row = ui->tableEmployes->rowCount();
            ui->tableEmployes->insertRow(row);

            const QString matricule = "EMP-00" + QString::number(i);
            const QString nom = "Employé Test " + QString::number(i);
            const QString specialite = "Mécanicien";
            const QString statut = "Disponible";
            const QString email = QString("employe%1@wasteguard.tn").arg(i);
            const QString cin = QString("1000000%1").arg(i);
            const int salaire = 1200 + ((i - 1) * 150);

            auto *matItem = new QTableWidgetItem(matricule);
            matItem->setData(Qt::UserRole + 1, email);
            matItem->setData(Qt::UserRole + 2, cin);
            matItem->setData(Qt::UserRole + 3, salaire);

            ui->tableEmployes->setItem(row, 0, matItem);
            ui->tableEmployes->setItem(row, 1, new QTableWidgetItem(nom));
            ui->tableEmployes->setItem(row, 2, new QTableWidgetItem(specialite));
            ui->tableEmployes->setItem(row, 3, new QTableWidgetItem(statut));
            installEmployeActionButtonsForRow(row);
        }
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
    connectToGlobalStats(findChild<QPushButton*>("btnGoStats_Client"));
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

    // --- CLIENT INITIALIZATION ---
    if (ui->tableWidget_Client) {
        ui->tableWidget_Client->setColumnCount(7);
        QStringList headers = { "Matricule", "Nom", "Email", "Bacs", "Score", "Paiement", "Actions" };
        ui->tableWidget_Client->setHorizontalHeaderLabels(headers);
        ui->tableWidget_Client->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        
        // Add default example client
        int row = ui->tableWidget_Client->rowCount();
        ui->tableWidget_Client->insertRow(row);
        ui->tableWidget_Client->setItem(row, 0, new QTableWidgetItem("CL-001"));
        ui->tableWidget_Client->setItem(row, 1, new QTableWidgetItem("Jean Dupont"));
        ui->tableWidget_Client->setItem(row, 2, new QTableWidgetItem("jean.dupont@email.com"));
        ui->tableWidget_Client->setItem(row, 3, new QTableWidgetItem("5"));
        ui->tableWidget_Client->setItem(row, 4, new QTableWidgetItem("85"));
        ui->tableWidget_Client->setItem(row, 5, new QTableWidgetItem("Mensuel"));
        addClientActionButtons(row);
    }
    if (ui->btn_save_ajouter) connect(ui->btn_save_ajouter, &QPushButton::clicked, this, &MainWindow::on_btn_ajouter_client_clicked);
    if (ui->btn_annuler_ajouter) connect(ui->btn_annuler_ajouter, &QPushButton::clicked, this, &MainWindow::on_btn_annuler_client_clicked);
    if (ui->btn_save_modifier) connect(ui->btn_save_modifier, &QPushButton::clicked, this, &MainWindow::on_btn_modifier_client_clicked);
    if (ui->btn_annuler_modifier) connect(ui->btn_annuler_modifier, &QPushButton::clicked, this, &MainWindow::on_btn_annuler_client_clicked);
    if (ui->btnNouveau_Client) connect(ui->btnNouveau_Client, &QPushButton::clicked, this, &MainWindow::on_btnNouveau_client_clicked);

    setupStatistics();
    setupAccueilDashboard();
    setupProduitModule();
    setupStockModule();
    setupMaintenanceModule();
    applyUnifiedTopBarStyle();
    forceApplySidebarStyles();
    showDashboardHome();
}

MainWindow::~MainWindow()
{
    delete ui;
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
    const QString cin = ui->txtCIN_Ajout ? ui->txtCIN_Ajout->text().trimmed() : QString();
    const int salaire = ui->sliderSalaire_Ajout ? ui->sliderSalaire_Ajout->value() : 1200;

    const int row = ui->tableEmployes->rowCount();
    ui->tableEmployes->insertRow(row);

    auto *matItem = new QTableWidgetItem(matricule);
    matItem->setData(Qt::UserRole + 1, email);
    matItem->setData(Qt::UserRole + 2, cin);
    matItem->setData(Qt::UserRole + 3, salaire);

    ui->tableEmployes->setItem(row, 0, matItem);
    ui->tableEmployes->setItem(row, 1, new QTableWidgetItem(nom));
    ui->tableEmployes->setItem(row, 2, new QTableWidgetItem(specialite));
    ui->tableEmployes->setItem(row, 3, new QTableWidgetItem(statut));

    installEmployeActionButtonsForRow(row);
    currentEmployeRow = row;
    ui->tableEmployes->selectRow(row);

    if (ui->txtNom_Ajout) ui->txtNom_Ajout->clear();
    if (ui->txtEmailAjout) ui->txtEmailAjout->clear();
    if (ui->txtCIN_Ajout) ui->txtCIN_Ajout->clear();
    if (ui->cbSpecialite_Ajout && ui->cbSpecialite_Ajout->count() > 0) ui->cbSpecialite_Ajout->setCurrentIndex(0);
    if (ui->cbStatut_Ajout && ui->cbStatut_Ajout->count() > 0) ui->cbStatut_Ajout->setCurrentIndex(0);
    if (ui->sliderSalaire_Ajout) {
        const int resetSalaire = qMax(ui->sliderSalaire_Ajout->minimum(), qMin(ui->sliderSalaire_Ajout->maximum(), 1200));
        ui->sliderSalaire_Ajout->setValue(resetSalaire);
    }

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
        setComboValue(ui->cbStatut, statut);

        if (auto *matItem = ui->tableEmployes->item(row, 0)) {
            const QString email = matItem->data(Qt::UserRole + 1).toString().trimmed();
            const QString cin = matItem->data(Qt::UserRole + 2).toString().trimmed();
            const QVariant salaireData = matItem->data(Qt::UserRole + 3);

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

    auto ensureItem = [this, row](int col) -> QTableWidgetItem* {
        QTableWidgetItem *item = ui->tableEmployes->item(row, col);
        if (!item) {
            item = new QTableWidgetItem();
            ui->tableEmployes->setItem(row, col, item);
        }
        return item;
    };

    const QString matricule = ui->txtMatricule ? ui->txtMatricule->text().trimmed() : QString();
    const QString nom = ui->txtNom ? ui->txtNom->text().trimmed() : QString();
    const QString specialite = ui->cbSpecialite ? ui->cbSpecialite->currentText().trimmed() : QString();
    const QString statut = ui->cbStatut ? ui->cbStatut->currentText().trimmed() : QString();
    const QString email = ui->txtEmailModif ? ui->txtEmailModif->text().trimmed() : QString();
    const QString cin = ui->txtCIN_Modif ? ui->txtCIN_Modif->text().trimmed() : QString();
    const int salaire = ui->sliderSalaire_Modif ? ui->sliderSalaire_Modif->value() : 0;

    QTableWidgetItem *matItem = ensureItem(0);
    matItem->setText(matricule);
    matItem->setData(Qt::UserRole + 1, email);
    matItem->setData(Qt::UserRole + 2, cin);
    matItem->setData(Qt::UserRole + 3, salaire);

    ensureItem(1)->setText(nom);
    ensureItem(2)->setText(specialite);
    ensureItem(3)->setText(statut);

    ui->tableEmployes->selectRow(row);
    currentEmployeRow = row;
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

    ui->tableEmployes->removeRow(row);
    refreshEmployeActionButtons();

    currentEmployeRow = -1;

    if (ui->tableEmployes->rowCount() > 0) {
        const int rowToSelect = qMax(0, qMin(row, ui->tableEmployes->rowCount() - 1));
        ui->tableEmployes->selectRow(rowToSelect);
    }

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
    QPieSeries *absenceSeries = new QPieSeries();
    absenceSeries->append("Présent", 85);
    absenceSeries->append("Absent", 15);

    QPieSlice *presentSlice = absenceSeries->slices().at(0);
    presentSlice->setLabelVisible(true);
    presentSlice->setBrush(QColor("#27ae60"));
    presentSlice->setLabelColor(Qt::black);

    QPieSlice *absentSlice = absenceSeries->slices().at(1);
    absentSlice->setLabelVisible(true);
    absentSlice->setExploded(true);
    absentSlice->setBrush(QColor("#c0392b"));
    absentSlice->setLabelColor(Qt::black);

    QChart *absenceChart = new QChart();
    absenceChart->addSeries(absenceSeries);
    absenceChart->setTitle("Taux de Présence Global");
    absenceChart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    absenceChart->legend()->setAlignment(Qt::AlignBottom);
    absenceChart->setAnimationOptions(QChart::SeriesAnimations);

    ui->chartViewAbsence->setChart(absenceChart);
    ui->chartViewAbsence->setRenderHint(QPainter::Antialiasing);

    QBarSet *set0 = new QBarSet("Heures Travaillées");
    *set0 << 40 << 35 << 42 << 38 << 45;
    set0->setColor(QColor("#3498db"));

    QBarSeries *workloadSeries = new QBarSeries();
    workloadSeries->append(set0);

    QChart *workloadChart = new QChart();
    workloadChart->addSeries(workloadSeries);
    workloadChart->setTitle("Charge de Travail par Employé (Semaine)");
    workloadChart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    workloadChart->setAnimationOptions(QChart::SeriesAnimations);
    workloadChart->legend()->setVisible(false);

    QStringList categories;
    categories << "Ali" << "Sara" << "Mohamed" << "Rania" << "Karim";
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    workloadChart->addAxis(axisX, Qt::AlignBottom);
    workloadSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 50);
    axisY->setTitleText("Heures / Semaine");
    workloadChart->addAxis(axisY, Qt::AlignLeft);
    workloadSeries->attachAxis(axisY);

    ui->chartViewWorkload->setChart(workloadChart);
    ui->chartViewWorkload->setRenderHint(QPainter::Antialiasing);

    updateTaskChart("Projet A");
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
    series->setName("Tâches accomplies - " + projectName);

    if (projectName == "Projet A") {
        series->append(0, 12);
        series->append(1, 18);
        series->append(2, 10);
        series->append(3, 22);
        series->append(4, 15);
    } else if (projectName == "Projet B") {
        series->append(0, 5);
        series->append(1, 8);
        series->append(2, 12);
        series->append(3, 10);
        series->append(4, 20);
    } else {
        series->append(0, 8);
        series->append(1, 15);
        series->append(2, 18);
        series->append(3, 12);
        series->append(4, 25);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Progression : " + projectName);
    chart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Jours");
    axisX->setLabelFormat("%d");
    axisX->setTickCount(6);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Tâches");
    axisY->setLabelFormat("%d");
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

    if (QTableWidget *t = produitTable()) {
        if (t->columnCount() < 8)
            t->setColumnCount(8);

        t->verticalHeader()->setDefaultSectionSize(36);
        t->setWordWrap(false);

        // Make Actions column wider (180px) and fixed, others stretch
        t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        // ACTIONS_COL is defined as 7
        t->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);
        t->setColumnWidth(7, 180);

        // Ensure headers are styled properly if not already
        t->horizontalHeader()->show(); 

        addExampleRow();

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

    int row = t->rowCount();
    t->insertRow(row);

    QColor textColor = Qt::black;

    auto setCell = [&](int col, const QString &txt)
    {
        auto *it = new QTableWidgetItem(txt);
        it->setForeground(QBrush(textColor));
        t->setItem(row, col, it);
    };

    setCell(0, "WG-001");
    setCell(1, "SmartBin X1");
    setCell(2, "Intérieur");
    setCell(3, "60 L");
    setCell(4, "450");
    setCell(5, "12");
    setCell(6, "Actif");
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

    t->removeRow(row);
    refreshActionButtons();
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
        
        // Add example row
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem("INT-001"));
        table->setItem(row, 1, new QTableWidgetItem("2024-02-11"));
        table->setItem(row, 2, new QTableWidgetItem("Ahmed Ali"));
        table->setItem(row, 3, new QTableWidgetItem("150.0 DT"));
        table->setItem(row, 4, new QTableWidgetItem("2h"));
        table->setItem(row, 5, new QTableWidgetItem("Normale"));
        
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
        {"REF-001", "Capteur Ultrason", "150", "OK", "25 TND", "TechSupply"},
        {"REF-002", "Batterie Lithium", "15", "CRITIQUE", "45 TND", "PowerPack"},
        {"REF-003", "Module GPS", "80", "MOYEN", "30 TND", "GeoTrack"}
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
}
