import re
import sys

# Force UTF-8 output
sys.stdout.reconfigure(encoding='utf-8')

# Read the file
with open(r'c:\Users\moham\OneDrive\Documents\ProjetQT\WasteGuard\mainwindow.ui', 'r', encoding='utf-8') as f:
    content = f.read()

# Replace Expanding with Fixed for verticalSpacer_Mission
content = re.sub(
    r'(<spacer name="verticalSpacer_Mission">.*?<property name="sizeType">.*?<enum>)QSizePolicy::Policy::Expanding(</enum>)',
    r'\1QSizePolicy::Policy::Fixed\2',
    content,
    flags=re.DOTALL
)

# Replace height 40 with 10 for verticalSpacer_Mission
content = re.sub(
    r'(<spacer name="verticalSpacer_Mission">.*?<height>)40(</height>)',
    r'\g<1>10\2',
    content,
    flags=re.DOTALL
)

# Replace Expanding with Fixed for verticalSpacer_Pointage
content = re.sub(
    r'(<spacer name="verticalSpacer_Pointage">.*?<property name="sizeType">.*?<enum>)QSizePolicy::Policy::Expanding(</enum>)',
    r'\1QSizePolicy::Policy::Fixed\2',
    content,
    flags=re.DOTALL
)

# Replace height 40 with 10 for verticalSpacer_Pointage
content = re.sub(
    r'(<spacer name="verticalSpacer_Pointage">.*?<height>)40(</height>)',
    r'\g<1>10\2',
    content,
    flags=re.DOTALL
)

# Write back
with open(r'c:\Users\moham\OneDrive\Documents\ProjetQT\WasteGuard\mainwindow.ui', 'w', encoding='utf-8') as f:
    f.write(content)

print("Modifications appliquees avec succes!")
print("- verticalSpacer_Mission: Expanding -> Fixed, height: 40 -> 10")
print("- verticalSpacer_Pointage: Expanding -> Fixed, height: 40 -> 10")
