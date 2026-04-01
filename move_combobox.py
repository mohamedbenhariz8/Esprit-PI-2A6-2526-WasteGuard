import re

# Read the file
with open(r'c:\Users\moham\OneDrive\Documents\ProjetQT\WasteGuard\mainwindow.ui', 'r', encoding='utf-8') as f:
    content = f.read()

# Find and extract the ComboBox section (cbProjetStats)
combobox_pattern = r'(\s+<item>\s+<widget class="QComboBox" name="cbProjetStats">.*?</widget>\s+</item>)'
combobox_match = re.search(combobox_pattern, content, re.DOTALL)

if combobox_match:
    combobox_section = combobox_match.group(1)
    
    # Remove the ComboBox from its current position
    content = content.replace(combobox_section, '')
    
    # Find the chartViewTasks section and insert the ComboBox after it
    charttasks_pattern = r'(</item>\s+<item>\s+<widget class="QChartView" name="chartViewTasks">.*?</widget>\s+</item>)'
    
    def insert_combobox(match):
        return match.group(1) + combobox_section
    
    content = re.sub(charttasks_pattern, insert_combobox, content, flags=re.DOTALL)
    
    # Write back
    with open(r'c:\Users\moham\OneDrive\Documents\ProjetQT\WasteGuard\mainwindow.ui', 'w', encoding='utf-8') as f:
        f.write(content)
    
    print("Modification appliquee avec succes!")
    print("- ComboBox deplace sous la courbe de progression")
else:
    print("Erreur: ComboBox non trouve")
