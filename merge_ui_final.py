import xml.etree.ElementTree as ET
import shutil

# Backup original
shutil.copy(r'c:\Users\moham\OneDrive\Documents\ProjetQT\WasteGuard\mainwindow.ui',
            r'c:\Users\moham\OneDrive\Documents\ProjetQT\WasteGuard\mainwindow.ui.backup')

# Parse files
login_tree = ET.parse(r'c:\Users\moham\OneDrive\Documents\ProjetQT\login\mainwindow.ui')
wasteguard_tree = ET.parse(r'c:\Users\moham\OneDrive\Documents\ProjetQT\WasteGuard\mainwindow.ui')

login_root = login_tree.getroot()
wasteguard_root = wasteguard_tree.getroot()

# Find centralwidget in both
login_central = login_root.find(".//widget[@name='centralwidget']")
wasteguard_central = wasteguard_root.find(".//widget[@name='centralwidget']")

# Remove all existing layouts from wasteguard centralwidget
for child in list(wasteguard_central):
    if child.tag == 'layout':
        wasteguard_central.remove(child)

# Create main VBoxLayout
main_layout = ET.SubElement(wasteguard_central, 'layout')
main_layout.set('class', 'QVBoxLayout')
main_layout.set('name', 'verticalLayout_Main')

# Add spacing property
spacing_prop = ET.SubElement(main_layout, 'property')
spacing_prop.set('name', 'spacing')
spacing_num = ET.SubElement(spacing_prop, 'number')
spacing_num.text = '0'

# Add margins property
margins_prop = ET.SubElement(main_layout, 'property')
margins_prop.set('name', 'leftMargin')
margins_num = ET.SubElement(margins_prop, 'number')
margins_num.text = '0'

for margin in ['topMargin', 'rightMargin', 'bottomMargin']:
    margin_prop = ET.SubElement(main_layout, 'property')
    margin_prop.set('name', margin)
    margin_num = ET.SubElement(margin_prop, 'number')
    margin_num.text = '0'

# Add item for stacked widget
layout_item = ET.SubElement(main_layout, 'item')
stacked_main = ET.SubElement(layout_item, 'widget')
stacked_main.set('class', 'QStackedWidget')
stacked_main.set('name', 'stackedWidgetMain')

# Add currentIndex property to stacked widget
current_idx_prop = ET.SubElement(stacked_main, 'property')
current_idx_prop.set('name', 'currentIndex')
current_idx_num = ET.SubElement(current_idx_prop, 'number')
current_idx_num.text = '0'

# PAGE 0: Login
page_login = ET.SubElement(stacked_main, 'widget')
page_login.set('class', 'QWidget')
page_login.set('name', 'pageLogin')

# Copy login centralwidget layout to pageLogin
login_layout = login_central.find('layout')
if login_layout is not None:
    page_login.append(login_layout)

# PAGE 1: WasteGuard  
page_wasteguard = ET.SubElement(stacked_main, 'widget')
page_wasteguard.set('class', 'QWidget')
page_wasteguard.set('name', 'pageWasteGuard')

# Copy original wasteguard layout
wasteguard_layout = wasteguard_tree.getroot().find(".//widget[@name='centralwidget']/layout")
if wasteguard_layout is not None:
    # Create a copy of the layout
    import copy
    wg_layout_copy = copy.deepcopy(wasteguard_layout)
    page_wasteguard.append(wg_layout_copy)

# Write output with UTF-8 encoding
wasteguard_tree.write(r'c:\Users\moham\OneDrive\Documents\ProjetQT\WasteGuard\mainwindow.ui',
                      encoding='UTF-8',
                      xml_declaration=True)

print("✅ UI fusionné avec succès!")
print("Page 0: Login (votre interface complète)")
print("Page 1: WasteGuard (gestion des employés)")
print("")
print("Backup sauvegardé: mainwindow.ui.backup")
