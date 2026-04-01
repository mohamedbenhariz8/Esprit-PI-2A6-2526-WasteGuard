with open('mainwindow.ui', 'r', encoding='utf-8') as f:
    lines = f.readlines()

# Find the stylesheet rule area mentioning btnGoStats_Stock
for i, line in enumerate(lines):
    if 'btnGoStats_Stock' in line:
        # Print surrounding context
        start = max(0, i-3)
        end = min(len(lines), i+4)
        print(f"Line {i+1}:")
        print("".join(lines[start:end]))
        print("---")
