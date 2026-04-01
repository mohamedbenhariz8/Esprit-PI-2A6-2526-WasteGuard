import re

# Read the file
with open(r'c:\Users\moham\OneDrive\Documents\ProjetQT\WasteGuard\mainwindow.ui', 'r', encoding='utf-8') as f:
    content = f.read()

# Find the statistics page layout section
# Add spacing to the horizontal layout containing the two top charts
content = re.sub(
    r'(<layout class="QHBoxLayout" name="horizontalLayout_Stats">)',
    r'\1\n              <property name="spacing">\n               <number>20</number>\n              </property>',
    content
)

# Add minimum height to chartViewAbsence
content = re.sub(
    r'(<widget class="QChartView" name="chartViewAbsence"/>)',
    r'<widget class="QChartView" name="chartViewAbsence">\n               <property name="minimumSize">\n                <size>\n                 <width>0</width>\n                 <height>350</height>\n                </size>\n               </property>\n              </widget>',
    content
)

# Add minimum height to chartViewWorkload
content = re.sub(
    r'(<widget class="QChartView" name="chartViewWorkload"/>)',
    r'<widget class="QChartView" name="chartViewWorkload">\n               <property name="minimumSize">\n                <size>\n                 <width>0</width>\n                 <height>350</height>\n                </size>\n               </property>\n              </widget>',
    content
)

# Change the verticalSpacer_Stats from Expanding to Fixed with smaller height
content = re.sub(
    r'(<spacer name="verticalSpacer_Stats">.*?<property name="sizeType">.*?<enum>)QSizePolicy::Policy::Expanding(</enum>)',
    r'\1QSizePolicy::Policy::Fixed\2',
    content,
    flags=re.DOTALL
)

content = re.sub(
    r'(<spacer name="verticalSpacer_Stats">.*?<height>)40(</height>)',
    r'\g<1>10\2',
    content,
    flags=re.DOTALL
)

# Write back
with open(r'c:\Users\moham\OneDrive\Documents\ProjetQT\WasteGuard\mainwindow.ui', 'w', encoding='utf-8') as f:
    f.write(content)

print("Modifications appliquees avec succes!")
print("- Espacement entre les charts: 20px")
print("- Hauteur minimale des charts du haut: 350px")
print("- Spacer du bas: Fixed 10px")
