#ifndef PRODUIT_H
#define PRODUIT_H

#include <QString>
#include <QSqlQueryModel>
#include <QStringList>
#include <QList>
#include <QPair>

class Produit
{
public:
    Produit();
    Produit(int id_mp, const QString &reference, const QString &nom, int quantite, int capacite, double prix, int capaciteBatterie = 10000, 
            const QString &imagePath = QString(), const QString &caracteristiques = QString(), const QString &localisation = QString());

    bool ajouter();
    bool modifier();
    bool supprimer(int id_mp);
    QSqlQueryModel *afficher(const QString &searchModel = "", const QString &sortCriteria = "id_bac ASC");
    bool findIdByReference(const QString &reference, int &id_mp);
    QString lastError() const;
    QList<QPair<int, QString>> fetchAvailableCapteurOptions(int minQty = 1, int idBac = -1, QString *error = nullptr) const;
    QStringList fetchAvailableCapteurFeatures(int minQty = 1, int idBac = -1, QString *error = nullptr) const;
    bool syncFabricationForBac(int idBac, const QList<int> &idMps, int qteUtiliseDefault = 1);
    bool syncFabricationForBacByNames(int idBac, const QStringList &capteurNames, int qteUtiliseDefault = 1);
    bool consumeMatiereForFabrication(int idBac, int produitQty);
    bool restoreMatiereForFabrication(int idBac, int produitQty);

    int getIdMp() const;
    QString getReference() const;
    QString getNom() const;
    int getQuantite() const;
    int getCapacite() const;
    double getPrix() const;
    int getCapaciteBatterie() const;
    QString getImagePath() const;
    QString getCaracteristiques() const;
    QString getLocalisation() const;

    void setIdMp(int value);
    void setReference(const QString &value);
    void setNom(const QString &value);
    void setQuantite(int value);
    void setCapacite(int value);
    void setPrix(double value);
    void setCapaciteBatterie(int value);
    void setImagePath(const QString &value);
    void setCaracteristiques(const QString &value);
    void setLocalisation(const QString &value);

private:
    int m_idMp;
    QString m_reference;
    QString m_nom;
    int m_quantite;
    int m_capacite;
    double m_prix;
    int m_capaciteBatterie;
    QString m_imagePath;
    QString m_caracteristiques;
    QString m_localisation;
    QString m_lastError;
};

#endif // PRODUIT_H
