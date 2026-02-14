void MainWindow::setupStatistics()
{
    // --- GRAPHIQUE 1 : Taux d'Absence (Pie Chart) ---
    QPieSeries *absenceSeries = new QPieSeries();
    absenceSeries->append("Présent", 85);
    absenceSeries->append("Absent", 15);

    // Personnalisation des tranches
    QPieSlice *presentSlice = absenceSeries->slices().at(0);
    presentSlice->setLabelVisible(true);
    presentSlice->setBrush(QColor("#27ae60")); // Vert
    presentSlice->setLabelColor(Qt::black);

    QPieSlice *absentSlice = absenceSeries->slices().at(1);
    absentSlice->setLabelVisible(true);
    absentSlice->setExploded(true);
    absentSlice->setBrush(QColor("#c0392b")); // Rouge
    absentSlice->setLabelColor(Qt::black);

    QChart *absenceChart = new QChart();
    absenceChart->addSeries(absenceSeries);
    absenceChart->setTitle("Taux de Présence Global");
    absenceChart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    absenceChart->legend()->setAlignment(Qt::AlignBottom);
    absenceChart->setAnimationOptions(QChart::SeriesAnimations);

    // Assigner à la vue
    ui->chartViewAbsence->setChart(absenceChart);
    ui->chartViewAbsence->setRenderHint(QPainter::Antialiasing);

    // --- GRAPHIQUE 2 : Charge de Travail (Bar Chart) ---
    QBarSet *set0 = new QBarSet("Heures Travaillées");
    *set0 << 40 << 35 << 42 << 38 << 45; // Données fictives pour 5 employés
    set0->setColor(QColor("#3498db")); // Bleu

    QBarSeries *workloadSeries = new QBarSeries();
    workloadSeries->append(set0);

    QChart *workloadChart = new QChart();
    workloadChart->addSeries(workloadSeries);
    workloadChart->setTitle("Charge de Travail par Employé (Semaine)");
    workloadChart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    workloadChart->setAnimationOptions(QChart::SeriesAnimations);
    workloadChart->legend()->setVisible(false); // Pas besoin de légende pour une seule série

    // Axe des Catégories (Noms des employés)
    QStringList categories;
    categories << "Ali" << "Sara" << "Mohamed" << "Rania" << "Karim";
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    workloadChart->addAxis(axisX, Qt::AlignBottom);
    workloadSeries->attachAxis(axisX);

    // Axe des Valeurs (Heures)
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 50); // Max 50 heures
    axisY->setTitleText("Heures / Semaine");
    workloadChart->addAxis(axisY, Qt::AlignLeft);
    workloadSeries->attachAxis(axisY);

    // Assigner à la vue
    ui->chartViewWorkload->setChart(workloadChart);
    ui->chartViewWorkload->setRenderHint(QPainter::Antialiasing);
}
