#!/usr/bin/env python3
"""
Patch on_btnSmartCart_clicked to:
1. Fetch QUANTITE, SEUIL_CRITIQUE, PRIX per item from DB
2. Before sending email, show a quantity-choice popup (Zone Securisee vs Zone Optimale)
3. Include the chosen quantity + cost in the email body
"""

filepath = "mainwindow.cpp"

with open(filepath, "r", encoding="utf-8") as f:
    content = f.read()

# ============================================================
# PART 1: Replace the DB query to also fetch quantity fields
# ============================================================
old_query = '''    QSqlQuery q;
    q.prepare("SELECT NOM, NOM_FOUR, EMAIL_FOUR "
              "FROM MATIERE_PREMIERE WHERE QUANTITE <= SEUIL_CRITIQUE");
    if (!q.exec()) {
        QMessageBox::critical(this, "Erreur", "Impossible de charger les alertes stock.");
        return;
    }
    while (q.next()) {
        QString nom      = q.value(0).toString();
        QString nomFour  = q.value(1).toString().trimmed();
        QString emailFour= q.value(2).toString().trimmed();
        if (nomFour.isEmpty()) nomFour = "Fournisseur inconnu";
        criticalBySupplier[nomFour] << nom;
        if (!emailFour.isEmpty())
            emailBySupplier[nomFour] = emailFour;
    }'''

new_query = '''    // Store per-item details needed for zone quantity calculations
    struct ItemDetail {
        QString nom;
        int     quantite;
        int     seuil;
        double  prix;
    };
    QMap<QString, QList<ItemDetail>> detailsBySupplier;

    QSqlQuery q;
    q.prepare("SELECT NOM, NOM_FOUR, EMAIL_FOUR, QUANTITE, SEUIL_CRITIQUE, PRIX "
              "FROM MATIERE_PREMIERE WHERE QUANTITE <= SEUIL_CRITIQUE");
    if (!q.exec()) {
        QMessageBox::critical(this, "Erreur", "Impossible de charger les alertes stock.");
        return;
    }
    while (q.next()) {
        QString nom      = q.value(0).toString();
        QString nomFour  = q.value(1).toString().trimmed();
        QString emailFour= q.value(2).toString().trimmed();
        int     quantite = q.value(3).toInt();
        int     seuil    = q.value(4).toInt();
        double  prix     = q.value(5).toDouble();
        if (nomFour.isEmpty()) nomFour = "Fournisseur inconnu";
        criticalBySupplier[nomFour] << nom;
        if (!emailFour.isEmpty())
            emailBySupplier[nomFour] = emailFour;
        detailsBySupplier[nomFour] << ItemDetail{nom, quantite, seuil, prix};
    }'''

if old_query in content:
    content = content.replace(old_query, new_query)
    print("[OK] Part 1 replaced (LF)")
else:
    crlf = old_query.replace('\n', '\r\n')
    if crlf in content:
        content = content.replace(crlf, new_query.replace('\n', '\r\n'))
        print("[OK] Part 1 replaced (CRLF)")
    else:
        print("[ERROR] Part 1 not found")
        exit(1)

# ============================================================
# PART 2: Replace the email button connect lambda
# ============================================================
old_connect = '''        connect(btn, &QPushButton::clicked, [supplierEmail, supplierName, items, dlg]() {
            QString subject = QString::fromUtf8("Commande d'urgence WasteGuard - %1").arg(supplierName);
            QString body    = QString::fromUtf8("Bonjour,\\n\\nNous avons un besoin urgent de reapprovisionnement pour les articles suivants :\\n");
            for (const QString &item : items)
                body += "- " + item + "\\n";
            body += QString::fromUtf8("\\nMerci de nous faire un retour rapide.\\n\\nCordialement,\\nL'equipe WasteGuard");

            QUrl url("mailto:" + supplierEmail
                     + "?subject=" + QUrl::toPercentEncoding(subject)
                     + "&body="    + QUrl::toPercentEncoding(body));
            QDesktopServices::openUrl(url);
            dlg->accept();
        });'''

