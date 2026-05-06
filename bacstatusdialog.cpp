#include "bacstatusdialog.h"
#include "mainwindow.h"

#include <QComboBox>
#include <QLabel>
#include <QFrame>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QGraphicsDropShadowEffect>
#include <QFontDatabase>
#include <QApplication>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QImage>
#include <QPixmap>
#include <QUrl>
#include <QScrollArea>

namespace {

constexpr int kCompartmentCount = 4;

// Compartment layout matches the Arduino sketch + classify_service.py:
//   0 = PLASTIC, 1 = METAL, 2 = GLASS, 3 = GENERAL_WASTE.
// The NEMA17 sits at bin 0 (PLASTIC) at rest, so a plastic detection means
// no rotation - the bin is already aligned.
QString compartmentLabel(int index)
{
    switch (index) {
    case 0: return QStringLiteral("Compartiment 1 - Plastique");
    case 1: return QStringLiteral("Compartiment 2 - Metal");
    case 2: return QStringLiteral("Compartiment 3 - Verre");
    case 3: return QStringLiteral("Compartiment 4 - Dechet general");
    }
    return QStringLiteral("Compartiment %1").arg(index + 1);
}

QString compartmentIcon(int index)
{
    static const char *icons[] = { "♻", "🥫", "🍶", "🗑" };
    if (index >= 0 && index < 4) return QString::fromUtf8(icons[index]);
    return QStringLiteral("•");
}

QString compartmentAccent(int index)
{
    switch (index) {
    case 0: return QStringLiteral("#3b82f6"); // plastique - bleu
    case 1: return QStringLiteral("#64748b"); // metal - acier
    case 2: return QStringLiteral("#10b981"); // verre - vert
    case 3: return QStringLiteral("#8b5cf6"); // dechet general - violet
    }
    return QStringLiteral("#64748b");
}

QString fillStatusText(int pct)
{
    if (pct >= 90) return QStringLiteral("Plein - Vidage requis");
    if (pct >= 70) return QStringLiteral("Presque plein");
    if (pct >= 30) return QStringLiteral("Niveau normal");
    if (pct > 0)   return QStringLiteral("Faible remplissage");
    return QStringLiteral("Vide");
}

QString fillStatusColor(int pct)
{
    if (pct >= 90) return QStringLiteral("#dc2626");
    if (pct >= 70) return QStringLiteral("#f59e0b");
    if (pct >= 30) return QStringLiteral("#10b981");
    return QStringLiteral("#64748b");
}

void applyShadow(QWidget *w, int blur = 24, int yOffset = 6, int alpha = 50)
{
    auto *eff = new QGraphicsDropShadowEffect(w);
    eff->setBlurRadius(blur);
    eff->setOffset(0, yOffset);
    eff->setColor(QColor(15, 23, 42, alpha));
    w->setGraphicsEffect(eff);
}

} // namespace

BacStatusDialog::BacStatusDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Etat des Bacs Intelligents"));
    setModal(true);
    resize(960, 680);
    // Reset child widget backgrounds globally so the application's theme
    // stylesheet (which paints QLabel/QProgressBar with light fills) does
    // not leak into our cards as solid rectangles around the text.
    setStyleSheet(QStringLiteral(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "  stop:0 #f8fafc, stop:1 #eef2f7); }"
        "QLabel { background: transparent; border: none; }"
    ));

    buildUi();
    loadBacs();

    // Hook into the live motion state from the Arduino bridge in MainWindow.
    // Walk up the parent chain so this works whether the dialog is opened
    // with the produit page or another widget as parent.
    for (QWidget *w = parent; w && !m_mw; w = w->parentWidget())
        m_mw = qobject_cast<MainWindow *>(w);
    if (!m_mw) {
        const auto topLevels = QApplication::topLevelWidgets();
        for (QWidget *w : topLevels) {
            if ((m_mw = qobject_cast<MainWindow *>(w))) break;
        }
    }
    if (m_mw) {
        m_motionActive = m_mw->isBacMotionActive();
        connect(m_mw, &MainWindow::bacMotionStateChanged,
                this, &BacStatusDialog::onMotionStateChanged);
        connect(m_mw, &MainWindow::wasteClassificationUpdate,
                this, &BacStatusDialog::onClassificationUpdate);
        connect(m_mw, &MainWindow::binFillUpdated,
                this, &BacStatusDialog::onBinFillUpdated);
        connect(m_mw, &MainWindow::machineStateChanged,
                this, &BacStatusDialog::onMachineStateChanged);
        // Make the dialog the single source of truth for the ESP32-CAM URL
        // so classify_service.py hits the same camera the preview is using.
        m_mw->setEsp32CamUrl(m_camUrl);
        // Prewarm the daemon so the model is already loaded by the time
        // the user clicks the trigger button. The dialog will receive
        // "loading" -> "ready" updates via wasteClassificationUpdate.
        QTimer::singleShot(150, m_mw, [this]() {
            if (m_mw) m_mw->ensureClassificationService();
        });
    }

    // Start polling the ESP32-CAM. The first reply will flip the panel
    // from "Connexion..." to a live frame; failures back off silently.
    if (m_camPollTimer) m_camPollTimer->start();
}

