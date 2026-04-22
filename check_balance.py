import re
import sys

path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.cpp'
with open(path, 'r', encoding='utf-8', errors='ignore') as f:
    content = f.read()

# Remove strings (handling escapes and raw strings)
content = re.sub(r'R\"\(.*?\)\"', '""', content, flags=re.DOTALL) # Raw strings
content = re.sub(r'\"([^\\\"]|\\.)*\"', '""', content) # Normal strings

# Remove comments
content = re.sub(r'//.*', '', content)
content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)

lines = content.split('\n')
b_count = 0
p_count = 0
for i, line in enumerate(lines):
    for char in line:
        if char == '{': b_count += 1
        elif char == '}': b_count -= 1
        elif char == '(': p_count += 1
        elif char == ')': p_count -= 1
    
    if b_count < 0:
        print(f'Braces count became negative at line {i+1}: {line.strip()}')
        sys.exit(0)
    if p_count < 0:
        print(f'Parens count became negative at line {i+1}: {line.strip()}')
        sys.exit(0)

print(f'Final Braces count: {b_count}')
print(f'Final Parens count: {p_count}')

if b_count != 0 or p_count != 0:
    print("UNBALANCED FILE!")
