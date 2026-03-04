$path = "c:\Users\gigabyte\Downloads\projetcpp2526-s2-2a6-smart-waste-collection\projetcpp2526-s2-2a6-smart-waste-collection\mainwindow.ui"
$content = [System.IO.File]::ReadAllText($path)

# Correctly escape backtick-n by using double quotes for the replacement string or just literal newlines
$newline = [System.Environment]::NewLine

$content = $content -replace '`n', $newline

[System.IO.File]::WriteAllText($path, $content)
Write-Host "mainwindow.ui newlines fixed"
