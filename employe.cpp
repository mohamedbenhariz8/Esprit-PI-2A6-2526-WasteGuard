#include "employe.h"
#include <QSqlError>
#include <QVariant>

Employe::Employe()
{
    id_emp = 0;
}

Employe::Employe(int id_emp, QString matricule, QString cin, QString nom,
                 QString email, QString specialite, QString disponibilite)
{
    this->id_emp = id_emp;
    this->matricule = matricule;
    this->cin = cin;
    this->nom = nom;
    this->email = email;
    this->specialite = specialite;
    this->disponibilite = disponibilite;
}


int Employe::getIdEmp() const { return id_emp; }
QString Employe::getMatricule() const { return matricule; }
QString Employe::getCin() const { return cin; }
QString Employe::getNom() const { return nom; }
QString Employe::getEmail() const { return email; }
QString Employe::getSpecialite() const { return specialite; }
QString Employe::getDisponibilite() const { return disponibilite; }


void Employe::setIdEmp(int value) { id_emp = value; }
void Employe::setMatricule(const QString &value) { matricule = value; }
void Employe::setCin(const QString &value) { cin = value; }
void Employe::setNom(const QString &value) { nom = value; }
void Employe::setEmail(const QString &value) { email = value; }
void Employe::setSpecialite(const QString &value) { specialite = value; }
void Employe::setDisponibilite(const QString &value) { disponibilite = value; }


bool Employe::ajouter()
{
    QSqlQuery query;
    if (id_emp > 0) {
        query.prepare(
            "INSERT INTO EMPLOYE (id_emp, matricule, cin, nom, email, specialite, disponibilite) "
            "VALUES (:id_emp, :matricule, :cin, :nom, :email, :specialite, :disponibilite)"
            );
        query.bindValue(":id_emp", id_emp);
    } else {
        // Auto-generate the next id_emp since Oracle has no auto-increment
        QSqlQuery idQuery;
        if (idQuery.exec("SELECT NVL(MAX(ID_EMP), 0) + 1 FROM EMPLOYE") && idQuery.next()) {
            id_emp = idQuery.value(0).toInt();
        } else {
            m_lastError = "Erreur generation ID employe.";
            return false;
        }
        query.prepare(
            "INSERT INTO EMPLOYE (id_emp, matricule, cin, nom, email, specialite, disponibilite) "
            "VALUES (:id_emp, :matricule, :cin, :nom, :email, :specialite, :disponibilite)"
            );
        query.bindValue(":id_emp", id_emp);
    }
    query.bindValue(":matricule", matricule);
    query.bindValue(":cin", cin);
    query.bindValue(":nom", nom);
    query.bindValue(":email", email);
    query.bindValue(":specialite", specialite);
    query.bindValue(":disponibilite", disponibilite);

    const bool ok = query.exec();
    m_lastError = ok ? QString() : query.lastError().text();
    return ok;
}


bool Employe::supprimer(int id_emp)
{
    QSqlQuery query;
    query.prepare("DELETE FROM EMPLOYE WHERE id_emp = :id_emp");
    query.bindValue(":id_emp", id_emp);
    const bool ok = query.exec();
    m_lastError = ok ? QString() : query.lastError().text();
    return ok;
}


bool Employe::modifier()
{
    QSqlQuery query;
    query.prepare(
        "UPDATE EMPLOYE SET "
        "matricule=:matricule, cin=:cin, nom=:nom, email=:email, "
        "specialite=:specialite, disponibilite=:disponibilite "
        "WHERE id_emp=:id_emp"
        );

    query.bindValue(":id_emp", id_emp);
    query.bindValue(":matricule", matricule);
    query.bindValue(":cin", cin);
    query.bindValue(":nom", nom);
    query.bindValue(":email", email);
    query.bindValue(":specialite", specialite);
    query.bindValue(":disponibilite", disponibilite);

    const bool ok = query.exec();
    m_lastError = ok ? QString() : query.lastError().text();
    return ok;
}

QSqlQueryModel *Employe::afficher()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT id_emp, matricule, cin, nom, email, specialite, disponibilite FROM EMPLOYE");
    m_lastError = model->lastError().isValid() ? model->lastError().text() : QString();
    return model;
}

bool Employe::findIdByMatricule(const QString &matricule, int &id_emp)
{
    id_emp = -1;
    QSqlQuery query;
    query.prepare("SELECT id_emp FROM EMPLOYE WHERE matricule = :matricule");
    query.bindValue(":matricule", matricule);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    if (query.next()) {
        id_emp = query.value(0).toInt();
        m_lastError.clear();
        return true;
    }

    m_lastError = "Employe introuvable.";
    return false;
}

QString Employe::lastError() const
{
    return m_lastError;
}
