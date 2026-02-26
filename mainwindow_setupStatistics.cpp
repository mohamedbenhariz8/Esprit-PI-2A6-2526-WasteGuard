void MainWindow::setupStatistics()
{
    // --- GRAPHIQUE 1 : Répartition des interventions par STATUT (Pie Chart) ---
    QPieSeries *statutSeries = new QPieSeries();
    QSqlQuery queryStatut;
    queryStatut.exec("SELECT STATUT, COUNT(*) FROM INTERVENTION GROUP BY STATUT ORDER BY STATUT");
    QStringList pieColors;
    pieColors << "#27ae60" << "#e67e22" << "#c0392b" << "#3498db" << "#9b59b6";
    int colorIdx = 0;
    while (queryStatut.next()) {
        QString statut = queryStatut.value(0).toString();
        int count = queryStatut.value(1).toInt();
        statutSeries->append(statut, count);
    }
    for (auto *slice : statutSeries->slices()) {
        slice->setLabelVisible(true);
        slice->setBrush(QColor(pieColors.at(colorIdx % pieColors.size())));
        slice->setLabelColor(Qt::black);
        colorIdx++;
    }
    if (statutSeries->slices().size() == 0) {
        statutSeries->append("Aucune donnée", 1);
        statutSeries->slices().at(0)->setLabelVisible(true);
        statutSeries->slices().at(0)->setBrush(QColor("#bdc3c7"));
    }

    QChart *statutChart = new QChart();
    statutChart->addSeries(statutSeries);
    statutChart->setTitle("Répartition des Interventions par Statut");
    statutChart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    statutChart->legend()->setAlignment(Qt::AlignBottom);
    statutChart->setAnimationOptions(QChart::SeriesAnimations);

    ui->chartViewAbsence->setChart(statutChart);
    ui->chartViewAbsence->setRenderHint(QPainter::Antialiasing);

    // --- GRAPHIQUE 2 : Durée totale (heures) par TYPE d'intervention (Bar Chart) ---
    QBarSet *setDuree = new QBarSet("Durée totale (h)");
    setDuree->setColor(QColor("#3498db"));
    QStringList typeCategories;
    double maxDuree = 0;

    QSqlQuery queryType;
    queryType.exec("SELECT TYPE, SUM(DUREE) FROM INTERVENTION GROUP BY TYPE ORDER BY TYPE");
    while (queryType.next()) {
        QString type = queryType.value(0).toString();
        double totalDuree = queryType.value(1).toDouble();
        typeCategories << type;
        *setDuree << totalDuree;
        if (totalDuree > maxDuree) maxDuree = totalDuree;
    }
    if (typeCategories.isEmpty()) {
        typeCategories << "Aucune donnée";
        *setDuree << 0;
    }

    QBarSeries *workloadSeries = new QBarSeries();
    workloadSeries->append(setDuree);

    QChart *workloadChart = new QChart();
    workloadChart->addSeries(workloadSeries);
    workloadChart->setTitle("Charge de Travail par Type d'Intervention");
    workloadChart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    workloadChart->setAnimationOptions(QChart::SeriesAnimations);
    workloadChart->legend()->setVisible(false);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(typeCategories);
    workloadChart->addAxis(axisX, Qt::AlignBottom);
    workloadSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, maxDuree > 0 ? maxDuree * 1.2 : 10);
    axisY->setTitleText("Durée totale (heures)");
    workloadChart->addAxis(axisY, Qt::AlignLeft);
    workloadSeries->attachAxis(axisY);

    ui->chartViewWorkload->setChart(workloadChart);
    ui->chartViewWorkload->setRenderHint(QPainter::Antialiasing);

    // --- Remplir le ComboBox dynamiquement depuis les TYPES ---
    ui->cbProjetStats->blockSignals(true);
    ui->cbProjetStats->clear();
    QSqlQuery queryTypes;
    queryTypes.exec("SELECT DISTINCT TYPE FROM INTERVENTION ORDER BY TYPE");
    while (queryTypes.next()) {
        ui->cbProjetStats->addItem(queryTypes.value(0).toString());
    }
    if (ui->cbProjetStats->count() == 0) {
        ui->cbProjetStats->addItem("Aucun type");
    }
    ui->cbProjetStats->blockSignals(false);

    updateTaskChart(ui->cbProjetStats->currentText());
}