BacStatusDialog::~BacStatusDialog()
{
    // Stop directing motion events at any bac when the dialog goes away,
    // and revert the target to EN_ATTENTE if it was held EN_SERVICE.
    if (m_mw) m_mw->setMotionTargetBac(0);

    if (m_camPollTimer) m_camPollTimer->stop();
    if (m_camReply) {
        m_camReply->abort();
        m_camReply->deleteLater();
    }
}

void BacStatusDialog::onMotionStateChanged(bool active)
{
    if (m_motionActive == active) return;
    m_motionActive = active;
    refreshCurrentBac();
}

void BacStatusDialog::buildUi()
{
    // Outer layout: scroll area on top, footer pinned at the bottom so the
    // "Fermer" button stays visible regardless of how much content scrolls.
    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(QStringLiteral(
        "QScrollArea { background: transparent; border: none; }"
        "QScrollBar:vertical { background: transparent; width: 10px; margin: 4px; }"
        "QScrollBar::handle:vertical { background: #cbd5e1; border-radius: 5px; min-height: 30px; }"
        "QScrollBar::handle:vertical:hover { background: #94a3b8; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }"
    ));

    auto *content = new QWidget(scroll);
    content->setStyleSheet("background: transparent;");
    auto *root = new QVBoxLayout(content);
    root->setContentsMargins(24, 24, 24, 24);
    root->setSpacing(18);

    // ----- Header card with gradient -----
    auto *header = new QFrame(content);
    header->setObjectName("bacStatusHeader");
    header->setStyleSheet(QStringLiteral(
        "QFrame#bacStatusHeader {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "    stop:0 #0f2b4c, stop:1 #2a5298);"
        "  border-radius: 18px;"
        "}"
        "QLabel { color: white; background: transparent; border: none; }"
    ));
    applyShadow(header, 30, 8, 80);

    auto *headerLayout = new QVBoxLayout(header);
    headerLayout->setContentsMargins(28, 22, 28, 22);
    headerLayout->setSpacing(6);

    m_lblTitle = new QLabel(QStringLiteral("Surveillance des Bacs"), header);
    QFont titleFont = m_lblTitle->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    m_lblTitle->setFont(titleFont);

    m_lblBacInfo = new QLabel(QStringLiteral("Selectionnez un bac pour consulter son etat en temps reel."), header);
    m_lblBacInfo->setStyleSheet("color: rgba(255,255,255,0.85); font-size: 13px;");

    headerLayout->addWidget(m_lblTitle);
    headerLayout->addWidget(m_lblBacInfo);

    root->addWidget(header);

    // ----- Selector row -----
    auto *selectorCard = new QFrame(this);
    selectorCard->setObjectName("bacSelectorCard");
    selectorCard->setStyleSheet(QStringLiteral(
        "QFrame#bacSelectorCard {"
        "  background: white; border: 1px solid #e2e8f0; border-radius: 14px;"
        "}"
        "QLabel { color: #1f2d3d; font-size: 13px; font-weight: 600;"
        "  background: transparent; border: none; }"
        "QComboBox {"
        "  background: #f8fafc; color: #0f172a;"
        "  border: 1px solid #cbd5e1; border-radius: 10px;"
        "  padding: 10px 14px; min-height: 22px; font-size: 14px;"
        "}"
        "QComboBox:hover { border-color: #2563eb; }"
        "QComboBox::drop-down { border: none; width: 28px; }"
        "QPushButton#bacBtnRefresh {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "    stop:0 #2563eb, stop:1 #3b82f6);"
        "  color: white; font-weight: 700; border: none;"
        "  border-radius: 10px; padding: 10px 18px;"
        "}"
        "QPushButton#bacBtnRefresh:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "    stop:0 #1d4ed8, stop:1 #2563eb);"
        "}"
    ));
    applyShadow(selectorCard, 18, 4, 30);

    auto *selectorLayout = new QHBoxLayout(selectorCard);
    selectorLayout->setContentsMargins(18, 14, 18, 14);
    selectorLayout->setSpacing(12);

    auto *lblPick = new QLabel(QStringLiteral("Bac :"), selectorCard);
    m_cbBac = new QComboBox(selectorCard);
    m_cbBac->setMinimumWidth(360);

    m_btnRefresh = new QPushButton(QStringLiteral("Actualiser"), selectorCard);
    m_btnRefresh->setObjectName("bacBtnRefresh");
    m_btnRefresh->setCursor(Qt::PointingHandCursor);

    selectorLayout->addWidget(lblPick);
    selectorLayout->addWidget(m_cbBac, 1);
    selectorLayout->addWidget(m_btnRefresh);

    root->addWidget(selectorCard);

    // ----- Global status badge card -----
    m_statusBadge = new QFrame(this);
    m_statusBadge->setObjectName("bacStatusBadge");
    m_statusBadge->setMinimumHeight(110);
    applyShadow(m_statusBadge, 22, 6, 40);

    auto *badgeLayout = new QHBoxLayout(m_statusBadge);
    badgeLayout->setContentsMargins(24, 18, 24, 18);
    badgeLayout->setSpacing(18);

    m_statusIcon = new QLabel(m_statusBadge);
    m_statusIcon->setFixedSize(56, 56);
    m_statusIcon->setAlignment(Qt::AlignCenter);
    m_statusIcon->setStyleSheet(
        "background: rgba(255,255,255,0.22); border-radius: 28px;"
        "color: white; font-size: 28px;"
    );

    auto *textCol = new QVBoxLayout();
    textCol->setSpacing(2);
    m_statusText = new QLabel(m_statusBadge);
    QFont stFont = m_statusText->font();
    stFont.setPointSize(16);
    stFont.setBold(true);
    m_statusText->setFont(stFont);

    m_statusHint = new QLabel(m_statusBadge);
    m_statusHint->setStyleSheet("color: rgba(255,255,255,0.9); font-size: 12px;");
    m_statusHint->setWordWrap(true);

    textCol->addWidget(m_statusText);
    textCol->addWidget(m_statusHint);

    badgeLayout->addWidget(m_statusIcon);
    badgeLayout->addLayout(textCol, 1);

    root->addWidget(m_statusBadge);

    // ----- Compartments grid -----
    auto *sectionTitle = new QLabel(QStringLiteral("Etat des Compartiments"), this);
    QFont stitle = sectionTitle->font();
    stitle.setPointSize(13);
    stitle.setBold(true);
    sectionTitle->setFont(stitle);
    sectionTitle->setStyleSheet("color: #1f2d3d; margin-top: 4px;");
    root->addWidget(sectionTitle);

    auto *gridContainer = new QWidget(this);
    auto *grid = new QGridLayout(gridContainer);
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(16);
    grid->setContentsMargins(0, 0, 0, 0);

    m_compartments.resize(kCompartmentCount);
    for (int i = 0; i < kCompartmentCount; ++i) {
        CompartmentCard c;
        c.card = new QFrame(gridContainer);
        c.card->setObjectName(QStringLiteral("compCard_%1").arg(i));
        c.card->setMinimumHeight(150);
        const QString accent = compartmentAccent(i);
        c.card->setStyleSheet(QStringLiteral(
            "QFrame#%1 {"
            "  background: white; border: 1px solid #e2e8f0;"
            "  border-left: 5px solid %2; border-radius: 14px;"
            "}"
        ).arg(c.card->objectName(), accent));
        applyShadow(c.card, 18, 4, 30);

        auto *cl = new QVBoxLayout(c.card);
        cl->setContentsMargins(18, 16, 18, 16);
        cl->setSpacing(8);

        auto *headRow = new QHBoxLayout();
        headRow->setSpacing(10);

        c.iconLabel = new QLabel(compartmentIcon(i), c.card);
        c.iconLabel->setFixedSize(40, 40);
        c.iconLabel->setAlignment(Qt::AlignCenter);
        c.iconLabel->setStyleSheet(QStringLiteral(
            "background: %1; color: white; border-radius: 20px; font-size: 20px;"
        ).arg(accent));

        c.titleLabel = new QLabel(compartmentLabel(i), c.card);
        QFont tf = c.titleLabel->font();
        tf.setBold(true);
        tf.setPointSize(11);
        c.titleLabel->setFont(tf);
        c.titleLabel->setStyleSheet("color: #0f172a;");
        c.titleLabel->setWordWrap(true);

        headRow->addWidget(c.iconLabel);
        headRow->addWidget(c.titleLabel, 1);

        c.fillLabel = new QLabel(QStringLiteral("0 %"), c.card);
        QFont ff = c.fillLabel->font();
        ff.setPointSize(22);
        ff.setBold(true);
        c.fillLabel->setFont(ff);
        c.fillLabel->setStyleSheet(QStringLiteral("color: %1;").arg(accent));

        c.progress = new QProgressBar(c.card);
        c.progress->setRange(0, 100);
        c.progress->setValue(0);
        c.progress->setTextVisible(false);
        c.progress->setFixedHeight(10);
        c.progress->setStyleSheet(QStringLiteral(
            "QProgressBar { background: #eef2f7; border: none; border-radius: 5px; }"
            "QProgressBar::chunk { background: %1; border-radius: 5px; }"
        ).arg(accent));

        c.statusLabel = new QLabel(QStringLiteral("--"), c.card);
        c.statusLabel->setStyleSheet("color: #64748b; font-size: 12px; font-weight: 600;");

        cl->addLayout(headRow);
        cl->addWidget(c.fillLabel);
        cl->addWidget(c.progress);
        cl->addWidget(c.statusLabel);

        grid->addWidget(c.card, i / 2, i % 2);
        m_compartments[i] = c;
    }
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);

    root->addWidget(gridContainer);

    // ----- Live camera + classification panel (ESP32-CAM + NEMA17) -----
    buildCameraPanel(root);

    root->addStretch();

    // Mount the scrollable content into the dialog.
    scroll->setWidget(content);
    outer->addWidget(scroll, 1);

    // ----- Footer pinned to the dialog (outside the scroll area) -----
    auto *footerWrap = new QFrame(this);
    footerWrap->setStyleSheet(
        "background: rgba(255,255,255,0.85); border-top: 1px solid #e2e8f0;"
    );
    auto *footer = new QHBoxLayout(footerWrap);
    footer->setContentsMargins(24, 12, 24, 12);
    footer->addStretch();
    m_btnClose = new QPushButton(QStringLiteral("Fermer"), footerWrap);
    m_btnClose->setCursor(Qt::PointingHandCursor);
    m_btnClose->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  background: #1f2d3d; color: white; font-weight: 700;"
        "  border: none; border-radius: 10px; padding: 10px 22px;"
        "}"
        "QPushButton:hover { background: #334155; }"
    ));
    footer->addWidget(m_btnClose);
    outer->addWidget(footerWrap);

    connect(m_cbBac, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &BacStatusDialog::onBacChanged);
    connect(m_btnRefresh, &QPushButton::clicked,
            this, &BacStatusDialog::onRefreshClicked);
    connect(m_btnClose, &QPushButton::clicked, this, &QDialog::accept);

    // Initial neutral badge
    renderGlobalStatus(QString());
}

