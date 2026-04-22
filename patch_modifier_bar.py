lines = open('c:/Users/Rudy/Desktop/INTEGRATION_FINALE1111/INTEGRATION_FINALE1/mainwindow.cpp', encoding='utf-8').readlines()

block1 = lines[15629:15642] # user_5 widget
block2 = lines[15671:15694] # user_5 label
block3 = lines[15709:15720] # lb_5
block4 = lines[15729:15736] # horizontalLayout_9
block5 = lines[15747:15756] # topFill_5
block6 = lines[15767:15776] # topGap_5
block7 = [lines[15781], lines[15783]] # btnnotif_5 and btnprofil_5

out = []
out.extend([l.replace('user_5', 'user_3') for l in block1])
out.append('\n')
out.extend([l.replace('user_5', 'user_3') for l in block2])
out.append('\n')
out.extend([l.replace('lb_5', 'lb_3') for l in block3])
out.append('\n')
out.extend([l.replace('horizontalLayout_9', 'horizontalLayout_3') for l in block4])
out.append('\n')
out.extend([l.replace('topFill_5', 'topFill_3') for l in block5])
out.append('\n')
out.extend([l.replace('topGap_5', 'topGap_3') for l in block6])
out.append('\n')
out.extend([l.replace('_5', '_3').replace('notif5', 'notif3').replace('profil5', 'profil3') for l in block7])

# Insert right after topGap_5 block (line 15776)
lines = lines[:15776] + out + lines[15776:]

with open('c:/Users/Rudy/Desktop/INTEGRATION_FINALE1111/INTEGRATION_FINALE1/mainwindow.cpp', 'w', encoding='utf-8') as f:
    f.writelines(lines)
print('Done!')
