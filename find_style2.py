with open('mainwindow.ui', 'r', encoding='utf-8') as f:
    content = f.read()

# Find the stylesheet rule defining btnGoStats_Stock styling
idx = content.find('btnGoStats_Stock')
# search backward for the opening of the CSS rule block
block_start = content.rfind('\nQPushButton#', 0, idx)
block_end = content.find('}', idx) + 1
print(repr(content[block_start:block_end]))
