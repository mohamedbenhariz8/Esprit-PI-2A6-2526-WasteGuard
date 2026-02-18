
# Appends the maintenance card view implementation to mainwindow.cpp with proper UTF-8 encoding

code = r"""
// ============================================================
// MAINTENANCE CARD VIEW MODULE
// ============================================================

void MainWindow::setupMaintCardViewContainer()
{
    QTimer::singleShot(200, this, [this]() {
        if (findChild<QPushButton*>("maint_btnToggleView")) return;

        QTableWidget *t = ui->tableMaintenance;
        if (!t) return;

        QWidget *pageWidget = t->parentWidget();
        if (!pageWidget) return;

        // Find the layouts by name
        QHBoxLayout *searchLayout = nullptr;
        QVBoxLayout *tableLayout  = nullptr;
        
        // Walk up to find the layouts
        QWidget *w = pageWidget;
        for (int attempt = 0; attempt < 5 && w; attempt++) {
            searchLayout = w->findChild<QHBoxLayout*>("searchRow");
            tableLayout  = w->findChild<QVBoxLayout*>("tableLayout");
            if (searchLayout && tableLayout) break;
            w = w->parentWidget();
        }
        
        if (!searchLayout || !tableLayout) return;

        // Toggle button
        QPushButton *btnToggle = new QPushButton(QString::fromUtf8("\xe2\x8a\x9e"));
        btnToggle->setObjectName("maint_btnToggleView");
        btnToggle->setFixedSize(40, 40);
        btnToggle->setCursor(Qt::PointingHandCursor);
        btnToggle->setToolTip("Vue cartes / tableau");
        btnToggle->setStyleSheet(
            "QPushButton { background: #0f2b4c; color: white; border-radius: 10px;"
            "  font-size: 18px; border: none; font-weight: 700; }"
            "QPushButton:hover { background: #1a3a5f; }"
        );
        connect(btnToggle, &QPushButton::clicked, this, &MainWindow::slot_toggleMaintView);
        searchLayout->addWidget(btnToggle);

        // Card scroll area
        QScrollArea *cardScroll = new QScrollArea();
        cardScroll->setObjectName("maint_cardScrollArea");
        cardScroll->setWidgetResizable(true);
        cardScroll->setFrameShape(QFrame::NoFrame);
        cardScroll->setStyleSheet("background: transparent; border: none;");
        cardScroll->setVisible(false);

        QWidget *cardContainer = new QWidget();
        cardContainer->setObjectName("maint_cardContainer");
        cardContainer->setStyleSheet("background: transparent;");
        QGridLayout *gridLayout = new QGridLayout(cardContainer);
        gridLayout->setObjectName("maint_cardLayout");
        gridLayout->setSpacing(16);
        gridLayout->setContentsMargins(8, 0, 8, 8);
        cardScroll->setWidget(cardContainer);
        m_maintCardLayout = gridLayout;

        // The table is inside a frame named "card" in the UI
        QFrame *tableFrame = findChild<QFrame*>("card");
        if (tableFrame && tableFrame->layout()) {
            tableFrame->layout()->addWidget(cardScroll);
        } else {
            tableLayout->addWidget(cardScroll);
        }

        // Pagination bar
        QWidget *pBar = new QWidget();
        pBar->setObjectName("maint_paginationBar");
        pBar->setFixedHeight(60);
        pBar->setVisible(false);
        pBar->setStyleSheet("background: #f8fafc; border-top: 1px solid #e2e8f0; border-radius: 12px;");

        QHBoxLayout *pLayout = new QHBoxLayout(pBar);
        pLayout->setContentsMargins(20, 0, 20, 0);

        QLabel *lblSize = new QLabel("Items par page:");
        lblSize->setStyleSheet("font-size: 13px; color: #64748b; font-weight: 600;");

        QComboBox *cbSize = new QComboBox();
        cbSize->setObjectName("maint_pagination_cbSize");
        cbSize->addItems({"4", "6", "12"});
        cbSize->setCurrentText("6");
        cbSize->setFixedWidth(70);
        cbSize->setStyleSheet("background: white; border: 1px solid #cbd5e1; border-radius: 8px; padding: 4px;");
        connect(cbSize, SIGNAL(currentIndexChanged(int)), this, SLOT(on_maint_pagination_cbSize_currentIndexChanged(int)));

        QPushButton *btnPrev = new QPushButton(QString::fromUtf8("\xe2\x86\x90") + " Precedent");
        btnPrev->setObjectName("maint_pagination_btnPrev");
        btnPrev->setFixedWidth(120);
        btnPrev->setCursor(Qt::PointingHandCursor);
        btnPrev->setEnabled(false);
        btnPrev->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnPrev, &QPushButton::clicked, this, &MainWindow::on_maint_pagination_btnPrev_clicked);

        QLabel *lblPage = new QLabel("Page 1 sur 1");
        lblPage->setObjectName("maint_pagination_lblPage");
        lblPage->setStyleSheet("font-size: 13px; color: #1e293b; font-weight: 700;");

        QPushButton *btnNext = new QPushButton("Suivant " + QString::fromUtf8("\xe2\x86\x92"));
        btnNext->setObjectName("maint_pagination_btnNext");
        btnNext->setFixedWidth(120);
        btnNext->setCursor(Qt::PointingHandCursor);
        btnNext->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnNext, &QPushButton::clicked, this, &MainWindow::on_maint_pagination_btnNext_clicked);

        pLayout->addWidget(lblSize);
        pLayout->addWidget(cbSize);
        pLayout->addStretch();
        pLayout->addWidget(btnPrev);
        pLayout->addWidget(lblPage);
        pLayout->addWidget(btnNext);

        if (tableFrame && tableFrame->layout()) {
            tableFrame->layout()->addWidget(pBar);
        } else {
            tableLayout->addWidget(pBar);
        }
    });
}

void MainWindow::slot_toggleMaintView()
{
    m_isMaintCardView = !m_isMaintCardView;

    if (ui->tableMaintenance)
        ui->tableMaintenance->setVisible(!m_isMaintCardView);
        
    // Also hide the title label "LISTE DES INTERVENTIONS" to save space if needed
    QLabel *lblTitle = findChild<QLabel*>("lblCardTitle");
    if (lblTitle) lblTitle->setVisible(!m_isMaintCardView);

    QScrollArea *cardScroll = findChild<QScrollArea*>("maint_cardScrollArea");
    if (cardScroll) cardScroll->setVisible(m_isMaintCardView);

    QWidget *pBar = findChild<QWidget*>("maint_paginationBar");
    if (pBar) pBar->setVisible(m_isMaintCardView);

    if (m_isMaintCardView) {
        if (!m_maintCardLayout) m_maintCardLayout = findChild<QGridLayout*>("maint_cardLayout");
        refreshMaintCardView();
    }
}

void MainWindow::refreshMaintCardView()
{
    if (!m_maintCardLayout) return;
    QTableWidget *t = ui->tableMaintenance;
    if (!t) return;

    // Clear existing cards
    while (QLayoutItem *item = m_maintCardLayout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    int totalItems = t->rowCount();
    int startIdx = m_maintCurrentPage * m_maintItemsPerPage;
    int endIdx = qMin(startIdx + m_maintItemsPerPage, totalItems);

    int col = 0, row = 0;
    const int cols = 3;
    for (int i = startIdx; i < endIdx; ++i) {
        QWidget *card = createMaintCard(i);
        if (card) {
            m_maintCardLayout->addWidget(card, row, col);
            col++;
            if (col >= cols) { col = 0; row++; }
        }
    }

    // Update pagination
    int totalPages = qMax(1, (totalItems + m_maintItemsPerPage - 1) / m_maintItemsPerPage);
    QLabel *lblPage = findChild<QLabel*>("maint_pagination_lblPage");
    if (lblPage) lblPage->setText(QString("Page %1 sur %2").arg(m_maintCurrentPage + 1).arg(totalPages));

    QPushButton *btnPrev = findChild<QPushButton*>("maint_pagination_btnPrev");
    if (btnPrev) btnPrev->setEnabled(m_maintCurrentPage > 0);

    QPushButton *btnNext = findChild<QPushButton*>("maint_pagination_btnNext");
    if (btnNext) btnNext->setEnabled(m_maintCurrentPage < totalPages - 1);
}

QWidget* MainWindow::createMaintCard(int row)
{
    QTableWidget *t = ui->tableMaintenance;
    if (!t || row < 0 || row >= t->rowCount()) return nullptr;

    auto getText = [&](int col) -> QString {
        QTableWidgetItem *item = t->item(row, col);
        return item ? item->text() : QString();
    };

    QString ref      = getText(0);
    QString date     = getText(1);
    QString tech     = getText(2);
    QString cout     = getText(3);
    QString duree    = getText(4);
    QString priorite = getText(5);

    // Priorite color
    QString prioColor = "#16a34a"; // Normale
    QString prioBg    = "#dcfce7";
    if (priorite == "Critique") { prioColor = "#dc2626"; prioBg = "#fee2e2"; }
    else if (priorite == "Urgente") { prioColor = "#d97706"; prioBg = "#fef3c7"; }

    // Avatar color
    QStringList avatarColors = {"#0f2b4c","#2563eb","#7c3aed","#0891b2","#059669","#d97706"};
    QString avatarColor = avatarColors[row % avatarColors.size()];

    // Initials
    QString initials = tech.isEmpty() ? "M" : QString(tech[0]).toUpper();

    // Card widget
    QFrame *card = new QFrame();
    card->setFrameShape(QFrame::StyledPanel);
    card->setMinimumWidth(240);
    card->setMaximumWidth(380);
    card->setStyleSheet("QFrame { background: white; border-radius: 16px; border: 1px solid #e2e8f0; }");
    
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(18);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 25));
    card->setGraphicsEffect(shadow);

    QVBoxLayout *vl = new QVBoxLayout(card);
    vl->setContentsMargins(18, 18, 18, 18);
    vl->setSpacing(12);

    // ── Header: avatar + ref/date + priority pill ──────────────────────────
    QHBoxLayout *headerRow = new QHBoxLayout();
    headerRow->setSpacing(12);

    QLabel *avatar = new QLabel(initials);
    avatar->setFixedSize(50, 50);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setStyleSheet(QString(
        "background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 %1,stop:1 %2);"
        "border-radius: 25px; color: white; font-size: 20px; font-weight: 700; border: none;"
    ).arg(avatarColor).arg(avatarColor));
    headerRow->addWidget(avatar);

    QVBoxLayout *nameCol = new QVBoxLayout();
    nameCol->setSpacing(2);
    QLabel *lblRef = new QLabel(ref);
    lblRef->setStyleSheet("font-size: 11px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *lblDate = new QLabel(date);
    lblDate->setStyleSheet("font-size: 14px; font-weight: 700; color: #1e293b; background: transparent; border: none;");
    nameCol->addWidget(lblRef);
    nameCol->addWidget(lblDate);
    headerRow->addLayout(nameCol, 1);

    // Priority pill
    QLabel *prioPill = new QLabel(priorite);
    prioPill->setAlignment(Qt::AlignCenter);
    prioPill->setFixedHeight(26);
    prioPill->setMinimumWidth(70);
    prioPill->setStyleSheet(QString(
        "background: %1; color: %2; border-radius: 13px; font-size: 11px; font-weight: 700; border: none; padding: 0 8px;"
    ).arg(prioBg).arg(prioColor));
    headerRow->addWidget(prioPill);
    vl->addLayout(headerRow);

    // ── Tech info ──────────────────────────────────────────────────────────
    QLabel *techLbl = new QLabel(QString::fromUtf8("\xf0\x9f\x91\xb7 ") + tech);
    techLbl->setStyleSheet("font-size: 13px; color: #1e293b; font-weight: 600; background: #f8fafc; border-radius: 8px; padding: 6px 10px; border: none;");
    vl->addWidget(techLbl);

    // ── Divider ────────────────────────────────────────────────────────────
    QFrame *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setFixedHeight(1);
    divider->setStyleSheet("background: #f1f5f9; border: none;");
    vl->addWidget(divider);

    // ── Stats: cout + duree ────────────────────────────────────────────────
    QHBoxLayout *statsRow = new QHBoxLayout();
    statsRow->setSpacing(10);

    // Cost box
    QWidget *costBox = new QWidget();
    costBox->setStyleSheet("background: #f0fdf4; border-radius: 10px; border: none;");
    QVBoxLayout *costL = new QVBoxLayout(costBox);
    costL->setContentsMargins(12, 8, 12, 8);
    costL->setSpacing(3);
    QLabel *costLbl = new QLabel("Cout");
    costLbl->setStyleSheet("font-size: 10px; color: #16a34a; font-weight: 600; background: transparent; border: none;");
    QLabel *costVal = new QLabel(cout);
    costVal->setStyleSheet("font-size: 15px; font-weight: 700; color: #1e293b; background: transparent; border: none;");
    costL->addWidget(costLbl);
    costL->addWidget(costVal);
    statsRow->addWidget(costBox, 1);

    // Duration box
    QWidget *durBox = new QWidget();
    durBox->setStyleSheet("background: #f8fafc; border-radius: 10px; border: none;");
    QVBoxLayout *durL = new QVBoxLayout(durBox);
    durL->setContentsMargins(12, 8, 12, 8);
    durL->setSpacing(3);
    QLabel *durLbl = new QLabel("Duree");
    durLbl->setStyleSheet("font-size: 10px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *durVal = new QLabel(duree);
    durVal->setStyleSheet("font-size: 15px; font-weight: 700; color: #1e293b; background: transparent; border: none;");
    durL->addWidget(durLbl);
    durL->addWidget(durVal);
    statsRow->addWidget(durBox, 1);

    vl->addLayout(statsRow);

    // ── Action buttons ─────────────────────────────────────────────────────
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->setSpacing(8);

    QPushButton *btnView = new QPushButton("D" + QString::fromUtf8("\xc3\xa9") + "tails");
    btnView->setCursor(Qt::PointingHandCursor);
    btnView->setStyleSheet(
        "QPushButton { background: #0f2b4c; color: white; border-radius: 10px;"
        "  font-size: 12px; font-weight: 700; padding: 9px 0; border: none; }"
        "QPushButton:hover { background: #1a3a5f; }"
    );

    QPushButton *btnEdit = new QPushButton("Modifier");
    btnEdit->setCursor(Qt::PointingHandCursor);
    btnEdit->setStyleSheet(
        "QPushButton { background: #fff7ed; color: #ea580c; border-radius: 10px;"
        "  font-size: 12px; font-weight: 700; padding: 9px 0; border: 1px solid #fed7aa; }"
        "QPushButton:hover { background: #ffedd5; }"
    );

    btnRow->addWidget(btnView, 1);
    btnRow->addWidget(btnEdit, 1);
    vl->addLayout(btnRow);

    return card;
}

void MainWindow::on_maint_pagination_cbSize_currentIndexChanged(int)
{
    QComboBox *cb = findChild<QComboBox*>("maint_pagination_cbSize");
    if (!cb) return;
    m_maintItemsPerPage = cb->currentText().toInt();
    m_maintCurrentPage = 0;
    refreshMaintCardView();
}

void MainWindow::on_maint_pagination_btnPrev_clicked()
{
    if (m_maintCurrentPage > 0) {
        m_maintCurrentPage--;
        refreshMaintCardView();
    }
}

void MainWindow::on_maint_pagination_btnNext_clicked()
{
    auto* t = maintenanceTable();
    if (!t) return;
    int totalItems = t->rowCount();
    int maxPages = (totalItems + m_maintItemsPerPage - 1) / m_maintItemsPerPage;
    if (m_maintCurrentPage < maxPages - 1) {
        m_maintCurrentPage++;
        refreshMaintCardView();
    }
}
"""

with open('mainwindow.cpp', 'ab') as f:
    f.write(code.encode('utf-8'))

print('DONE')
