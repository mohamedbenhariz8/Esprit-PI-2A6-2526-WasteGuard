import re

with open('mainwindow.ui', 'r', encoding='utf-8') as f:
    lines = f.readlines()

for i, line in enumerate(lines):
    if 'inputFournisseur_add' in line:
        print(f"--- ADD START (around {i}) ---")
        print("".join(lines[max(0, i-15):i+20]))
        break

for i, line in enumerate(lines):
    if 'inputFournisseur_mod' in line:
        print(f"--- MOD START (around {i}) ---")
        print("".join(lines[max(0, i-15):i+20]))
        break
