#ifndef EMPLOYE_H
#define EMPLOYE_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Employe
{
    int id_emp;
    QString matricule, cin, nom, email, specialite, disponibilite;

public:
    Employe();
    Employe(int id_emp, QString matricule, QString cin, QString nom,
            QString email, QString specialite, QString disponibilite);

    // CRUD
    bool ajouter();
    bool supprimer(int id_emp);
    bool modifier();
    QSqlQueryModel *afficher();
    bool findIdByMatricule(const QString &matricule, int &id_emp);
    QString lastError() const;

    // getters
    int getIdEmp() const;
    QString getMatricule() const;
    QString getCin() const;
    QString getNom() const;
    QString getEmail() const;
    QString getSpecialite() const;
    QString getDisponibilite() const;

    // setters
    void setIdEmp(int value);
    void setMatricule(const QString &value);
    void setCin(const QString &value);
    void setNom(const QString &value);
    void setEmail(const QString &value);
    void setSpecialite(const QString &value);
    void setDisponibilite(const QString &value);

private:
    QString m_lastError;
};

#endif // EMPLOYE_H
