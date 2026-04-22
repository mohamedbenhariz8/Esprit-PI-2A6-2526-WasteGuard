
import sys
import os

path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.cpp'
if not os.path.exists(path):
    print("File not found")
    sys.exit(1)

with open(path, 'r', encoding='utf-8', errors='ignore') as f:
    lines = f.readlines()

new_block = [
    '        connect(btnPlanifier, &QPushButton::clicked, this, [this, calendar]() {\n',
    '        if (QWidget *pageAjout = findChild<QWidget*>("pageCmdAjout")) {\n',
    '            QStackedWidget *sw = mainStacked();\n',
    '            if (sw) {\n',
    '                sw->setCurrentWidget(pageAjout);\n',
    '                \n',
    '                QDate selected = calendar->selectedDate();\n',
    '                QDate today = QDate::currentDate();\n',
    '\n',
    '                // 1. Pre-fill Dates (Order Date = Today, Delivery Date = Selected)\n',
    '                if (ui->comboBox_19) ui->comboBox_19->setCurrentText(QString("%1").arg(today.day(), 2, 10, QChar(\'0\')));\n',
    '                if (ui->comboBox_20) ui->comboBox_20->setCurrentText(QString("%1").arg(today.month(), 2, 10, QChar(\'0\')));\n',
    '                if (ui->comboBox_21) ui->comboBox_21->setCurrentText(QString::number(today.year()));\n',
    '\n',
    '                if (ui->comboBox_22) ui->comboBox_22->setCurrentText(QString("%1").arg(selected.day(), 2, 10, QChar(\'0\')));\n',
    '                if (ui->comboBox_23) ui->comboBox_23->setCurrentText(QString("%1").arg(selected.month(), 2, 10, QChar(\'0\')));\n',
    '                if (ui->comboBox_24) ui->comboBox_24->setCurrentText(QString::number(selected.year()));\n',
    '\n',
    '                // 2. Set Status and Priority defaults\n',
    '                if (ui->cb_status_add_4) {\n',
    '                    ui->cb_status_add_4->setCurrentText("EN_PREPARATION");\n',
    '                    ui->cb_status_add_4->hide();\n',
    '                }\n',
    '                if (ui->comboPrioAdd) {\n',
    '                    ui->comboPrioAdd->setCurrentText("NORMALE");\n',
    '                    ui->comboPrioAdd->hide();\n',
    '                }\n',
    '\n',
    '                // 3. Hide Delivery Date fields and all related labels for a simplified "Planning" view\n',
    '                if (ui->comboBox_22) ui->comboBox_22->hide();\n',
    '                if (ui->comboBox_23) ui->comboBox_23->hide();\n',
    '                if (ui->comboBox_24) ui->comboBox_24->hide();\n',
    '                \n',
    '                // Aggressive cleaning of labels and extra fields\n',
    '                for (auto* lbl : pageAjout->findChildren<QLabel*>()) {\n',
    '                    QString t = lbl->text().toLower();\n',
    '                    // Hide labels for priority, status, and delivery\n',
    '                    if (t.contains("priorit") || t.contains("statut") || t.contains("livraison")) {\n',
    '                        lbl->hide();\n',
    '                    }\n',
    '                    // Hide the repeated "Jour", "Mois", "Annee" labels for the delivery date row\n',
    '                    if (t == "jour" || t == "mois" || t == "annee") {\n',
    '                        if (lbl->y() > 320) lbl->hide(); \n',
    '                    }\n',
    '                }\n',
    '\n',
    '                // 4. Trigger starting location automatically\n',
    '                QTimer::singleShot(600, this, [this]() {\n',
    '                    fetchMyPosition();\n',
    '                });\n',
    '            }\n',
    '        }\n',
    '    });\n'
]

# Replacement range: lines 25155 to 25215 (inclusive, 1-indexed)
# In 0-indexed: 25154 to 25215.
lines[25154:25216] = new_block

with open(path, 'w', encoding='utf-8') as f:
    f.writelines(lines)
print("Repair successful")
