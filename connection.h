#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
class Connection
{
public:
    // Returns the unique instance (Singleton)
    static Connection& createInstance();

    // Method to open the database connection
    bool createConnection();

private:
    QSqlDatabase db;

    // Private constructor & destructor
    Connection();
    ~Connection();

    // Disable copy constructor and assignment operator
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
};

#endif // CONNECTION_H
