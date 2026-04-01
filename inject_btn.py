with open('mainwindow.ui', 'r', encoding='utf-8') as f:
    content = f.read()

# Insert Smart Cart button after btnStatistiques block
target = '''        <widget class="QPushButton" name="btnStatistiques">
         <property name="visible">
          <bool>false</bool>
         </property>
         <property name="text">
          <string>📊 Statistiques</string>
         </property>
        </widget>
       </item>'''

replacement = target + '''
       <item>
        <widget class="QPushButton" name="btnSmartCart">
         <property name="text">
          <string>📧 Contacter Fournisseur</string>
         </property>
        </widget>
       </item>'''

if target in content:
    content = content.replace(target, replacement, 1)
    with open('mainwindow.ui', 'w', encoding='utf-8') as f:
        f.write(content)
    print("Injected btnSmartCart successfully.")
else:
    print("Target not found!")
