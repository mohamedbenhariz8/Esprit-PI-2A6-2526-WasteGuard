$path = "c:\Users\gigabyte\Downloads\projetcpp2526-s2-2a6-smart-waste-collection\projetcpp2526-s2-2a6-smart-waste-collection\mainwindow.ui"
$content = [System.IO.File]::ReadAllText($path)

# Regex to remove anything between the widget tag and the closing widget tag, 
# then insert the placeholder property.
# We target specifically the newly created QLineEdit widgets.

$patternAdd = '(?s)(<widget class="QLineEdit" name="prod_ln_model_add">).*?(</widget>)'
$replacementAdd = '$1`n                                     <property name="placeholderText">`n                                      <string>Ex: ProBin 240L</string>`n                                     </property>`n                                    $2'
$content = [regex]::Replace($content, $patternAdd, $replacementAdd)

$patternMod = '(?s)(<widget class="QLineEdit" name="prod_ln_model_mod">).*?(</widget>)'
$replacementMod = '$1`n                                     <property name="placeholderText">`n                                      <string>Ex: ProBin 240L</string>`n                                     </property>`n                                    $2'
$content = [regex]::Replace($content, $patternMod, $replacementMod)

[System.IO.File]::WriteAllText($path, $content)
Write-Host "mainwindow.ui cleaned and placeholders added"
