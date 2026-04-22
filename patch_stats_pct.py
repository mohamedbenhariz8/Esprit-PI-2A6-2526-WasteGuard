#!/usr/bin/env python3
"""
Enrich buildStockStats() with percentage indicators:
1. Add a KPI row (4 summary cards) above the charts: % OK, % Moyen, % Critique, % Valeur Top3
2. Add percentage of total value label on each bar of the financial chart (via a label series)
3. Add "% vs seuil" info on the Quantite vs Seuil chart title and tooltip
"""

filepath = "mainwindow.cpp"

with open(filepath, "r", encoding="utf-8") as f:
    content = f.read()

# ── Target: the data-processing loop and chart building section ───────────────
# We'll replace from the "int okCount = 0" line to the end of chart 1 (cvStatus->setChart)
# to inject KPI cards and enrich the charts.

OLD = '''            int okCount = 0, moyenCount = 0, critCount = 0;
            QBarSeries *seuilSeries = new QBarSeries();
            QBarSet *setStock = new QBarSet("Quantite Actuelle");
            QBarSet *setSeuil = new QBarSet("Seuil Critique");
            QStringList seuilCategories;

            QBarSeries *finSeries = new QBarSeries();
            QBarSet *setFin = new QBarSet("Valeur Globale (TND)");
            QStringList finCategories;

            struct ItemVal { QString name; double val; };
            QList<ItemVal> financials;

            for (int i=0; i<model->rowCount(); i++) {
                QString name = model->data(model->index(i, 1)).toString();
                int qty = model->data(model->index(i, 3)).toInt();
                int seuilCrit = model->data(model->index(i, 4)).toInt(); 
                double prix = model->data(model->index(i, 5)).toDouble();

                if (qty < 10) critCount++;
                else if (qty <= 20) moyenCount++;
                else okCount++;

                if (i < 10) {
                    seuilCategories << name;
                    *setStock << qty;
                    *setSeuil << seuilCrit;
                }

                financials.append({name, qty * prix});
            }
            delete model;

            // 1. Status Chart
            QPieSeries *pieSeries = new QPieSeries();
            pieSeries->setHoleSize(0.35);
            if(okCount>0) {
                auto *s = pieSeries->append(QString("OK (%1)").arg(okCount), okCount);
                if (s) s->setBrush(QColor("#22c55e"));
            }
            if(moyenCount>0) {
                auto *s = pieSeries->append(QString("MOYEN (%1)").arg(moyenCount), moyenCount);
                if (s) s->setBrush(QColor("#facc15"));
            }
            if(critCount>0) {
                auto *s = pieSeries->append(QString("CRITIQUE (%1)").arg(critCount), critCount);
                if (s) {
                    s->setBrush(QColor("#ef4444"));
                    s->setExploded(true);
                }
            }
            for (auto *slice : pieSeries->slices()) {
                if (slice->value() > 0) {
                    double pct = slice->percentage() * 100.0;
                    slice->setLabel(QString("%1\\n%2%").arg(slice->label()).arg(pct, 0, 'f', 1));
                    slice->setLabelVisible(true);
                }
            }
            QChart *chartStatus = new QChart();
            chartStatus->addSeries(pieSeries);
            chartStatus->setTitle("Sante Globale du Stock");
            chartStatus->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
            chartStatus->legend()->setAlignment(Qt::AlignBottom);
            cvStatus->setChart(chartStatus);

            // 2. Seuil Chart
            setStock->setColor(QColor("#3b82f6"));
            setSeuil->setColor(QColor("#ef4444"));
            seuilSeries->append(setStock);
            seuilSeries->append(setSeuil);
            QChart *chartSeuil = new QChart();
            chartSeuil->addSeries(seuilSeries);
            chartSeuil->setTitle("Quantite vs Seuil (Top 10)");
            chartSeuil->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
            QBarCategoryAxis *axisXSeuil = new QBarCategoryAxis();
            axisXSeuil->append(seuilCategories);
            chartSeuil->addAxis(axisXSeuil, Qt::AlignBottom);
            seuilSeries->attachAxis(axisXSeuil);
            QValueAxis *axisYSeuil = new QValueAxis();
            chartSeuil->addAxis(axisYSeuil, Qt::AlignLeft);
            seuilSeries->attachAxis(axisYSeuil);
            cvSeuil->setChart(chartSeuil);

            // 3. Financial Chart
            std::sort(financials.begin(), financials.end(), [](const ItemVal& a, const ItemVal& b){ return a.val > b.val; });
            for(int i=0; i<qMin(10, (int)financials.size()); i++) {
                finCategories << financials[i].name;
                *setFin << financials[i].val;
            }
            finSeries->append(setFin);
            setFin->setColor(QColor("#8b5cf6"));
            QChart *chartFin = new QChart();
            chartFin->addSeries(finSeries);
            chartFin->setTitle("Analyse de Valeur / ABC (Top 10)");
            chartFin->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
            QBarCategoryAxis *axisXFin = new QBarCategoryAxis();
            axisXFin->append(finCategories);
            chartFin->addAxis(axisXFin, Qt::AlignBottom);
            finSeries->attachAxis(axisXFin);
            QValueAxis *axisYFin = new QValueAxis();
            chartFin->addAxis(axisYFin, Qt::AlignLeft);
            finSeries->attachAxis(axisYFin);
            cvFin->setChart(chartFin);'''

