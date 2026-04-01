with open('mainwindow.h', 'r', encoding='utf-8', errors='surrogateescape') as f:
    for i, l in enumerate(f):
        if 'showStockAlerts' in l or 'checkStockN' in l:
            print(f"{i+1}: {l.strip()}")
