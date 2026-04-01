with open('mainwindow.cpp', 'rb') as f:
    text = f.read()

idx = text.find(b'depletionAlerts;')
print("---Target 1---")
print(repr(text[idx-20:idx+200]))

idx2 = text.find(b'btnRow->addStretch()')
print("\n---Target 2---")
print(repr(text[idx2-50:idx2+100]))