void BacStatusDialog::loadBacs()
{
    m_cbBac->blockSignals(true);
    m_cbBac->clear();
    m_cbBac->addItem(QStringLiteral("-- Choisir un bac --"), -1);

    QSqlQuery q;
    q.prepare(QStringLiteral(
        "SELECT ID_BAC, NUM_SERIE, MODELE, REMPLISSAGE, LOCALISATION_STOCK "
        "FROM BAC_INTEL ORDER BY ID_BAC"
    ));
    if (q.exec()) {
        while (q.next()) {
            const int    id       = q.value(0).toInt();
            const QString numSer  = q.value(1).toString();
            const QString modele  = q.value(2).toString();
            const QString display = QStringLiteral("Bac #%1 - %2 (%3)")
                                        .arg(id)
                                        .arg(modele.isEmpty() ? QStringLiteral("Modele inconnu") : modele,
                                             numSer.isEmpty() ? QStringLiteral("S/N --")        : numSer);
            m_cbBac->addItem(display, id);
        }
    }
    m_cbBac->blockSignals(false);

    if (m_cbBac->count() <= 1) {
        m_lblBacInfo->setText(QStringLiteral("Aucun bac disponible dans la base de donnees."));
    } else {
        m_lblBacInfo->setText(QStringLiteral("Selectionnez un bac pour consulter son etat en temps reel."));
    }
}

