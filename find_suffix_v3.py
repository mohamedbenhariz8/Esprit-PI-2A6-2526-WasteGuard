import re
import sys

path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.cpp'
with open(path, 'r', encoding='utf-8', errors='ignore') as f:
    content = f.read()

# Search for " followed by word (opening) or word followed by " (closing)
# But wait, the error is specifically about suffixes.
# Let's search for the pattern "something"decroissant
matches = re.finditer(r'\"[a-zA-Z_]', content)
for m in matches:
    line_no = content.count('\n', 0, m.start()) + 1
    # Check if this " is a CLOSING quote.
    # We heuristic: if it's followed by a letter, and preceded by non-space.
    # Actually, let's just print all of them.
    print(f'Match at L{line_no}: ...{content[m.start()-5:m.end()+15]}...')
