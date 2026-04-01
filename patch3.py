import sys, re

with open('mainwindow.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

# Make sure we have the includes
if "#include <QBuffer>" not in content:
    content = "#include <QBuffer>\n" + content
if "#include <QImage>" not in content:
    content = "#include <QImage>\n" + content

pattern = re.compile(r"void MainWindow::exportStockExcel\(\)\s*\{.*?\n\}\s*\n// ---------- Stock module stats ----------", re.DOTALL)

replacement = """void MainWindow::exportStockExcel()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Format d'Export");
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    QLabel *label = new QLabel("Sélectionnez le type d'inventaire à exporter :", &dialog);
    label->setStyleSheet("font-size: 14pt; margin-bottom: 10px; font-weight: bold;");
    label->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(label);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    const QString blueStyle = "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0f2b4c, stop:1 #2a5298);"
                              "color: white; border: none; border-radius: 12px; padding: 14px 16px; font-size: 14px; font-weight: 700; text-align: center;";
    const QString redStyle = "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #991b1b, stop:1 #ef4444);"
                             "color: white; border: none; border-radius: 12px; padding: 14px 16px; font-size: 14px; font-weight: 700; text-align: center;";

    QPushButton *btnAll = new QPushButton("Complet", &dialog);
    btnAll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    btnAll->setStyleSheet(blueStyle);

    QPushButton *btnReorder = new QPushButton("Nécessaires", &dialog);
    btnReorder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    btnReorder->setStyleSheet(blueStyle);
    
    btnLayout->addWidget(btnAll);
    btnLayout->addWidget(btnReorder);
    
    mainLayout->addLayout(btnLayout);
    
    QPushButton *btnCancel = new QPushButton("Annuler", &dialog);
    btnCancel->setStyleSheet(redStyle);
    mainLayout->addWidget(btnCancel);
    
    int choice = -1;
    
    connect(btnAll, &QPushButton::clicked, [&](){ choice = 0; dialog.accept(); });
    connect(btnReorder, &QPushButton::clicked, [&](){ choice = 1; dialog.accept(); });
    connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    
    bool filterCriticalOnly = (choice == 1);
    
    QString defaultName = filterCriticalOnly ? "Commande_Reappro_Stock.xls" : "Inventaire_Stock.xls";
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter Inventaire (Excel HTML)", defaultName, "Excel Web Archive (*.xls *.html)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de creer le fichier.");
        return;
    }

    file.write("\\xEF\\xBB\\xBF");
    QTextStream out(&file);

    // Embed WasteGuard Logo inside HTML
    QImage logo(":/wasteguard_logo.png");
    QString base64Logo;
    if (!logo.isNull()) {
        QByteArray imgArray;
        QBuffer buffer(&imgArray);
        buffer.open(QIODevice::WriteOnly);
        logo.save(&buffer, "PNG");
        base64Logo = QString("data:image/png;base64,") + QString::fromLatin1(imgArray.toBase64());
    }

    QString title = filterCriticalOnly ? "Inventaire à Commander (Critique / Moyen)" : "Inventaire Complet - WasteGuard";
    
    out << "<html>\\n"
        << "<head>\\n"
        << "<meta charset=\\"UTF-8\\">\\n"
        << "<style>\\n"
        << "body { font-family: 'Segoe UI', Arial, sans-serif; background-color: #f4f7f6; color: #333; }\\n"
        << ".header { text-align: center; margin-bottom: 30px; }\\n"
        << ".title { font-size: 24pt; font-weight: bold; color: #0f2b4c; margin-top: 10px; }\\n"
        << "table { border-collapse: collapse; width: 100%; max-width: 1000px; margin: auto; background-color: white; border: 1px solid #ddd; }\\n"
        << "th { background-color: #0f2b4c; color: white; padding: 12px; font-size: 13pt; text-align: left; border: 1px solid #ddd; }\\n"
        << "td { padding: 10px; border: 1px solid #ddd; font-size: 11pt; }\\n"
        << ".row-even { background-color: #f9fafb; }\\n"
        << ".row-odd { background-color: #ffffff; }\\n"
        << ".status-critique { color: #ef4444; font-weight: bold; }\\n"
        << ".status-moyen { color: #f59e0b; font-weight: bold; }\\n"
        << ".status-ok { color: #10b981; font-weight: bold; }\\n"
        << "</style>\\n"
        << "</head>\\n"
        << "<body>\\n"
        << "<div class=\\"header\\">\\n";
        
    if (!base64Logo.isEmpty()) {
        out << "<img src=\\"" << base64Logo << "\\" height=\\"80\\" alt=\\"WasteGuard Logo\\"/><br>\\n";
    }
    
    out << "<div class=\\"title\\">" << title << "</div>\\n"
        << "</div>\\n"
        << "<table>\\n"
        << "<tr>\\n"
        << "<th>Reference</th><th>Nom</th><th>Stock Actuel</th><th>Seuil/Statut</th><th>Prix Unitaire</th><th>Fournisseur</th>\\n"
        << "</tr>\\n";

    QTableWidget *t = ui->tableWidget;
    int exportCount = 0;
    
    if (t) {
        for (int i = 0; i < t->rowCount(); ++i) {
            if (t->isRowHidden(i)) continue;

            QString seuil;
            if (QWidget *w = t->cellWidget(i, 3)) {
                if (QLabel *lbl = w->findChild<QLabel*>()) seuil = lbl->text();
            }

            if (filterCriticalOnly && seuil != "CRITIQUE" && seuil != "MOYEN") {
                continue;
            }

            QString ref = t->item(i, 0) ? t->item(i, 0)->text() : "";
            QString nom = t->item(i, 1) ? t->item(i, 1)->text() : "";
            
            QString stock = t->item(i, 2) ? t->item(i, 2)->text() : "";
            if (stock.isEmpty() && t->item(i, 2)) {
                stock = t->item(i, 2)->data(Qt::DisplayRole).toString();
            }

            QString prix = t->item(i, 4) ? t->item(i, 4)->text() : "";
            QString fournisseur = t->item(i, 5) ? t->item(i, 5)->text() : "";
            
            QString trClass = (exportCount % 2 == 0) ? "row-even" : "row-odd";
            QString statusStyle = "status-ok";
            if (seuil == "CRITIQUE") statusStyle = "status-critique";
            else if (seuil == "MOYEN") statusStyle = "status-moyen";

            out << "<tr class=\\"" << trClass << "\\">\\n"
                << "<td>" << ref << "</td>\\n"
                << "<td><strong>" << nom << "</strong></td>\\n"
                << "<td>" << stock << "</td>\\n"
                << "<td class=\\"" << statusStyle << "\\">" << seuil << "</td>\\n"
                << "<td>" << prix << " TND</td>\\n"
                << "<td>" << fournisseur << "</td>\\n"
                << "</tr>\\n";
            
            exportCount++;
        }
    }

    out << "</table>\\n</body>\\n</html>\\n";
    file.close();
    
    if (exportCount > 0) {
        QMessageBox::information(this, "Export Excel", QString::fromUtf8("L'export stylisé a été généré avec succès (%1 articles).").arg(exportCount));
    } else {
        QMessageBox::information(this, "Export Excel", filterCriticalOnly ? "Aucun article à recommander. L'export est vide." : "L'inventaire est vide.");
    }
}

// ---------- Stock module stats ----------"""

new_content, count = pattern.subn(replacement, content)
if count > 0:
    with open('mainwindow.cpp', 'w', encoding='utf-8') as f:
        f.write(new_content)
    print("PATCH_SUCCESS")
else:
    print("Regex matched nothing!")
