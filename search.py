with open('mainwindow.cpp', 'r', encoding='utf-8', errors='surrogateescape') as f:
    lines = f.readlines()
for i, l in enumerate(lines):
    if 'inputFournisseur_mod' in l:
        print(f"{i+1}: {l.strip()}")
