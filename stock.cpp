#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initial state: Show Dashboard
    ui->stackedWidget->setCurrentIndex(0);

    // Setup Logo
    setupLogo();

    // Setup Table
    setupTableData();

    // Connexions Sliders -> Inputs (Ajouter)
    connect(ui->sliderStock_add, &QSlider::valueChanged, this, [=](int val){ ui->inputStock_add->setText(QString::number(val)); });
    connect(ui->sliderPrix_add, &QSlider::valueChanged, this, [=](int val){ ui->inputPrix_add->setText(QString::number(val)); });

    // Connexions Sliders -> Inputs (Modifier)
    connect(ui->sliderStock_mod, &QSlider::valueChanged, this, [=](int val){ ui->inputStock_mod->setText(QString::number(val)); });
    connect(ui->sliderPrix_mod, &QSlider::valueChanged, this, [=](int val){ ui->inputPrix_mod->setText(QString::number(val) + " TND"); });
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupTableData() {
    ui->tableWidget->setColumnCount(7);
    ui->tableWidget->setRowCount(3);
    ui->tableWidget->setHorizontalHeaderLabels({"REF", "NOM", "STOCK", "SEUIL", "PRIX", "FOURNISSEUR", "ACTIONS"});
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableWidget->verticalHeader()->setDefaultSectionSize(50); // Augmente la hauteur des lignes
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed); // Fixe la colonne Actions
    ui->tableWidget->setColumnWidth(6, 180); // Élargit la colonne Actions

    struct Product { QString ref, nom, stock, seuil, prix, fournisseur; };
    QList<Product> items = {
        {"REF-001", "Capteur Ultrason", "150", "OK", "25 TND", "TechSupply"},
        {"REF-002", "Batterie Lithium", "15", "CRITIQUE", "45 TND", "PowerPack"},
        {"REF-003", "Module GPS", "80", "MOYEN", "30 TND", "GeoTrack"}
    };

    double totalValue = 0;
    int criticalCount = 0;

    for(int i = 0; i < items.size(); ++i) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(items[i].ref));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(items[i].nom));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(items[i].stock));

        // Affichage du SEUIL avec style (Badge)
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

        // Calculs Stats
        double price = items[i].prix.split(" ")[0].toDouble();
        totalValue += items[i].stock.toInt() * price;

        // Create Action Buttons
        QWidget* container = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(container);
        QPushButton* editBtn = new QPushButton("Modifier");
        QPushButton* delBtn = new QPushButton("Supprimer");

        editBtn->setStyleSheet("color: #3182CE; border: none; font-weight: bold;");
        delBtn->setStyleSheet("color: #E53E3E; border: none; font-weight: bold;");

        // NAVIGATION: Pre-fill Modifier Page and Switch
        connect(editBtn, &QPushButton::clicked, this, [=]() {
            ui->inputRef_mod->setText(ui->tableWidget->item(i, 0)->text());
            ui->inputNom_mod->setText(ui->tableWidget->item(i, 1)->text());
            ui->inputStock_mod->setText(ui->tableWidget->item(i, 2)->text());
            ui->inputPrix_mod->setText(ui->tableWidget->item(i, 4)->text());
            ui->inputFournisseur_mod->setText(ui->tableWidget->item(i, 5)->text());

            // Sync Sliders avec les valeurs existantes
            ui->sliderStock_mod->setValue(ui->tableWidget->item(i, 2)->text().toInt());
            ui->sliderPrix_mod->setValue(ui->tableWidget->item(i, 4)->text().split(" ")[0].toInt());

            ui->stackedWidget->setCurrentIndex(2); // Go to Modifier Page
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
        });

        layout->addWidget(editBtn);
        layout->addWidget(delBtn);
        layout->setContentsMargins(0,0,0,0);
        ui->tableWidget->setCellWidget(i, 6, container);
    }

    // Mise à jour des Widgets Stats & Commandes
    ui->totalStock->setText(QString::number(totalValue, 'f', 3) + " TND");
    ui->lblCriticalStock->setText(QString("⚠ %1 Produits Critiques").arg(criticalCount));

    if(criticalCount > 0) {
        ui->lblOrderSummary->setText(QString("Urgent : %1 articles à commander").arg(criticalCount));
        ui->lblOrderSummary->setStyleSheet("font-size: 13px; color: #E53E3E; font-weight: bold; margin-bottom: 5px;");
    } else {
        ui->lblOrderSummary->setText("Stock suffisant");
        ui->lblOrderSummary->setStyleSheet("font-size: 13px; color: #28a745; margin-bottom: 5px;");
    }
}

// Navigation Slot for Sidebar or +Nouveau button
void MainWindow::on_btnNew_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

// Global "Back" logic
void MainWindow::on_btnCancel_add_clicked() { ui->stackedWidget->setCurrentIndex(0); }
void MainWindow::on_btnCancel_mod_clicked() { ui->stackedWidget->setCurrentIndex(0); }

void MainWindow::setupLogo() {
    QPixmap logo(":/wasteguard_logo.png");

    if (logo.isNull()) {
        // Affiche un message si l'image n'est pas chargée (problème de chemin ou de build)
        ui->label_logo->setText("Logo introuvable");
        ui->label_logo->setStyleSheet("color: white; font-weight: bold; border: 1px dashed white; padding: 10px;");
        ui->label_logo->setFixedSize(180, 50);
    } else {
        ui->label_logo->setPixmap(logo);
        ui->label_logo->setScaledContents(true);
        ui->label_logo->setFixedSize(150, 150); // Taille ajustée pour la sidebar
    }
    ui->label_logo->setAlignment(Qt::AlignCenter);
}
