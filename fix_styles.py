import sys
import re

path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.cpp'
with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# Pattern to find the light green style and replace it with dark green
old_style = r'b->setStyleSheet\(\s*"QPushButton\{background-color:#f0f7f4;color:#1a3a2e;border:1px solid #d4e8dc;"\s*"border-radius:10px;padding:12px 16px;font-weight:700;font-size:13px;text-align:center;\}"\s*"QPushButton:hover\{background-color:#27ae60;color:#ffffff;border-color:#27ae60;\}"\);'

# Let's try a more flexible regex for the style
style_pattern = r'b->setStyleSheet\(\s*"QPushButton\{background-color:#f0f7f4;.*?\);'

new_style = """b->setStyleSheet(
            "QPushButton { "
            "  background-color: #1f4d3a; color: #ffffff; border: 1px solid #2a6a51; "
            "  border-radius: 10px; padding: 12px 16px; font-weight: 800; font-size: 13px; "
            "} "
            "QPushButton:hover { background-color: #27ae60; border-color: #27ae60; }"
        );"""

new_content = re.sub(style_pattern, new_style, content, flags=re.DOTALL)

with open(path, 'w', encoding='utf-8') as f:
    f.write(new_content)
print("Successfully updated button styles")
