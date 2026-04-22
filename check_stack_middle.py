import sys

path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.cpp'
with open(path, 'r', encoding='utf-8', errors='ignore') as f:
    content = f.read()

lines = content.split('\n')
stack = []
in_string = False
in_char = False
in_line_comment = False
in_block_comment = False
escaped = False

for j, line in enumerate(lines):
    for i, char in enumerate(line):
        if escaped:
            escaped = False
            continue
        if char == '\\':
            escaped = True
            continue
        if in_line_comment: continue
        if in_block_comment:
            if char == '*' and i+1 < len(line) and line[i+1] == '/':
                in_block_comment = False
            continue
        if in_string:
            if char == '"': in_string = False
            continue
        if in_char:
            if char == "'": in_char = False
            continue
        if char == '/' and i+1 < len(line):
            if line[i+1] == '/':
                in_line_comment = True
                continue
            if line[i+1] == '*':
                in_block_comment = True
                continue
        if char == '"':
            in_string = True
            continue
        if char == "'":
            in_char = True
            continue
        if char == '{': stack.append(j+1)
        elif char == '}': 
            if stack: stack.pop()
    
    in_line_comment = False
    escaped = False
    
    if j+1 >= 10450 and j+1 <= 10515:
        print(f'L{j+1} (Stack:{stack}): {line.strip()}')
