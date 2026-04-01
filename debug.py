import codecs, json

path = r'c:\Users\ASUS\Downloads\INTEGRATION_FINALEeee\INTEGRATION_FINALE\mainwindow.cpp'
with codecs.open(path, 'r', 'utf-8') as f:
    text = f.read()

lines = text.splitlines(True)
print(json.dumps("".join(lines[18760:18780])))
print(json.dumps("".join(lines[18873:18886])))