void BacStatusDialog::onBacChanged(int /*index*/)
{
    refreshCurrentBac();
}

void BacStatusDialog::onRefreshClicked()
{
    loadBacs();
    refreshCurrentBac();
}

void BacStatusDialog::refreshCurrentBac()
{
    const int idBac = m_cbBac->currentData().toInt();
    // Tell MainWindow which bac the PIR pulses should hit. Passing 0
    // means "no bac selected" -> motion is ignored on the C++ side.
    if (m_mw) m_mw->setMotionTargetBac(idBac > 0 ? idBac : 0);

    if (idBac <= 0) {
        renderGlobalStatus(QString());
        // Reset compartments
        for (auto &c : m_compartments) {
            c.fillLabel->setText(QStringLiteral("--"));
            c.progress->setValue(0);
            c.statusLabel->setText(QStringLiteral("Aucun bac selectionne"));
            c.statusLabel->setStyleSheet("color: #94a3b8; font-size: 12px; font-weight: 600;");
        }
        return;
    }

    int globalRemplissage = 0;
    QString numSer, modele, localisation;
    QSqlQuery q;
    q.prepare(QStringLiteral(
        "SELECT NUM_SERIE, MODELE, REMPLISSAGE, LOCALISATION_STOCK "
        "FROM BAC_INTEL WHERE ID_BAC = :id"
    ));
    q.bindValue(":id", idBac);
    if (q.exec() && q.next()) {
        numSer            = q.value(0).toString();
        modele            = q.value(1).toString();
        globalRemplissage = q.value(2).toInt();
        localisation      = q.value(3).toString();
    }

    m_lblBacInfo->setText(QStringLiteral("Bac #%1 - %2 - S/N %3 - %4")
                              .arg(idBac)
                              .arg(modele.isEmpty() ? QStringLiteral("Modele inconnu") : modele)
                              .arg(numSer.isEmpty() ? QStringLiteral("--")            : numSer)
                              .arg(localisation.isEmpty() ? QStringLiteral("Localisation inconnue") : localisation));

    const QString statut = resolveBacStatus(idBac);
    renderGlobalStatus(statut);
    renderCompartments(idBac, globalRemplissage);
}

