with open('mainwindow.ui', 'r', encoding='utf-8') as f:
    content = f.read()

# 1. Remove btnSmartCart from global nav
remove_target = '''       <item>
        <widget class="QPushButton" name="btnSmartCart">
         <property name="text">
          <string>📧 Contacter Fournisseur</string>
         </property>
        </widget>
       </item>
       <item>
        <widget class="QPushButton" name="btnMaintenance">'''

remove_replacement = '''       <item>
        <widget class="QPushButton" name="btnMaintenance">'''

# 2. Add btnSmartCart to stock page after btnGoStats_Stock
add_target = '''                    <widget class="QPushButton" name="btnGoStats_Stock">
                     <property name="text">
                      <string>Statistiques</string>
                     </property>
                    </widget>
                   </item>
                   <item>
                    <spacer name="verticalSpacer_right">'''

add_replacement = '''                    <widget class="QPushButton" name="btnGoStats_Stock">
                     <property name="text">
                      <string>Statistiques</string>
                     </property>
                    </widget>
                   </item>
                   <item>
                    <widget class="QPushButton" name="btnSmartCart">
                     <property name="text">
                      <string>📧 Contacter Fournisseur</string>
                     </property>
                    </widget>
                   </item>
                   <item>
                    <spacer name="verticalSpacer_right">'''

ok1 = remove_target in content
ok2 = add_target in content

if ok1 and ok2:
    content = content.replace(remove_target, remove_replacement, 1)
    content = content.replace(add_target, add_replacement, 1)
    with open('mainwindow.ui', 'w', encoding='utf-8') as f:
        f.write(content)
    print("Patched successfully.")
else:
    if not ok1: print("Remove target NOT found")
    if not ok2: print("Add target NOT found")
