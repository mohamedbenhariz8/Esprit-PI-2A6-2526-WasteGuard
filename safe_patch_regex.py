import re

path = 'mainwindow.cpp'
with open(path, 'r', encoding='utf-8', errors='surrogateescape') as f:
    text = f.read()

pattern1 = r'(q\.prepare\("SELECT NOM, QUANTITE, SEUIL_CRITIQUE, REFERENCE "\s*"FROM MATIERE_PREMIERE WHERE QUANTITE <= SEUIL_CRITIQUE"\);\s*if \(q\.exec\(\)\) \{\s*while \(q\.next\(\)\) \{\s*QString nom\s*=\s*q\.value\(0\)\.toString\(\);\s*int qty\s*=\s*q\.value\(1\)\.toInt\(\);\s*int seuil\s*=\s*q\.value\(2\)\.toInt\(\);\s*QString ref\s*=\s*q\.value\(3\)\.toString\(\);)'

replacement1 = r'''    QMap<QString, QStringList> criticalBySupplier;
    QMap<QString, QString> emailBySupplier;

    q.prepare("SELECT NOM, QUANTITE, SEUIL_CRITIQUE, REFERENCE, NOM_FOUR, EMAIL_FOUR "
              "FROM MATIERE_PREMIERE WHERE QUANTITE <= SEUIL_CRITIQUE");
    if (q.exec()) {
        while (q.next()) {
            QString nom      = q.value(0).toString();
            int qty          = q.value(1).toInt();
            int seuil        = q.value(2).toInt();
            QString ref      = q.value(3).toString();
            QString nomFour  = q.value(4).toString();
            QString emailFour= q.value(5).toString();

            if (!nomFour.isEmpty()) {
                criticalBySupplier[nomFour] << nom;
                emailBySupplier[nomFour] = emailFour;
            }'''

pattern2 = r'(    // [\s\S]*?Close button [\s\S]*?\n    QHBoxLayout \*btnRow = new QHBoxLayout\(\);\n    btnRow->addStretch\(\);\n    QPushButton \*btnRefresh =)'

replacement2 = r'''    // Close button and Smart Cart
    QHBoxLayout *btnRow = new QHBoxLayout();
    
    for (auto it = criticalBySupplier.begin(); it != criticalBySupplier.end(); ++it) {
        if (it.value().size() >= 3) {
            QString supplierName = it.key();
            QString supplierEmail = emailBySupplier[supplierName];
            QStringList items = it.value();
            
            QPushButton *btnContact = new QPushButton("Contacter " + supplierName + " (Smart Cart)", dlg);
            btnContact->setStyleSheet("background:#16a34a;color:white;font-weight:bold;padding:8px 15px;border-radius:6px;margin-right:10px;");
            
            QObject::connect(btnContact, &QPushButton::clicked, [supplierEmail, items]() {
                QString subject = "Commande d'urgence WasteGuard";
                QString body = "Bonjour,\n\nNous avons un besoin urgent de reapprovisionnement pour les articles suivants :\n";
                for (const QString& item : items) {
                    body += "- " + item + "\n";
                }
                body += "\nMerci de nous faire un retour rapide.\n\nCordialement,\nL'equipe WasteGuard";
                
                QUrl url("mailto:" + supplierEmail + "?subject=" + QUrl::toPercentEncoding(subject) + "&body=" + QUrl::toPercentEncoding(body));
                QDesktopServices::openUrl(url);
            });
            btnRow->addWidget(btnContact);
        }
    }

    btnRow->addStretch();
    QPushButton *btnRefresh ='''

new_text, count1 = re.subn(pattern1, replacement1, text)
print(f"Patched 1: {count1} times")

new_text, count2 = re.subn(pattern2, replacement2, new_text)
print(f"Patched 2: {count2} times")

if count1 > 0 and count2 > 0:
    with open(path, 'w', encoding='utf-8', errors='surrogateescape') as f:
        f.write(new_text)
    print("Saved file successfully.")
else:
    print("Modification failed.")
