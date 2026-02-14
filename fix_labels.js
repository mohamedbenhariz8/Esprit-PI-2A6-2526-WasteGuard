const fs = require('fs');
const path = 'C:\\Users\\gigabyte\\Downloads\\integration (1)\\integration\\mainwindow.ui';
let content = fs.readFileSync(path, 'utf8');

// Maintenance
content = content.replace(/(<widget [^>]*name="st_lbl1">[\s\S]*?<string>).*?(<\/string>)/, '$1Total Interventions$2');
content = content.replace(/(<widget [^>]*name="st_lbl2">[\s\S]*?<string>).*?(<\/string>)/, '$1Total Coût$2');
content = content.replace(/(<widget [^>]*name="st_lbl3">[\s\S]*?<string>).*?(<\/string>)/, '$1Nombre des heures de maintenance$2');

// Style (Maintenance)
content = content.replace(/(<widget [^>]*name="stat3">[\s\S]*?<string>)alertCard(<\/string>)/, '$1statCard$2');
content = content.replace(/(<widget [^>]*name="st_val3">[\s\S]*?<string>)alertValue(<\/string>)/, '$1statValue$2');
content = content.replace(/(<widget [^>]*name="st_val3">[\s\S]*?<string>).*?(<\/string>)/, '$10$2');

// Commande
content = content.replace(/(<widget [^>]*name="st_lbl1_Cmd">[\s\S]*?<string>).*?(<\/string>)/, '$1Total des Commandes$2');
content = content.replace(/(<widget [^>]*name="st_lbl2_Cmd">[\s\S]*?<string>).*?(<\/string>)/, '$1Chiffre d\'Affaire$2');
content = content.replace(/(<widget [^>]*name="st_lbl3_Cmd">[\s\S]*?<string>).*?(<\/string>)/, '$1Bacs Vendus$2');

fs.writeFileSync(path, content, 'utf8');
console.log('Successfully updated mainwindow.ui');
