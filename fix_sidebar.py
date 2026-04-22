import sys
import re

path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.cpp'
with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# Locate the actionBtns list and the loop
# We want to replace the block that starts with QList<QPushButton*> actionBtns;
# and ends with the loop closing brace.
search_pattern = r'QList<QPushButton\*>\s+actionBtns;.*?for\s*\(const\s+char\s*\*n\s*:\s*\{.*?\}\)\s*\{.*?\}\s*\}'
match = re.search(search_pattern, content, re.DOTALL)

if match:
    # Insert the label after the loop
    original = match.group(0)
    # The label definition
    label_def = """
            auto *actLbl = new QLabel(QString::fromUtf8("\\xE2\\x9A\\xA1 Actions Rapides"));
            actLbl->setStyleSheet(
                "font-size:11px;font-weight:800;color:#ffffff;letter-spacing:1px;"
                "background:#1f4d3a;border:1px solid #2a6a51;border-radius:8px;padding:8px 10px;"
            );
"""
    new_block = original + label_def
    new_content = content.replace(original, new_block)
    
    with open(path, 'w', encoding='utf-8') as f:
        f.write(new_content)
    print("Successfully restored actLbl")
else:
    print("Could not find the actionBtns block")
