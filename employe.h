#ifndef EMPLOYE_H
#define EMPLOYE_H

#include <QString>
#include <QByteArray>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Employe
{
    int id_emp;
    int salaire;
    QString matricule, cin, nom, email, specialite, disponibilite, rfid;
    QByteArray photo;

public:
    Employe();
    Employe(int id_emp, QString matricule, QString cin, QString nom,
            QString email, QString specialite, QString disponibilite,
            int salaire,
            const QByteArray &photo = QByteArray());
    Employe(int id_emp, QString matricule, QString cin, QString nom,
            QString email, QString specialite, QString disponibilite,
            const QByteArray &photo = QByteArray());

    // CRUD
    bool ajouter();
    bool supprimer(int id_emp);
    bool modifier();
    QSqlQueryModel *afficher();
    bool findIdByMatricule(const QString &matricule, int &id_emp);

    // RFID helpers (pointage entree/sortie)
    enum PointageResult {
        PointageEntree,       // badge accepted, employee clocked IN
        PointageSortie,       // badge accepted, employee clocked OUT
        PointageError         // SQL / session error (see lastError)
    };
    bool findIdByRfid(const QString &rfidTag, int &id_emp);
    bool resetDailyAttendance();
    bool markPresentById(int id_emp);
    PointageResult pointById(int id_emp);
    double computeTodayHours(int id_emp);
    double computeMonthHours(int id_emp);
    bool isCurrentlyPresent(int id_emp);

    QString lastError() const;

    // getters
    int getIdEmp() const;
    QString getMatricule() const;
    QString getCin() const;
    QString getNom() const;
    QString getEmail() const;
    QString getSpecialite() const;
    QString getDisponibilite() const;
    int getSalaire() const;
    QByteArray getPhoto() const;
    QString getRfid() const;

    // setters
    void setIdEmp(int value);
    void setMatricule(const QString &value);
    void setCin(const QString &value);
    void setNom(const QString &value);
    void setEmail(const QString &value);
    void setSpecialite(const QString &value);
    void setDisponibilite(const QString &value);
    void setSalaire(int value);
    void setPhoto(const QByteArray &value);
    void setRfid(const QString &value);

private:
    QString m_lastError;
};

#endif // EMPLOYE_H
