import sys
import re

path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.cpp'
with open(path, 'r', encoding='utf-8', errors='ignore') as f:
    content = f.read()

# Fix the encoding in getCmdSortCriteria
content = content.replace('r\\xef\\xbf\\xbd" "f"', '"ref"')
content = content.replace('d\\xc3\\xa9" "croissant"', '"decroissant"')
content = content.replace('d\\xef\\xbf\\xbd" "c"', '"desc"')

# Fix the encoding in checkScheduledDeliveries (if not already fixed)
content = content.replace("STATUT = 'En prparation'", "STATUT LIKE '%PREPARA%'")

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)
print("Cleaned up remaining encoding artifacts")
