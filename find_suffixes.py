import re
import sys

path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.cpp'
with open(path, 'r', encoding='utf-8', errors='ignore') as f:
    content = f.read()

# Search for " followed by word characters without a space
# Wait, the error is about a literal suffix, which means AFTER the closing quote.
# Like "string"decroissant
matches = re.finditer(r'\"(decroissant|desc)', content)
for m in matches:
    line_no = content.count('\n', 0, m.start()) + 1
    print(f'Match BEFORE quote at {line_no}: ...{content[m.start()-5:m.end()+5]}...')

matches2 = re.finditer(r'\"(decroissant|desc)', content) # Wait, this is the same.
# I want to find AFTER the quote.
matches3 = re.finditer(r'\"(decroissant|desc)', content) 

# Actually, let's just search for any text.contains("decroissant") etc.
# The error "inconsistent user-defined literal suffixes" is VERY specific.
# It means the compiler sees "something"decroissant
matches4 = re.finditer(r'\"[a-zA-Z_]', content)
for m in matches4:
    # Check if there is a matching opening quote before it.
    # This is hard.
    pass

# Let's search for the exact strings "decroissant" and "desc"
# and check if there's a space before them if they follow a quote.
lines = content.split('\n')
for i, line in enumerate(lines):
    if '\"decroissant' in line or '\"desc' in line:
        print(f'Possible issue at L{i+1}: {line.strip()}')
