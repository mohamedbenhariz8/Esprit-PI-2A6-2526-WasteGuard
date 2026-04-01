import codecs

path = r'c:\Users\ASUS\Downloads\INTEGRATION_FINALEeee\INTEGRATION_FINALE\mainwindow.cpp'
with codecs.open(path, 'r', 'utf-8') as f:
    text = f.read()

target1 = '''    QStringList depletionAlerts;
    QStringList expiryAlerts;

    QSqlQuery q;

    q.prepare("SELECT NOM, QUANTITE, SEUIL_CRITIQUE, REFERENCE "
              "FROM MATIERE_PREMIERE WHERE QUANTITE <= SEUIL_CRITIQUE");
    if (q.exec()) {
        while (q.next()) {
            QString nom  = q.value(0).toString();
            int qty      = q.value(1).toInt();
            int seuil    = q.value(2).toInt();
            QString ref  = q.value(3).toString();'''

repl1 = '''    QStringList depletionAlerts;
    QStringList expiryAlerts;
    
    QMap<QString, QStringList> criticalBySupplier;
    QMap<QString, QString> emailBySupplier;

    QSqlQuery q;

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

target2 = '''    // ── Close button ──
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch();
    QPushButton *btnRefresh = new QPushButton(QString::fromUtf8("🔄 Actualiser"), dlg);'''

repl2 = '''    // ── Close button ──
    QHBoxLayout *btnRow = new QHBoxLayout();
    
    for (auto it = criticalBySupplier.begin(); it != criticalBySupplier.end(); ++it) {
        if (it.value().size() >= 3) {
            QString supplierName = it.key();
            QString supplierEmail = emailBySupplier[supplierName];
            QStringList items = it.value();
            
            QPushButton *btnContact = new QPushButton(QString::fromUtf8("📧 Contacter %1 (Smart Cart)").arg(supplierName), dlg);
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
    QPushButton *btnRefresh = new QPushButton(QString::fromUtf8("🔄 Actualiser"), dlg);'''

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
