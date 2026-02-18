
# Appends the client card view implementation to mainwindow.cpp with proper UTF-8 encoding

code = r"""
// ============================================================
// CLIENT CARD VIEW MODULE
// ============================================================

void MainWindow::setupClientCardViewContainer()
{
    QTimer::singleShot(200, this, [this]() {
        if (findChild<QPushButton*>("client_btnToggleView")) return;

        QTableWidget *t = ui->tableWidget_Client;
        if (!t) return;

        QWidget *pageWidget = t->parentWidget();
        if (!pageWidget) return;

        // Walk up to find the layouts by name
        QHBoxLayout *searchLayout = nullptr;
        QVBoxLayout *tableLayout  = nullptr;
        QWidget *w = pageWidget;
        for (int attempt = 0; attempt < 5 && w; attempt++) {
            searchLayout = w->findChild<QHBoxLayout*>("horizontalLayout_filters");
            tableLayout  = w->findChild<QVBoxLayout*>("verticalLayout_repertoire");
            if (searchLayout && tableLayout) break;
            w = w->parentWidget();
        }
        if (!searchLayout || !tableLayout) return;

        // Toggle button
        QPushButton *btnToggle = new QPushButton(QString::fromUtf8("\xe2\x8a\x9e"));
        btnToggle->setObjectName("client_btnToggleView");
        btnToggle->setFixedSize(40, 40);
        btnToggle->setCursor(Qt::PointingHandCursor);
        btnToggle->setToolTip("Vue cartes / tableau");
        btnToggle->setStyleSheet(
            "QPushButton { background: #7c3aed; color: white; border-radius: 10px;"
            "  font-size: 18px; border: none; font-weight: 700; }"
            "QPushButton:hover { background: #6d28d9; }"
        );
        connect(btnToggle, &QPushButton::clicked, this, &MainWindow::slot_toggleClientView);
        searchLayout->addWidget(btnToggle);

        // Card scroll area
        QScrollArea *cardScroll = new QScrollArea();
        cardScroll->setObjectName("client_cardScrollArea");
        cardScroll->setWidgetResizable(true);
        cardScroll->setFrameShape(QFrame::NoFrame);
        cardScroll->setStyleSheet("background: transparent; border: none;");
        cardScroll->setVisible(false);

        QWidget *cardContainer = new QWidget();
        cardContainer->setObjectName("client_cardContainer");
        cardContainer->setStyleSheet("background: transparent;");
        QGridLayout *gridLayout = new QGridLayout(cardContainer);
        gridLayout->setObjectName("client_cardLayout");
        gridLayout->setSpacing(16);
        gridLayout->setContentsMargins(8, 8, 8, 8);
        cardScroll->setWidget(cardContainer);
        m_clientCardLayout = gridLayout;

        tableLayout->addWidget(cardScroll);

        // Pagination bar
        QWidget *pBar = new QWidget();
        pBar->setObjectName("client_paginationBar");
        pBar->setFixedHeight(60);
        pBar->setVisible(false);
        pBar->setStyleSheet("background: #f8fafc; border-top: 1px solid #e2e8f0; border-radius: 12px;");

        QHBoxLayout *pLayout = new QHBoxLayout(pBar);
        pLayout->setContentsMargins(20, 0, 20, 0);

        QLabel *lblSize = new QLabel("Items par page:");
        lblSize->setStyleSheet("font-size: 13px; color: #64748b; font-weight: 600;");

        QComboBox *cbSize = new QComboBox();
        cbSize->setObjectName("client_pagination_cbSize");
        cbSize->addItems({"4", "6", "12"});
        cbSize->setCurrentText("6");
        cbSize->setFixedWidth(70);
        cbSize->setStyleSheet("background: white; border: 1px solid #cbd5e1; border-radius: 8px; padding: 4px;");
        connect(cbSize, SIGNAL(currentIndexChanged(int)), this, SLOT(on_client_pagination_cbSize_currentIndexChanged(int)));

        QPushButton *btnPrev = new QPushButton(QString::fromUtf8("\xe2\x86\x90") + " Precedent");
        btnPrev->setObjectName("client_pagination_btnPrev");
        btnPrev->setFixedWidth(120);
        btnPrev->setCursor(Qt::PointingHandCursor);
        btnPrev->setEnabled(false);
        btnPrev->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnPrev, &QPushButton::clicked, this, &MainWindow::on_client_pagination_btnPrev_clicked);

        QLabel *lblPage = new QLabel("Page 1 sur 1");
        lblPage->setObjectName("client_pagination_lblPage");
        lblPage->setStyleSheet("font-size: 13px; color: #1e293b; font-weight: 700;");

        QPushButton *btnNext = new QPushButton("Suivant " + QString::fromUtf8("\xe2\x86\x92"));
        btnNext->setObjectName("client_pagination_btnNext");
        btnNext->setFixedWidth(120);
        btnNext->setCursor(Qt::PointingHandCursor);
        btnNext->setStyleSheet("QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 8px; font-weight: 700; color: #475569; padding: 8px; } QPushButton:disabled { color: #cbd5e1; }");
        connect(btnNext, &QPushButton::clicked, this, &MainWindow::on_client_pagination_btnNext_clicked);

        pLayout->addWidget(lblSize);
        pLayout->addWidget(cbSize);
        pLayout->addStretch();
        pLayout->addWidget(btnPrev);
        pLayout->addWidget(lblPage);
        pLayout->addWidget(btnNext);

        tableLayout->addWidget(pBar);
    });
}

void MainWindow::slot_toggleClientView()
{
    m_isClientCardView = !m_isClientCardView;

    if (ui->tableWidget_Client)
        ui->tableWidget_Client->setVisible(!m_isClientCardView);

    QScrollArea *cardScroll = findChild<QScrollArea*>("client_cardScrollArea");
    if (cardScroll) cardScroll->setVisible(m_isClientCardView);

    QWidget *pBar = findChild<QWidget*>("client_paginationBar");
    if (pBar) pBar->setVisible(m_isClientCardView);

    if (m_isClientCardView) {
        if (!m_clientCardLayout) m_clientCardLayout = findChild<QGridLayout*>("client_cardLayout");
        refreshClientCardView();
    }
}

void MainWindow::refreshClientCardView()
{
    if (!m_clientCardLayout) return;
    QTableWidget *t = ui->tableWidget_Client;
    if (!t) return;

    // Clear existing cards
    while (QLayoutItem *item = m_clientCardLayout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    int totalItems = t->rowCount();
    int startIdx = m_clientCurrentPage * m_clientItemsPerPage;
    int endIdx = qMin(startIdx + m_clientItemsPerPage, totalItems);

    int col = 0, row = 0;
    const int cols = 3;
    for (int i = startIdx; i < endIdx; ++i) {
        QWidget *card = createClientCard(i);
        if (card) {
            m_clientCardLayout->addWidget(card, row, col);
            col++;
            if (col >= cols) { col = 0; row++; }
        }
    }

    // Update pagination
    int totalPages = qMax(1, (totalItems + m_clientItemsPerPage - 1) / m_clientItemsPerPage);
    QLabel *lblPage = findChild<QLabel*>("client_pagination_lblPage");
    if (lblPage) lblPage->setText(QString("Page %1 sur %2").arg(m_clientCurrentPage + 1).arg(totalPages));

    QPushButton *btnPrev = findChild<QPushButton*>("client_pagination_btnPrev");
    if (btnPrev) btnPrev->setEnabled(m_clientCurrentPage > 0);

    QPushButton *btnNext = findChild<QPushButton*>("client_pagination_btnNext");
    if (btnNext) btnNext->setEnabled(m_clientCurrentPage < totalPages - 1);
}

QWidget* MainWindow::createClientCard(int row)
{
    QTableWidget *t = ui->tableWidget_Client;
    if (!t || row < 0 || row >= t->rowCount()) return nullptr;

    auto getText = [&](int col) -> QString {
        QTableWidgetItem *item = t->item(row, col);
        return item ? item->text() : QString();
    };

    QString matricule = getText(0);
    QString nom       = getText(1);
    QString email     = getText(2);
    QString bacs      = getText(3);
    QString score     = getText(4);
    QString paiement  = getText(5);

    // Score color
    int scoreVal = score.toInt();
    QString scoreColor = scoreVal >= 80 ? "#16a34a" : (scoreVal >= 60 ? "#d97706" : "#dc2626");
    QString scoreBg    = scoreVal >= 80 ? "#dcfce7" : (scoreVal >= 60 ? "#fef3c7" : "#fee2e2");

    // Paiement color
    QString paiColor = "#2563eb";
    QString paiBg    = "#eff6ff";
    if (paiement == "Trimestriel") { paiColor = "#7c3aed"; paiBg = "#f5f3ff"; }
    else if (paiement == "Annuel") { paiColor = "#0891b2"; paiBg = "#ecfeff"; }

    // Avatar color
    QStringList avatarColors = {"#7c3aed","#2563eb","#0891b2","#059669","#d97706","#dc2626","#0f766e","#9333ea","#c2410c"};
    QString avatarColor = avatarColors[row % avatarColors.size()];

    // Initials (first + last name)
    QStringList parts = nom.split(' ', Qt::SkipEmptyParts);
    QString initials = parts.size() >= 2
        ? QString(parts[0][0]).toUpper() + QString(parts[1][0]).toUpper()
        : (nom.isEmpty() ? "?" : QString(nom[0]).toUpper());

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

    // ── Header: avatar + matricule/name + score pill ───────────────────────
    QHBoxLayout *headerRow = new QHBoxLayout();
    headerRow->setSpacing(12);

    QLabel *avatar = new QLabel(initials);
    avatar->setFixedSize(56, 56);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setStyleSheet(QString(
        "background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 %1,stop:1 %2);"
        "border-radius: 28px; color: white; font-size: 18px; font-weight: 700; border: none;"
    ).arg(avatarColor).arg(avatarColor));
    headerRow->addWidget(avatar);

    QVBoxLayout *nameCol = new QVBoxLayout();
    nameCol->setSpacing(2);
    QLabel *lblMat = new QLabel(matricule);
    lblMat->setStyleSheet("font-size: 11px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *lblNom = new QLabel(nom);
    lblNom->setStyleSheet("font-size: 15px; font-weight: 700; color: #1e293b; background: transparent; border: none;");
    lblNom->setWordWrap(true);
    nameCol->addWidget(lblMat);
    nameCol->addWidget(lblNom);
    headerRow->addLayout(nameCol, 1);

    // Score pill
    QLabel *scorePill = new QLabel(score + "%");
    scorePill->setAlignment(Qt::AlignCenter);
    scorePill->setFixedHeight(28);
    scorePill->setMinimumWidth(52);
    scorePill->setContentsMargins(10, 0, 10, 0);
    scorePill->setStyleSheet(QString(
        "background: %1; color: %2; border-radius: 14px; font-size: 12px; font-weight: 700; border: none;"
    ).arg(scoreBg).arg(scoreColor));
    headerRow->addWidget(scorePill);
    vl->addLayout(headerRow);

    // ── Email ──────────────────────────────────────────────────────────────
    QLabel *emailLbl = new QLabel(email);
    emailLbl->setStyleSheet(
        "font-size: 12px; color: #64748b; background: #f8fafc; border-radius: 8px;"
        "padding: 6px 10px; border: none;"
    );
    emailLbl->setWordWrap(true);
    vl->addWidget(emailLbl);

    // ── Divider ────────────────────────────────────────────────────────────
    QFrame *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setFixedHeight(1);
    divider->setStyleSheet("background: #f1f5f9; border: none;");
    vl->addWidget(divider);

    // ── Stats: bacs + paiement ─────────────────────────────────────────────
    QHBoxLayout *statsRow = new QHBoxLayout();
    statsRow->setSpacing(10);

    // Bacs box
    QWidget *bacsBox = new QWidget();
    bacsBox->setStyleSheet("background: #f8fafc; border-radius: 10px; border: none;");
    QVBoxLayout *bacsL = new QVBoxLayout(bacsBox);
    bacsL->setContentsMargins(12, 8, 12, 8);
    bacsL->setSpacing(3);
    QLabel *bacsLbl = new QLabel("Bacs");
    bacsLbl->setStyleSheet("font-size: 10px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *bacsVal = new QLabel(bacs + " bacs");
    bacsVal->setStyleSheet("font-size: 15px; font-weight: 700; color: #1e293b; background: transparent; border: none;");
    bacsL->addWidget(bacsLbl);
    bacsL->addWidget(bacsVal);
    statsRow->addWidget(bacsBox, 1);

    // Paiement box
    QWidget *paiBox = new QWidget();
    paiBox->setStyleSheet(QString("background: %1; border-radius: 10px; border: none;").arg(paiBg));
    QVBoxLayout *paiL = new QVBoxLayout(paiBox);
    paiL->setContentsMargins(12, 8, 12, 8);
    paiL->setSpacing(3);
    QLabel *paiLbl = new QLabel("Paiement");
    paiLbl->setStyleSheet("font-size: 10px; color: #94a3b8; font-weight: 600; background: transparent; border: none;");
    QLabel *paiVal = new QLabel(paiement);
    paiVal->setStyleSheet(QString("font-size: 13px; font-weight: 700; color: %1; background: transparent; border: none;").arg(paiColor));
    paiL->addWidget(paiLbl);
    paiL->addWidget(paiVal);
    statsRow->addWidget(paiBox, 1);

    vl->addLayout(statsRow);

    // ── Action buttons ─────────────────────────────────────────────────────
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->setSpacing(8);

    QPushButton *btnView = new QPushButton("Voir Profil");
    btnView->setCursor(Qt::PointingHandCursor);
    btnView->setStyleSheet(
        "QPushButton { background: #7c3aed; color: white; border-radius: 10px;"
        "  font-size: 12px; font-weight: 700; padding: 9px 0; border: none; }"
        "QPushButton:hover { background: #6d28d9; }"
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

void MainWindow::on_client_pagination_cbSize_currentIndexChanged(int)
{
    QComboBox *cb = findChild<QComboBox*>("client_pagination_cbSize");
    if (!cb) return;
    m_clientItemsPerPage = cb->currentText().toInt();
    m_clientCurrentPage = 0;
    refreshClientCardView();
}

void MainWindow::on_client_pagination_btnPrev_clicked()
{
    if (m_clientCurrentPage > 0) {
        m_clientCurrentPage--;
        refreshClientCardView();
    }
}

void MainWindow::on_client_pagination_btnNext_clicked()
{
    if (!ui->tableWidget_Client) return;
    int totalItems = ui->tableWidget_Client->rowCount();
    int maxPages = (totalItems + m_clientItemsPerPage - 1) / m_clientItemsPerPage;
    if (m_clientCurrentPage < maxPages - 1) {
        m_clientCurrentPage++;
        refreshClientCardView();
    }
}
"""

with open('mainwindow.cpp', 'ab') as f:
    f.write(code.encode('utf-8'))

print('DONE')
