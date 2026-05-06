import os
import re

def main():
    ui_path = "mainwindow.ui"
    with open(ui_path, "r", encoding="utf-8") as f:
        ui_content = f.read()

    # Replace rfidRow_Modif layout with just the QPushButton
    modif_layout_pattern = r'<layout class="QHBoxLayout" name="rfidRow_Modif" stretch="1,0">.*?</layout>'
    modif_button_xml = """<widget class="QPushButton" name="btnScanRFID_Modif">
                      <property name="minimumSize">
                       <size>
                        <width>120</width>
                        <height>35</height>
                       </size>
                      </property>
                      <property name="cursor">
                       <cursorShape>PointingHandCursor</cursorShape>
                      </property>
                      <property name="styleSheet">
                       <string>QPushButton { background-color: #f39c12; color: white; border-radius: 5px; font-weight: bold; }</string>
                      </property>
                      <property name="text">
                       <string>Scanner badge</string>
                      </property>
                     </widget>"""
    ui_content = re.sub(modif_layout_pattern, modif_button_xml, ui_content, flags=re.DOTALL)

    # Replace rfidRow_Ajout layout with just the QPushButton
    ajout_layout_pattern = r'<layout class="QHBoxLayout" name="rfidRow_Ajout" stretch="1,0">.*?</layout>'
    ajout_button_xml = """<widget class="QPushButton" name="btnScanRFID_Ajout">
                      <property name="minimumSize">
                       <size>
                        <width>120</width>
                        <height>35</height>
                       </size>
                      </property>
                      <property name="cursor">
                       <cursorShape>PointingHandCursor</cursorShape>
                      </property>
                      <property name="styleSheet">
                       <string>QPushButton { background-color: #f39c12; color: white; border-radius: 5px; font-weight: bold; }</string>
                      </property>
                      <property name="text">
                       <string>Scanner badge</string>
                      </property>
                     </widget>"""
    ui_content = re.sub(ajout_layout_pattern, ajout_button_xml, ui_content, flags=re.DOTALL)

    with open(ui_path, "w", encoding="utf-8") as f:
        f.write(ui_content)

    print("Updated mainwindow.ui")

    h_path = "mainwindow.h"
    with open(h_path, "r", encoding="utf-8") as f:
        h_content = f.read()

    h_content = h_content.replace('QLineEdit *m_rfidScanTarget = nullptr;', 'QPushButton *m_rfidScanTarget = nullptr;')
    h_content = h_content.replace('void startRfidCapture(QLineEdit *targetField);', 'void startRfidCapture(QPushButton *targetField);')

    with open(h_path, "w", encoding="utf-8") as f:
        f.write(h_content)

    print("Updated mainwindow.h")

    cpp_path = "mainwindow.cpp"
    with open(cpp_path, "r", encoding="utf-8") as f:
        cpp_content = f.read()

    # Change function signature
    cpp_content = cpp_content.replace('void MainWindow::startRfidCapture(QLineEdit *targetField)', 'void MainWindow::startRfidCapture(QPushButton *targetField)')

    # Update logic in startRfidCapture
    cpp_content = cpp_content.replace(
        'm_rfidScanTarget->setText(tag.toUpper());',
        'm_rfidScanTarget->setText("Badge: " + tag.toUpper());\n            m_rfidScanTarget->setProperty("rfid_tag", tag.toUpper());'
    )

    # Change txtRFID checks
    cpp_content = cpp_content.replace('ui->txtRFID_Ajout', 'ui->btnScanRFID_Ajout')
    cpp_content = cpp_content.replace('ui->txtRFID_Modif', 'ui->btnScanRFID_Modif')

    # Update reading logic
    cpp_content = re.sub(
        r'const QString rfidAjout = ui->btnScanRFID_Ajout \? ui->btnScanRFID_Ajout->text\(\)\.trimmed\(\) : QString\(\);',
        r'const QString rfidAjout = ui->btnScanRFID_Ajout ? ui->btnScanRFID_Ajout->property("rfid_tag").toString() : QString();',
        cpp_content
    )
    cpp_content = re.sub(
        r'const QString rfidModif = ui->btnScanRFID_Modif \? ui->btnScanRFID_Modif->text\(\)\.trimmed\(\) : QString\(\);',
        r'const QString rfidModif = ui->btnScanRFID_Modif ? ui->btnScanRFID_Modif->property("rfid_tag").toString() : QString();',
        cpp_content
    )

    # Update clearing logic (Ajout)
    cpp_content = cpp_content.replace(
        'if (ui->btnScanRFID_Ajout) ui->btnScanRFID_Ajout->clear();',
        'if (ui->btnScanRFID_Ajout) {\n        ui->btnScanRFID_Ajout->setText("Scanner badge");\n        ui->btnScanRFID_Ajout->setProperty("rfid_tag", QString());\n    }'
    )

    # Update modifying logic
    cpp_content = cpp_content.replace(
        'if (ui->btnScanRFID_Modif) ui->btnScanRFID_Modif->setText(rfidVal);',
        'if (ui->btnScanRFID_Modif) {\n                if (!rfidVal.isEmpty()) {\n                    ui->btnScanRFID_Modif->setText("Badge: " + rfidVal);\n                    ui->btnScanRFID_Modif->setProperty("rfid_tag", rfidVal);\n                } else {\n                    ui->btnScanRFID_Modif->setText("Scanner badge");\n                    ui->btnScanRFID_Modif->setProperty("rfid_tag", QString());\n                }\n            }'
    )

    with open(cpp_path, "w", encoding="utf-8") as f:
        f.write(cpp_content)
        
    print("Updated mainwindow.cpp")

if __name__ == "__main__":
    main()
