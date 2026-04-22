import sys

path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.cpp'
with open(path, 'r', encoding='utf-8', errors='ignore') as f:
    content = f.read()

b_count = 0
p_count = 0
in_string = False
in_char = False
in_line_comment = False
in_block_comment = False
escaped = False

for i, char in enumerate(content):
    if escaped:
        escaped = False
        continue
    
    if char == '\\':
        escaped = True
        continue
    
    if in_line_comment:
        if char == '\n':
            in_line_comment = False
        continue
    
    if in_block_comment:
        if char == '*' and i+1 < len(content) and content[i+1] == '/':
            in_block_comment = False
            # skip the slash in the next iteration would be better but let's just avoid counting it
        continue
    
    if in_string:
        if char == '"':
            in_string = False
        continue
    
    if in_char:
        if char == "'":
            in_char = False
        continue
    
    if char == '/' and i+1 < len(content):
        if content[i+1] == '/':
            in_line_comment = True
            continue
        if content[i+1] == '*':
            in_block_comment = True
            continue
            
    if char == '"':
        in_string = True
        continue
    if char == "'":
        in_char = True
        continue
        
    if char == '{': b_count += 1
    elif char == '}': b_count -= 1
    elif char == '(': p_count += 1
    elif char == ')': p_count -= 1
    
    # Check around the problematic lines
    # We need to know the line number. Let's count newlines.
    # Actually, let's just print the count when we see a newline.

lines = content.split('\n')
# Let's do the same logic but per line to get counts
b_count = 0
p_count = 0
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
        if char == '{': b_count += 1
        elif char == '}': b_count -= 1
        elif char == '(': p_count += 1
        elif char == ')': p_count -= 1
    
    in_line_comment = False
    escaped = False # should not carry across lines unless it's a multi-line string/comment which we handled via state
    
    if j+1 >= 10520 and j+1 <= 10620:
        print(f'L{j+1} (B:{b_count} P:{p_count}): {line.strip()}')

print(f'Final counts: B:{b_count} P:{p_count}')
