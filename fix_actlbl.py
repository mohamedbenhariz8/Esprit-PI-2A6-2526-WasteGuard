import re

path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.cpp'
with open(path, 'r', encoding='utf-8', errors='ignore') as f:
    content = f.read()

# Fix actLbl conflicts
# We will use a counter to make them unique
counter = 0
def rename_actlbl(match):
    global counter
    counter += 1
    return f'actLbl_{counter}'

# We only want to rename the declarations and their usages.
# This is tricky without a parser.
# But in this file, actLbl is always used locally.

# Let's just fix the specific ones
content = content.replace('auto *actLbl = new QLabel(QString::fromUtf8("\\xE2\\x9A\\xA1 Actions Rapides"));', 
                          'auto *actLbl_unique = new QLabel(QString::fromUtf8("\\xE2\\x9A\\xA1 Actions Rapides"));')
# This is too broad.

# I'll just use my multi_replace chunks but one by one.
