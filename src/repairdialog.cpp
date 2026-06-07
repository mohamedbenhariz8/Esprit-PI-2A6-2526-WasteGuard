#include "repairdialog.h"
#include "intervention.h"
#include "thememanager.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDate>
#include <QFrame>
#include <QHBoxLayout>
#include <QFont>
#include <QDebug>

RepairDialog::RepairDialog(const QString &technicianName, int technicianId, QWidget *parent)
    : QDialog(parent),
      m_technicianName(technicianName),
      m_technicianId(technicianId)
{
    setupUi();
    loadBacs();
    loadStockItems();
}

RepairDialog::~RepairDialog()
{
}

void RepairDialog::setupUi()
{
    setWindowTitle(QString::fromUtf8("Mode Reparation \xe2\x80\x94 WasteGuard"));
    resize(780, 640);
    setMinimumSize(680, 540);
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);

    const ThemePalette tp = ThemeManager::instance()->palette();

    setStyleSheet(QString(
        "QDialog { background: %1; color: %2; }"
        "QLabel  { background: transparent; border: none; color: %2; }"
    ).arg(tp.bgApp, tp.textBody));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(28, 24, 28, 20);
    mainLayout->setSpacing(14);

    // ─── Header Row ───
    QHBoxLayout *headerRow = new QHBoxLayout();
    headerRow->setSpacing(12);

    QLabel *titleBadge = new QLabel(QString::fromUtf8("\xF0\x9F\x94\xA7"), this);
    titleBadge->setFixedSize(44, 44);
    titleBadge->setAlignment(Qt::AlignCenter);
    titleBadge->setStyleSheet(QString(
        "background: %1; border-radius: 11px; font-size: 22px; color: %2;"
    ).arg(tp.accent, tp.textTitle));
    headerRow->addWidget(titleBadge);

    QVBoxLayout *titleCol = new QVBoxLayout();
    titleCol->setSpacing(0);
    m_lblTitle = new QLabel(QString::fromUtf8("Mode Reparation"), this);
    m_lblTitle->setStyleSheet(QString(
        "color: %1; font-size: 22px; font-weight: 800; font-family: 'Segoe UI';"
    ).arg(tp.textTitle));
    QLabel *subTitle = new QLabel("WasteGuard  -  Maintenance Bac Intelligent", this);
    subTitle->setStyleSheet(QString("color: %1; font-size: 11px; font-weight: 500; letter-spacing: 1px;").arg(tp.textMuted));
    titleCol->addWidget(m_lblTitle);
    titleCol->addWidget(subTitle);
    headerRow->addLayout(titleCol);
    headerRow->addStretch();

    QLabel *techChip = new QLabel(QString::fromUtf8("\xF0\x9F\x91\xB7 ") + m_technicianName, this);
    techChip->setAlignment(Qt::AlignCenter);
    techChip->setStyleSheet(QString(
        "color: %1; font-size: 12px; font-weight: 700; font-family: 'Consolas';"
        "background: %2; border: 1px solid %3; border-radius: 12px; padding: 6px 14px;"
    ).arg(tp.accent, tp.bgCardAlt, tp.border));
    headerRow->addWidget(techChip);
    mainLayout->addLayout(headerRow);

    // ─── Info Card ───
    QFrame *infoCard = new QFrame(this);
    infoCard->setStyleSheet(QString(
        "QFrame { background: %1; border: 1px solid %2; border-radius: 16px; }"
    ).arg(tp.bgCard, tp.border));
    QHBoxLayout *infoLayout = new QHBoxLayout(infoCard);
    infoLayout->setContentsMargins(18, 14, 18, 14);
    infoLayout->setSpacing(24);

    auto addInfoCol = [&](const QString &label, QWidget *valueWidget) {
        QVBoxLayout *col = new QVBoxLayout();
        QLabel *lbl = new QLabel(label, this);
        lbl->setStyleSheet(QString("color: %1; font-size: 10px;").arg(tp.textMuted));
        col->addWidget(lbl);
        col->addWidget(valueWidget);
        infoLayout->addLayout(col);
    };

    m_lblTechName = new QLabel(m_technicianName, this);
    m_lblTechName->setStyleSheet(QString("color: %1; font-size: 14px; font-weight: bold;").arg(tp.accent));
    addInfoCol("Technicien", m_lblTechName);

    m_lblDate = new QLabel(QDate::currentDate().toString("dd/MM/yyyy"), this);
    m_lblDate->setStyleSheet(QString("color: %1; font-size: 14px; font-weight: bold;").arg(tp.primary));
    addInfoCol("Date", m_lblDate);

    m_editReference = new QLineEdit(this);
    m_editReference->setText("REP-" + QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss"));
    m_editReference->setReadOnly(true);
    m_editReference->setStyleSheet(QString(
        "color: %1; font-size: 12px; font-family: Consolas; background: %2;"
        "border: 1px solid %3; border-radius: 6px; padding: 4px 8px;"
    ).arg(tp.textBody, tp.bgApp, tp.border));
    addInfoCol("Reference", m_editReference);

    m_comboBac = new QComboBox(this);
    m_comboBac->setStyleSheet(QString(
        "QComboBox { color: %1; font-size: 12px; background: %2;"
        "  border: 1px solid %3; border-radius: 6px; padding: 4px 8px; }"
        "QComboBox QAbstractItemView { background: %2; color: %1;"
        "  selection-background-color: %4; selection-color: %5; }"
    ).arg(tp.textBody, tp.bgApp, tp.border, tp.primary, tp.textWhite));
    addInfoCol("Bac Intelligent", m_comboBac);

    mainLayout->addWidget(infoCard);

    // ─── Parts Table ───
    QLabel *partsLabel = new QLabel(QString::fromUtf8("\xF0\x9F\x93\xA6  Pieces a utiliser depuis le Stock"), this);
    partsLabel->setStyleSheet(QString("color: %1; font-size: 13px; font-weight: 700; padding-top: 4px;").arg(tp.textTitle));
    mainLayout->addWidget(partsLabel);

    m_partsTable = new QTableWidget(this);
    m_partsTable->setColumnCount(4);
    m_partsTable->setHorizontalHeaderLabels({"Nom", "Reference", "Disponible", "Quantite"});
    m_partsTable->horizontalHeader()->setStretchLastSection(true);
    m_partsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_partsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_partsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_partsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_partsTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_partsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_partsTable->verticalHeader()->hide();
    m_partsTable->setStyleSheet(QString(
        "QTableWidget { background: %1; color: %2;"
        "  gridline-color: %3; border: 1px solid %3; border-radius: 12px; font-size: 12px; }"
        "QHeaderView::section { background: %4; color: %5;"
        "  border: 1px solid %3; padding: 6px; font-weight: bold; font-size: 11px; }"
        "QTableWidget::item { padding: 4px; }"
        "QSpinBox { background: %1; color: %6; border: 1px solid %3;"
        "  border-radius: 4px; padding: 2px 4px; font-weight: bold; }"
    ).arg(tp.bgApp, tp.textBody, tp.border, tp.bgCard, tp.primary, tp.accent));
    mainLayout->addWidget(m_partsTable, 1);

    // ─── Description ───
    QLabel *descLabel = new QLabel("Description (auto-generee)", this);
    descLabel->setStyleSheet(QString("color: %1; font-size: 11px;").arg(tp.textMuted));
    mainLayout->addWidget(descLabel);

    m_editDescription = new QTextEdit(this);
    m_editDescription->setMaximumHeight(70);
    m_editDescription->setStyleSheet(QString(
        "QTextEdit { background: %1; color: %2; font-size: 12px;"
        "  border: 1px solid %3; border-radius: 10px; padding: 8px; }"
    ).arg(tp.bgApp, tp.textBody, tp.border));
    mainLayout->addWidget(m_editDescription);

    // ─── Status ───
    m_lblStatus = new QLabel(QString::fromUtf8("Selectionnez les pieces utilisees pour la reparation"), this);
    m_lblStatus->setAlignment(Qt::AlignCenter);
    m_lblStatus->setStyleSheet(QString("color: %1; font-size: 13px; font-weight: 600;").arg(tp.primary));
    mainLayout->addWidget(m_lblStatus);

    // ─── Buttons ───
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);

    m_btnCancel = new QPushButton("Annuler", this);
    m_btnCancel->setStyleSheet(QString(
        "QPushButton { background: %1; color: %2;"
        "  border: 1px solid %3; border-radius: 12px;"
        "  padding: 12px 22px; font-weight: 600; font-size: 13px; }"
        "QPushButton:hover { background: %4; color: %5; border-color: %6; }"
    ).arg(tp.bgCard, tp.textMuted, tp.border, tp.bgCardAlt, tp.textTitle, tp.primary));
    m_btnCancel->setCursor(Qt::PointingHandCursor);
    connect(m_btnCancel, &QPushButton::clicked, this, &RepairDialog::onCancelClicked);

    m_btnSave = new QPushButton(QString::fromUtf8("\xE2\x9C\x85  Enregistrer Reparation"), this);
    m_btnSave->setStyleSheet(QString(
        "QPushButton { background: %1; color: %2; border: none; border-radius: 12px;"
        "  padding: 12px 26px; font-weight: bold; font-size: 14px; }"
        "QPushButton:hover { background: %3; }"
        "QPushButton:disabled { background: %4; color: %5; }"
    ).arg(tp.primary, tp.textWhite, tp.primaryDk, tp.bgCardAlt, tp.textMuted));
    m_btnSave->setCursor(Qt::PointingHandCursor);
    connect(m_btnSave, &QPushButton::clicked, this, &RepairDialog::onSaveClicked);

    btnLayout->addWidget(m_btnCancel);
    btnLayout->addStretch();
    btnLayout->addWidget(m_btnSave);
    mainLayout->addLayout(btnLayout);

    // ─── Fade-in ───
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
    a->setDuration(400);
    a->setStartValue(0);
    a->setEndValue(1);
    a->setEasingCurve(QEasingCurve::OutCubic);
    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void RepairDialog::loadBacs()
{
    m_comboBac->clear();
    m_comboBac->addItem("-- Choisir un bac --", -1);

    QSqlQuery q;
    if (q.exec("SELECT ID_BAC, MODELE, STATUT_BAC FROM BAC_INTEL ORDER BY ID_BAC")) {
        while (q.next()) {
            int id = q.value(0).toInt();
            QString modele = q.value(1).toString().trimmed();
            QString statut = q.value(2).toString().trimmed();
            QString label = QString("Bac #%1 - %2 [%3]").arg(id).arg(modele, statut);
            m_comboBac->addItem(label, id);
        }
    }
}

