#!/usr/bin/env python3
"""
Remove estimated cost/price from the Smart Cart supplier email bodies.
Targets:
  1. safeLines per-item string: remove "(%3 DT)" part
  2. optimalLines per-item string: remove "(%3 DT)" part
  3. Safe zone email body: remove "Cout estime" line + its .arg()
  4. Optimal zone email body: remove "Cout estime" line + its .arg()
  5. btnSafe label: remove "/ %2 DT" part
  6. btnOptimal label: remove "/ %2 DT" part
  7. Lambda captures: remove unused totalCostSafe / totalCostOptimal
"""

filepath = "mainwindow.cpp"

with open(filepath, "r", encoding="utf-8") as f:
    content = f.read()

changes = 0

# ── 1. safeLines: "  - %1 : %2 unites (%3 DT)\n" → "  - %1 : %2 unites\n" ──
old1 = '''                safeLines += QString("  - %1 : %2 unites (%3 DT)\\n")
                    .arg(det.nom).arg(qtySafe)
                    .arg(QString::number(qtySafe * det.prix, 'f', 2));'''
new1 = '''                safeLines += QString("  - %1 : %2 unites\\n")
                    .arg(det.nom).arg(qtySafe);'''
if old1 in content:
    content = content.replace(old1, new1); changes += 1; print("[OK] 1. safeLines")
else:
    print("[WARN] 1. safeLines not found")

# ── 2. optimalLines: same ──────────────────────────────────────────────────
old2 = '''                optimalLines += QString("  - %1 : %2 unites (%3 DT)\\n")
                    .arg(det.nom).arg(qtyOptimal)
                    .arg(QString::number(qtyOptimal * det.prix, 'f', 2));'''
new2 = '''                optimalLines += QString("  - %1 : %2 unites\\n")
                    .arg(det.nom).arg(qtyOptimal);'''
if old2 in content:
    content = content.replace(old2, new2); changes += 1; print("[OK] 2. optimalLines")
else:
    print("[WARN] 2. optimalLines not found")

# ── 3. btnSafe label: remove "/ %2 DT" ────────────────────────────────────
old3 = '''                QString::fromUtf8("Envoyer email - Zone Securisee (%1 u. / %2 DT)")
                    .arg(totalSafe).arg(QString::number(totalCostSafe, 'f', 2)),'''
new3 = '''                QString::fromUtf8("Envoyer email - Zone Securisee (%1 unites)")
                    .arg(totalSafe),'''
if old3 in content:
    content = content.replace(old3, new3); changes += 1; print("[OK] 3. btnSafe label")
else:
    print("[WARN] 3. btnSafe label not found")

# ── 4. btnOptimal label: remove "/ %2 DT" ─────────────────────────────────
old4 = '''                QString::fromUtf8("Envoyer email - Zone Optimale (%1 u. / %2 DT)")
                    .arg(totalOptimal).arg(QString::number(totalCostOptimal, 'f', 2)),'''
new4 = '''                QString::fromUtf8("Envoyer email - Zone Optimale (%1 unites)")
                    .arg(totalOptimal),'''
if old4 in content:
    content = content.replace(old4, new4); changes += 1; print("[OK] 4. btnOptimal label")
else:
    print("[WARN] 4. btnOptimal label not found")

# ── 5. Safe zone email body: remove "Cout estime" line + .arg(cost) ────────
old5 = '''            connect(btnSafe, &QPushButton::clicked,
                [supplierEmail, supplierName, itemLines, safeLines,
                 totalSafe, totalCostSafe, qtyDlg, dlg]() {
                    QString subject = QString::fromUtf8("Commande Zone Securisee WasteGuard - %1").arg(supplierName);
                    QString body = QString::fromUtf8(
                        "Bonjour,\\n\\n"
                        "Suite a l\'analyse IA de notre systeme WasteGuard, nous avons besoin d\'un "
                        "reapprovisionnement urgent (Zone Securisee) pour les articles suivants :\\n\\n"
                        "%1\\n"
                        "QUANTITES DEMANDEES (Zone Securisee - minimum requis) :\\n"
                        "%2\\n"
                        "Total commande : %3 unites\\n"
                        "Cout estime    : %4 DT\\n\\n"
                        "Merci de nous confirmer la disponibilite et le delai de livraison.\\n\\n"
                        "Cordialement,\\nL\'equipe WasteGuard")
                        .arg(itemLines)
                        .arg(safeLines)
                        .arg(totalSafe)
                        .arg(QString::number(totalCostSafe, 'f', 2));'''
