#include "connection.h"

#include <QSqlError>
#include <QDebug>
#include <QProcessEnvironment>

namespace {
void applyOdbcTimeoutOptions(QSqlDatabase &db)
{
    db.setConnectOptions(
        "SQL_ATTR_LOGIN_TIMEOUT=5;"
        "SQL_ATTR_CONNECTION_TIMEOUT=5;"
        "SQL_ATTR_QUERY_TIMEOUT=5"
    );
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

    const QString dsn = qEnvironmentVariable("WG_DSN", "Source_Projet2A").trimmed();
    const QString envUser = qEnvironmentVariable("WG_DB_USER").trimmed();
    const QString envPass = qEnvironmentVariable("WG_DB_PASS");

    // 1) Try DSN as configured in ODBC (no forced credentials).
    applyOdbcTimeoutOptions(db);
    db.setDatabaseName(dsn);
    db.setUserName(QString());
    db.setPassword(QString());
    if (db.open()) {
        qDebug() << "Connexion base reussie via DSN (sans credentials forces):" << dsn;
        return true;
    }
    const QString errDsnNoCred = db.lastError().text();

    // 2) Try DSN connection string form.
    applyOdbcTimeoutOptions(db);
    db.setDatabaseName(QString("DSN=%1;").arg(dsn));
    db.setUserName(QString());
    db.setPassword(QString());
    if (db.open()) {
        qDebug() << "Connexion base reussie via chaine ODBC DSN=" << dsn;
        return true;
    }
    const QString errDsnString = db.lastError().text();

    // 3) If env credentials are provided, try them.
    if (!envUser.isEmpty()) {
        if (createConnect(envUser, envPass)) {
            return true;
        }
    }

    // 4) Legacy fallback for existing setups.
    if (createConnect("wasteguard", "123")) {
        return true;
    }

    qDebug() << "Erreur de connexion ODBC pour DSN" << dsn
             << "| try1:" << errDsnNoCred
             << "| try2:" << errDsnString
             << "| final:" << db.lastError().text();
    return false;
}

bool Connection::createConnect(const QString &userName, const QString &password)
{
    if (db.isOpen()) {
        db.close();
    }

    const QString dsn = qEnvironmentVariable("WG_DSN", "Source_Projet2A").trimmed();
    const QString user = userName.trimmed();

    applyOdbcTimeoutOptions(db);
    db.setDatabaseName(dsn);
    db.setUserName(user);
    db.setPassword(password);

    const bool ok = db.open();
    if (ok) {
        qDebug() << "Connexion base reussie pour" << db.userName() << "sur DSN" << dsn;
    } else {
        const QString err1 = db.lastError().text();

        // Retry with ODBC connection string format.
        applyOdbcTimeoutOptions(db);
        db.setDatabaseName(QString("DSN=%1;UID=%2;PWD=%3;")
                               .arg(dsn, user, password));
        db.setUserName(QString());
        db.setPassword(QString());
        if (db.open()) {
            qDebug() << "Connexion base reussie via chaine ODBC pour" << user << "sur DSN" << dsn;
            return true;
        }

        qDebug() << "Erreur de connexion:" << err1
                 << "| retry:" << db.lastError().text();
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