QString BacStatusDialog::resolveBacStatus(int idBac) const
{
    // Single source of truth: BAC_INTEL.STATUT_BAC. Motion sensor edges
    // already write EN_ATTENTE <-> EN_SERVICE here from MainWindow, so the
    // column always reflects the live state. EN_PANNE and A_VIDER stay
    // untouched by motion.
    QSqlQuery q;
    q.prepare(QStringLiteral("SELECT STATUT_BAC FROM BAC_INTEL WHERE ID_BAC = :id"));
    q.bindValue(":id", idBac);
    if (q.exec() && q.next()) {
        return q.value(0).toString().toUpper();
    }
    return QStringLiteral("EN_ATTENTE");
}

void BacStatusDialog::renderGlobalStatus(const QString &statut)
{
    // Maps BAC_INTEL.STATUT_BAC values to a colored badge.
    QString bg, icon, text, hint;
    if (statut == QLatin1String("EN_PANNE")) {
        bg   = "qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #b91c1c, stop:1 #ef4444)";
        icon = QStringLiteral("✕");
        text = QStringLiteral("EN PANNE");
        hint = QStringLiteral("Bac defectueux - intervention requise.");
    } else if (statut == QLatin1String("A_VIDER")) {
        bg   = "qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #d97706, stop:1 #f59e0b)";
        icon = QStringLiteral("!");
        text = QStringLiteral("A VIDER");
        hint = QStringLiteral("Capacite atteinte - vidage du bac requis.");
    } else if (statut == QLatin1String("EN_SERVICE")) {
        bg   = "qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #047857, stop:1 #10b981)";
        icon = QStringLiteral("✓");
        text = QStringLiteral("EN SERVICE");
        hint = QStringLiteral("Capteur de mouvement actif - bac operationnel (60 s).");
    } else if (statut == QLatin1String("EN_ATTENTE") || statut.isEmpty()) {
        bg   = "qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #475569, stop:1 #64748b)";
        icon = QStringLiteral("⏻");
        text = QStringLiteral("EN ATTENTE");
        hint = QStringLiteral("Aucun mouvement detecte - bac en veille.");
    } else {
        bg   = "qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #475569, stop:1 #64748b)";
        icon = QStringLiteral("?");
        text = QStringLiteral("STATUT INCONNU");
        hint = QStringLiteral("Statut non reconnu : ") + statut;
    }

    m_statusBadge->setStyleSheet(QStringLiteral(
        "QFrame#bacStatusBadge { background: %1; border-radius: 16px; }"
        "QLabel { color: white; background: transparent; border: none; }"
    ).arg(bg));
    m_statusIcon->setText(icon);
    m_statusText->setText(text);
    m_statusHint->setText(hint);
}

QVector<int> BacStatusDialog::provisionalCompartmentFills(int idBac, int globalRemplissage)
{
    // Provisional distribution: deterministic per bac so the values feel stable
    // between refreshes, biased around the global REMPLISSAGE if available.
    // Will be replaced by real per-compartment columns once the schema is extended.
    QVector<int> fills(kCompartmentCount, 0);
    const int base = (globalRemplissage > 0 ? globalRemplissage : 45);
    static const int offsets[kCompartmentCount] = { +12, -8, +20, -15 };
    for (int i = 0; i < kCompartmentCount; ++i) {
        int seedOffset = ((idBac * 7 + i * 13) % 21) - 10; // -10..+10
        int v = base + offsets[i] + seedOffset;
        if (v < 0)   v = 0;
        if (v > 100) v = 100;
        fills[i] = v;
    }
    return fills;
}

void BacStatusDialog::renderCompartments(int idBac, int globalRemplissage)
{
    const QVector<int> fills = provisionalCompartmentFills(idBac, globalRemplissage);
    for (int i = 0; i < m_compartments.size() && i < fills.size(); ++i) {
        const int pct = fills[i];
        auto &c = m_compartments[i];
        c.fillLabel->setText(QStringLiteral("%1 %").arg(pct));
        c.progress->setValue(pct);
        c.statusLabel->setText(fillStatusText(pct));
        c.statusLabel->setStyleSheet(QStringLiteral(
            "color: %1; font-size: 12px; font-weight: 700;"
        ).arg(fillStatusColor(pct)));
    }
}

