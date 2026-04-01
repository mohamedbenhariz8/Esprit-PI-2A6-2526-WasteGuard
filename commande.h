#ifndef COMMANDE_H
#define COMMANDE_H

#include <QString>
#include <QSqlQueryModel>
#include <QDate>

class Commande
{
public:
    Commande();
    Commande(int id, int idClient, int qte, const QString &priorite,
             const QString &statut, const QDate &dateCommande,
             const QDate &dateLivraison, double prixTotal);

    bool ajouter();
    bool modifier();
    bool supprimer(int id);
    QSqlQueryModel *afficher(const QString &searchField = "", const QString &searchValue = "", const QString &sortCriteria = "id_commande ASC");
    
    // Helper to resolve client
    int findClientByMatricule(const QString &matricule);
    QString getClientMatricule(int idClient);

    QString lastError() const;

    int id() const;
    QString refer() const;
    int idClient() const;
    int qte() const;
    QString priorite() const;
    QString statut() const;
    QDate dateCommande() const;
    QDate dateLivraison() const;
    double prixTotal() const;
    QString adresse() const;

    void setId(int id);
    void setRefer(const QString &refer);
    void setIdClient(int idClient);
    void setQte(int qte);
    void setPriorite(const QString &priorite);
    void setStatut(const QString &statut);
    void setDateCommande(const QDate &date);
    void setDateLivraison(const QDate &date);
    void setPrixTotal(double prix);
    void setAdresse(const QString &adresse);

private:
    int m_id;
    QString m_refer;
    int m_idClient;
    int m_qte;
    QString m_priorite;
    QString m_statut;
    QString m_dateCmdStr;
    QString m_dateLivStr;
    double m_prixTotal;
    QString m_adresse;
    QString m_lastError;

    // We'll keep QDate versions for getters if needed, but primary focus is now the strings
    QDate m_dateCommande;
    QDate m_dateLivraison;
public:
    void setDateCmdStr(const QString &s) { m_dateCmdStr = s; }
    void setDateLivStr(const QString &s) { m_dateLivStr = s; }
};

#endif // COMMANDE_H
