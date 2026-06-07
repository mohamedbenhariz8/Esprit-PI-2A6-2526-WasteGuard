#ifndef MAINTENANCESTATUSDELEGATE_H
#define MAINTENANCESTATUSDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QTableWidget>

// ─────────────────────────────────────────────────────────────────────────────
// MaintenanceStatusDelegate — Custom delegate for coloring maintenance rows
//
// Colors rows based on STATUT column value:
//   EN_ATTENTE  →  Orange  (RGB: 255, 237, 213)
//   TERMINEE    →  Green   (RGB: 212, 237, 218)
//   ANNULEE     →  Red     (RGB: 248, 215, 218)
//   EN_COURS    →  Blue    (RGB: 219, 234, 254)
// ─────────────────────────────────────────────────────────────────────────────
class MaintenanceStatusDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit MaintenanceStatusDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

private:
    QColor getRowColorForStatus(const QString &statut) const;
};

#endif // MAINTENANCESTATUSDELEGATE_H
