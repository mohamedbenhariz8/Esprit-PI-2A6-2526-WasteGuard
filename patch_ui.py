with open('mainwindow.ui', 'r', encoding='utf-8') as f:
    content = f.read()

target_add = '''                      <item>
                       <widget class="QLineEdit" name="inputSeuil_add">'''
repl_add = '''                      <item>
                       <widget class="QLineEdit" name="inputEmailFour_add">
                        <property name="placeholderText">
                         <string>Email Fournisseur</string>
                        </property>
                       </widget>
                      </item>
                      <item>
                       <widget class="QLineEdit" name="inputSeuil_add">'''

target_mod = '''                      <item>
                       <widget class="QLineEdit" name="inputSeuil_mod">'''
repl_mod = '''                      <item>
                       <widget class="QLineEdit" name="inputEmailFour_mod">
                        <property name="placeholderText">
                         <string>Email Fournisseur</string>
                        </property>
                       </widget>
                      </item>
                      <item>
                       <widget class="QLineEdit" name="inputSeuil_mod">'''

if target_add in content and target_mod in content:
    content = content.replace(target_add, repl_add)
    content = content.replace(target_mod, repl_mod)
    with open('mainwindow.ui', 'w', encoding='utf-8') as f:
        f.write(content)
    print("UI successfully patched.")
else:
    print("Targets not found!")
    if target_add not in content: print("Missing target_add")
    if target_mod not in content: print("Missing target_mod")
