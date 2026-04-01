with open('mainwindow.ui', 'r', encoding='utf-8') as f:
    content = f.read()

# Replace the plain btnSmartCart XML to include the matching styleSheet
old = '''                    <widget class="QPushButton" name="btnSmartCart">
                     <property name="text">
                      <string>📧 Contacter Fournisseur</string>
                     </property>
                    </widget>'''

new = '''                    <widget class="QPushButton" name="btnSmartCart">
                     <property name="styleSheet">
                      <string notr="true">background-color: #ffffff;
color: #1f2d3d;
border: 1px solid #dce1e6;
border-radius: 10px;
padding: 12px 16px;
font-size: 14px;
font-weight: 700;
text-align: center;</string>
                     </property>
                     <property name="text">
                      <string>📧 Contacter Fournisseur</string>
                     </property>
                    </widget>'''

if old in content:
    content = content.replace(old, new, 1)
    with open('mainwindow.ui', 'w', encoding='utf-8') as f:
        f.write(content)
    print("Styled btnSmartCart successfully.")
else:
    print("Target not found!")