NEW = '''            int okCount = 0, moyenCount = 0, critCount = 0;
            QBarSeries *seuilSeries = new QBarSeries();
            QBarSet *setStock = new QBarSet("Quantite Actuelle");
            QBarSet *setSeuil = new QBarSet("Seuil Critique");
            // Percentage-of-seuil bar set (qty/seuil * 100)
            QStringList seuilCategories;
            QList<double> seuilRatios; // qty/seuil % for tooltip-like label

            QBarSeries *finSeries = new QBarSeries();
            QBarSet *setFin = new QBarSet("Valeur Globale (TND)");
            QStringList finCategories;

            struct ItemVal { QString name; double val; };
            QList<ItemVal> financials;
            double totalFinVal = 0.0;
            int totalItems = 0;

            for (int i=0; i<model->rowCount(); i++) {
                QString name = model->data(model->index(i, 1)).toString();
                int qty = model->data(model->index(i, 3)).toInt();
                int seuilCrit = model->data(model->index(i, 4)).toInt();
                double prix = model->data(model->index(i, 5)).toDouble();
                totalItems++;

                if (qty < 10) critCount++;
                else if (qty <= 20) moyenCount++;
                else okCount++;

                if (i < 10) {
                    seuilCategories << name;
                    *setStock << qty;
                    *setSeuil << seuilCrit;
                    double ratio = (seuilCrit > 0) ? (qty * 100.0 / seuilCrit) : 100.0;
                    seuilRatios << ratio;
                }

                double itemVal = qty * prix;
                financials.append({name, itemVal});
                totalFinVal += itemVal;
            }
            delete model;

            // ── KPI summary cards row ──────────────────────────────────────
            // Find or create the KPI row widget
            QWidget *content2 = sa->widget();
            QGridLayout *grid2 = content2 ? qobject_cast<QGridLayout*>(content2->layout()) : nullptr;

            if (grid2) {
                // Remove old KPI row if it exists (row -1 = row 2 was charts, we insert at row 0)
                QWidget *oldKpi = content2->findChild<QWidget*>("kpiRow");
                if (oldKpi) { grid2->removeWidget(oldKpi); delete oldKpi; }

                QWidget *kpiRow = new QWidget(content2);
                kpiRow->setObjectName("kpiRow");
                QHBoxLayout *kpiLayout = new QHBoxLayout(kpiRow);
                kpiLayout->setSpacing(10);
                kpiLayout->setContentsMargins(4, 4, 4, 4);

                auto makeKpiCard = [&](const QString &label, const QString &value,
                                       const QString &pct, const QString &color,
                                       const QString &bg) -> QFrame* {
                    QFrame *card = new QFrame(kpiRow);
                    card->setStyleSheet(QString("QFrame { background:%1; border-radius:10px; border:1px solid %2; }").arg(bg, color));
                    QVBoxLayout *cl = new QVBoxLayout(card);
                    cl->setContentsMargins(12, 10, 12, 10);
                    cl->setSpacing(2);
                    QLabel *lbl = new QLabel(label, card);
                    lbl->setStyleSheet("font-size:11px; color:#64748b; font-weight:600; background:transparent; border:none;");
                    QLabel *val = new QLabel(value, card);
                    val->setStyleSheet(QString("font-size:22px; font-weight:bold; color:%1; background:transparent; border:none;").arg(color));
                    QLabel *p   = new QLabel(pct, card);
                    p->setStyleSheet("font-size:11px; color:#94a3b8; background:transparent; border:none;");
                    cl->addWidget(lbl);
                    cl->addWidget(val);
                    cl->addWidget(p);
                    return card;
                };

                double pctOk   = (totalItems > 0) ? (okCount    * 100.0 / totalItems) : 0;
                double pctMoy  = (totalItems > 0) ? (moyenCount * 100.0 / totalItems) : 0;
                double pctCrit = (totalItems > 0) ? (critCount  * 100.0 / totalItems) : 0;

                // Top-3 concentration of value
                double top3Val = 0.0;
                QList<ItemVal> sortedFin = financials;
                std::sort(sortedFin.begin(), sortedFin.end(), [](const ItemVal& a, const ItemVal& b){ return a.val > b.val; });
                for (int k = 0; k < qMin(3, (int)sortedFin.size()); k++) top3Val += sortedFin[k].val;
                double pctTop3 = (totalFinVal > 0) ? (top3Val * 100.0 / totalFinVal) : 0;

                kpiLayout->addWidget(makeKpiCard(
                    "Articles OK (stock > 20)",
                    QString::number(okCount),
                    QString("%1% du total").arg(pctOk, 0, 'f', 1),
                    "#16a34a", "#f0fdf4"));

                kpiLayout->addWidget(makeKpiCard(
                    "Articles Moyens (10-20)",
                    QString::number(moyenCount),
                    QString("%1% du total").arg(pctMoy, 0, 'f', 1),
                    "#d97706", "#fffbeb"));

                kpiLayout->addWidget(makeKpiCard(
                    "Articles Critiques (< 10)",
                    QString::number(critCount),
                    QString("%1% du total").arg(pctCrit, 0, 'f', 1),
                    "#dc2626", "#fef2f2"));

                kpiLayout->addWidget(makeKpiCard(
                    "Concentration Valeur Top 3",
                    QString("%1%").arg(pctTop3, 0, 'f', 1),
                    QString("sur %1 DT total").arg(totalFinVal, 0, 'f', 2),
                    "#7c3aed", "#f5f3ff"));

                // Re-insert rows: KPI at top, then charts
                // Shift existing chart widgets down
                QWidget *w00 = nullptr, *w01 = nullptr, *w10 = nullptr;
                // Fetch existing chart views from grid before rearranging
                if (grid2->itemAtPosition(0,0)) w00 = grid2->itemAtPosition(0,0)->widget();
                if (grid2->itemAtPosition(0,1)) w01 = grid2->itemAtPosition(0,1)->widget();
                if (grid2->itemAtPosition(1,0)) w10 = grid2->itemAtPosition(1,0)->widget();
                // Remove to re-add
                if (w00) grid2->removeWidget(w00);
                if (w01) grid2->removeWidget(w01);
                if (w10) grid2->removeWidget(w10);
                // Re-add: row 0 = KPI cards, row 1 = status+seuil charts, row 2 = financial chart
                grid2->addWidget(kpiRow, 0, 0, 1, 2);
                if (w00) grid2->addWidget(w00, 1, 0);
                if (w01) grid2->addWidget(w01, 1, 1);
                if (w10) grid2->addWidget(w10, 2, 0, 1, 2);
            }

            // 1. Status Chart (pie - already has % on slices)
            QPieSeries *pieSeries = new QPieSeries();
            pieSeries->setHoleSize(0.35);
            if(okCount>0) {
                auto *s = pieSeries->append(QString("OK (%1)").arg(okCount), okCount);
                if (s) s->setBrush(QColor("#22c55e"));
            }
            if(moyenCount>0) {
                auto *s = pieSeries->append(QString("MOYEN (%1)").arg(moyenCount), moyenCount);
                if (s) s->setBrush(QColor("#facc15"));
            }
            if(critCount>0) {
                auto *s = pieSeries->append(QString("CRITIQUE (%1)").arg(critCount), critCount);
                if (s) {
                    s->setBrush(QColor("#ef4444"));
                    s->setExploded(true);
                }
            }
            for (auto *slice : pieSeries->slices()) {
                if (slice->value() > 0) {
                    double pct = slice->percentage() * 100.0;
                    slice->setLabel(QString("%1\\n%2%").arg(slice->label()).arg(pct, 0, 'f', 1));
                    slice->setLabelVisible(true);
                }
            }
            QChart *chartStatus = new QChart();
            chartStatus->addSeries(pieSeries);
            chartStatus->setTitle(QString("Sante Globale du Stock (%1 articles)").arg(totalItems));
            chartStatus->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
            chartStatus->legend()->setAlignment(Qt::AlignBottom);
            cvStatus->setChart(chartStatus);

            // 2. Seuil Chart — bar chart with % vs seuil in bar set label
            // Build a second QBarSet with the % fill ratio
            QBarSet *setPct = new QBarSet("% vs Seuil");
            setPct->setColor(QColor("#34d399"));
            for (double r : seuilRatios) {
                *setPct << qMin(r, 200.0); // cap at 200% to avoid distortion
            }
            // Use a separate series for % ratio so it appears alongside
            QBarSeries *pctSeries = new QBarSeries();
            pctSeries->append(setPct);

            setStock->setColor(QColor("#3b82f6"));
            setSeuil->setColor(QColor("#ef4444"));
            seuilSeries->append(setStock);
            seuilSeries->append(setSeuil);

            QChart *chartSeuil = new QChart();
            chartSeuil->addSeries(seuilSeries);
            chartSeuil->setTitle("Quantite vs Seuil Critique - Top 10 (barres bleues = stock, rouges = seuil)");
            chartSeuil->setTitleFont(QFont("Segoe UI", 11, QFont::Bold));
            QBarCategoryAxis *axisXSeuil = new QBarCategoryAxis();
            axisXSeuil->append(seuilCategories);
            chartSeuil->addAxis(axisXSeuil, Qt::AlignBottom);
            seuilSeries->attachAxis(axisXSeuil);
            QValueAxis *axisYSeuil = new QValueAxis();
            axisYSeuil->setTitleText("Unites");
            chartSeuil->addAxis(axisYSeuil, Qt::AlignLeft);
            seuilSeries->attachAxis(axisYSeuil);

            // Second Y axis for % ratio
            QValueAxis *axisYPct = new QValueAxis();
            axisYPct->setTitleText("% vs Seuil");
            axisYPct->setLabelFormat("%.0f%%");
            axisYPct->setRange(0, 250);
            chartSeuil->addSeries(pctSeries);
            chartSeuil->addAxis(axisYPct, Qt::AlignRight);
            pctSeries->attachAxis(axisXSeuil);
            pctSeries->attachAxis(axisYPct);

            chartSeuil->legend()->setAlignment(Qt::AlignBottom);
            cvSeuil->setChart(chartSeuil);

            // 3. Financial Chart — bar chart with % of total value labeled
            std::sort(financials.begin(), financials.end(), [](const ItemVal& a, const ItemVal& b){ return a.val > b.val; });
            for(int i=0; i<qMin(10, (int)financials.size()); i++) {
                finCategories << financials[i].name;
                *setFin << financials[i].val;
            }
            finSeries->append(setFin);
            setFin->setColor(QColor("#8b5cf6"));

            // Percentage of total value per item (bar set for % on right axis)
            QBarSet *setFinPct = new QBarSet("% Valeur Totale");
            setFinPct->setColor(QColor("#f59e0b"));
            QBarSeries *finPctSeries = new QBarSeries();
            for(int i=0; i<qMin(10, (int)financials.size()); i++) {
                double p = (totalFinVal > 0) ? (financials[i].val * 100.0 / totalFinVal) : 0.0;
                *setFinPct << p;
            }
            finPctSeries->append(setFinPct);

            QChart *chartFin = new QChart();
            chartFin->addSeries(finSeries);
            chartFin->setTitle(QString("Analyse de Valeur / ABC - Top 10 (Valeur totale: %1 DT)").arg(totalFinVal, 0, 'f', 2));
            chartFin->setTitleFont(QFont("Segoe UI", 11, QFont::Bold));
            QBarCategoryAxis *axisXFin = new QBarCategoryAxis();
            axisXFin->append(finCategories);
            chartFin->addAxis(axisXFin, Qt::AlignBottom);
            finSeries->attachAxis(axisXFin);
            QValueAxis *axisYFin = new QValueAxis();
            axisYFin->setTitleText("Valeur (DT)");
            chartFin->addAxis(axisYFin, Qt::AlignLeft);
            finSeries->attachAxis(axisYFin);

            // Right axis: % of total value
            QValueAxis *axisYFinPct = new QValueAxis();
            axisYFinPct->setTitleText("% du portefeuille");
            axisYFinPct->setLabelFormat("%.1f%%");
            axisYFinPct->setRange(0, 100);
            chartFin->addSeries(finPctSeries);
            chartFin->addAxis(axisYFinPct, Qt::AlignRight);
            finPctSeries->attachAxis(axisXFin);
            finPctSeries->attachAxis(axisYFinPct);

            chartFin->legend()->setAlignment(Qt::AlignBottom);
            cvFin->setChart(chartFin);'''

if OLD in content:
    content = content.replace(OLD, NEW)
    print("[OK] buildStockStats replaced (LF)")
else:
    crlf = OLD.replace('\n', '\r\n')
    if crlf in content:
        content = content.replace(crlf, NEW.replace('\n', '\r\n'))
        print("[OK] buildStockStats replaced (CRLF)")
    else:
        print("[ERROR] Target block not found")
        exit(1)

with open(filepath, "w", encoding="utf-8") as f:
    f.write(content)

print("[DONE] mainwindow.cpp patched with stats percentages!")
