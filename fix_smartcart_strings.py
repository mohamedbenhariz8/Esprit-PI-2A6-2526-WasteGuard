#!/usr/bin/env python3
"""
Fix the broken C++ string literals that have real newlines instead of \\n escape sequences.
Targets lines 29874-29886 (itemLines/safeLines/optimalLines) and 29972-29995 / 30014-30037 (email bodies).
"""

filepath = "mainwindow.cpp"

with open(filepath, "r", encoding="utf-8") as f:
    content = f.read()

# ── Fix 1: itemLines QString with embedded newline ──────────────────────────
old1 = 'itemLines += QString("  - %1 (stock: %2, seuil: %3)\r\n")\r\n                    .arg(det.nom).arg(det.quantite).arg(det.seuil);'
new1 = 'itemLines += QString("  - %1 (stock: %2, seuil: %3)\\n")\r\n                    .arg(det.nom).arg(det.quantite).arg(det.seuil);'

if old1 in content:
    content = content.replace(old1, new1)
    print("[OK] Fix 1: itemLines")
else:
    print("[WARN] Fix 1 not found - trying LF variant")
    old1lf = old1.replace('\r\n', '\n')
    new1lf = new1.replace('\r\n', '\n')
    if old1lf in content:
        content = content.replace(old1lf, new1lf)
        print("[OK] Fix 1: itemLines (LF)")
    else:
        print("[ERROR] Fix 1 not found")

# ── Fix 2: safeLines QString with embedded newline ───────────────────────────
old2 = 'safeLines += QString("  - %1 : %2 unites (%3 DT)\r\n")\r\n                    .arg(det.nom).arg(qtySafe)\r\n                    .arg(QString::number(qtySafe * det.prix, \'f\', 2));'
new2 = 'safeLines += QString("  - %1 : %2 unites (%3 DT)\\n")\r\n                    .arg(det.nom).arg(qtySafe)\r\n                    .arg(QString::number(qtySafe * det.prix, \'f\', 2));'

if old2 in content:
    content = content.replace(old2, new2)
    print("[OK] Fix 2: safeLines")
else:
    print("[WARN] Fix 2 not found - trying LF variant")
    old2lf = old2.replace('\r\n', '\n')
    new2lf = new2.replace('\r\n', '\n')
    if old2lf in content:
        content = content.replace(old2lf, new2lf)
        print("[OK] Fix 2: safeLines (LF)")
    else:
        print("[ERROR] Fix 2 not found")

# ── Fix 3: optimalLines QString with embedded newline ────────────────────────
old3 = 'optimalLines += QString("  - %1 : %2 unites (%3 DT)\r\n")\r\n                    .arg(det.nom).arg(qtyOptimal)\r\n                    .arg(QString::number(qtyOptimal * det.prix, \'f\', 2));'
new3 = 'optimalLines += QString("  - %1 : %2 unites (%3 DT)\\n")\r\n                    .arg(det.nom).arg(qtyOptimal)\r\n                    .arg(QString::number(qtyOptimal * det.prix, \'f\', 2));'

if old3 in content:
    content = content.replace(old3, new3)
    print("[OK] Fix 3: optimalLines")
else:
    print("[WARN] Fix 3 not found - trying LF variant")
    old3lf = old3.replace('\r\n', '\n')
    new3lf = new3.replace('\r\n', '\n')
    if old3lf in content:
        content = content.replace(old3lf, new3lf)
        print("[OK] Fix 3: optimalLines (LF)")
    else:
        print("[ERROR] Fix 3 not found")

# ── Fix 4: Safe zone email body (broken multi-line) ──────────────────────────
# Replace the entire broken body string for the Safe zone lambda
old_safe_body = (
    '                    QString body = QString::fromUtf8(\r\n'
    '                        \"Bonjour,\r\n'
    '\r\n'
    '\"\r\n'
    '                        \"Suite a l\'analyse IA de notre systeme WasteGuard, nous avons besoin d\'un \"\r\n'
    '                        \"reapprovisionnement urgent (Zone Securisee) pour les articles suivants :\r\n'
    '\r\n'
    '\"\r\n'
    '                        \"%1\r\n'
    '\"\r\n'
    '                        \"QUANTITES DEMANDEES (Zone Securisee - minimum requis) :\r\n'
    '\"\r\n'
    '                        \"%2\r\n'
    '\"\r\n'
    '                        \"Total commande : %3 unites\r\n'
    '\"\r\n'
    '                        \"Cout estime    : %4 DT\r\n'
    '\r\n'
    '\"\r\n'
    '                        \"Merci de nous confirmer la disponibilite et le delai de livraison.\r\n'
    '\r\n'
    '\"\r\n'
    '                        \"Cordialement,\r\n'
    ' L\'equipe WasteGuard\")\r\n'
    '                        .arg(itemLines)\r\n'
    '                        .arg(safeLines)\r\n'
    '                        .arg(totalSafe)\r\n'
    '                        .arg(QString::number(totalCostSafe, \'f\', 2));'
)

