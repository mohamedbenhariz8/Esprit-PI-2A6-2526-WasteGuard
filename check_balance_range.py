import re
import sys

path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.cpp'
with open(path, 'r', encoding='utf-8', errors='ignore') as f:
    content = f.read()

# Remove strings (handling escapes and raw strings)
content_clean = re.sub(r'R\"\(.*?\)\"', '""', content, flags=re.DOTALL) # Raw strings
content_clean = re.sub(r'\"([^\\\"]|\\.)*\"', '""', content_clean) # Normal strings

# Remove comments
content_clean = re.sub(r'//.*', '', content_clean)
content_clean = re.sub(r'/\*.*?\*/', '', content_clean, flags=re.DOTALL)

lines_clean = content_clean.split('\n')
lines_raw = content.split('\n')
b_count = 0
for i, line in enumerate(lines_clean):
    for char in line:
        if char == '{': b_count += 1
        elif char == '}': b_count -= 1
    
    if i+1 >= 10520 and i+1 <= 10620:
        print(f'L{i+1} (B:{b_count}): {lines_raw[i].strip()}')
