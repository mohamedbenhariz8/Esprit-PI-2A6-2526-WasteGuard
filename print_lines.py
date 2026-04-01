path = 'mainwindow.cpp'
with open(path, 'r', encoding='utf-8', errors='surrogateescape') as f:
    text = f.read()
    
# Let's count newlines directly
print(f"Length of text: {len(text)}")
lines = text.split('\n')
print(f"Total lines from splitting by \\n: {len(lines)}")

for i, line in enumerate(lines):
    if 'depletionAlerts << QString::fromUtf8' in line:
        print(f"Found match at line {i}: {repr(line)}")
    if 'Actualiser' in line:
        print(f"Found Actualiser at line {i}: {repr(line)}")
