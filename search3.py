with open('mainwindow.cpp', 'r', encoding='utf-8', errors='surrogateescape') as f:
    for i, l in enumerate(f):
        if 'checkStock' in l or 'btnStatistiq' in l.lower() or 'btn_stat' in l.lower():
            print(f"{i+1}: {l.strip()}")