// ===========================================================================
// Live ESP32-CAM panel + waste classification trigger
// ---------------------------------------------------------------------------
// The ESP32-CAM serves a raw RGB565 frame at /capture.raw (160x120, 2 bytes
// per pixel). We poll it on a QTimer (~5 fps), decode to QImage, and show it
// next to a "Trigger now" button that mirrors the physical pin-5 button.
// On classification, MainWindow runs classify_single.py and emits the
// material via wasteClassificationUpdate(); we mirror it here.
// ===========================================================================

void BacStatusDialog::buildCameraPanel(QVBoxLayout *root)
{
    auto *sectionTitle = new QLabel(QStringLiteral("Camera & Classification"), this);
    QFont stitle = sectionTitle->font();
    stitle.setPointSize(13);
    stitle.setBold(true);
    sectionTitle->setFont(stitle);
    sectionTitle->setStyleSheet("color: #1f2d3d; margin-top: 4px;");
    root->addWidget(sectionTitle);

    m_camCard = new QFrame(this);
    m_camCard->setObjectName("camCard");
    m_camCard->setStyleSheet(QStringLiteral(
        "QFrame#camCard {"
        "  background: white; border: 1px solid #e2e8f0;"
        "  border-radius: 14px;"
        "}"
        "QLabel { background: transparent; border: none; color: #0f172a; }"
        "QPushButton#camBtnTrigger {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "    stop:0 #16a34a, stop:1 #22c55e);"
        "  color: white; font-weight: 700; border: none;"
        "  border-radius: 10px; padding: 12px 20px; font-size: 13px;"
        "}"
        "QPushButton#camBtnTrigger:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "    stop:0 #15803d, stop:1 #16a34a);"
        "}"
        "QPushButton#camBtnTrigger:disabled { background: #94a3b8; }"
    ));
    applyShadow(m_camCard, 18, 4, 30);

    auto *outer = new QHBoxLayout(m_camCard);
    outer->setContentsMargins(18, 16, 18, 16);
    outer->setSpacing(18);

    // Live preview (RGB565 -> QImage). Fixed aspect ratio of 160x120 scaled.
    m_camView = new QLabel(m_camCard);
    m_camView->setFixedSize(320, 240);
    m_camView->setAlignment(Qt::AlignCenter);
    m_camView->setStyleSheet(
        "background: #0f172a; color: #94a3b8;"
        "border-radius: 10px; font-size: 12px;"
    );
    m_camView->setText(QStringLiteral("Connexion ESP32-CAM..."));
    outer->addWidget(m_camView);

    // Right column: status + material + trigger button
    auto *right = new QVBoxLayout();
    right->setSpacing(10);

    auto *lblTitle = new QLabel(QStringLiteral("Detection en temps reel"), m_camCard);
    QFont tf = lblTitle->font();
    tf.setBold(true);
    tf.setPointSize(13);
    lblTitle->setFont(tf);

    m_camStatus = new QLabel(QStringLiteral("Connexion en cours..."), m_camCard);
    m_camStatus->setStyleSheet(
        "background: #f1f5f9; color: #475569;"
        "border-radius: 8px; padding: 8px 12px; font-size: 12px; font-weight: 600;"
    );

    auto *lblMatLabel = new QLabel(QStringLiteral("Dernier materiel detecte :"), m_camCard);
    lblMatLabel->setStyleSheet("color: #475569; font-size: 12px;");

    m_camMaterial = new QLabel(QStringLiteral("--"), m_camCard);
    QFont mf = m_camMaterial->font();
    mf.setBold(true);
    mf.setPointSize(16);
    m_camMaterial->setFont(mf);
    m_camMaterial->setStyleSheet("color: #0f172a;");

    m_btnTrigger = new QPushButton(QStringLiteral("Declencher la classification"), m_camCard);
    m_btnTrigger->setObjectName("camBtnTrigger");
    m_btnTrigger->setCursor(Qt::PointingHandCursor);
    m_btnTrigger->setToolTip(QStringLiteral(
        "Equivaut au bouton physique sur la broche 5.\n"
        "Capture une image, classifie le materiel,\n"
        "puis fait tourner le NEMA17 vers le bac correspondant."));

    right->addWidget(lblTitle);
    right->addWidget(m_camStatus);
    right->addSpacing(4);
    right->addWidget(lblMatLabel);
    right->addWidget(m_camMaterial);
    right->addStretch();
    right->addWidget(m_btnTrigger);

    outer->addLayout(right, 1);

    root->addWidget(m_camCard);

    // ----- Networking + polling -----
    m_camNet = new QNetworkAccessManager(this);
    m_camPollTimer = new QTimer(this);
    m_camPollTimer->setInterval(200); // ~5 fps poll
    connect(m_camPollTimer, &QTimer::timeout,
            this, &BacStatusDialog::onCameraPollTick);
    connect(m_btnTrigger, &QPushButton::clicked,
            this, &BacStatusDialog::onTriggerClassifyClicked);
}

