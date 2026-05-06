#ifndef REPAIRDIALOG_H
#define REPAIRDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QTextEdit>
#include <QProgressBar>
#include <QVBoxLayout>

class RepairDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RepairDialog(const QString &technicianName,
                          int technicianId,
                          QWidget *parent = nullptr);
    ~RepairDialog();

signals:
    void repairSaved();       // Emitted after successful save
    void repairClosed();      // Emitted when dialog closes (save or cancel)

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void updateDescription();

private:
    void setupUi();
    void loadBacs();
    void loadStockItems();
    QString buildPartsDescription() const;

    QString m_technicianName;
    int m_technicianId;

    // UI elements
    QLabel *m_lblTitle;
    QLabel *m_lblStatus;
    QLabel *m_lblTechName;
    QLabel *m_lblDate;
    QComboBox *m_comboBac;
    QLineEdit *m_editReference;
    QTableWidget *m_partsTable;
    QTextEdit *m_editDescription;
    QPushButton *m_btnSave;
    QPushButton *m_btnCancel;
    QProgressBar *m_progressBar;
};

#endif // REPAIRDIALOG_H