void RepairDialog::loadStockItems()
{
    m_partsTable->setRowCount(0);

    QSqlQuery q;
    if (!q.exec("SELECT ID_MP, NOM, REFERENCE, QUANTITE FROM MATIERE_PREMIERE ORDER BY NOM")) {
        qWarning() << "[RepairDialog] Stock query failed:" << q.lastError().text();
        return;
    }

    int row = 0;
    while (q.next()) {
        int idMp = q.value(0).toInt();
        QString nom = q.value(1).toString().trimmed();
        QString ref = q.value(2).toString().trimmed();
        int qty = q.value(3).toInt();

        m_partsTable->insertRow(row);

        // Name
        auto *nameItem = new QTableWidgetItem(nom);
        nameItem->setData(Qt::UserRole, idMp);  // Store ID_MP
        m_partsTable->setItem(row, 0, nameItem);

        // Reference
        m_partsTable->setItem(row, 1, new QTableWidgetItem(ref));

        // Available quantity
        auto *qtyItem = new QTableWidgetItem(QString::number(qty));
        qtyItem->setTextAlignment(Qt::AlignCenter);
        if (qty <= 0) {
            qtyItem->setForeground(QColor("#ef4444"));
        } else if (qty <= 5) {
            qtyItem->setForeground(QColor("#fbbf24"));
        } else {
            qtyItem->setForeground(QColor("#6ee7b7"));
        }
        m_partsTable->setItem(row, 2, qtyItem);

        // Quantity to use (SpinBox)
        QSpinBox *spin = new QSpinBox();
        spin->setRange(0, qMax(0, qty));
        spin->setValue(0);
        spin->setAlignment(Qt::AlignCenter);
        connect(spin, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &RepairDialog::updateDescription);
        m_partsTable->setCellWidget(row, 3, spin);

        row++;
    }
}

