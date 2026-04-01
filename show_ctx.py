with open('mainwindow.ui', 'r', encoding='utf-8') as f:
    lines = f.readlines()

# Show global nav area (existing smart cart - to be removed)
print("=== Global nav (to remove) ===")
print("".join(lines[1465:1484]))

# Show stock-page stats button area (where we want to add)
print("=== Stock page stats btn area ===")
print("".join(lines[5325:5355]))
