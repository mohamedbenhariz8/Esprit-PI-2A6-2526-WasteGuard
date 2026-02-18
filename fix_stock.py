with open('mainwindow.cpp', 'rb') as f:
    content = f.read().decode('utf-8', errors='replace')

old_items = (
    '    struct Product { QString ref, nom, stock, seuil, prix, fournisseur; };\r\n'
    '\r\n'
    '    QList<Product> items = {\r\n'
    '\r\n'
    '        {"REF-001", "Capteur Ultrason", "150", "OK", "25 TND", "TechSupply"},\r\n'
    '\r\n'
    '        {"REF-002", "Batterie Lithium", "15", "CRITIQUE", "45 TND", "PowerPack"},\r\n'
    '\r\n'
    '        {"REF-003", "Module GPS", "80", "MOYEN", "30 TND", "GeoTrack"}\r\n'
    '\r\n'
    '    };'
)

new_items = (
    '    struct Product { QString ref, nom, stock, seuil, prix, fournisseur; };\r\n'
    '\r\n'
    '    QList<Product> items = {\r\n'
    '        {"REF-001", "Capteur Ultrason HC-SR04",   "150", "OK",       "25 TND",  "TechSupply"},\r\n'
    '        {"REF-002", "Batterie Lithium 18650",      "15",  "CRITIQUE", "45 TND",  "PowerPack"},\r\n'
    '        {"REF-003", "Module GPS NEO-6M",           "80",  "MOYEN",    "30 TND",  "GeoTrack"},\r\n'
    '        {"REF-004", "Moteur DC 12V",               "60",  "OK",       "55 TND",  "MotorTech"},\r\n'
    '        {"REF-005", "Capteur Infrarouge IR",       "200", "OK",       "12 TND",  "SensorHub"},\r\n'
    '        {"REF-006", "Raspberry Pi 4 Model B",      "8",   "CRITIQUE", "320 TND", "DigiComp"},\r\n'
    '        {"REF-007", "Servo Moteur SG90",           "45",  "MOYEN",    "18 TND",  "RoboTech"},\r\n'
    '        {"REF-008", "Module WiFi ESP8266",         "120", "OK",       "22 TND",  "IoTStore"},\r\n'
    '        {"REF-009", "Capteur Temp DS18B20",        "35",  "MOYEN",    "15 TND",  "TechSupply"}\r\n'
    '    };'
)

if old_items in content:
    content = content.replace(old_items, new_items, 1)
    with open('mainwindow.cpp', 'wb') as f:
        f.write(content.encode('utf-8'))
    print('ITEMS REPLACED')
else:
    # Debug: show what we have
    idx = content.find('struct Product')
    print('NOT FOUND, context:', repr(content[idx:idx+300]))
