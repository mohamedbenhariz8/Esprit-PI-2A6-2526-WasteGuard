#ifndef PRODUIT_H
#define PRODUIT_H

#include <QString>
#include <QSqlQueryModel>

class Produit
{
public:
    Produit();
    Produit(int id_mp, const QString &reference, const QString &nom, int quantite, int capacite, double prix, int capaciteBatterie = 10000);

    bool ajouter();
    bool modifier();
    bool supprimer(int id_mp);
    QSqlQueryModel *afficher();
    bool findIdByReference(const QString &reference, int &id_mp);
    QString lastError() const;

    int getIdMp() const;
    QString getReference() const;
    QString getNom() const;
    int getQuantite() const;
    int getCapacite() const;
    double getPrix() const;
    int getCapaciteBatterie() const;

    void setIdMp(int value);
    void setReference(const QString &value);
    void setNom(const QString &value);
    void setQuantite(int value);
    void setCapacite(int value);
    void setPrix(double value);
    void setCapaciteBatterie(int value);

private:
    int m_idMp;
    QString m_reference;
    QString m_nom;
    int m_quantite;
    int m_capacite;
    double m_prix;
    int m_capaciteBatterie;
    QString m_lastError;
};

#endif // PRODUIT_H
