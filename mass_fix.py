import re

path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.cpp'
with open(path, 'r', encoding='utf-8', errors='ignore') as f:
    lines = f.readlines()

new_lines = []
for line in lines:
    # Rename actLbl based on context (rough heuristic)
    if 'auto *actLbl = new QLabel' in line:
        if 'stockScrollV' in ''.join(lines[lines.index(line):lines.index(line)+20]):
             line = line.replace('actLbl', 'actLblStock')
        elif 'clientScrollV' in ''.join(lines[lines.index(line):lines.index(line)+20]):
             line = line.replace('actLbl', 'actLblProd')
        elif 'scrollV' in ''.join(lines[lines.index(line):lines.index(line)+20]):
             line = line.replace('actLbl', 'actLblClient')
        elif 'cmdScrollV' in ''.join(lines[lines.index(line):lines.index(line)+20]):
             line = line.replace('actLbl', 'actLblCmd')
    
    # Usage of actLbl
    if 'actLbl->' in line or 'addWidget(actLbl' in line:
        # This is harder. 
        pass
    
    new_lines.append(line)

# Actually, I'll just do a very simple replace for all actLbl with a counter
content = "".join(lines)
def replace_func(match):
    # Get the line number to make it unique
    # This is not perfect but should work for this file
    return f'actLbl_{match.start()}'

# content = re.sub(r'actLbl', replace_func, content) 
# No, that will replace everything.

# I'll just use a simpler approach. 
# Rename EVERY occurrence of actLbl to actLbl_Unique
# If it's used locally, it will work.
content = content.replace('actLbl', 'actLbl_u')

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)
