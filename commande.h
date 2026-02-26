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
    QSqlQueryModel *afficher();
    
    // Helper to resolve client
    int findClientByMatricule(const QString &matricule);
    QString getClientMatricule(int idClient);

    QString lastError() const;

    int id() const;
    int idClient() const;
    int qte() const;
    QString priorite() const;
    QString statut() const;
    QDate dateCommande() const;
    QDate dateLivraison() const;
    double prixTotal() const;
    QString adresse() const;

    void setId(int id);
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
    int m_idClient;
    int m_qte;
    QString m_priorite;
    QString m_statut;
    QDate m_dateCommande;
    QDate m_dateLivraison;
    double m_prixTotal;
    QString m_adresse;
    QString m_lastError;
};

#endif // COMMANDE_H
