with open('mainwindow.ui', 'r', encoding='utf-8') as f:
    lines = f.readlines()

# Find all QPushButton definitions and print their objectName and surrounding context
for i, line in enumerate(lines):
    if 'btnStatistiques' in line or 'btnSmartCart' in line or 'btnExportStock' in line or 'btnGoStats' in line:
        print(f"{i+1}: {line.rstrip()}")
