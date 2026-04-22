import re
import sys

path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.cpp'
with open(path, 'r', encoding='utf-8', errors='ignore') as f:
    content = f.read()

# Search for " followed by a letter, where " is likely a closing quote.
# A closing quote is usually preceded by a character that is NOT a space/operator etc.
matches = re.finditer(r'\"[a-zA-Z_]', content)
for m in matches:
    # Check if it looks like a suffix
    # A suffix must be immediately after the quote.
    # But wait, "string" is usually followed by space, semicolon, paren, etc.
    # If it's followed by a letter, it's a suffix.
    line_no = content.count('\n', 0, m.start()) + 1
    # Check what is before the quote
    before = content[max(0, m.start()-20):m.start()]
    print(f'Match at L{line_no}: ...{before} -> {content[m.start():m.end()+15]}')
