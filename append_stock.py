
# This script appends the corrected stock card view implementation to mainwindow.cpp
# using proper UTF-8 encoding

code = r"""
// ============================================================
// STOCK CARD VIEW MODULE
// ============================================================

void MainWindow::setupStockCardViewContainer()
{
    QTimer::singleShot(200, this, [this]() {
        if (findChild<QPushButton*>("stock_btnToggleView")) return;

        QTableWidget *t = ui->tableWidget;
        if (!t) return;

        QWidget *pageWidget = t->parentWidget();
        if (!pageWidget) return;

        QHBoxLayout *searchLayout = pageWidget->findChild<QHBoxLayout*>("stockSearchSortRow");
        QVBoxLayout *tableLayout  = pageWidget->findChild<QVBoxLayout*>("tableCardLayout");

        if (!searchLayout || !tableLayout) {
            pageWidget = pageWidget->parentWidget();
            if (!pageWidget) return;
            searchLayout = pageWidget->findChild<QHBoxLayout*>("stockSearchSortRow");
            tableLayout  = pageWidget->findChild<QVBoxLayout*>("tableCardLayout");
        }
        if (!searchLayout || !tableLayout) return;

        // Toggle button
        QPushButton *btnToggle = new QPushButton(QString::fromUtf8("\xe2\x8a\x9e"));
        btnToggle->setObjectName("stock_btnToggleView");
        btnToggle->setFixedSize(40, 40);
        btnToggle->setCursor(Qt::PointingHandCursor);
        btnToggle->setToolTip("Vue cartes / tableau");
        btnToggle->setStyleSheet(
            "QPushButton { background: #2563eb; color: white; border-radius: 10px;"
            "  font-size: 18px; border: none; font-weight: 700; }"
            "QPushButton:hover { background: #1d4ed8; }"
        );
        connect(btnToggle, &QPushButton::clicked, this, &MainWindow::slot_toggleStockView);
        searchLayout->addWidget(btnToggle);

        // Card scroll area
        QScrollArea *cardScroll = new QScrollArea();
        cardScroll->setObjectName("stock_cardScrollArea");
        cardScroll->setWidgetResizable(true);
        cardScroll->setFrameShape(QFrame::NoFrame);
        cardScroll->setStyleSheet("background: transparent; border: none;");
        cardScroll->setVisible(false);

        QWidget *cardContainer = new QWidget();
        cardContainer->setObjectName("stock_cardContainer");
        cardContainer->setStyleSheet("background: transparent;");
        QGridLayout *gridLayout = new QGridLayout(cardContainer);
        gridLayout->setObjectName("stock_cardLayout");
        gridLayout->setSpacing(16);
        gridLayout->setContentsMargins(8, 8, 8, 8);
        cardScroll->setWidget(cardContainer);
        m_stockCardLayout = gridLayout;

        tableLayout->addWidget(cardScroll);

        // Pagination bar
        QWidget *pBar = new QWidget();
        pBar->setObjectName("stock_paginationBar");
        pBar->setFixedHeight(60);
        pBar->setVisible(false);
        pBar->setStyleSheet("background: #f8fafc; border-top: 1px solid #e2e8f0; border-radius: 12px;");

        QHBoxLayout *pLayout = new QHBoxLayout(pBar);
        pLayout->setContentsMargins(20, 0, 20, 0);

        QLabel *lblSize = new QLabel("Items par page:");
        lblSize->setStyleSheet("font-size: 13px; color: #64748b; font-weight: 600;");

        QComboBox *cbSize = new QComboBox();
        cbSize->setObjectName("stock_pagination_cbSize");
        cbSize->addItems({"4", "6", "12"});
        cbSize->setCurrentText("6");
        cbSize->setFixedWidth(70);
        cbSize->setStyleSheet("background: white; border: 1px solid #cbd5e1; border-radius: 8px; padding: 4px;");
        connect(cbSize, SIGNAL(currentIndexChanged(int)), this, SLOT(on_stock_pagination_cbSize_currentIndexChanged(int)));

        QPushButton *btnPrev = new QPushButton(QString::fromUtf8("\xe2\x86\x90") + " Precedent");
        btnPrev->setObjectName("stock_pagination_btnPrev");
        btnPrev->setFixedWidth(120);
        btnPrev->setCursor(Qt::PointingHandCursor);
        btnPrev->setEnabled(false);
        btnPrev->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnPrev, &QPushButton::clicked, this, &MainWindow::on_stock_pagination_btnPrev_clicked);

        QLabel *lblPage = new QLabel("Page 1 sur 1");
        lblPage->setObjectName("stock_pagination_lblPage");
        lblPage->setStyleSheet("font-size: 13px; color: #1e293b; font-weight: 700;");

        QPushButton *btnNext = new QPushButton("Suivant " + QString::fromUtf8("\xe2\x86\x92"));
        btnNext->setObjectName("stock_pagination_btnNext");
        btnNext->setFixedWidth(120);
        btnNext->setCursor(Qt::PointingHandCursor);
        btnNext->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnNext, &QPushButton::clicked, this, &MainWindow::on_stock_pagination_btnNext_clicked);

        pLayout->addWidget(lblSize);
        pLayout->addWidget(cbSize);
        pLayout->addStretch();
        pLayout->addWidget(btnPrev);
        pLayout->addWidget(lblPage);
        pLayout->addWidget(btnNext);

        tableLayout->addWidget(pBar);
    });
}

void MainWindow::slot_toggleStockView()
{
    m_isStockCardView = !m_isStockCardView;

    if (ui->tableWidget)
        ui->tableWidget->setVisible(!m_isStockCardView);

    QScrollArea *cardScroll = findChild<QScrollArea*>("stock_cardScrollArea");
    if (cardScroll) cardScroll->setVisible(m_isStockCardView);

    QWidget *pBar = findChild<QWidget*>("stock_paginationBar");
    if (pBar) pBar->setVisible(m_isStockCardView);

    if (m_isStockCardView) {
        if (!m_stockCardLayout) m_stockCardLayout = findChild<QGridLayout*>("stock_cardLayout");
        refreshStockCardView();
    }
}

void MainWindow::refreshStockCardView()
{
    if (!m_stockCardLayout) return;
    QTableWidget *t = ui->tableWidget;
    if (!t) return;

    // Clear existing cards
    while (QLayoutItem *item = m_stockCardLayout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    int totalItems = t->rowCount();
    int startIdx = m_stockCurrentPage * m_stockItemsPerPage;
    int endIdx = qMin(startIdx + m_stockItemsPerPage, totalItems);

    int col = 0, row = 0;
    const int cols = 3;
    for (int i = startIdx; i < endIdx; ++i) {
        QWidget *card = createStockCard(i);
        if (card) {
            m_stockCardLayout->addWidget(card, row, col);
            col++;
            if (col >= cols) { col = 0; row++; }
        }
    }

    // Update pagination
    int totalPages = qMax(1, (totalItems + m_stockItemsPerPage - 1) / m_stockItemsPerPage);
    QLabel *lblPage = findChild<QLabel*>("stock_pagination_lblPage");
    if (lblPage) lblPage->setText(QString("Page %1 sur %2").arg(m_stockCurrentPage + 1).arg(totalPages));

    QPushButton *btnPrev = findChild<QPushButton*>("stock_pagination_btnPrev");
    if (btnPrev) btnPrev->setEnabled(m_stockCurrentPage > 0);

    QPushButton *btnNext = findChild<QPushButton*>("stock_pagination_btnNext");
    if (btnNext) btnNext->setEnabled(m_stockCurrentPage < totalPages - 1);
}

QWidget* MainWindow::createStockCard(int row)
{
    QTableWidget *t = ui->tableWidget;
    if (!t || row < 0 || row >= t->rowCount()) return nullptr;

    auto getText = [&](int col) -> QString {
        QTableWidgetItem *item = t->item(row, col);
        return item ? item->text() : QString();
    };

    QString ref         = getText(0);
    QString nom         = getText(1);
    QString stockQty    = getText(2);
    // stockQty may be stored as int via DisplayRole
    if (stockQty.isEmpty()) {
        QTableWidgetItem *si = t->item(row, 2);
        if (si) stockQty = si->data(Qt::DisplayRole).toString();
    }
    QString prix        = getText(4);
    QString fournisseur = getText(5);

    // Get seuil from badge widget
    QString seuil;
    QWidget *badgeW = t->cellWidget(row, 3);
    if (badgeW) {
        QLabel *lbl = badgeW->findChild<QLabel*>();
        if (lbl) seuil = lbl->text();
    }
    if (seuil.isEmpty()) seuil = getText(3);

    // Seuil colors
    QString seuilColor = "#16a34a";
    QString seuilBg    = "#dcfce7";
    QString seuilText  = seuil.isEmpty() ? "OK" : seuil;
    if (seuil == "CRITIQUE") { seuilColor = "#dc2626"; seuilBg = "#fee2e2"; }
    else if (seuil == "MOYEN") { seuilColor = "#d97706"; seuilBg = "#fef3c7"; }

    // Avatar color based on row
    QStringList avatarColors = {"#2563eb","#7c3aed","#0891b2","#059669","#d97706","#dc2626","#0f766e","#9333ea","#c2410c"};
    QString avatarColor = avatarColors[row % avatarColors.size()];
    QString initials = nom.isEmpty() ? "?" : QString(nom[0]).toUpper();

    // Card widget
    QFrame *card = new QFrame();
    card->setFrameShape(QFrame::StyledPanel);
    card->setMinimumWidth(240);
    card->setMaximumWidth(380);
    card->setStyleSheet(
        "QFrame { background: white; border-radius: 16px; border: 1px solid #e2e8f0; }"
    );
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(18);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 28));
    card->setGraphicsEffect(shadow);

    QVBoxLayout *vl = new QVBoxLayout(card);
    vl->setContentsMargins(18, 18, 18, 18);
    vl->setSpacing(12);

    // ── Header: avatar + ref/name + seuil pill ─────────────────────────────
    QHBoxLayout *headerRow = new QHBoxLayout();
    headerRow->setSpacing(12);

    // Circular avatar
    QLabel *avatar = new QLabel(initials);
    avatar->setFixedSize(52, 52);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setStyleSheet(QString(
        "background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 %1,stop:1 %2);"
        "border-radius: 26px; color: white; font-size: 22px; font-weight: 700;"
    ).arg(avatarColor).arg(avatarColor));
    headerRow->addWidget(avatar);

    QVBoxLayout *nameCol = new QVBoxLayout();
    nameCol->setSpacing(2);
    QLabel *lblRef = new QLabel(ref);
    lblRef->setStyleSheet("font-size: 11px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *lblNom = new QLabel(nom);
    lblNom->setStyleSheet("font-size: 14px; font-weight: 700; color: #1e293b; background: transparent; border: none;");
    lblNom->setWordWrap(true);
    nameCol->addWidget(lblRef);
    nameCol->addWidget(lblNom);
    headerRow->addLayout(nameCol, 1);

    // Seuil pill
    QLabel *seuilPill = new QLabel(seuilText);
    seuilPill->setAlignment(Qt::AlignCenter);
    seuilPill->setFixedHeight(26);
    seuilPill->setMinimumWidth(60);
    seuilPill->setContentsMargins(10, 0, 10, 0);
    seuilPill->setStyleSheet(QString(
        "background: %1; color: %2; border-radius: 13px; font-size: 11px; font-weight: 700; border: none;"
    ).arg(seuilBg).arg(seuilColor));
    headerRow->addWidget(seuilPill);
    vl->addLayout(headerRow);

    // ── Divider ────────────────────────────────────────────────────────────
    QFrame *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setFixedHeight(1);
    divider->setStyleSheet("background: #f1f5f9; border: none;");
    vl->addWidget(divider);

    // ── Stats: stock qty + prix ────────────────────────────────────────────
    QHBoxLayout *statsRow = new QHBoxLayout();
    statsRow->setSpacing(10);

    // Stock stat box
    QWidget *stockBox = new QWidget();
    stockBox->setStyleSheet("background: #f8fafc; border-radius: 10px; border: none;");
    QVBoxLayout *stockBoxL = new QVBoxLayout(stockBox);
    stockBoxL->setContentsMargins(12, 8, 12, 8);
    stockBoxL->setSpacing(3);
    QLabel *stockLbl = new QLabel("Stock");
    stockLbl->setStyleSheet("font-size: 10px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *stockVal = new QLabel(stockQty + " " + QString::fromUtf8("unit\xc3\xa9s"));
    stockVal->setStyleSheet("font-size: 14px; font-weight: 700; color: #1e293b; background: transparent; border: none;");
    stockBoxL->addWidget(stockLbl);
    stockBoxL->addWidget(stockVal);
    statsRow->addWidget(stockBox, 1);

    // Prix stat box
    QWidget *prixBox = new QWidget();
    prixBox->setStyleSheet("background: #eff6ff; border-radius: 10px; border: none;");
    QVBoxLayout *prixBoxL = new QVBoxLayout(prixBox);
    prixBoxL->setContentsMargins(12, 8, 12, 8);
    prixBoxL->setSpacing(3);
    QLabel *prixLbl = new QLabel("Prix unitaire");
    prixLbl->setStyleSheet("font-size: 10px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *prixVal = new QLabel(prix.isEmpty() ? "---" : prix);
    prixVal->setStyleSheet("font-size: 14px; font-weight: 700; color: #2563eb; background: transparent; border: none;");
    prixBoxL->addWidget(prixLbl);
    prixBoxL->addWidget(prixVal);
    statsRow->addWidget(prixBox, 1);

    vl->addLayout(statsRow);

    // ── Fournisseur badge ──────────────────────────────────────────────────
    QLabel *fournBadge = new QLabel("[F] " + (fournisseur.isEmpty() ? "---" : fournisseur));
    fournBadge->setStyleSheet(
        "background: #f0fdf4; color: #15803d; border-radius: 8px; padding: 6px 12px;"
        "font-size: 12px; font-weight: 600; border: none;"
    );
    vl->addWidget(fournBadge);

    // ── Action buttons ─────────────────────────────────────────────────────
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->setSpacing(8);

    QPushButton *btnView = new QPushButton("Voir " + QString::fromUtf8("D\xc3\xa9tails"));
    btnView->setCursor(Qt::PointingHandCursor);
    btnView->setStyleSheet(
        "QPushButton { background: #2563eb; color: white; border-radius: 10px;"
        "  font-size: 12px; font-weight: 700; padding: 9px 0; border: none; }"
        "QPushButton:hover { background: #1d4ed8; }"
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

void MainWindow::on_stock_pagination_cbSize_currentIndexChanged(int)
{
    QComboBox *cb = findChild<QComboBox*>("stock_pagination_cbSize");
    if (!cb) return;
    m_stockItemsPerPage = cb->currentText().toInt();
    m_stockCurrentPage = 0;
    refreshStockCardView();
}

void MainWindow::on_stock_pagination_btnPrev_clicked()
{
    if (m_stockCurrentPage > 0) {
        m_stockCurrentPage--;
        refreshStockCardView();
    }
}

void MainWindow::on_stock_pagination_btnNext_clicked()
{
    if (!ui->tableWidget) return;
    int totalItems = ui->tableWidget->rowCount();
    int maxPages = (totalItems + m_stockItemsPerPage - 1) / m_stockItemsPerPage;
    if (m_stockCurrentPage < maxPages - 1) {
        m_stockCurrentPage++;
        refreshStockCardView();
    }
}
"""

with open('mainwindow.cpp', 'ab') as f:
    f.write(code.encode('utf-8'))

print('DONE')
