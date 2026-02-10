#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QBrush>
#include <QPalette>
#include <QTimer>
#include <QFrame>
#include <QVBoxLayout>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>

// REMOVE THIS (it causes your error)
// using namespace QtCharts;

static constexpr int ACTIONS_COL = 7;



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 1) Connect stats button safely (no crash even if name changes)
    if (auto *btn = findChild<QPushButton*>("btnOpenStats")) {
        connect(btn, &QPushButton::clicked, this, &MainWindow::openStatsPopup);
    }

    // 2) Build charts once
    buildStatsCharts();

    // ---- Popup overlay initial state ----
    if (ui->overlayDim) {
        ui->overlayDim->hide();
        ui->overlayDim->raise();
    }

    // Close popup button
    connect(ui->btnClosePopup, &QPushButton::clicked, this, &MainWindow::closePopup);

    // Open Ajout popup:
    // Use your existing "+ Ajouter" button on dashboard
    // (objectName from your UI: btnAddProduct)
    if (ui->btnAddProduct)
        connect(ui->btnAddProduct, &QPushButton::clicked, this, &MainWindow::openAjoutPopup);

    // ---- Table setup + example row ----
    if (ui->tableProduits) {
        QTableWidget *t = ui->tableProduits;

        if (t->columnCount() < 8)
            t->setColumnCount(8);

        t->verticalHeader()->setDefaultSectionSize(36);
        t->setWordWrap(false);

        t->horizontalHeader()->setStretchLastSection(false);
        t->horizontalHeader()->setSectionResizeMode(ACTIONS_COL, QHeaderView::Fixed);
        t->setColumnWidth(ACTIONS_COL, 90);

        addExampleRow();

        // Add action buttons after layout is ready
        QTimer::singleShot(0, this, [this]() {
            refreshActionButtons();
        });
    }
}
void MainWindow::openStatsPopup()
{
    if (!ui || !ui->overlayDim || !ui->popupStack) return;

    QWidget *statsPage = nullptr;
    for (int i = 0; i < ui->popupStack->count(); ++i) {
        QWidget *w = ui->popupStack->widget(i);
        if (w && w->objectName() == "statsPopup") {
            statsPage = w;
            break;
        }
    }
    if (!statsPage) return;

    ui->popupStack->setCurrentWidget(statsPage);

    ui->overlayDim->setGeometry(centralWidget()->rect());
    ui->overlayDim->show();
    ui->overlayDim->raise();
}

void MainWindow::buildStatsCharts()
{
    // These are the REAL objectNames inside your .ui
    QFrame *chart1Host = findChild<QFrame*>("graphFrame1Plot");
    QFrame *chart2Host = findChild<QFrame*>("graphFrame2Plot");
    if (!chart1Host || !chart2Host) return;

    auto clearAndLayout = [](QWidget *w) {
        if (!w) return;

        // Remove old layout + placeholder label
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

    // Give them a visible height (your UI had min height = 0)
    chart1Host->setMinimumHeight(260);
    chart2Host->setMinimumHeight(260);

    // -----------------------------
    // Chart 1: PIE (Répartition)
    // -----------------------------
    auto *pieSeries = new QPieSeries();
    pieSeries->append("Produits", 45);
    pieSeries->append("Stock", 30);
    pieSeries->append("Maintenance", 15);
    pieSeries->append("Commandes", 10);

    if (!pieSeries->slices().isEmpty()) {
        auto *slice = pieSeries->slices().at(0);
        slice->setExploded(true);
        slice->setLabelVisible(true);
    }
    for (auto *s : pieSeries->slices())
        s->setLabelVisible(true);

    auto *pieChart = new QChart();
    pieChart->addSeries(pieSeries);
    pieChart->setTitle("Répartition (exemple)");
    pieChart->legend()->setAlignment(Qt::AlignRight);

    auto *pieView = new QChartView(pieChart);
    pieView->setRenderHint(QPainter::Antialiasing);

    // -----------------------------
    // Chart 2: LINE (Croissance)
    // -----------------------------
    auto *line = new QLineSeries();
    line->setName("Croissance");
    line->append(1, 12);
    line->append(2, 18);
    line->append(3, 22);
    line->append(4, 30);
    line->append(5, 38);
    line->append(6, 44);

    auto *growthChart = new QChart();
    growthChart->addSeries(line);
    growthChart->setTitle("Évolution / Croissance (exemple)");
    growthChart->legend()->setAlignment(Qt::AlignBottom);

    auto *axisX = new QValueAxis();
    axisX->setRange(1, 6);
    axisX->setTickCount(6);
    axisX->setLabelFormat("%d");
    growthChart->addAxis(axisX, Qt::AlignBottom);
    line->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setRange(0, 50);
    growthChart->addAxis(axisY, Qt::AlignLeft);
    line->attachAxis(axisY);

    auto *growthView = new QChartView(growthChart);
    growthView->setRenderHint(QPainter::Antialiasing);

    chart1Host->layout()->addWidget(pieView);
    chart2Host->layout()->addWidget(growthView);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);

    // Keep overlay covering the whole central area
    if (ui && ui->overlayDim && centralWidget()) {
        ui->overlayDim->setGeometry(centralWidget()->rect());
        ui->overlayDim->raise();
    }
}