new_connect = '''        // Capture item details for zone calculations
        QList<ItemDetail> currentDetails = detailsBySupplier.value(supplierName);

        connect(btn, &QPushButton::clicked, [this, supplierEmail, supplierName, currentDetails, dlg]() {
            // ── Quantity choice popup ──────────────────────────────────────
            QDialog *qtyDlg = new QDialog(this);
            qtyDlg->setWindowTitle(QString::fromUtf8("Quantite a commander - %1").arg(supplierName));
            qtyDlg->setMinimumWidth(480);
            qtyDlg->setAttribute(Qt::WA_DeleteOnClose);

            QVBoxLayout *qtyLayout = new QVBoxLayout(qtyDlg);
            qtyLayout->setSpacing(14);
            qtyLayout->setContentsMargins(20, 20, 20, 20);

            QLabel *qtyTitle = new QLabel(
                QString::fromUtf8("<b style='font-size:14px;color:#0f2b4c;'>Choisissez la quantite a commander</b>"), qtyDlg);
            qtyTitle->setTextFormat(Qt::RichText);
            qtyLayout->addWidget(qtyTitle);

            QLabel *qtySubtitle = new QLabel(
                QString::fromUtf8("<i style='color:#64748b;font-size:12px;'>L'IA WasteGuard propose deux niveaux de reapprovisionnement.</i>"), qtyDlg);
            qtySubtitle->setTextFormat(Qt::RichText);
            qtyLayout->addWidget(qtySubtitle);

            // Compute aggregated zone quantities across all critical items for this supplier
            int totalSafe    = 0;
            int totalOptimal = 0;
            double totalCostSafe    = 0.0;
            double totalCostOptimal = 0.0;

            QString itemLines; // for email body
            QString safeLines;
            QString optimalLines;

            for (const ItemDetail &det : currentDetails) {
                int safeTarget    = qRound(det.seuil * 1.10);
                int qtySafe       = qMax(0, safeTarget - det.quantite);
                int optimalTarget = qMax(det.seuil * 2, qRound(det.quantite * 1.5));
                if (optimalTarget < safeTarget) optimalTarget = safeTarget;
                int qtyOptimal    = qMax(0, optimalTarget - det.quantite);

                totalSafe        += qtySafe;
                totalOptimal     += qtyOptimal;
                totalCostSafe    += qtySafe    * det.prix;
                totalCostOptimal += qtyOptimal * det.prix;

                itemLines += QString("  - %1 (stock: %2, seuil: %3)\n")
                    .arg(det.nom).arg(det.quantite).arg(det.seuil);

                safeLines += QString("  - %1 : %2 unites (%3 DT)\n")
                    .arg(det.nom).arg(qtySafe)
                    .arg(QString::number(qtySafe * det.prix, 'f', 2));

                optimalLines += QString("  - %1 : %2 unites (%3 DT)\n")
                    .arg(det.nom).arg(qtyOptimal)
                    .arg(QString::number(qtyOptimal * det.prix, 'f', 2));
            }

            // ── Zone Securisee card ────────────────────────────────────────
            QFrame *safeCard = new QFrame(qtyDlg);
            safeCard->setStyleSheet(
                "QFrame { background:#fffbeb; border:2px solid #fcd34d; border-radius:10px; padding:4px; }");
            QVBoxLayout *safeLayout = new QVBoxLayout(safeCard);

            QLabel *safeTitle = new QLabel(
                QString::fromUtf8("<b style='color:#b45309;font-size:13px;'>🟡 Zone Securisee (minimum requis)</b>"), safeCard);
            safeTitle->setTextFormat(Qt::RichText);
            safeLayout->addWidget(safeTitle);

            QLabel *safeDesc = new QLabel(
                QString::fromUtf8(
                    "<span style='color:#92400e;'>"
                    "Quantite totale : <b>%1 unites</b> &nbsp;|&nbsp; Cout total : <b>%2 DT</b><br/>"
                    "<span style='font-size:11px;color:#78350f;'>"
                    "Permet de rester juste au-dessus du seuil critique (seuil +10%)."
                    "</span></span>")
                    .arg(totalSafe)
                    .arg(QString::number(totalCostSafe, 'f', 2)),
                safeCard);
            safeDesc->setTextFormat(Qt::RichText);
            safeDesc->setWordWrap(true);
            safeLayout->addWidget(safeDesc);

            QPushButton *btnSafe = new QPushButton(
                QString::fromUtf8("Envoyer email - Zone Securisee (%1 u. / %2 DT)")
                    .arg(totalSafe).arg(QString::number(totalCostSafe, 'f', 2)),
                safeCard);
            btnSafe->setStyleSheet(
                "background:#f59e0b;color:white;font-weight:bold;padding:8px 14px;border-radius:6px;");
            safeLayout->addWidget(btnSafe);
            qtyLayout->addWidget(safeCard);

            // ── Zone Optimale card ─────────────────────────────────────────
            QFrame *optCard = new QFrame(qtyDlg);
            optCard->setStyleSheet(
                "QFrame { background:#eff6ff; border:2px solid #93c5fd; border-radius:10px; padding:4px; }");
            QVBoxLayout *optLayout = new QVBoxLayout(optCard);

            QLabel *optTitle = new QLabel(
                QString::fromUtf8("<b style='color:#1d4ed8;font-size:13px;'>🔵 Zone Optimale (niveau ideal)</b>"), optCard);
            optTitle->setTextFormat(Qt::RichText);
            optLayout->addWidget(optTitle);

            QLabel *optDesc = new QLabel(
                QString::fromUtf8(
                    "<span style='color:#1e40af;'>"
                    "Quantite totale : <b>%1 unites</b> &nbsp;|&nbsp; Cout total : <b>%2 DT</b><br/>"
                    "<span style='font-size:11px;color:#1e3a8a;'>"
                    "Atteint un niveau sain de stock (2x seuil ou 1.5x stock actuel)."
                    "</span></span>")
                    .arg(totalOptimal)
                    .arg(QString::number(totalCostOptimal, 'f', 2)),
                optCard);
            optDesc->setTextFormat(Qt::RichText);
            optDesc->setWordWrap(true);
            optLayout->addWidget(optDesc);

            QPushButton *btnOptimal = new QPushButton(
                QString::fromUtf8("Envoyer email - Zone Optimale (%1 u. / %2 DT)")
                    .arg(totalOptimal).arg(QString::number(totalCostOptimal, 'f', 2)),
                optCard);
            btnOptimal->setStyleSheet(
                "background:#2563eb;color:white;font-weight:bold;padding:8px 14px;border-radius:6px;");
            optLayout->addWidget(btnOptimal);
            qtyLayout->addWidget(optCard);

            // ── Cancel button ──────────────────────────────────────────────
            QPushButton *btnCancel = new QPushButton(QString::fromUtf8("Annuler"), qtyDlg);
            btnCancel->setStyleSheet("background:#64748b;color:white;padding:7px 18px;border-radius:5px;");
            QHBoxLayout *cancelRow = new QHBoxLayout();
            cancelRow->addStretch();
            cancelRow->addWidget(btnCancel);
            qtyLayout->addLayout(cancelRow);

            connect(btnCancel, &QPushButton::clicked, qtyDlg, &QDialog::reject);

            // ── Safe zone send ─────────────────────────────────────────────
            connect(btnSafe, &QPushButton::clicked,
                [supplierEmail, supplierName, itemLines, safeLines,
                 totalSafe, totalCostSafe, qtyDlg, dlg]() {
                    QString subject = QString::fromUtf8("Commande Zone Securisee WasteGuard - %1").arg(supplierName);
                    QString body = QString::fromUtf8(
                        "Bonjour,\n\n"
                        "Suite a l\'analyse IA de notre systeme WasteGuard, nous avons besoin d\'un "
                        "reapprovisionnement urgent (Zone Securisee) pour les articles suivants :\n\n"
                        "%1\n"
                        "QUANTITES DEMANDEES (Zone Securisee - minimum requis) :\n"
                        "%2\n"
                        "Total commande : %3 unites\n"
                        "Cout estime    : %4 DT\n\n"
                        "Merci de nous confirmer la disponibilite et le delai de livraison.\n\n"
                        "Cordialement,\nL\'equipe WasteGuard")
                        .arg(itemLines)
                        .arg(safeLines)
                        .arg(totalSafe)
                        .arg(QString::number(totalCostSafe, 'f', 2));

                    QUrl url("mailto:" + supplierEmail
                             + "?subject=" + QUrl::toPercentEncoding(subject)
                             + "&body="    + QUrl::toPercentEncoding(body));
                    QDesktopServices::openUrl(url);
                    qtyDlg->accept();
                    dlg->accept();
            });

            // ── Optimal zone send ──────────────────────────────────────────
            connect(btnOptimal, &QPushButton::clicked,
                [supplierEmail, supplierName, itemLines, optimalLines,
                 totalOptimal, totalCostOptimal, qtyDlg, dlg]() {
                    QString subject = QString::fromUtf8("Commande Zone Optimale WasteGuard - %1").arg(supplierName);
                    QString body = QString::fromUtf8(
                        "Bonjour,\n\n"
                        "Suite a l\'analyse IA de notre systeme WasteGuard, nous souhaitons effectuer "
                        "un reapprovisionnement complet (Zone Optimale) pour les articles suivants :\n\n"
                        "%1\n"
                        "QUANTITES DEMANDEES (Zone Optimale - niveau ideal de stock) :\n"
                        "%2\n"
                        "Total commande : %3 unites\n"
                        "Cout estime    : %4 DT\n\n"
                        "Merci de nous confirmer la disponibilite et le delai de livraison.\n\n"
                        "Cordialement,\nL\'equipe WasteGuard")
                        .arg(itemLines)
                        .arg(optimalLines)
                        .arg(totalOptimal)
                        .arg(QString::number(totalCostOptimal, 'f', 2));

                    QUrl url("mailto:" + supplierEmail
                             + "?subject=" + QUrl::toPercentEncoding(subject)
                             + "&body="    + QUrl::toPercentEncoding(body));
                    QDesktopServices::openUrl(url);
                    qtyDlg->accept();
                    dlg->accept();
            });

            qtyDlg->exec();
        });'''

if old_connect in content:
    content = content.replace(old_connect, new_connect)
    print("[OK] Part 2 replaced (LF)")
else:
    crlf = old_connect.replace('\n', '\r\n')
    if crlf in content:
        content = content.replace(crlf, new_connect.replace('\n', '\r\n'))
        print("[OK] Part 2 replaced (CRLF)")
    else:
        print("[ERROR] Part 2 not found")
        exit(1)

with open(filepath, "w", encoding="utf-8") as f:
    f.write(content)

print("[DONE] mainwindow.cpp patched successfully!")