void BacStatusDialog::setCameraStatus(const QString &text, const QString &color)
{
    if (!m_camStatus) return;
    m_camStatus->setText(text);
    m_camStatus->setStyleSheet(QStringLiteral(
        "background: %1; color: white;"
        "border-radius: 8px; padding: 8px 12px; font-size: 12px; font-weight: 700;"
    ).arg(color));
}

void BacStatusDialog::highlightTargetCompartment(const QString &material)
{
    // Map material -> compartment index, matching the Arduino sketch:
    //   0 = PLASTIC, 1 = METAL, 2 = GLASS, 3 = GENERAL_WASTE.
    int idx = -1;
    if      (material == "PLASTIC")       idx = 0;
    else if (material == "METAL")         idx = 1;
    else if (material == "GLASS")         idx = 2;
    else if (material == "GENERAL_WASTE") idx = 3;
    if (idx < 0 || idx >= m_compartments.size()) return;

    // Briefly thicken the border on the target compartment so the user
    // can visually correlate the camera result with the bin that will
    // receive the waste.
    auto &c = m_compartments[idx];
    if (!c.card) return;
    const QString accent = compartmentAccent(idx);
    c.card->setStyleSheet(QStringLiteral(
        "QFrame#%1 {"
        "  background: white; border: 2px solid %2;"
        "  border-left: 8px solid %2; border-radius: 14px;"
        "}"
    ).arg(c.card->objectName(), accent));

    QTimer::singleShot(2500, this, [this, idx]() {
        if (idx >= m_compartments.size()) return;
        auto &cc = m_compartments[idx];
        if (!cc.card) return;
        const QString accent = compartmentAccent(idx);
        cc.card->setStyleSheet(QStringLiteral(
            "QFrame#%1 {"
            "  background: white; border: 1px solid #e2e8f0;"
            "  border-left: 5px solid %2; border-radius: 14px;"
            "}"
        ).arg(cc.card->objectName(), accent));
    });
}

void BacStatusDialog::onCameraPollTick()
{
    // Skip if a request is already in flight - avoids piling up replies
    // when the ESP32-CAM is slow to answer.
    if (m_camReply) return;

    QNetworkRequest req((QUrl(m_camUrl)));
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                     QNetworkRequest::NoLessSafeRedirectPolicy);
    req.setRawHeader("Connection", "close");

    m_camReply = m_camNet->get(req);
    connect(m_camReply, &QNetworkReply::finished,
            this, &BacStatusDialog::onCameraReplyFinished);
}

