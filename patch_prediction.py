#!/usr/bin/env python3
"""Patch mainwindow.cpp to add safe zone and optimal zone to stock prediction."""

import sys

filepath = "mainwindow.cpp"

with open(filepath, "r", encoding="utf-8") as f:
    content = f.read()

# ============ PART 1: Replace prediction calculation section ============
old_calc = '''    // 4) Calculate Predictions
    QString predictionMsg;
    QString frequencyMsg;
    
    if (m >= -0.01) {
        predictionMsg = "La consommation est stable ou en hausse. Pas de risque de rupture lineaire estime.";
        frequencyMsg = "Maintien des seuils classiques recommande.";
    } else {
        // x for y = seuil
        double x_seuil = (seuil - b) / m;
        double daysReal = x_seuil - ((nowMs - minX) / 86400000.0);
        
        if (daysReal <= 0) {
            predictionMsg = "ALERTE : Le stock est deja dans un etat critique par rapport a la courbe IA !";
            frequencyMsg = "Action immediate requise.";
        } else {
            QDate depDate = QDate::currentDate().addDays(qRound(daysReal));
            predictionMsg = QString("Epuisement estime au : <b>%1</b> (dans %2 jours).")
                                .arg(depDate.toString("dd/MM/yyyy")).arg(qRound(daysReal));
            
            // Frequency is roughly absolute value of (average quantity ordered / daily consumption)
            // Daily consumption is roughly -m.
            int avgQtyOrdered = qMax(20, (int)(currentStock * 1.5)); // rough estimate
            int freqDays = qRound(avgQtyOrdered / -m);
            frequencyMsg = QString::fromUtf8("L'IA recommande d'acheter en lots tous les <b>%1 jours</b> pour maintenir la rotation optimale (base sur une diminution de %2/jour).")
                                .arg(freqDays).arg(QString::number(-m, 'f', 1));
        }
    }'''

new_calc = '''    // 4) Calculate Predictions
    QString predictionMsg;
    QString frequencyMsg;

    // -- Zone quantity calculations --
    // Safe zone: quantity needed to bring stock just above seuil (threshold + 10% margin)
    int safeTarget = qRound(seuil * 1.10); // 10% above critical threshold
    int qtySafeZone = qMax(0, safeTarget - currentStock);

    // Optimal zone: quantity to reach a healthy stock level
    // Defined as 2x seuil or 1.5x current stock (whichever is higher)
    int optimalTarget = qMax(seuil * 2, qRound(currentStock * 1.5));
    if (optimalTarget < safeTarget) optimalTarget = safeTarget; // optimal must be >= safe
    int qtyOptimalZone = qMax(0, optimalTarget - currentStock);

    if (m >= -0.01) {
        predictionMsg = "La consommation est stable ou en hausse. Pas de risque de rupture lineaire estime.";
        frequencyMsg = "Maintien des seuils classiques recommande.";
    } else {
        // x for y = seuil
        double x_seuil = (seuil - b) / m;
        double daysReal = x_seuil - ((nowMs - minX) / 86400000.0);
        
        if (daysReal <= 0) {
            predictionMsg = "ALERTE : Le stock est deja dans un etat critique par rapport a la courbe IA !";
            frequencyMsg = "Action immediate requise.";
        } else {
            QDate depDate = QDate::currentDate().addDays(qRound(daysReal));
            predictionMsg = QString("Epuisement estime au : <b>%1</b> (dans %2 jours).")
                                .arg(depDate.toString("dd/MM/yyyy")).arg(qRound(daysReal));
            
            // Frequency is roughly absolute value of (average quantity ordered / daily consumption)
            // Daily consumption is roughly -m.
            int avgQtyOrdered = qMax(20, (int)(currentStock * 1.5)); // rough estimate
            int freqDays = qRound(avgQtyOrdered / -m);
            frequencyMsg = QString::fromUtf8("L'IA recommande d'acheter en lots tous les <b>%1 jours</b> pour maintenir la rotation optimale (base sur une diminution de %2/jour).")
                                .arg(freqDays).arg(QString::number(-m, 'f', 1));
        }
    }'''

if old_calc not in content:
    # Try with \r\n
    old_calc_crlf = old_calc.replace('\n', '\r\n')
    if old_calc_crlf in content:
        content = content.replace(old_calc_crlf, new_calc.replace('\n', '\r\n'))
        print("[OK] Part 1 replaced (CRLF)")
    else:
        print("[ERROR] Part 1: Could not find old calculation block")
        sys.exit(1)
else:
    content = content.replace(old_calc, new_calc)
    print("[OK] Part 1 replaced (LF)")

# ============ PART 2: Replace the UI stats card section ============
old_ui = '''    // 5) Build AI UI Interface
    QDialog *aiDialog = new QDialog(this);
    aiDialog->setWindowTitle("WasteGuard AI - Module de Prediction de Stock");
    aiDialog->resize(900, 600);
    QVBoxLayout *layout = new QVBoxLayout(aiDialog);
    
    QLabel *lblTitle = new QLabel(QString::fromUtf8("IA Predictive : <b>%1</b> (Ref: %2)").arg(nom, ref), aiDialog);
    lblTitle->setStyleSheet("font-size: 18px; color: #0f2b4c; background: #e0f2fe; padding: 10px; border-radius: 8px;");
    lblTitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(lblTitle);

    // AI summary card
    QFrame *statsCard = new QFrame(aiDialog);
    statsCard->setStyleSheet("QFrame { background-color: #f8fafc; border: 1px solid #cbd5e1; border-radius: 10px; }");
    QVBoxLayout *statsLayout = new QVBoxLayout(statsCard);
    
    QLabel *lblPred = new QLabel(predictionMsg, statsCard);
    lblPred->setStyleSheet("font-size: 14px; color: #b91c1c; font-weight: bold; padding: 5px;");
    
    QLabel *lblFreq = new QLabel(frequencyMsg, statsCard);
    lblFreq->setStyleSheet("font-size: 14px; color: #059669; padding: 5px;");
    
    statsLayout->addWidget(lblPred);
    statsLayout->addWidget(lblFreq);
    layout->addWidget(statsCard);'''

