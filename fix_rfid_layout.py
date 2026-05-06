import xml.etree.ElementTree as ET

def fix_ui(file_path):
    tree = ET.parse(file_path)
    root = tree.getroot()
    changed = False

    for layout in root.iter('layout'):
        name = layout.get('name')
        if name in ('rfidRow_Modif', 'rfidRow_Ajout'):
            # Check if stretch property already exists
            has_stretch = any(p.get('name') == 'stretch' for p in layout.findall('property'))
            if not has_stretch:
                stretch_prop = ET.Element('property', {'name': 'stretch'})
                stretch_val = ET.Element('string')
                stretch_val.text = '1,0'
                stretch_prop.append(stretch_val)
                # insert it right after the class and name (at beginning)
                layout.insert(0, stretch_prop)
                changed = True
                print(f"Added stretch to {name}")
                
            # also make sure the QPushButton has a fixed max width so it doesn't grow wildly
            for item in layout.findall('item'):
                btn = item.find('widget[@class="QPushButton"]')
                if btn is not None:
                    has_max_width = any(p.get('name') == 'maximumSize' for p in btn.findall('property'))
                    if not has_max_width:
                        max_size_prop = ET.Element('property', {'name': 'maximumSize'})
                        size = ET.Element('size')
                        w = ET.Element('width')
                        w.text = '120'
                        h = ET.Element('height')
                        h.text = '35'
                        size.append(w)
                        size.append(h)
                        max_size_prop.append(size)
                        btn.append(max_size_prop)
                        changed = True
                        print(f"Added max width to {btn.get('name')}")
                        
            # Set QLineEdit size policy and min height
            for item in layout.findall('item'):
                le = item.find('widget[@class="QLineEdit"]')
                if le is not None:
                    # set min height to 35 to match standard
                    has_min_height = any(p.get('name') == 'minimumSize' for p in le.findall('property'))
                    if not has_min_height:
                        min_size_prop = ET.Element('property', {'name': 'minimumSize'})
                        size = ET.Element('size')
                        w = ET.Element('width')
                        w.text = '0'
                        h = ET.Element('height')
                        h.text = '35'
                        size.append(w)
                        size.append(h)
                        min_size_prop.append(size)
                        le.append(min_size_prop)
                        changed = True
                        print(f"Added min height to {le.get('name')}")

    if changed:
        tree.write(file_path, encoding='utf-8', xml_declaration=True)
        print("Updated mainwindow.ui")
    else:
        print("No changes needed")

fix_ui('mainwindow.ui')