void BacStatusDialog::onCameraReplyFinished()
{
    if (!m_camReply) return;
    QNetworkReply *r = m_camReply;
    m_camReply = nullptr;
    r->deleteLater();

    if (r->error() != QNetworkReply::NoError) {
        m_camErrorCount++;
        if (m_camOnline || m_camErrorCount == 1) {
            m_camOnline = false;
            m_camView->setText(QStringLiteral("ESP32-CAM hors-ligne\n%1").arg(r->errorString()));
            setCameraStatus(QStringLiteral("Camera hors-ligne"), QStringLiteral("#ef4444"));
        }
        return;
    }

    const QByteArray raw = r->readAll();
    const int expected = m_camWidth * m_camHeight * 2;
    if (raw.size() != expected) {
        m_camErrorCount++;
        m_camView->setText(QStringLiteral("Trame invalide (%1 octets)").arg(raw.size()));
        return;
    }

    // Decode RGB565 (big-endian, as sent by the sketch via ESP byteswap).
    QImage img(m_camWidth, m_camHeight, QImage::Format_RGB888);
    const quint8 *src = reinterpret_cast<const quint8 *>(raw.constData());
    for (int y = 0; y < m_camHeight; ++y) {
        uchar *dst = img.scanLine(y);
        for (int x = 0; x < m_camWidth; ++x) {
            // The sketch sends bytes in network order; high byte first.
            const quint16 hi = src[(y * m_camWidth + x) * 2 + 0];
            const quint16 lo = src[(y * m_camWidth + x) * 2 + 1];
            const quint16 px = (hi << 8) | lo;
            const quint8 r5 = (px >> 11) & 0x1F;
            const quint8 g6 = (px >> 5)  & 0x3F;
            const quint8 b5 =  px        & 0x1F;
            dst[x * 3 + 0] = (r5 << 3) | (r5 >> 2);
            dst[x * 3 + 1] = (g6 << 2) | (g6 >> 4);
            dst[x * 3 + 2] = (b5 << 3) | (b5 >> 2);
        }
    }

    m_camView->setPixmap(QPixmap::fromImage(img).scaled(
        m_camView->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_camErrorCount = 0;
    if (!m_camOnline) {
        m_camOnline = true;
        setCameraStatus(QStringLiteral("Camera en direct"), QStringLiteral("#16a34a"));
    }
}

void BacStatusDialog::onTriggerClassifyClicked()
{
    if (!m_mw) {
        setCameraStatus(QStringLiteral("Bridge MainWindow indisponible"),
                        QStringLiteral("#ef4444"));
        return;
    }
    m_btnTrigger->setEnabled(false);
    m_mw->triggerWasteClassification(QStringLiteral("dialog"));
}

void BacStatusDialog::onClassificationUpdate(const QString &phase, const QString &material)
{
    if (phase == "loading") {
        setCameraStatus(QStringLiteral("Chargement du modele IA..."),
                        QStringLiteral("#0ea5e9"));
        if (m_btnTrigger) {
            m_btnTrigger->setEnabled(false);
            m_btnTrigger->setText(QStringLiteral("Modele en cours de chargement..."));
        }
    } else if (phase == "ready") {
        setCameraStatus(QStringLiteral("Modele IA pret"),
                        QStringLiteral("#16a34a"));
        if (m_btnTrigger) {
            m_btnTrigger->setEnabled(true);
            m_btnTrigger->setText(QStringLiteral("Declencher la classification"));
        }
    } else if (phase == "started") {
        setCameraStatus(QStringLiteral("Analyse IA en cours..."),
                        QStringLiteral("#7c3aed"));
        if (m_btnTrigger) m_btnTrigger->setEnabled(false);
    } else if (phase == "result") {
        setCameraStatus(QStringLiteral("Materiel detecte"),
                        QStringLiteral("#2563eb"));
        if (m_camMaterial) {
            m_camMaterial->setText(material);
            const QString color =
                (material == "PLASTIC")       ? "#3b82f6" :
                (material == "METAL")         ? "#64748b" :
                (material == "GLASS")         ? "#10b981" :
                (material == "GENERAL_WASTE") ? "#8b5cf6" : "#0f172a";
            m_camMaterial->setStyleSheet(QStringLiteral("color: %1;").arg(color));
        }
        highlightTargetCompartment(material);
        if (m_btnTrigger) m_btnTrigger->setEnabled(true);
    } else { // error
        setCameraStatus(QStringLiteral("Erreur IA: %1").arg(material),
                        QStringLiteral("#ef4444"));
        if (m_btnTrigger) m_btnTrigger->setEnabled(true);
    }
}

void BacStatusDialog::onBinFillUpdated(int binIndex, int percent)
{
    // BAC_FILL: arrives after the lid closes. The Arduino has only one
    // ultrasonic sensor for now, so the report applies to whichever
    // compartment the carousel is currently aligned with.
    if (binIndex < 0 || binIndex >= m_compartments.size()) return;
    auto &c = m_compartments[binIndex];
    if (!c.fillLabel || !c.progress || !c.statusLabel) return;

    if (percent < 0) {
        c.fillLabel->setText(QStringLiteral("--"));
        c.statusLabel->setText(QStringLiteral("Capteur HC-SR04: erreur"));
        c.statusLabel->setStyleSheet(
            "color: #ef4444; font-size: 12px; font-weight: 700;");
        return;
    }

    const int pct = qBound(0, percent, 100);
    c.fillLabel->setText(QStringLiteral("%1 %").arg(pct));
    c.progress->setValue(pct);

    QString text;
    QString color;
    if      (pct >= 90) { text = QStringLiteral("Plein - Vidage requis"); color = "#dc2626"; }
    else if (pct >= 70) { text = QStringLiteral("Presque plein");         color = "#f59e0b"; }
    else if (pct >= 30) { text = QStringLiteral("Niveau normal");         color = "#10b981"; }
    else if (pct > 0)   { text = QStringLiteral("Faible remplissage");    color = "#64748b"; }
    else                { text = QStringLiteral("Vide");                  color = "#64748b"; }
    c.statusLabel->setText(QStringLiteral("%1 (mesure live)").arg(text));
    c.statusLabel->setStyleSheet(QStringLiteral(
        "color: %1; font-size: 12px; font-weight: 700;"
    ).arg(color));
}

void BacStatusDialog::onMachineStateChanged(bool on)
{
    setCameraStatus(
        on ? QStringLiteral("Machine ALLUMEE (bouton physique)")
           : QStringLiteral("Machine eteinte"),
        on ? QStringLiteral("#16a34a") : QStringLiteral("#64748b"));
}
