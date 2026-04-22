import sys
import re

ui_path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.ui'
with open(ui_path, 'r', encoding='utf-8', errors='ignore') as f:
    ui_content = f.read()

# Look for buttons with text containing LOC or Position
pattern = r'<widget\s+class="QPushButton"\s+name="(.*?)">.*?<property\s+name="text">\s*<string>(.*?)</string>'
matches = re.findall(pattern, ui_content, re.DOTALL)

for name, text in matches:
    t_upper = text.upper()
    if 'LOC' in t_upper or 'POSITION' in t_upper:
        print(f'Button Name: {name}, Text: {text}')

# Also look for group boxes to find containers
pattern_gb = r'<widget\s+class="QGroupBox"\s+name="(.*?)">.*?<property\s+name="title">\s*<string>(.*?)</string>'
matches_gb = re.findall(pattern_gb, ui_content, re.DOTALL)
for name, text in matches_gb:
    t_upper = text.upper()
    if 'DATE' in t_upper or 'PRIORIT' in t_upper:
        print(f'Group Name: {name}, Title: {text}')
