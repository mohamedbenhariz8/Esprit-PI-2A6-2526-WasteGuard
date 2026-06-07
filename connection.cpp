#include "connection.h"

#include <QSqlError>
#include <QDebug>
#include <QProcessEnvironment>

// Database connection settings are read from environment variables so that
// no credentials are stored in source control. See .env.example for the
// list of variables. Defaults match the local Oracle dev instance, except
// the password, which MUST be provided via WASTEGUARD_DB_PASSWORD.
namespace {
QString dbEnv(const char *key, const QString &fallback = QString())
{
    const QString value = QProcessEnvironment::systemEnvironment().value(key);
    return value.isEmpty() ? fallback : value;
}
}

Connection* Connection::p_instance = nullptr;

Connection::Connection()
{
    db = QSqlDatabase::addDatabase("QODBC");
}

Connection* Connection::instance()
{
    if (!p_instance) {
        p_instance = new Connection();
    }
    return p_instance;
}

bool Connection::createConnect()
{
    if (db.isOpen()) {
        db.close();
    }

    db.setDatabaseName(dbEnv("WASTEGUARD_DB_NAME", "Source_Projet2A"));
    db.setUserName(dbEnv("WASTEGUARD_DB_USER", "wasteguard"));
    db.setPassword(dbEnv("WASTEGUARD_DB_PASSWORD"));

    if (db.open()) {
        qDebug() << "Connexion a la base de donnees reussie";
        return true;
    }

    qDebug() << "Erreur de connexion:" << db.lastError().text();
    return false;
}

bool Connection::createConnect(const QString &userName, const QString &password)
{
    if (db.isOpen()) {
        db.close();
    }

    const QString user = userName.trimmed();

    db.setDatabaseName(dbEnv("WASTEGUARD_DB_NAME", "Source_Projet2A"));
    db.setUserName(user);
    db.setPassword(password);

    const bool ok = db.open();
    if (ok) {
        qDebug() << "Connexion base reussie pour" << db.userName();
    } else {
        qDebug() << "Erreur de connexion:" << db.lastError().text();
    }
    return ok;
}

void Connection::closeConnection()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool Connection::isOpen() const
{
    return db.isOpen();
}

QString Connection::lastError() const
{
    return db.lastError().text();
}

Connection::~Connection()
{
    closeConnection();
}