new_safe_body = (
    '                    QString body = QString::fromUtf8(\r\n'
    '                        "Bonjour,\\n\\n"\r\n'
    '                        "Suite a l\'analyse IA de notre systeme WasteGuard, nous avons besoin d\'un "\r\n'
    '                        "reapprovisionnement urgent (Zone Securisee) pour les articles suivants :\\n\\n"\r\n'
    '                        "%1\\n"\r\n'
    '                        "QUANTITES DEMANDEES (Zone Securisee - minimum requis) :\\n"\r\n'
    '                        "%2\\n"\r\n'
    '                        "Total commande : %3 unites\\n"\r\n'
    '                        "Cout estime    : %4 DT\\n\\n"\r\n'
    '                        "Merci de nous confirmer la disponibilite et le delai de livraison.\\n\\n"\r\n'
    '                        "Cordialement,\\nL\'equipe WasteGuard")\r\n'
    '                        .arg(itemLines)\r\n'
    '                        .arg(safeLines)\r\n'
    '                        .arg(totalSafe)\r\n'
    '                        .arg(QString::number(totalCostSafe, \'f\', 2));'
)

if old_safe_body in content:
    content = content.replace(old_safe_body, new_safe_body)
    print("[OK] Fix 4: Safe zone email body")
else:
    print("[ERROR] Fix 4 not found - trying to match by key fragment")
    # Try line-by-line targeted replacement
    import re
    # Match the broken safe body block
    pattern = (
        r'(                    QString body = QString::fromUtf8\(\r?\n'
        r'                        "Bonjour,\r?\n'
        r'\r?\n'
        r'"\r?\n'
        r'.*?'
        r'                        \.arg\(QString::number\(totalCostSafe, \'f\', 2\)\);)'
    )
    m = re.search(pattern, content, re.DOTALL)
    if m:
        content = content.replace(m.group(0), new_safe_body)
        print("[OK] Fix 4: Safe zone email body (regex)")
    else:
        print("[ERROR] Fix 4 regex also failed")

# ── Fix 5: Optimal zone email body (broken multi-line) ───────────────────────
old_opt_body = (
    '                    QString body = QString::fromUtf8(\r\n'
    '                        \"Bonjour,\r\n'
    '\r\n'
    '\"\r\n'
    '                        \"Suite a l\'analyse IA de notre systeme WasteGuard, nous souhaitons effectuer \"\r\n'
    '                        \"un reapprovisionnement complet (Zone Optimale) pour les articles suivants :\r\n'
    '\r\n'
    '\"\r\n'
    '                        \"%1\r\n'
    '\"\r\n'
    '                        \"QUANTITES DEMANDEES (Zone Optimale - niveau ideal de stock) :\r\n'
    '\"\r\n'
    '                        \"%2\r\n'
    '\"\r\n'
    '                        \"Total commande : %3 unites\r\n'
    '\"\r\n'
    '                        \"Cout estime    : %4 DT\r\n'
    '\r\n'
    '\"\r\n'
    '                        \"Merci de nous confirmer la disponibilite et le delai de livraison.\r\n'
    '\r\n'
    '\"\r\n'
    '                        \"Cordialement,\r\n'
    ' L\'equipe WasteGuard\")\r\n'
    '                        .arg(itemLines)\r\n'
    '                        .arg(optimalLines)\r\n'
    '                        .arg(totalOptimal)\r\n'
    '                        .arg(QString::number(totalCostOptimal, \'f\', 2));'
)

new_opt_body = (
    '                    QString body = QString::fromUtf8(\r\n'
    '                        "Bonjour,\\n\\n"\r\n'
    '                        "Suite a l\'analyse IA de notre systeme WasteGuard, nous souhaitons effectuer "\r\n'
    '                        "un reapprovisionnement complet (Zone Optimale) pour les articles suivants :\\n\\n"\r\n'
    '                        "%1\\n"\r\n'
    '                        "QUANTITES DEMANDEES (Zone Optimale - niveau ideal de stock) :\\n"\r\n'
    '                        "%2\\n"\r\n'
    '                        "Total commande : %3 unites\\n"\r\n'
    '                        "Cout estime    : %4 DT\\n\\n"\r\n'
    '                        "Merci de nous confirmer la disponibilite et le delai de livraison.\\n\\n"\r\n'
    '                        "Cordialement,\\nL\'equipe WasteGuard")\r\n'
    '                        .arg(itemLines)\r\n'
    '                        .arg(optimalLines)\r\n'
    '                        .arg(totalOptimal)\r\n'
    '                        .arg(QString::number(totalCostOptimal, \'f\', 2));'
)

if old_opt_body in content:
    content = content.replace(old_opt_body, new_opt_body)
    print("[OK] Fix 5: Optimal zone email body")
else:
    print("[ERROR] Fix 5 not found - trying regex")
    import re
    pattern2 = (
        r'(                    QString body = QString::fromUtf8\(\r?\n'
        r'                        "Bonjour,\r?\n'
        r'\r?\n'
        r'"\r?\n'
        r'.*?'
        r'                        \.arg\(QString::number\(totalCostOptimal, \'f\', 2\)\);)'
    )
    m2 = re.search(pattern2, content, re.DOTALL)
    if m2:
        content = content.replace(m2.group(0), new_opt_body)
        print("[OK] Fix 5: Optimal zone email body (regex)")
    else:
        print("[ERROR] Fix 5 regex also failed")

with open(filepath, "w", encoding="utf-8") as f:
    f.write(content)

print("[DONE] All fixes applied!")
