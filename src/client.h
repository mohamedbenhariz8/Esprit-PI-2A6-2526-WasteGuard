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
           const QString &telephone, const QString &dateCreationContrat = "",
           const QString &dateExpiration = "", int tauxTri = 0);

    // CRUD
    bool ajouter();
    bool modifier();
    bool supprimer(int idClient);
    QSqlQueryModel *afficher(const QString &searchField = "", const QString &searchValue = "", const QString &sortCriteria = "id_client ASC");
    bool findIdByMatricule(const QString &matricule, int &idClient);
    QString lastError() const;

    // Getters
    int getIdClient() const;
    QString getNom() const;
    QString getMatricule() const;
    QString getEmail() const;
    QString getTypeContrat() const;
    QString getTelephone() const;
    QString getDateCreationContrat() const;
    QString getDateExpiration() const;
    int getTauxTri() const;

    // Setters
    void setIdClient(int value);
    void setNom(const QString &value);
    void setMatricule(const QString &value);
    void setEmail(const QString &value);
    void setTypeContrat(const QString &value);
    void setTelephone(const QString &value);
    void setDateCreationContrat(const QString &value);
    void setDateExpiration(const QString &value);
    void setTauxTri(int value);

private:
    int m_idClient;
    QString m_nom;
    QString m_matricule;
    QString m_email;
    QString m_typeContrat;
    QString m_telephone;
    QString m_dateCreationContrat;
    QString m_dateExpiration;
    int m_tauxTri;
    QString m_lastError;
};

#endif // CLIENT_H
