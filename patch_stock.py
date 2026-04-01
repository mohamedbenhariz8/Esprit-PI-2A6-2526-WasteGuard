import os
import re

file_path = r"c:\Users\ASUS\Downloads\final\INTEGRATION_FINALE\mainwindow.cpp"

with open(file_path, "r", encoding="utf-8") as f:
    content = f.read()

# 1. Ajout de la connexion des boutons dans setupStockModule
if "ui->btnSave_add->setProperty" not in content:
    content = re.sub(
        r"(    applyStockFilterAndSort\(\);\s*// Setup card view toggle)",
        r"    // Connexion des boutons pour ajouter/modifier\n"
        r"    if (ui->btnSave_add && !ui->btnSave_add->property(\"connected\").toBool()) {\n"
        r"        connect(ui->btnSave_add, &QPushButton::clicked, this, &MainWindow::on_btnSave_add_clicked);\n"
        r"        ui->btnSave_add->setProperty(\"connected\", true);\n"
        r"    }\n"
        r"    if (ui->btnSave_mod && !ui->btnSave_mod->property(\"connected\").toBool()) {\n"
        r"        connect(ui->btnSave_mod, &QPushButton::clicked, this, &MainWindow::on_btnSave_mod_clicked);\n"
        r"        ui->btnSave_mod->setProperty(\"connected\", true);\n"
        r"    }\n\n\1",
        content
    )

# 2. Remplacement de la fonction hardcodée setupStockTableData
pattern = re.compile(r"void MainWindow::setupStockTableData\(\)[\s\S]*?(?=\n// --- CLIENT MODULE)", re.MULTILINE)
match = pattern.search(content)

if match:
    new_setup_stock = """void MainWindow::setupStockTableData()
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

    QSqlQueryModel *model = Stmp.afficher();
    if (!model) return;
    if (model->lastError().isValid()) {
        showFriendlySqlError(this, "charger le stock", model->lastError().text());
        delete model;
        return;
    }

    double totalValue = 0;
    int criticalCount = 0;

    ui->tableWidget->setRowCount(model->rowCount());

    for(int i = 0; i < model->rowCount(); ++i) {
        int id_mp = model->data(model->index(i, 0)).toInt();
        QString ref = model->data(model->index(i, 1)).toString();
        QString nom = model->data(model->index(i, 2)).toString();
        int stockQty = model->data(model->index(i, 3)).toInt();
        int seuilCritique = model->data(model->index(i, 4)).toInt();
        double prix = model->data(model->index(i, 5)).toDouble();
        QString fournisseur = model->data(model->index(i, 7)).toString();

        QString seuilStr = stockSeuilLabel(stockQty, seuilCritique);

        QTableWidgetItem *refItem = new QTableWidgetItem(ref);
        refItem->setData(Qt::UserRole, id_mp);
        ui->tableWidget->setItem(i, 0, refItem);
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(nom));

        QTableWidgetItem *stockItem = new QTableWidgetItem();
        stockItem->setData(Qt::DisplayRole, stockQty);
        ui->tableWidget->setItem(i, 2, stockItem);

        QLabel* badge = new QLabel(seuilStr);
        badge->setAlignment(Qt::AlignCenter);
        badge->setFixedSize(90, 25);
        QString style = "border-radius: 5px; font-weight: bold;";

        if(seuilStr == "CRITIQUE") {
            style += "background-color: #dc3545; color: white;";
            criticalCount++;
        }
        else if(seuilStr == "MOYEN") style += "background-color: #ffc107; color: #333;";
        else style += "background-color: #28a745; color: white;";

        badge->setStyleSheet(style);

        QWidget* badgeWidget = new QWidget();
        QHBoxLayout* badgeLayout = new QHBoxLayout(badgeWidget);
        badgeLayout->addWidget(badge);
        badgeLayout->setAlignment(Qt::AlignCenter);
        badgeLayout->setContentsMargins(0,0,0,0);
        ui->tableWidget->setCellWidget(i, 3, badgeWidget);

        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString("%1 TND").arg(prix, 0, 'f', 2)));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(fournisseur));

        totalValue += stockQty * prix;

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
            QString pxStr = ui->tableWidget->item(row, 4)->text();
            if (ui->inputPrix_mod) ui->inputPrix_mod->setText(pxStr);
            if (ui->inputFournisseur_mod) ui->inputFournisseur_mod->setText(ui->tableWidget->item(row, 5)->text());

            if (ui->sliderStock_mod) ui->sliderStock_mod->setValue(ui->tableWidget->item(row, 2)->data(Qt::DisplayRole).toInt());
            if (ui->sliderPrix_mod) ui->sliderPrix_mod->setValue(parseStockPrice(pxStr));

            ui->tableWidget->selectRow(row);

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

            int idMp = ui->tableWidget->item(row, 0)->data(Qt::UserRole).toInt();
            if (Stmp.supprimer(idMp)) {
                setupStockTableData();
            } else {
                QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression: " + Stmp.lastError());
            }
        });

        layout->addWidget(editBtn);
        layout->addWidget(delBtn);
        layout->setContentsMargins(0,0,0,0);
        ui->tableWidget->setCellWidget(i, 6, container);
    }
    
    delete model;

    if (ui->totalStock) ui->totalStock->setText(QString::number(totalValue, 'f', 3) + " TND");
    if (ui->lblCriticalStock) ui->lblCriticalStock->setText(QString::fromUtf8("\\xE2\\x9A\\xA0 %1 Produits critiques").arg(criticalCount));

    if (ui->lblOrderSummary) {
        if(criticalCount > 0) {
            ui->lblOrderSummary->setText(QString::fromUtf8("Urgent : %1 articles \\xC3\\xA0 commander").arg(criticalCount));
            ui->lblOrderSummary->setStyleSheet("font-size: 13px; color: #E53E3E; font-weight: bold; margin-bottom: 5px;");
        } else {
            ui->lblOrderSummary->setText("Stock suffisant");
            ui->lblOrderSummary->setStyleSheet("font-size: 13px; color: #28a745; margin-bottom: 5px;");
        }
    }

    applyStockFilterAndSort();
    if (m_isStockCardView) {
        refreshStockCardView();
    }
}
"""
    # Fix des caractères UTF-8 échappés pour que Python les écrive correctement.
    new_setup_stock = new_setup_stock.replace("\\xE2\\x9A\\xA0", "\xE2\x9A\xA0")
    new_setup_stock = new_setup_stock.replace("\\xC3\\xA0", "\xC3\xA0")
    
    content = content[:match.start()] + new_setup_stock + content[match.end():]
