#include "connection.h"

// Return the unique instance (Singleton)
Connection& Connection::createInstance()
{
    static Connection instance;   // Created once only
    return instance;
}

// Private constructor
Connection::Connection()
{
    db = QSqlDatabase::addDatabase("QODBC");
}

// Destructor
Connection::~Connection()
{
    if(db.isOpen())
        db.close();
}

// Create and open connection
bool Connection::createConnection()
{
    db.setDatabaseName("Source_Projet2A");  // Your ODBC name
    db.setUserName("wasteguard");             // Your username
    db.setPassword("projet");                 // Your password

    if(db.open())
    {
        qDebug() << "Connexion réussie !";
        return true;
    }
    else
    {
        qDebug() << "Erreur :" << db.lastError().text();
        return false;
    }
}
