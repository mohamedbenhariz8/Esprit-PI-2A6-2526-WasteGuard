#include "intervention.h"
#include <QSqlError>
#include <QVariant>

Intervention::Intervention()
    : m_idInter(0), m_duree(0.0), m_cout(0.0), m_idBac(0)
{
}

Intervention::Intervention(int idInter, const QDate &dateInter, const QString &reference,
                           double duree, double cout, const QString &statut,
                           const QString &type, const QString &priorite, int idBac)
    : m_idInter(idInter), m_dateInter(dateInter), m_reference(reference),
      m_duree(duree), m_cout(cout), m_statut(statut),
      m_type(type), m_priorite(priorite), m_idBac(idBac)
{
}

// ---------- Getters ----------
int Intervention::getIdInter() const { return m_idInter; }
QDate Intervention::getDateInter() const { return m_dateInter; }
QString Intervention::getReference() const { return m_reference; }
double Intervention::getDuree() const { return m_duree; }
double Intervention::getCout() const { return m_cout; }
QString Intervention::getStatut() const { return m_statut; }
QString Intervention::getType() const { return m_type; }
QString Intervention::getPriorite() const { return m_priorite; }
int Intervention::getIdBac() const { return m_idBac; }

// ---------- Setters ----------
void Intervention::setIdInter(int value) { m_idInter = value; }
void Intervention::setDateInter(const QDate &value) { m_dateInter = value; }
void Intervention::setReference(const QString &value) { m_reference = value; }
void Intervention::setDuree(double value) { m_duree = value; }
void Intervention::setCout(double value) { m_cout = value; }
void Intervention::setStatut(const QString &value) { m_statut = value; }
void Intervention::setType(const QString &value) { m_type = value; }
void Intervention::setPriorite(const QString &value) { m_priorite = value; }
void Intervention::setIdBac(int value) { m_idBac = value; }

// ---------- CRUD ----------
bool Intervention::ajouter()
{
    QSqlQuery query;
    int nextId = m_idInter;
    if (nextId <= 0) {
        // Use a robust ID that avoids collisions even with gaps from deleted rows
        QSqlQuery idQuery;
        if (idQuery.exec("SELECT NVL(MAX(ID_INTER), 0) + 1 FROM INTERVENTION") && idQuery.next()) {
            nextId = idQuery.value(0).toInt();
        } else {
            m_lastError = "Erreur generation ID intervention.";
            return false;
        }
        // Ensure the generated ID does not already exist (handle edge cases)
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT COUNT(*) FROM INTERVENTION WHERE ID_INTER = :id");
        checkQuery.bindValue(":id", nextId);
        while (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
            nextId++;
            checkQuery.bindValue(":id", nextId);
        }
    }

    // Check for duplicate REFERENCE before inserting
    QSqlQuery refCheck;
    refCheck.prepare("SELECT COUNT(*) FROM INTERVENTION WHERE UPPER(REFERENCE) = UPPER(:ref)");
    refCheck.bindValue(":ref", m_reference.trimmed());
    if (refCheck.exec() && refCheck.next() && refCheck.value(0).toInt() > 0) {
        m_lastError = "Une intervention avec cette référence existe déjà.";
        return false;
    }

    query.prepare(
        "INSERT INTO INTERVENTION (ID_INTER, DATE_INTER, REFERENCE, DUREE, COUT, STATUT, TYPE, PRIORITE, ID_BAC) "
        "VALUES (:id, :date_inter, :reference, :duree, :cout, :statut, :type, :priorite, :id_bac)"
    );
    query.bindValue(":id", nextId);
    query.bindValue(":date_inter", m_dateInter);
    query.bindValue(":reference", m_reference);
    query.bindValue(":duree", m_duree);
    query.bindValue(":cout", m_cout);
    query.bindValue(":statut", m_statut);
    query.bindValue(":type", m_type);
    query.bindValue(":priorite", m_priorite);
    query.bindValue(":id_bac", m_idBac);

    if (query.exec()) {
        m_idInter = nextId;
        m_lastError.clear();
        return true;
    }
    m_lastError = query.lastError().text();
    return false;
}

bool Intervention::modifier()
{
    QSqlQuery query;
    query.prepare(
        "UPDATE INTERVENTION SET "
        "DATE_INTER = :date_inter, REFERENCE = :reference, DUREE = :duree, "
        "COUT = :cout, STATUT = :statut, TYPE = :type, "
        "PRIORITE = :priorite, ID_BAC = :id_bac "
        "WHERE ID_INTER = :id"
    );
    query.bindValue(":id", m_idInter);
    query.bindValue(":date_inter", m_dateInter);
    query.bindValue(":reference", m_reference);
    query.bindValue(":duree", m_duree);
    query.bindValue(":cout", m_cout);
    query.bindValue(":statut", m_statut);
    query.bindValue(":type", m_type);
    query.bindValue(":priorite", m_priorite);
    query.bindValue(":id_bac", m_idBac);

    if (query.exec()) {
        m_lastError.clear();
        return true;
    }
    m_lastError = query.lastError().text();
    return false;
}

bool Intervention::supprimer(int idInter)
{
    // First delete from EFFECTUATION (child table)
    QSqlQuery delEff;
    delEff.prepare("DELETE FROM EFFECTUATION WHERE ID_INTER = :id");
    delEff.bindValue(":id", idInter);
    delEff.exec(); // ignore error if no rows

    QSqlQuery query;
    query.prepare("DELETE FROM INTERVENTION WHERE ID_INTER = :id");
    query.bindValue(":id", idInter);
    if (query.exec()) {
        m_lastError.clear();
        return true;
    }
    m_lastError = query.lastError().text();
    return false;
}

QSqlQueryModel *Intervention::afficher()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(
        "SELECT ID_INTER, REFERENCE, DATE_INTER, DUREE, COUT, STATUT, TYPE, PRIORITE, ID_BAC "
        "FROM INTERVENTION ORDER BY ID_INTER"
    );
    m_lastError = model->lastError().isValid() ? model->lastError().text() : QString();
    return model;
}

bool Intervention::findIdByReference(const QString &reference, int &idInter)
{
    idInter = -1;
    QSqlQuery query;
    query.prepare("SELECT ID_INTER FROM INTERVENTION WHERE UPPER(REFERENCE) = UPPER(:ref)");
    query.bindValue(":ref", reference.trimmed());

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    if (query.next()) {
        idInter = query.value(0).toInt();
        m_lastError.clear();
        return true;
    }

    m_lastError = "Intervention introuvable.";
    return false;
}

QString Intervention::lastError() const
{
    return m_lastError;
}