else:
    print("ATTENTION: Impossible de trouver la fonction setupStockTableData dans le fichier.")

# 3. Ajout des slots de sauvegarde (add/mod)
slots_code = """
void MainWindow::on_btnSave_add_clicked()
{
    QString ref = ui->inputRef_add ? ui->inputRef_add->text().trimmed() : "";
    QString nom = ui->inputNom_add ? ui->inputNom_add->text().trimmed() : "";
    int stock = ui->inputStock_add ? ui->inputStock_add->text().toInt() : 0;
    double prix = ui->inputPrix_add ? parseStockPrice(ui->inputPrix_add->text()) : 0.0;
    QString fournisseur = ui->inputFournisseur_add ? ui->inputFournisseur_add->text().trimmed() : "";

    if (ref.isEmpty() || nom.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "La reference et le nom sont obligatoires.");
        return;
    }

    Stmp.setIdMp(0);
    Stmp.setReference(ref);
    Stmp.setNom(nom);
    Stmp.setQuantite(stock);
    Stmp.setPrix(prix);
    Stmp.setFournisseurInput(fournisseur);
    
    if (Stmp.ajouter()) {
        QMessageBox::information(this, "Succes", "Matiere premiere ajoutee.");
        setupStockTableData();
        if (ui->stock_stackedWidget) ui->stock_stackedWidget->setCurrentIndex(0);
        
        // Vider les champs
        if (ui->inputRef_add) ui->inputRef_add->clear();
        if (ui->inputNom_add) ui->inputNom_add->clear();
        if (ui->inputStock_add) ui->inputStock_add->clear();
        if (ui->inputPrix_add) ui->inputPrix_add->clear();
        if (ui->inputFournisseur_add) ui->inputFournisseur_add->clear();
        if (ui->sliderStock_add) ui->sliderStock_add->setValue(0);
        if (ui->sliderPrix_add) ui->sliderPrix_add->setValue(0);
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible d'ajouter: " + Stmp.lastError());
    }
}

void MainWindow::on_btnSave_mod_clicked()
{
    if (!ui->tableWidget) return;
    int row = ui->tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Aucun element selectionne.");
        return;
    }

    int idMp = ui->tableWidget->item(row, 0)->data(Qt::UserRole).toInt();

    QString ref = ui->inputRef_mod ? ui->inputRef_mod->text().trimmed() : "";
    QString nom = ui->inputNom_mod ? ui->inputNom_mod->text().trimmed() : "";
    int stock = ui->inputStock_mod ? ui->inputStock_mod->text().toInt() : 0;
    double prix = ui->inputPrix_mod ? parseStockPrice(ui->inputPrix_mod->text()) : 0.0;
    QString fournisseur = ui->inputFournisseur_mod ? ui->inputFournisseur_mod->text().trimmed() : "";

    if (ref.isEmpty() || nom.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "La reference et le nom sont obligatoires.");
        return;
    }

    Stmp.setIdMp(idMp);
    Stmp.setReference(ref);
    Stmp.setNom(nom);
    Stmp.setQuantite(stock);
    Stmp.setPrix(prix);
    Stmp.setFournisseurInput(fournisseur);
    
    if (Stmp.modifier()) {
        QMessageBox::information(this, "Succes", "Matiere premiere modifiee.");
        setupStockTableData();
        if (ui->stock_stackedWidget) ui->stock_stackedWidget->setCurrentIndex(0);
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible de modifier: " + Stmp.lastError());
    }
}
"""

if "void MainWindow::on_btnSave_add_clicked()" not in content:
    content += slots_code

with open(file_path, "w", encoding="utf-8") as f:
    f.write(content)

print("Modification appliquee avec succes a mainwindow.cpp !")
