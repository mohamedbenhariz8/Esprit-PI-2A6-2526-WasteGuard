import sys
import re

path = r'c:\Users\Rudy\Desktop\INTEGRATION_FINALE1111\INTEGRATION_FINALE1\mainwindow.cpp'
with open(path, 'r', encoding='utf-8', errors='ignore') as f:
    content = f.read()

# Pattern to find the loop in refreshCalendarEvents
# We'll match from the loop start to the end of the loop
pattern = r'for\s*\(auto\s+it\s*=\s*dayStatuses\.begin\(\);\s*it\s*!=\s*dayStatuses\.end\(\);\s*\+\+it\)\s*\{.*?calendar->setDayDots\(it\.key\(\),\s*dots\);.*?\}'

new_loop = """for (auto it = dayStatuses.begin(); it != dayStatuses.end(); ++it) {
        QList<QColor> dots;
        for (const QString &s : it.value()) {
            QString statut = s.toUpper();
            if (statut.contains("PREPARA")) 
                dots.append(QColor(255, 235, 59)); // Yellow (En préparation)
            else if (statut.contains("COURS") || statut.contains("LIVRAISON")) 
                dots.append(QColor(255, 152, 0)); // Orange (En cours)
            else if (statut.contains("LIVRE")) 
                dots.append(QColor(76, 175, 80));  // Green (Livré)
            else if (statut.contains("ANNUL")) 
                dots.append(QColor(244, 67, 54));  // Red (Annulé)
        }
        calendar->setDayDots(it.key(), dots);
    }"""

new_content = re.sub(pattern, new_loop, content, flags=re.DOTALL)

# Also fix checkScheduledDeliveries while we are at it
pattern2 = r'updateQuery\.exec\("UPDATE\s+COMMANDE\s+SET\s+STATUT\s*=\s*\'En\s+cours\s+de\s+livraison\'\s+"\s+"WHERE\s+STATUT\s*=\s*\'En\s+.*?\s+AND\s+DATE_LIVRAISON\s*<=\s*CURRENT_DATE"\);'
new_query = 'updateQuery.exec("UPDATE COMMANDE SET STATUT = \'En cours de livraison\' WHERE (STATUT LIKE \'%PREPARA%\' OR STATUT LIKE \'%Pr%paration%\') AND DATE_LIVRAISON <= CURRENT_DATE");'
new_content = re.sub(pattern2, new_query, new_content, flags=re.DOTALL)

with open(path, 'w', encoding='utf-8') as f:
    f.write(new_content)
print("Successfully updated refreshCalendarEvents with robust status matching")
