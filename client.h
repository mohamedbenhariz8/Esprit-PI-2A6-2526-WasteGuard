#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Client
{
public:
    Client();
    Client(int idClient, const QString &nom, const QString &matricule,
           const QString &email, const QString &typeContrat,
           const QString &statutPaiement);

    // CRUD
    bool ajouter();
    bool modifier();
    bool supprimer(int idClient);
    QSqlQueryModel *afficher();
    bool findIdByMatricule(const QString &matricule, int &idClient);
    QString lastError() const;

    // Getters
    int getIdClient() const;
    QString getNom() const;
    QString getMatricule() const;
    QString getEmail() const;
    QString getTypeContrat() const;
    QString getStatutPaiement() const;

    // Setters
    void setIdClient(int value);
    void setNom(const QString &value);
    void setMatricule(const QString &value);
    void setEmail(const QString &value);
    void setTypeContrat(const QString &value);
    void setStatutPaiement(const QString &value);

private:
    int m_idClient;
    QString m_nom;
    QString m_matricule;
    QString m_email;
    QString m_typeContrat;
    QString m_statutPaiement;
    QString m_lastError;
};

#endif // CLIENT_H
