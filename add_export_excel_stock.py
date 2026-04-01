import os
import re

def main():
    dir_path = r"c:\Users\ASUS\Downloads\final\INTEGRATION_FINALE"
    h_file = os.path.join(dir_path, "mainwindow.h")
    cpp_file = os.path.join(dir_path, "mainwindow.cpp")

    print(f"Modification de {h_file}...")
    with open(h_file, 'r', encoding='utf-8') as f:
        h_content = f.read()

    if "void exportStockExcel();" not in h_content:
        h_content = re.sub(
            r'(void on_btnSave_mod_clicked\(\);)',
            r'\1\n    void exportStockExcel();',
            h_content
        )
        with open(h_file, 'w', encoding='utf-8') as f:
            f.write(h_content)
        print("  -> Déclaration ajoutée dans mainwindow.h")
    else:
        print("  -> La déclaration existe déjà dans mainwindow.h")

    print(f"Modification de {cpp_file}...")
    with open(cpp_file, 'r', encoding='utf-8') as f:
        cpp_content = f.read()

    if "void MainWindow::exportStockExcel()" not in cpp_content:
        # 1. Ajout de la connexion (Signal / Slot)
        connect_code = """
    // Connexion Export Excel (Générer Commande)
    if (ui->btnOrder && !ui->btnOrder->property("connected_export").toBool()) {
        connect(ui->btnOrder, &QPushButton::clicked, this, &MainWindow::exportStockExcel);
        ui->btnOrder->setProperty("connected_export", true);
    }

    // Connexion des boutons pour ajouter/modifier"""
        cpp_content = cpp_content.replace('    // Connexion des boutons pour ajouter/modifier', connect_code)

        # 2. Ajout de l'implémentation de la fonction
        export_func_code = """
void MainWindow::exportStockExcel()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter Inventaire (Excel/CSV)", "Inventaire_Stock.csv", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de creer le fichier.");
        return;
    }

    // Ajout du BOM UTF-8 pour forcer MS Excel à lire les accents correctement
    file.write("\\xEF\\xBB\\xBF");
    
    QTextStream out(&file);

    // En-têtes CSV (Délimiteur point-virgule idéal pour Excel en configuration FR)
    out << "Reference;Nom;Stock;Seuil;Prix Unitaire;Fournisseur\\n";

    QTableWidget *t = ui->tableWidget;
    if (t) {
        for (int i = 0; i < t->rowCount(); ++i) {
            if (t->isRowHidden(i)) continue;

            QString ref = t->item(i, 0) ? t->item(i, 0)->text() : "";
            QString nom = t->item(i, 1) ? t->item(i, 1)->text() : "";
            
            QString stock = t->item(i, 2) ? t->item(i, 2)->text() : "";
            if (stock.isEmpty() && t->item(i, 2)) {
                stock = t->item(i, 2)->data(Qt::DisplayRole).toString();
            }

            QString seuil;
            if (QWidget *w = t->cellWidget(i, 3)) {
                if (QLabel *lbl = w->findChild<QLabel*>()) seuil = lbl->text();
            }

            QString prix = t->item(i, 4) ? t->item(i, 4)->text() : "";
            QString fournisseur = t->item(i, 5) ? t->item(i, 5)->text() : "";

            auto escapeCsv = [](QString str) {
                str.replace("\\"", "\"\"");
                if (str.contains(";") || str.contains("\\"") || str.contains("\\n")) {
                    str = "\\"" + str + "\\"";
                }
                return str;
            };

            out << escapeCsv(ref) << ";" << escapeCsv(nom) << ";" << escapeCsv(stock) << ";" << escapeCsv(seuil) << ";" << escapeCsv(prix) << ";" << escapeCsv(fournisseur) << "\\n";
        }
    }

    file.close();
    QMessageBox::information(this, "Export Excel", QString::fromUtf8("L'inventaire a été exporté au format Excel (CSV) avec succès."));
}
"""
        cpp_content += "\n" + export_func_code

        with open(cpp_file, 'w', encoding='utf-8') as f:
            f.write(cpp_content)
        print("  -> Implémentation de l'export ajoutée dans mainwindow.cpp")
    else:
        print("  -> La fonction d'export existe déjà dans mainwindow.cpp")

    print("Terminé !")

if __name__ == "__main__":
    main()