with open('mainwindow.ui', 'r', encoding='utf-8') as f:
    lines = f.readlines()

for i, line in enumerate(lines):
    if 'btnStatistiques' in line:
        # Print 25 lines of context
        snippet = lines[max(0,i-5):i+30]
        print("".join(snippet))
        print("---")
        break
