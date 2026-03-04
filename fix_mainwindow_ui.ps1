$path = "c:\Users\gigabyte\Downloads\projetcpp2526-s2-2a6-smart-waste-collection\projetcpp2526-s2-2a6-smart-waste-collection\mainwindow.ui"
$content = [System.IO.File]::ReadAllText($path)

# For Add form
$oldAdd = '<widget class="QComboBox" name="prod_cb_model_add">'
$newAdd = '<widget class="QLineEdit" name="prod_ln_model_add">'
$content = $content.Replace($oldAdd, $newAdd)

# For Mod form
$oldMod = '<widget class="QComboBox" name="prod_cb_model_mod">'
$newMod = '<widget class="QLineEdit" name="prod_ln_model_mod">'
$content = $content.Replace($oldMod, $newMod)

# Replace the item block common to both (since they have the same ProBin 240L default)
$oldItem = @"
                                     <item>
                                      <property name="text">
                                       <string>ProBin 240L</string>
                                      </property>
                                     </item>
"@

$newProp = @"
                                     <property name="placeholderText">
                                      <string>Ex: ProBin 240L</string>
                                     </property>
"@

# Note: The indentation might be slightly different in the actual file. 
# Let's try to be less specific about leading whitespace for the Replace call or do it twice if needed.
# From the view_file output, it looks consistent.

$content = $content.Replace($oldItem, $newProp)

[System.IO.File]::WriteAllText($path, $content)
Write-Host "mainwindow.ui updated"