void MainWindow::showPopup(int pageIndex)
{
    if (!ui || !ui->overlayDim || !ui->popupStack) return;

    ui->popupStack->setCurrentIndex(pageIndex); // 0 Ajout, 1 Modifier
    ui->overlayDim->setGeometry(centralWidget()->rect());
    ui->overlayDim->show();
    ui->overlayDim->raise();
}

void MainWindow::closePopup()
{
    if (!ui || !ui->overlayDim) return;
    ui->overlayDim->hide();
}
static void forceSlotBtnStyle(QWidget *root)
{
    if (!root) return;

    const auto buttons = root->findChildren<QPushButton*>();
    for (auto *b : buttons)
    {
        // Only target the depot-slot buttons (A-01, B-01... etc)
        // They all have objectName starting with "btn_" in your UI.
        if (!b->objectName().startsWith("btn_")) continue;

        b->setProperty("class", "slotBtn");

        // Force Qt to re-apply stylesheet rules
        b->style()->unpolish(b);
        b->style()->polish(b);
        b->update();
    }
}
void MainWindow::openAjoutPopup()
{
    showPopup(0);
    forceSlotBtnStyle(ui->ajoutPopup);
    // Ajout
}

void MainWindow::openModifierPopup()
{
    showPopup(1); // Modifier
}


void MainWindow::addExampleRow()
{
    QTableWidget *t = ui->tableProduits;
    if (!t) return;

    int row = t->rowCount();
    t->insertRow(row);

    // Use table palette color so text never becomes white-on-white
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
    QTableWidget *t = ui->tableProduits;
    if (!t) return;
    if (row < 0 || row >= t->rowCount()) return;
    if (t->columnCount() <= ACTIONS_COL) return;

    // If already exists, just update row properties
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

    auto *btnEdit = new QPushButton(QString::fromUtf8("✏️"), cell);
    auto *btnDel  = new QPushButton(QString::fromUtf8("🗑️"), cell);

    btnEdit->setFixedSize(24, 24);
    btnDel->setFixedSize(24, 24);
    btnEdit->setFlat(true);
    btnDel->setFlat(true);
    btnEdit->setFocusPolicy(Qt::NoFocus);
    btnDel->setFocusPolicy(Qt::NoFocus);
    btnEdit->setCursor(Qt::PointingHandCursor);
    btnDel->setCursor(Qt::PointingHandCursor);
    btnEdit->setToolTip("Modifier");
    btnDel->setToolTip("Supprimer");
    btnEdit->setStyleSheet("padding:0; margin:0;");
    btnDel->setStyleSheet("padding:0; margin:0;");

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
    if (!ui || !ui->tableProduits) return;

    for (int r = 0; r < ui->tableProduits->rowCount(); ++r)
        installActionButtonsForRow(r);
}

void MainWindow::handleEditClicked()
{
    auto *btn = qobject_cast<QPushButton*>(sender());
    if (!btn || !ui->tableProduits) return;

    int row = btn->property("row").toInt();
    if (row < 0 || row >= ui->tableProduits->rowCount()) return;

    // Open Modifier popup
    openModifierPopup();

    // (Optional later) You can also pre-fill the modifier fields from the row here.
}

void MainWindow::handleDeleteClicked()
{
    auto *btn = qobject_cast<QPushButton*>(sender());
    if (!btn || !ui->tableProduits) return;

    int row = btn->property("row").toInt();
    if (row < 0 || row >= ui->tableProduits->rowCount()) return;

    ui->tableProduits->removeRow(row);

    // Update buttons properties after row shift
    refreshActionButtons();
}