new_ui = '''    // -- Fetch unit price early for zone cost display --
    double unitPriceForDisplay = 1.0;
    {
        QSqlQuery pq2;
        pq2.prepare("SELECT PRIX FROM MATIERE_PREMIERE WHERE ID_MP = :id");
        pq2.bindValue(":id", idMp);
        if (pq2.exec() && pq2.next()) unitPriceForDisplay = pq2.value(0).toDouble();
    }
    double costSafeZone    = qtySafeZone    * unitPriceForDisplay;
    double costOptimalZone = qtyOptimalZone * unitPriceForDisplay;

    // Build zone recommendation messages
    QString safeZoneMsg = (qtySafeZone > 0)
        ? QString::fromUtf8("Zone Securisee : acheter au moins <b>%1 unites</b> (cout : <b>%2 DT</b>) pour rester au-dessus du seuil critique (%3).")
              .arg(qtySafeZone).arg(QString::number(costSafeZone, 'f', 2)).arg(seuil)
        : QString::fromUtf8("Zone Securisee : le stock actuel (%1) est deja au-dessus du seuil critique (%2). Aucun achat urgent necessaire.")
              .arg(currentStock).arg(seuil);

    QString optimalZoneMsg = (qtyOptimalZone > 0)
        ? QString::fromUtf8("Zone Optimale : acheter <b>%1 unites</b> (cout : <b>%2 DT</b>) pour atteindre le niveau optimal de <b>%3 unites</b>.")
              .arg(qtyOptimalZone).arg(QString::number(costOptimalZone, 'f', 2)).arg(optimalTarget)
        : QString::fromUtf8("Zone Optimale : le stock actuel (%1) est deja au niveau optimal (%2). Aucun reapprovisionnement necessaire.")
              .arg(currentStock).arg(optimalTarget);

    // 5) Build AI UI Interface
    QDialog *aiDialog = new QDialog(this);
    aiDialog->setWindowTitle("WasteGuard AI - Module de Prediction de Stock");
    aiDialog->resize(900, 650);
    QVBoxLayout *layout = new QVBoxLayout(aiDialog);
    
    QLabel *lblTitle = new QLabel(QString::fromUtf8("IA Predictive : <b>%1</b> (Ref: %2)").arg(nom, ref), aiDialog);
    lblTitle->setStyleSheet("font-size: 18px; color: #0f2b4c; background: #e0f2fe; padding: 10px; border-radius: 8px;");
    lblTitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(lblTitle);

    // AI summary card
    QFrame *statsCard = new QFrame(aiDialog);
    statsCard->setStyleSheet("QFrame { background-color: #f8fafc; border: 1px solid #cbd5e1; border-radius: 10px; }");
    QVBoxLayout *statsLayout = new QVBoxLayout(statsCard);
    
    QLabel *lblPred = new QLabel(predictionMsg, statsCard);
    lblPred->setStyleSheet("font-size: 14px; color: #b91c1c; font-weight: bold; padding: 5px;");
    
    QLabel *lblFreq = new QLabel(frequencyMsg, statsCard);
    lblFreq->setStyleSheet("font-size: 14px; color: #059669; padding: 5px;");

    // -- Zone Securisee indicator --
    QLabel *lblSafe = new QLabel(safeZoneMsg, statsCard);
    lblSafe->setTextFormat(Qt::RichText);
    lblSafe->setWordWrap(true);
    lblSafe->setStyleSheet(
        qtySafeZone > 0
            ? "font-size: 13px; color: #b45309; background: #fffbeb; border: 1px solid #fcd34d; border-radius: 6px; padding: 6px 8px; margin-top: 4px;"
            : "font-size: 13px; color: #15803d; background: #f0fdf4; border: 1px solid #86efac; border-radius: 6px; padding: 6px 8px; margin-top: 4px;"
    );

    // -- Zone Optimale indicator --
    QLabel *lblOptimal = new QLabel(optimalZoneMsg, statsCard);
    lblOptimal->setTextFormat(Qt::RichText);
    lblOptimal->setWordWrap(true);
    lblOptimal->setStyleSheet(
        qtyOptimalZone > 0
            ? "font-size: 13px; color: #1d4ed8; background: #eff6ff; border: 1px solid #93c5fd; border-radius: 6px; padding: 6px 8px; margin-top: 2px;"
            : "font-size: 13px; color: #15803d; background: #f0fdf4; border: 1px solid #86efac; border-radius: 6px; padding: 6px 8px; margin-top: 2px;"
    );

    statsLayout->addWidget(lblPred);
    statsLayout->addWidget(lblFreq);
    statsLayout->addWidget(lblSafe);
    statsLayout->addWidget(lblOptimal);
    layout->addWidget(statsCard);'''

if old_ui not in content:
    old_ui_crlf = old_ui.replace('\n', '\r\n')
    if old_ui_crlf in content:
        content = content.replace(old_ui_crlf, new_ui.replace('\n', '\r\n'))
        print("[OK] Part 2 replaced (CRLF)")
    else:
        print("[ERROR] Part 2: Could not find old UI block")
        sys.exit(1)
else:
    content = content.replace(old_ui, new_ui)
    print("[OK] Part 2 replaced (LF)")

with open(filepath, "w", encoding="utf-8") as f:
    f.write(content)

print("[DONE] mainwindow.cpp patched successfully!")
