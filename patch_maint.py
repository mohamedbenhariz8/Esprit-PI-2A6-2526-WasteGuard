with open('mainwindow.cpp', 'rb') as f:
    content = f.read().decode('utf-8', errors='replace')

# 1. Replace single maintenance row with 9 realistic interventions
old_maint = (
    '        // Add example row\r\n'
    '\r\n'
    '        int row = table->rowCount();\r\n'
    '\r\n'
    '        table->insertRow(row);\r\n'
    '\r\n'
    '        table->setItem(row, 0, new QTableWidgetItem("INT-001"));\r\n'
    '\r\n'
    '        table->setItem(row, 1, new QTableWidgetItem("2024-02-11"));\r\n'
    '\r\n'
    '        table->setItem(row, 2, new QTableWidgetItem("Ahmed Ali"));\r\n'
    '\r\n'
    '        table->setItem(row, 3, new QTableWidgetItem("150.0 DT"));\r\n'
    '\r\n'
    '        table->setItem(row, 4, new QTableWidgetItem("2h"));\r\n'
    '\r\n'
    '        table->setItem(row, 5, new QTableWidgetItem("Normale"));\r\n'
    '\r\n'
    '        \r\n'
    '\r\n'
    '        refreshMaintActionButtons();'
)

new_maint = (
    '        // Maintenance interventions data\r\n'
    '        struct MaintData { QString ref, date, tech, cout, duree, priorite; };\r\n'
    '        QList<MaintData> items = {\r\n'
    '            {"INT-001", "2024-01-15", "Ahmed Ali",       "150 TND", "2h",   "Normale"},\r\n'
    '            {"INT-002", "2024-01-22", "Sami Jebali",     "320 TND", "5h",   "Urgente"},\r\n'
    '            {"INT-003", "2024-02-03", "Karim Bouazizi",  "85 TND",  "1h30", "Normale"},\r\n'
    '            {"INT-004", "2024-02-10", "Mohamed Trabelsi","500 TND", "8h",   "Critique"},\r\n'
    '            {"INT-005", "2024-02-18", "Ahmed Ali",       "200 TND", "3h",   "Urgente"},\r\n'
    '            {"INT-006", "2024-03-01", "Youssef Hamdi",   "120 TND", "2h",   "Normale"},\r\n'
    '            {"INT-007", "2024-03-12", "Sami Jebali",     "450 TND", "6h",   "Critique"},\r\n'
    '            {"INT-008", "2024-03-20", "Karim Bouazizi",  "75 TND",  "1h",   "Normale"},\r\n'
    '            {"INT-009", "2024-04-05", "Mohamed Trabelsi","280 TND", "4h",   "Urgente"}\r\n'
    '        };\r\n'
    '        for (const auto &m : items) {\r\n'
    '            int row = table->rowCount();\r\n'
    '            table->insertRow(row);\r\n'
    '            table->setItem(row, 0, new QTableWidgetItem(m.ref));\r\n'
    '            table->setItem(row, 1, new QTableWidgetItem(m.date));\r\n'
    '            table->setItem(row, 2, new QTableWidgetItem(m.tech));\r\n'
    '            table->setItem(row, 3, new QTableWidgetItem(m.cout));\r\n'
    '            table->setItem(row, 4, new QTableWidgetItem(m.duree));\r\n'
    '            table->setItem(row, 5, new QTableWidgetItem(m.priorite));\r\n'
    '        }\r\n'
    '        refreshMaintActionButtons();'
)

if old_maint in content:
    content = content.replace(old_maint, new_maint, 1)
    print('MAINT DATA REPLACED')
else:
    idx = content.find('Add example row')
    print('NOT FOUND, context:', repr(content[idx:idx+300]) if idx >= 0 else 'NOT FOUND AT ALL')

# 2. Add setupMaintCardViewContainer() call after setupMaintenanceModule()
old_setup = '    setupMaintenanceModule();\r\n\r\n    applyUnifiedTopBarStyle();'
new_setup = '    setupMaintenanceModule();\r\n\r\n    setupMaintCardViewContainer();\r\n\r\n    applyUnifiedTopBarStyle();'

if old_setup in content:
    content = content.replace(old_setup, new_setup, 1)
    print('SETUP CALL ADDED')
else:
    print('SETUP CALL NOT FOUND')

with open('mainwindow.cpp', 'wb') as f:
    f.write(content.encode('utf-8'))
print('DONE')
