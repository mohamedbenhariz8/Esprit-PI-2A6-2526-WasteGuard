
import os

# RELATIVE PATH TO SIBLING DIRECTORY
file_path = "../projettcpp/mainwindow.ui"
temp_file_path = "../projettcpp/mainwindow_new.ui"

# New stylesheet content (lines 60-110 replacement)
# We will match the START: "/* STAT CARDS &amp; FRAMES */"
# And END: "/* ACTION BUTTONS (Add, Save) */"
new_stylesheet = """/* STAT CARDS &amp; FRAMES */
QFrame#rightSidebar, QFrame#stat1, QFrame#stat2, QFrame#stat3, QFrame#stat4 { 
    background-color: #ffffff;
    border-radius: 12px;
    border: 1px solid #dce1e6;
    border-bottom: 2px solid #dce1e6;
}

/* BLUE STAT CARDS */
QFrame#stat1, QFrame#stat2, QFrame#stat4 {
    background-color: #e3f2fd;
    border: 1px solid #90caf9;
}
QFrame#stat1 QLabel#st_val1, QFrame#stat2 QLabel#st_val2 {
    color: #1565c0 !important;
}
QFrame#stat1 QLabel#st_lbl1, QFrame#stat2 QLabel#st_lbl2 {
    color: #546e7a !important;
}

/* RED ALERT CARD */
QFrame#stat3 {
    background-color: #ffebee;
    border: 1px solid #ef9a9a;
}
QFrame#stat3 QLabel#st_val3 {
    color: #c62828 !important;
}
QFrame#stat3 QLabel#st_lbl3 {
    color: #b71c1c !important;
}

/* STATISTICS CARD */
QFrame#stat4 QLabel#lblStatTitle {
    color: #37474f !important;
    font-weight: bold;
    font-size: 14px;
}
QFrame#stat4 QLabel {
    color: #455a64;
    font-weight: 600;
}

/* STAT LABELS */
QLabel#st_val1, QLabel#st_val2, QLabel#st_val3 {
    font-size: 24px;
    font-weight: bold;
}
QLabel#st_lbl1, QLabel#st_lbl2, QLabel#st_lbl3, QLabel#lblFunc, QLabel#lblExport {
    font-size: 12px;
    color: #37474f !important;
    font-weight: 700;
    text-transform: uppercase;
}

/* EXPORT BUTTONS */
QPushButton#btnPdf, QPushButton#btnCsv {
    background-color: #37474f;
    color: #ffffff !important;
    border: none;
    border-radius: 8px;
    padding: 10px 15px;
    text-align: center;
    font-weight: bold;
    font-size: 13px;
    margin-top: 5px;
}
QPushButton#btnPdf:hover, QPushButton#btnCsv:hover {
    background-color: #455a64;
}

/* PROGRESS BARS */
QProgressBar {
    border: none;
    background-color: #cfd8dc;
    border-radius: 4px;
    text-align: center;
}
QProgressBar::chunk {
    background-color: #1e88e5;
    border-radius: 4px;
}

/* LINK LABEL */
QLabel#lblViewDetails {
    color: #1565c0;
    font-size: 12px;
    font-weight: bold;
}
QLabel#lblViewDetails:hover {
    text-decoration: underline;
}

"""

# New card content (Stat4)
stat4_content = """                <item>
                 <widget class="QFrame" name="stat4">
                  <property name="cssClass" stdset="0">
                   <string>statCard</string>
                  </property>
                  <layout class="QVBoxLayout" name="vboxStat4">
                   <property name="spacing">
                    <number>10</number>
                   </property>
                   <item>
                    <widget class="QLabel" name="lblStatTitle">
                     <property name="text">
                      <string>STATISTIQUES</string>
                     </property>
                    </widget>
                   </item>
                   <item>
                     <layout class="QGridLayout" name="gridStats">
                      <item row="0" column="0">
                       <widget class="QLabel" name="lblStatProd">
                        <property name="text">
                         <string>Produits</string>
                        </property>
                       </widget>
                      </item>
                      <item row="0" column="1">
                       <widget class="QProgressBar" name="progProduits">
                        <property name="value">
                         <number>70</number>
                        </property>
                        <property name="maximumHeight">
                         <number>6</number>
                        </property>
                       </widget>
                      </item>
                      <item row="1" column="0">
                       <widget class="QLabel" name="lblStatStock">
                        <property name="text">
                         <string>Stock</string>
                        </property>
                       </widget>
                      </item>
                      <item row="1" column="1">
                       <widget class="QProgressBar" name="progStock">
                        <property name="value">
                         <number>45</number>
                        </property>
                         <property name="maximumHeight">
                         <number>6</number>
                        </property>
                       </widget>
                      </item>
                     </layout>
                   </item>
                   <item>
                    <widget class="QLabel" name="lblViewDetails">
                     <property name="text">
                      <string>Voir détails →</string>
                     </property>
                     <property name="alignment">
                      <set>Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter</set>
                     </property>
                     <property name="cursor">
                      <cursorShape>PointingHandCursor</cursorShape>
                     </property>
                    </widget>
                   </item>
                  </layout>
                 </widget>
                </item>
"""

with open(file_path, "r", encoding="utf-8") as f:
    lines = f.readlines()

new_lines = []
skip = False
stat3_found = False
stat3_inserted = False

for i, line in enumerate(lines):
    # --- STYLESHEET REPLACEMENT ---
    if "/* STAT CARDS &amp; FRAMES */" in line:
        new_lines.append(new_stylesheet)
        skip = True
        continue
    
    if skip and "/* ACTION BUTTONS (Add, Save) */" in line:
        skip = False
        new_lines.append(line)
        continue

    if skip:
        continue

    # --- STAT4 INSERTION ---
    # Detect stat3 widget
    if 'name="stat3"' in line:
        stat3_found = True
    
    # If inside stat3 block (or recently passed it), look for the CLOSING </item> tag of the stat3 item wrapper
    # Structure is: <item> <widget stat3> ... </widget> </item>
    
    # We append the current line first
    new_lines.append(line)

    if stat3_found and not stat3_inserted:
        if "</item>" in line:
            # Basic check: verify we just closed the item containing stat3
            # We look back a few lines to be sure (simple heuristic)
            context = "".join(lines[max(0, i-30):i+1])
            if 'name="stat3"' in context:
                 new_lines.append(stat4_content)
                 stat3_inserted = True

with open(temp_file_path, "w", encoding="utf-8") as f:
    f.writelines(new_lines)

# Overwrite original
import shutil
shutil.move(temp_file_path, file_path)
print("File updated successfully.")
