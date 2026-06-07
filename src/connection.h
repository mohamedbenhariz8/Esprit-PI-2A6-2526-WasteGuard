#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QString>

class Connection
{
public:
    static Connection* instance();
    bool createConnect();
    bool createConnect(const QString &userName, const QString &password);
    void closeConnection();
    bool isOpen() const;
    QString lastError() const;

private:
    Connection();
    ~Connection();
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    static Connection* p_instance;
    QSqlDatabase db;
};

#endif // CONNECTION_H

