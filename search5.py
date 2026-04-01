with open('mainwindow.cpp', 'r', encoding='utf-8', errors='surrogateescape') as f:
    for i, l in enumerate(f):
        if 'btnGoStats_Stock' in l or 'btnSmartCart' in l:
            print(f"{i+1}: {l.strip()}")
