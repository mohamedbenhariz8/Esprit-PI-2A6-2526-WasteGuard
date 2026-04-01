path = r'c:\Users\ASUS\Downloads\INTEGRATION_FINALEeee\INTEGRATION_FINALE\mainwindow.cpp'
with open(path, 'rb') as f:
    text = f.read()

target1 = b'''    QStringList depletionAlerts;\r
    QStringList expiryAlerts;\r
\r
    QSqlQuery q;\r
\r
    q.prepare("SELECT NOM, QUANTITE, SEUIL_CRITIQUE, REFERENCE "\r
              "FROM MATIERE_PREMIERE WHERE QUANTITE <= SEUIL_CRITIQUE");\r
    if (q.exec()) {\r
        while (q.next()) {\r
            QString nom  = q.value(0).toString();\r
            int qty      = q.value(1).toInt();\r
            int seuil    = q.value(2).toInt();\r
            QString ref  = q.value(3).toString();'''

repl1 = b'''    QStringList depletionAlerts;\r
    QStringList expiryAlerts;\r
    \r
    QMap<QString, QStringList> criticalBySupplier;\r
    QMap<QString, QString> emailBySupplier;\r
\r
    QSqlQuery q;\r
\r
    q.prepare("SELECT NOM, QUANTITE, SEUIL_CRITIQUE, REFERENCE, NOM_FOUR, EMAIL_FOUR "\r
              "FROM MATIERE_PREMIERE WHERE QUANTITE <= SEUIL_CRITIQUE");\r
    if (q.exec()) {\r
        while (q.next()) {\r
            QString nom      = q.value(0).toString();\r
            int qty          = q.value(1).toInt();\r
            int seuil        = q.value(2).toInt();\r
            QString ref      = q.value(3).toString();\r
            QString nomFour  = q.value(4).toString();\r
            QString emailFour= q.value(5).toString();\r
\r
            if (!nomFour.isEmpty()) {\r
                criticalBySupplier[nomFour] << nom;\r
                emailBySupplier[nomFour] = emailFour;\r
            }'''

target2 = b"    QHBoxLayout *btnRow = new QHBoxLayout();\r\n    btnRow->addStretch();\r\n    QPushButton *btnRefresh"

repl2 = b'''    QHBoxLayout *btnRow = new QHBoxLayout();\r
    \r
    for (auto it = criticalBySupplier.begin(); it != criticalBySupplier.end(); ++it) {\r
        if (it.value().size() >= 3) {\r
            QString supplierName = it.key();\r
            QString supplierEmail = emailBySupplier[supplierName];\r
            QStringList items = it.value();\r
            \r
            QPushButton *btnContact = new QPushButton(QString::fromUtf8("Contacter ") + supplierName + " (Smart Cart)", dlg);\r
            btnContact->setStyleSheet("background:#16a34a;color:white;font-weight:bold;padding:8px 15px;border-radius:6px;margin-right:10px;");\r
            \r
            QObject::connect(btnContact, &QPushButton::clicked, [supplierEmail, items]() {\r
                QString subject = "Commande d'urgence WasteGuard";\r
                QString body = "Bonjour,\\n\\nNous avons un besoin urgent de reapprovisionnement pour les articles suivants :\\n";\r
                for (const QString& item : items) {\r
                    body += "- " + item + "\\n";\r
                }\r
                body += "\\nMerci de nous faire un retour rapide.\\n\\nCordialement,\\nL'equipe WasteGuard";\r
                \r
                QUrl url("mailto:" + supplierEmail + "?subject=" + QUrl::toPercentEncoding(subject) + "&body=" + QUrl::toPercentEncoding(body));\r
                QDesktopServices::openUrl(url);\r
            });\r
            btnRow->addWidget(btnContact);\r
        }\r
    }\r
\r
    btnRow->addStretch();\r
    QPushButton *btnRefresh'''

if target1 in text and target2 in text:
    text = text.replace(target1, repl1)
    text = text.replace(target2, repl2)
    with open(path, 'wb') as f:
        f.write(text)
    print("Success")
else:
    print("Not found")
    if target1 not in text: print("Target 1 missing")
    if target2 not in text: print("Target 2 missing")
