import sys

path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.cpp'
with open(path, 'r', encoding='utf-8', errors='ignore') as f:
    content = f.read()

lines = content.split('\n')
p_count = 0
in_string = False
in_char = False
in_line_comment = False
in_block_comment = False
escaped = False

found_start = False
for j, line in enumerate(lines):
    if j+1 == 9798: found_start = True
    if not found_start: continue
    
    for i, char in enumerate(line):
        if escaped: escaped = False; continue
        if char == '\\': escaped = True; continue
        if in_line_comment: continue
        if in_block_comment:
            if char == '*' and i+1 < len(line) and line[i+1] == '/': in_block_comment = False
            continue
        if in_string:
            if char == '"': in_string = False
            continue
        if in_char:
            if char == "'": in_char = False
            continue
        if char == '/' and i+1 < len(line):
            if line[i+1] == '/': in_line_comment = True; continue
            if line[i+1] == '*': in_block_comment = True; continue
        if char == '"': in_string = True; continue
        if char == "'": in_char = True; continue
        
        if char == '(': p_count += 1
        elif char == ')': p_count -= 1
    
    in_line_comment = False
    escaped = False
    
    if p_count < 0:
        print(f'P reached negative at L{j+1}: {line.strip()}')
        # Print a few lines before to see the cause
        for k in range(max(0, j-5), j+1):
            print(f'  L{k+1}: {lines[k].strip()}')
        sys.exit(0)
