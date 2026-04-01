with open('mainwindow.ui', 'r', encoding='utf-8') as f:
    lines = f.readlines()

for i, line in enumerate(lines):
    if 'statistiques' in line.lower() or 'btnStats' in line or 'stat' in line.lower():
        print(f"{i+1}: {line.rstrip()}")

print("---")
# find stock action buttons area
for i, line in enumerate(lines):
    if 'btnAlerts' in line or 'btnAlert' in line:
        print(f"ALERT {i+1}: {line.rstrip()}")
