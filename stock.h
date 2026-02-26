#ifndef STOCK_H
#define STOCK_H

#include <QSqlQueryModel>
#include <QString>

class Stock
{
public:
    Stock();
    Stock(int idMp,
          const QString &reference,
          const QString &nom,
          int quantite,
          int seuilCritique,
          double prix,
          const QString &fournisseurInput = QString());

    bool ajouter();
    bool modifier();
    bool supprimer(int idMp);
    QSqlQueryModel *afficher();
    bool findIdByReference(const QString &reference, int &idMp);

    QString lastError() const;

    int idMp() const;
    QString reference() const;
    QString nom() const;
    int quantite() const;
    int seuilCritique() const;
    double prix() const;
    QString fournisseurInput() const;

    void setIdMp(int value);
    void setReference(const QString &value);
    void setNom(const QString &value);
    void setQuantite(int value);
    void setSeuilCritique(int value);
    void setPrix(double value);
    void setFournisseurInput(const QString &value);

private:
    int resolveNextMpId();
    int resolveNextFournisseurId();
    bool resolveFournisseurIdFromInput(const QString &inputRaw, int &idFour, QString &nomFour);

private:
    int m_idMp;
    QString m_reference;
    QString m_nom;
    int m_quantite;
    int m_seuilCritique;
    double m_prix;
    QString m_fournisseurInput;
    QString m_lastError;
};

#endif // STOCK_H