QString RepairDialog::buildPartsDescription() const
{
    QStringList parts;
    for (int r = 0; r < m_partsTable->rowCount(); ++r) {
        QSpinBox *spin = qobject_cast<QSpinBox*>(m_partsTable->cellWidget(r, 3));
        if (!spin || spin->value() <= 0) continue;

        QString name = m_partsTable->item(r, 0)->text();
        int used = spin->value();
        parts << QString("%1 (x%2)").arg(name).arg(used);
    }
    if (parts.isEmpty()) return QString();
    return "Pieces utilisees: " + parts.join(", ");
}

void RepairDialog::updateDescription()
{
    QString desc = buildPartsDescription();
    if (desc.isEmpty()) {
        m_editDescription->setPlainText("");
    } else {
        m_editDescription->setPlainText(desc);
    }
}

void RepairDialog::onSaveClicked()
{
    // ─── Validate ───
    int bacId = m_comboBac->currentData().toInt();
    if (bacId <= 0) {
        m_lblStatus->setText(QString::fromUtf8("\xe2\x9a\xa0 Veuillez selectionner un bac."));
        m_lblStatus->setStyleSheet(
            "color: #fca5a5; font-size: 13px; font-weight: bold;"
            "background: transparent; border: none;");
        return;
    }

    // Check at least one part selected
    bool hasParts = false;
    for (int r = 0; r < m_partsTable->rowCount(); ++r) {
        QSpinBox *spin = qobject_cast<QSpinBox*>(m_partsTable->cellWidget(r, 3));
        if (spin && spin->value() > 0) { hasParts = true; break; }
    }
    if (!hasParts) {
        m_lblStatus->setText(QString::fromUtf8("\xe2\x9a\xa0 Selectionnez au moins une piece."));
        m_lblStatus->setStyleSheet(
            "color: #fca5a5; font-size: 13px; font-weight: bold;"
            "background: transparent; border: none;");
        return;
    }

    m_btnSave->setEnabled(false);
    m_lblStatus->setText("Enregistrement en cours...");
    m_lblStatus->setStyleSheet(
        "color: #fbbf24; font-size: 13px; font-weight: 600;"
        "background: transparent; border: none;");

    // ─── Build description ───
    QString description = m_editDescription->toPlainText().trimmed();
    if (description.isEmpty()) {
        description = buildPartsDescription();
    }

    // ─── Create Intervention ───
    Intervention inter;
    inter.setReference(m_editReference->text().trimmed());
    inter.setDateInter(QDate::currentDate());
    inter.setType("Curative");
    inter.setStatut("EN_COURS");
    inter.setPriorite("NORMALE");
    inter.setTechnicien(m_technicianName);
    inter.setIdBac(bacId);
    inter.setDescript(description);
    inter.setDuree(1);
    inter.setCout(1);
    inter.setMainOeuvre(0);

    if (!inter.ajouter()) {
        m_lblStatus->setText(QString::fromUtf8("\xe2\x9d\x8c Erreur: ") + inter.lastError());
        m_lblStatus->setStyleSheet(
            "color: #ef4444; font-size: 13px; font-weight: bold;"
            "background: transparent; border: none;");
        m_btnSave->setEnabled(true);
        return;
    }

    // ─── Deduct stock ───
    QSqlDatabase db = QSqlDatabase::database();
    bool stockOk = true;
    QString stockError;

    for (int r = 0; r < m_partsTable->rowCount(); ++r) {
        QSpinBox *spin = qobject_cast<QSpinBox*>(m_partsTable->cellWidget(r, 3));
        if (!spin || spin->value() <= 0) continue;

        int idMp = m_partsTable->item(r, 0)->data(Qt::UserRole).toInt();
        int used = spin->value();

        QSqlQuery update;
        update.prepare(
            "UPDATE MATIERE_PREMIERE SET QUANTITE = QUANTITE - :used "
            "WHERE ID_MP = :id AND QUANTITE >= :check");
        update.bindValue(":used", used);
        update.bindValue(":id", idMp);
        update.bindValue(":check", used);

        if (!update.exec() || update.numRowsAffected() == 0) {
            stockError = QString("Stock insuffisant pour %1").arg(
                m_partsTable->item(r, 0)->text());
            stockOk = false;
            break;
        }
    }

    if (!stockOk) {
        m_lblStatus->setText(QString::fromUtf8("\xe2\x9a\xa0 Intervention creee mais: ") + stockError);
        m_lblStatus->setStyleSheet(
            "color: #fbbf24; font-size: 13px; font-weight: bold;"
            "background: transparent; border: none;");
        m_btnSave->setEnabled(true);
        return;
    }

    // ─── Success ───
    m_lblStatus->setText(QString::fromUtf8("\xe2\x9c\x85 Reparation enregistree avec succes!"));
    m_lblStatus->setStyleSheet(
        "color: #a3e635; font-size: 14px; font-weight: bold;"
        "background: transparent; border: none;");

    emit repairSaved();

    // Auto-close after 1.5 seconds
    QTimer::singleShot(1500, this, [this]() {
        emit repairClosed();
        accept();
    });
}

void RepairDialog::onCancelClicked()
{
    emit repairClosed();

    // Fade out
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
    a->setDuration(300);
    a->setStartValue(1);
    a->setEndValue(0);
    a->setEasingCurve(QEasingCurve::OutCubic);
    connect(a, &QPropertyAnimation::finished, this, &QDialog::reject);
    a->start(QPropertyAnimation::DeleteWhenStopped);
}
