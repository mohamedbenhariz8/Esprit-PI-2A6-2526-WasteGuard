import pyodbc
conn = pyodbc.connect('DSN=Source_Projet2A;UID=WasteGuard;PWD=esprit')
cursor = conn.cursor()
cursor.execute("SELECT search_condition FROM user_constraints WHERE table_name = 'INTERVENTIONS' AND constraint_type = 'C'")
for row in cursor.fetchall():
    print(row[0])
