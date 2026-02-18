with open('mainwindow.cpp', 'rb') as f:
    content = f.read().decode('utf-8', errors='replace')

# 1. Replace the single client entry with 9 realistic clients
old_client = (
    '        // Add default example client\r\n'
    '\r\n'
    '        int row = ui->tableWidget_Client->rowCount();\r\n'
    '\r\n'
    '        ui->tableWidget_Client->insertRow(row);\r\n'
    '\r\n'
    '        ui->tableWidget_Client->setItem(row, 0, new QTableWidgetItem("CL-001"));\r\n'
    '\r\n'
    '        ui->tableWidget_Client->setItem(row, 1, new QTableWidgetItem("Jean Dupont"));\r\n'
    '\r\n'
    '        ui->tableWidget_Client->setItem(row, 2, new QTableWidgetItem("jean.dupont@email.com"));\r\n'
    '\r\n'
    '        ui->tableWidget_Client->setItem(row, 3, new QTableWidgetItem("5"));\r\n'
    '\r\n'
    '        ui->tableWidget_Client->setItem(row, 4, new QTableWidgetItem("85"));\r\n'
    '\r\n'
    '        ui->tableWidget_Client->setItem(row, 5, new QTableWidgetItem("Mensuel"));\r\n'
    '\r\n'
    '        addClientActionButtons(row);\r\n'
    '\r\n'
    '    }'
)

new_client = (
    '        // Client data\r\n'
    '        struct ClientData { QString mat, nom, email, bacs, score, paiement; };\r\n'
    '        QList<ClientData> clients = {\r\n'
    '            {"CL-001", "Ahmed Ben Salah",   "ahmed.bensalah@gmail.com",  "5",  "92", "Mensuel"},\r\n'
    '            {"CL-002", "Sarah Khalifa",     "sarah.khalifa@outlook.com", "3",  "78", "Trimestriel"},\r\n'
    '            {"CL-003", "Mohamed Trabelsi",  "m.trabelsi@yahoo.fr",       "8",  "65", "Annuel"},\r\n'
    '            {"CL-004", "Leila Mansouri",    "leila.mansouri@gmail.com",  "2",  "88", "Mensuel"},\r\n'
    '            {"CL-005", "Karim Bouazizi",    "k.bouazizi@hotmail.com",    "12", "55", "Trimestriel"},\r\n'
    '            {"CL-006", "Fatma Chaabane",    "fatma.chaabane@gmail.com",  "6",  "95", "Mensuel"},\r\n'
    '            {"CL-007", "Youssef Hamdi",     "y.hamdi@entreprise.tn",     "4",  "72", "Annuel"},\r\n'
    '            {"CL-008", "Nadia Belhaj",      "nadia.belhaj@gmail.com",    "9",  "81", "Mensuel"},\r\n'
    '            {"CL-009", "Sami Jebali",       "sami.jebali@outlook.com",   "1",  "60", "Trimestriel"}\r\n'
    '        };\r\n'
    '        for (const auto &c : clients) {\r\n'
    '            int row = ui->tableWidget_Client->rowCount();\r\n'
    '            ui->tableWidget_Client->insertRow(row);\r\n'
    '            ui->tableWidget_Client->setItem(row, 0, new QTableWidgetItem(c.mat));\r\n'
    '            ui->tableWidget_Client->setItem(row, 1, new QTableWidgetItem(c.nom));\r\n'
    '            ui->tableWidget_Client->setItem(row, 2, new QTableWidgetItem(c.email));\r\n'
    '            ui->tableWidget_Client->setItem(row, 3, new QTableWidgetItem(c.bacs));\r\n'
    '            ui->tableWidget_Client->setItem(row, 4, new QTableWidgetItem(c.score));\r\n'
    '            ui->tableWidget_Client->setItem(row, 5, new QTableWidgetItem(c.paiement));\r\n'
    '            addClientActionButtons(row);\r\n'
    '        }\r\n'
    '    }'
)

if old_client in content:
    content = content.replace(old_client, new_client, 1)
    print('CLIENTS REPLACED')
else:
    idx = content.find('Add default example client')
    print('NOT FOUND, context:', repr(content[idx:idx+200]) if idx >= 0 else 'NOT FOUND AT ALL')

# 2. Add setupClientCardViewContainer() call after setupStockModule()
old_setup = '    setupStockModule();\r\n\r\n    setupMaintenanceModule();'
new_setup = '    setupStockModule();\r\n\r\n    setupClientCardViewContainer();\r\n\r\n    setupMaintenanceModule();'

if old_setup in content:
    content = content.replace(old_setup, new_setup, 1)
    print('SETUP CALL ADDED')
else:
    print('SETUP CALL NOT FOUND')

with open('mainwindow.cpp', 'wb') as f:
    f.write(content.encode('utf-8'))
print('DONE')
