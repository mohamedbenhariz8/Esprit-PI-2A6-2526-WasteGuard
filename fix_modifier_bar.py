lines = open("c:/Users/Rudy/Desktop/INTEGRATION_FINALE1111/INTEGRATION_FINALE1/mainwindow.cpp", encoding="utf-8").readlines()
start = -1
end = -1
for i in range(15770, 15900):
    if "if (auto *cmdUserAjoutWidget = findChild<QWidget*>("user_3")) {" in lines[i]:
        start = i
    if "if (auto *profil3 = findChild<QPushButton*>("btnprofil_3")) profil3->hide();" in lines[i]:
        end = i
        break

if start != -1 and end != -1:
    print(f"Deleting {start+1} to {end+1}")
    del lines[start:end+1]

    # Re-insert the proper blocks WITHOUT stealing btnAddProduct_2
    out = []

    out.append("    if (auto *cmdUserModWidget = findChild<QWidget*>("user_3")) {\n")
    out.append("        cmdUserModWidget->setStyleSheet("background-color: #ffffff; border: none; border-bottom: 1px solid #dce1e6; padding: 0 6px;");\n")
    out.append("        cmdUserModWidget->setMinimumHeight(64);\n")
    out.append("        cmdUserModWidget->setMaximumHeight(64);\n")
    out.append("    }\n")

    out.append("    if (auto *cmdUserMod = findChild<QLabel*>("user_3")) {\n")
    out.append("        cmdUserMod->setTextFormat(Qt::RichText);\n")
    out.append("        cmdUserMod->setText("<span style=\\"color:#0f2b4c;font-size:14px;font-weight:700;\\">Admin System</span><br/><span style=\\"color:#1ba976;font-size:12px;\\">Chef d\\"Atelier</span>");\n")
    out.append("        cmdUserMod->setAlignment(Qt::AlignRight | Qt::AlignVCenter);\n")
    out.append("        cmdUserMod->setStyleSheet("background-color: #ffffff; border: none; border-bottom: 1px solid #dce1e6; padding: 0 6px;");\n")
    out.append("        cmdUserMod->setMinimumHeight(64);\n")
    out.append("        cmdUserMod->setMaximumHeight(64);\n")
    out.append("    }\n")

    out.append("    if (auto *cmdTitleMod = findChild<QLabel*>("lb_3")) {\n")
    out.append("        cmdTitleMod->setStyleSheet("color: #1f2d3d; font-size: 18px; font-weight: 700; background-color: #ffffff; border: none; border-bottom: 1px solid #dce1e6; padding: 0 6px;");\n")
    out.append("        cmdTitleMod->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);\n")
    out.append("        cmdTitleMod->setMinimumHeight(64);\n")
    out.append("        cmdTitleMod->setMaximumHeight(64);\n")
    out.append("    }\n")

    out.append("    if (auto *layoutMod = findChild<QHBoxLayout*>("horizontalLayout_3")) {\n")
    out.append("        layoutMod->setContentsMargins(16, 0, 10, 0);\n")
    out.append("        layoutMod->setSpacing(4);\n")
    out.append("    }\n")

    out.append("    if (auto *fillMod = findChild<QFrame*>("topFill_3")) {\n")
    out.append("        fillMod->setStyleSheet("background-color: #ffffff; border: none; border-bottom: 1px solid #dce1e6;");\n")
    out.append("        fillMod->setMinimumHeight(64);\n")
    out.append("        fillMod->setMaximumHeight(64);\n")
    out.append("    }\n")

    out.append("    if (auto *gapMod = findChild<QFrame*>("topGap_3")) {\n")
    out.append("        gapMod->setStyleSheet("background-color: #ffffff; border: none; border-bottom: 1px solid #dce1e6;");\n")
    out.append("        gapMod->setMinimumSize(2, 64);\n")
    out.append("        gapMod->setMaximumSize(2, 64);\n")
    out.append("    }\n")

    out.append("    if (auto *notif3 = findChild<QPushButton*>("btnnotif_3")) notif3->hide();\n")
    out.append("    if (auto *profil3 = findChild<QPushButton*>("btnprofil_3")) profil3->hide();\n")

    lines = lines[:start] + out + lines[start:]

    with open("c:/Users/Rudy/Desktop/INTEGRATION_FINALE1111/INTEGRATION_FINALE1/mainwindow.cpp", "w", encoding="utf-8") as f:
        f.writelines(lines)
    print("Fixed.")
else:
    print("Block not found!")
