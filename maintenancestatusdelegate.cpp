#include "maintenancestatusdelegate.h"
#include <QApplication>
#include <QTableWidget>

MaintenanceStatusDelegate::MaintenanceStatusDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void MaintenanceStatusDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    if (!index.isValid()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    // Get the table widget to access the STATUT column (column 4)
    const QTableWidget *table = qobject_cast<const QTableWidget*>(index.model()->parent());
    if (!table) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    // Get the STATUT value from column 4 of the current row
    const int statutColumn = 4;
    QTableWidgetItem *statutItem = table->item(index.row(), statutColumn);
    QString statut;
    if (statutItem) {
        statut = statutItem->text().trimmed();
    }

    // Determine row background color based on status
    QColor rowColor = getRowColorForStatus(statut);

    // Create a modified option with the background color
    QStyleOptionViewItem modifiedOption = option;
    if (rowColor.isValid()) {
        modifiedOption.backgroundBrush = QBrush(rowColor);
    }

    // Paint the cell with the modified background
    painter->save();
    if (rowColor.isValid()) {
        painter->fillRect(option.rect, rowColor);
    }
    painter->restore();

    // Let the base class handle the rest (text, selection, etc.)
    QStyledItemDelegate::paint(painter, modifiedOption, index);
}

QColor MaintenanceStatusDelegate::getRowColorForStatus(const QString &statut) const
{
    if (statut == "EN_ATTENTE") {
        return QColor(255, 237, 213); // Orange
    } else if (statut == "TERMINEE") {
        return QColor(212, 237, 218); // Green
    } else if (statut == "ANNULEE") {
        return QColor(248, 215, 218); // Red/Pink
    } else if (statut == "EN_COURS") {
        return QColor(219, 234, 254); // Blue
    }
    return QColor(); // Invalid color = no override
}
