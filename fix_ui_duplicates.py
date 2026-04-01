#!/usr/bin/env python3
"""Fix duplicate widget names in mainwindow.ui"""
import re
from collections import defaultdict

# Read the UI file
with open("mainwindow.ui", "r", encoding="utf-8") as f:
    content = f.read()

# Dictionary to track widget names and their contexts
name_occurrences = defaultdict(list)

# Find all <name>...</name> patterns with their line numbers and context
lines = content.split('\n')
for i, line in enumerate(lines):
    match = re.search(r'<name>([^<]+)</name>', line)
    if match:
        name = match.group(1)
        name_occurrences[name].append((i, line))

# Display duplicates found
duplicates = {name: occurrences for name, occurrences in name_occurrences.items() if len(occurrences) > 1}
print("Duplicate widget names found:")
for name, occurrences in sorted(duplicates.items()):
    print(f"\n  '{name}' appears {len(occurrences)} times:")
    for line_num, line_text in occurrences:
        print(f"    Line {line_num + 1}: {line_text.strip()}")

# Build a mapping of replacements
# For horizontalLayout_Main: rename 2nd and 3rd occurrences
replacements = {}

# Fix horizontalLayout_Main (3 occurrences in centralwidget, pageStock, pageCmdDashboard)
if len(name_occurrences['horizontalLayout_Main']) == 3:
    replacements['horizontalLayout_Main'] = [
        ('horizontalLayout_Main', 0),  # Keep first as is
        ('horizontalLayout_Stock', 1),  # Rename second to horizontalLayout_Stock  
        ('horizontalLayout_CmdDashboard', 2),  # Rename third to horizontalLayout_CmdDashboard
    ]

# Fix horizontalLayout_1 (2 occurrences)
if len(name_occurrences['horizontalLayout_1']) >= 2:
    # Find if these have unique identifiers in context
    for idx, (line_num, line_text) in enumerate(name_occurrences['horizontalLayout_1']):
        if idx > 0:  # Skip first, rename others
            # Check context for disambiguation
            parent_widget = None
            for j in range(line_num, max(-1, line_num - 20), -1):
                if '<widget' in lines[j]:
                    parent_match = re.search(r'name="([^"]+)"', lines[j])
                    if parent_match:
                        parent_widget = parent_match.group(1)
                        break
            if parent_widget:
                print(f"  horizontalLayout_1 [{idx}] parent: {parent_widget}")

# Apply replacements to the content
modified_content = content

# Fix horizontalLayout_Main specifically by line
fixes = [
    # Keep line 1331 as is
    (1330, 'horizontalLayout_Main', 'horizontalLayout_Main'),  # 0-indexed
    (4965, 'horizontalLayout_Main', 'horizontalLayout_Stock'),  # 0-indexed
    (8262, 'horizontalLayout_Main', 'horizontalLayout_CmdDashboard'),  # 0-indexed
]

# Sort in reverse order so line numbers don't get messed up
for line_num_0based, old_name, new_name in sorted(fixes, reverse=True, key=lambda x: x[0]):
    # Find and replace only on that specific line
    target_line = lines[line_num_0based]
    if old_name in target_line:
        # Replace this specific occurrence on this line
        modified_lines = lines.copy()
        modified_lines[line_num_0based] = target_line.replace(
            f'<name>{old_name}</name>',
            f'<name>{new_name}</name>',
            1  # Replace only the first occurrence on this line
        )
        lines = modified_lines
        print(f"Fixed line {line_num_0based + 1}: {old_name} -> {new_name}")

# Reconstruct modified content
modified_content = '\n'.join(lines)

# Save the modified content
with open("mainwindow.ui", "w", encoding="utf-8") as f:
    f.write(modified_content)

print("\nFixed mainwindow.ui - duplicate widget names corrected")