new5 = '''            connect(btnSafe, &QPushButton::clicked,
                [supplierEmail, supplierName, itemLines, safeLines,
                 totalSafe, qtyDlg, dlg]() {
                    QString subject = QString::fromUtf8("Commande Zone Securisee WasteGuard - %1").arg(supplierName);
                    QString body = QString::fromUtf8(
                        "Bonjour,\\n\\n"
                        "Suite a l\'analyse IA de notre systeme WasteGuard, nous avons besoin d\'un "
                        "reapprovisionnement urgent (Zone Securisee) pour les articles suivants :\\n\\n"
                        "%1\\n"
                        "QUANTITES DEMANDEES (Zone Securisee - minimum requis) :\\n"
                        "%2\\n"
                        "Total commande : %3 unites\\n\\n"
                        "Merci de nous confirmer la disponibilite et le delai de livraison.\\n\\n"
                        "Cordialement,\\nL\'equipe WasteGuard")
                        .arg(itemLines)
                        .arg(safeLines)
                        .arg(totalSafe);'''
if old5 in content:
    content = content.replace(old5, new5); changes += 1; print("[OK] 5. Safe zone email body")
else:
    print("[WARN] 5. Safe zone email body not found")

# ── 6. Optimal zone email body: remove "Cout estime" line + .arg(cost) ─────
old6 = '''            connect(btnOptimal, &QPushButton::clicked,
                [supplierEmail, supplierName, itemLines, optimalLines,
                 totalOptimal, totalCostOptimal, qtyDlg, dlg]() {
                    QString subject = QString::fromUtf8("Commande Zone Optimale WasteGuard - %1").arg(supplierName);
                    QString body = QString::fromUtf8(
                        "Bonjour,\\n\\n"
                        "Suite a l\'analyse IA de notre systeme WasteGuard, nous souhaitons effectuer "
                        "un reapprovisionnement complet (Zone Optimale) pour les articles suivants :\\n\\n"
                        "%1\\n"
                        "QUANTITES DEMANDEES (Zone Optimale - niveau ideal de stock) :\\n"
                        "%2\\n"
                        "Total commande : %3 unites\\n"
                        "Cout estime    : %4 DT\\n\\n"
                        "Merci de nous confirmer la disponibilite et le delai de livraison.\\n\\n"
                        "Cordialement,\\nL\'equipe WasteGuard")
                        .arg(itemLines)
                        .arg(optimalLines)
                        .arg(totalOptimal)
                        .arg(QString::number(totalCostOptimal, 'f', 2));'''
new6 = '''            connect(btnOptimal, &QPushButton::clicked,
                [supplierEmail, supplierName, itemLines, optimalLines,
                 totalOptimal, qtyDlg, dlg]() {
                    QString subject = QString::fromUtf8("Commande Zone Optimale WasteGuard - %1").arg(supplierName);
                    QString body = QString::fromUtf8(
                        "Bonjour,\\n\\n"
                        "Suite a l\'analyse IA de notre systeme WasteGuard, nous souhaitons effectuer "
                        "un reapprovisionnement complet (Zone Optimale) pour les articles suivants :\\n\\n"
                        "%1\\n"
                        "QUANTITES DEMANDEES (Zone Optimale - niveau ideal de stock) :\\n"
                        "%2\\n"
                        "Total commande : %3 unites\\n\\n"
                        "Merci de nous confirmer la disponibilite et le delai de livraison.\\n\\n"
                        "Cordialement,\\nL\'equipe WasteGuard")
                        .arg(itemLines)
                        .arg(optimalLines)
                        .arg(totalOptimal);'''
if old6 in content:
    content = content.replace(old6, new6); changes += 1; print("[OK] 6. Optimal zone email body")
else:
    print("[WARN] 6. Optimal zone email body not found")

with open(filepath, "w", encoding="utf-8") as f:
    f.write(content)

print(f"\n[DONE] {changes}/6 changes applied.")
