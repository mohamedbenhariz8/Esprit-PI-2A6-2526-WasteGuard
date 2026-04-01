#ifndef INTERVENTION_H
#define INTERVENTION_H

#include <QString>
#include <QDate>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Intervention
{
public:
    Intervention();
    Intervention(int idInter, const QDate &dateInter, const QString &reference,
                 double duree, double cout, const QString &statut,
                 const QString &type, const QString &priorite, int idBac,
                 const QString &technicien = "", const QString &adresse = "", const QString &descript = "");

    // CRUD
    bool ajouter();
    bool modifier();
    bool supprimer(int idInter);
    QSqlQueryModel *afficher(const QString &searchField = "", const QString &searchValue = "", const QString &sortCriteria = "id_inter ASC");
    bool findIdByReference(const QString &reference, int &idInter);
    QString lastError() const;

    // Getters
    int getIdInter() const;
    QDate getDateInter() const;
    QString getReference() const;
    double getDuree() const;
    double getCout() const;
    QString getStatut() const;
    QString getType() const;
    QString getPriorite() const;
    int getIdBac() const;
    int getIdEmp() const;
    QString getTechnicien() const;
    QString getAdresse() const;
    QString getDescript() const;

    // Setters
    void setIdInter(int value);
    void setDateInter(const QDate &value);
    void setReference(const QString &value);
    void setDuree(double value);
    void setCout(double value);
    void setStatut(const QString &value);
    void setType(const QString &value);
    void setPriorite(const QString &value);
    void setIdBac(int value);
    void setIdEmp(int value);
    void setTechnicien(const QString &value);
    void setAdresse(const QString &value);
    void setDescript(const QString &value);

private:
    int m_idInter;
    QDate m_dateInter;
    QString m_reference;
    double m_duree;
    double m_cout;
    QString m_statut;
    QString m_type;
    QString m_priorite;
    int m_idBac;
    int m_idEmp;
    QString m_technicien;
    QString m_adresse;
    QString m_descript;
    QString m_lastError;
};

#endif // INTERVENTION_H
