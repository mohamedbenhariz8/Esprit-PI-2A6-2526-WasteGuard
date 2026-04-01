import codecs

path = r'c:\Users\ASUS\Downloads\INTEGRATION_FINALEeee\INTEGRATION_FINALE\mainwindow.cpp'
with codecs.open(path, 'r', 'utf-8') as f:
    text = f.read()

target1 = (
    "    QStringList depletionAlerts;\r\n"
    "    QStringList expiryAlerts;\r\n"
    "\r\n"
    "    QSqlQuery q;\r\n"
    "\r\n"
    "    q.prepare(\"SELECT NOM, QUANTITE, SEUIL_CRITIQUE, REFERENCE \"\r\n"
    "              \"FROM MATIERE_PREMIERE WHERE QUANTITE <= SEUIL_CRITIQUE\");\r\n"
    "    if (q.exec()) {\r\n"
    "        while (q.next()) {\r\n"
    "            QString nom  = q.value(0).toString();\r\n"
    "            int qty      = q.value(1).toInt();\r\n"
    "            int seuil    = q.value(2).toInt();\r\n"
    "            QString ref  = q.value(3).toString();\r\n"
    "            depletionAlerts << QString::fromUtf8(\"<li>\u26a0\ufe0f <b>%1</b> (R\u00e9f: %2) \u2014 <b>%3</b> unit\u00e9s restantes (Seuil: %4)</li>\")\r\n"
    "                               .arg(nom.toHtmlEscaped(), ref.toHtmlEscaped()).arg(qty).arg(seuil);\r\n"
    "        }\r\n"
    "    }"
)

target2 = "    QHBoxLayout *btnRow = new QHBoxLayout();\r\n    btnRow->addStretch();\r\n    QPushButton *btnRefresh"

repl1 = (
    "    QStringList depletionAlerts;\r\n"
    "    QStringList expiryAlerts;\r\n"
    "    \r\n"
    "    QMap<QString, QStringList> criticalBySupplier;\r\n"
    "    QMap<QString, QString> emailBySupplier;\r\n"
    "\r\n"
    "    QSqlQuery q;\r\n"
    "\r\n"
    "    q.prepare(\"SELECT NOM, QUANTITE, SEUIL_CRITIQUE, REFERENCE, NOM_FOUR, EMAIL_FOUR \"\r\n"
    "              \"FROM MATIERE_PREMIERE WHERE QUANTITE <= SEUIL_CRITIQUE\");\r\n"
    "    if (q.exec()) {\r\n"
    "        while (q.next()) {\r\n"
    "            QString nom      = q.value(0).toString();\r\n"
    "            int qty          = q.value(1).toInt();\r\n"
    "            int seuil        = q.value(2).toInt();\r\n"
    "            QString ref      = q.value(3).toString();\r\n"
    "            QString nomFour  = q.value(4).toString();\r\n"
    "            QString emailFour= q.value(5).toString();\r\n"
    "\r\n"
    "            if (!nomFour.isEmpty()) {\r\n"
    "                criticalBySupplier[nomFour] << nom;\r\n"
    "                emailBySupplier[nomFour] = emailFour;\r\n"
    "            }\r\n"
    "\r\n"
    "            depletionAlerts << QString::fromUtf8(\"<li>\u26a0\ufe0f <b>%1</b> (R\u00e9f: %2) \u2014 <b>%3</b> unit\u00e9s restantes (Seuil: %4)</li>\")\r\n"
    "                               .arg(nom.toHtmlEscaped(), ref.toHtmlEscaped()).arg(qty).arg(seuil);\r\n"
    "        }\r\n"
    "    }"
)

repl2 = (
    "    QHBoxLayout *btnRow = new QHBoxLayout();\r\n"
    "    \r\n"
    "    for (auto it = criticalBySupplier.begin(); it != criticalBySupplier.end(); ++it) {\r\n"
    "        if (it.value().size() >= 3) {\r\n"
    "            QString supplierName = it.key();\r\n"
    "            QString supplierEmail = emailBySupplier[supplierName];\r\n"
    "            QStringList items = it.value();\r\n"
    "            \r\n"
    "            QPushButton *btnContact = new QPushButton(QString::fromUtf8(\"\ud83d\udce7 Contacter %1 (Smart Cart)\").arg(supplierName), dlg);\r\n"
    "            btnContact->setStyleSheet(\"background:#16a34a;color:white;font-weight:bold;padding:8px 15px;border-radius:6px;margin-right:10px;\");\r\n"
    "            \r\n"
    "            QObject::connect(btnContact, &QPushButton::clicked, [supplierEmail, items]() {\r\n"
    "                QString subject = \"Commande d'urgence WasteGuard\";\r\n"
    "                QString body = \"Bonjour,\\n\\nNous avons un besoin urgent de reapprovisionnement pour les articles suivants :\\n\";\r\n"
    "                for (const QString& item : items) {\r\n"
    "                    body += \"- \" + item + \"\\n\";\r\n"
    "                }\r\n"
    "                body += \"\\nMerci de nous faire un retour rapide.\\n\\nCordialement,\\nL'equipe WasteGuard\";\r\n"
    "                \r\n"
    "                QUrl url(\"mailto:\" + supplierEmail + \"?subject=\" + QUrl::toPercentEncoding(subject) + \"&body=\" + QUrl::toPercentEncoding(body));\r\n"
    "                QDesktopServices::openUrl(url);\r\n"
    "            });\r\n"
    "            btnRow->addWidget(btnContact);\r\n"
    "        }\r\n"
    "    }\r\n"
    "\r\n"
    "    btnRow->addStretch();\r\n"
    "    QPushButton *btnRefresh"
)

if target1 in text and target2 in text:
    text = text.replace(target1, repl1)
    text = text.replace(target2, repl2)
    with codecs.open(path, 'w', 'utf-8') as f:
        f.write(text)
    print("Success")
else:
    print("Not found")
    if target1 not in text: print("Target 1 missing")
    if target2 not in text: print("Target 2 missing")
