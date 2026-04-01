#include "intervention.h"
#include <QSqlError>
#include <QVariant>

Intervention::Intervention()
    : m_idInter(0), m_duree(0.0), m_cout(0.0), m_idBac(0), m_idEmp(1)
{
}

Intervention::Intervention(int idInter, const QDate &dateInter, const QString &reference,
                           double duree, double cout, const QString &statut,
                           const QString &type, const QString &priorite, int idBac,
                           const QString &technicien, const QString &adresse, const QString &descript)
    : m_idInter(idInter), m_dateInter(dateInter), m_reference(reference),
      m_duree(duree), m_cout(cout), m_statut(statut),
      m_type(type), m_priorite(priorite), m_idBac(idBac), m_idEmp(1),
      m_technicien(technicien), m_adresse(adresse), m_descript(descript)
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
int Intervention::getIdEmp() const { return m_idEmp; }
QString Intervention::getTechnicien() const { return m_technicien; }
QString Intervention::getAdresse() const { return m_adresse; }
QString Intervention::getDescript() const { return m_descript; }

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
void Intervention::setIdEmp(int value) { m_idEmp = value; }
void Intervention::setTechnicien(const QString &value) { m_technicien = value; }
void Intervention::setAdresse(const QString &value) { m_adresse = value; }
void Intervention::setDescript(const QString &value) { m_descript = value; }

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
        "INSERT INTO INTERVENTION (ID_INTER, DATE_INTER, REFERENCE, DUREE, COUT, STATUT, TYPE, PRIORITE, ID_BAC, ID_EMP, TECHNICIEN, ADRESSE, DESCRIPT) "
        "VALUES (:id, :date_inter, :reference, :duree, :cout, :statut, :type, :priorite, :id_bac, :id_emp, :technicien, :adresse, :descript)"
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
    query.bindValue(":id_emp", m_idEmp);
    query.bindValue(":technicien", m_technicien);
    query.bindValue(":adresse", m_adresse);
    query.bindValue(":descript", m_descript);

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
        "PRIORITE = :priorite, ID_BAC = :id_bac, ID_EMP = :id_emp, "
        "TECHNICIEN = :technicien, ADRESSE = :adresse, DESCRIPT = :descript "
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
    query.bindValue(":id_emp", m_idEmp);
    query.bindValue(":technicien", m_technicien);
    query.bindValue(":adresse", m_adresse);
    query.bindValue(":descript", m_descript);

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

QSqlQueryModel *Intervention::afficher(const QString &searchField, const QString &searchValue, const QString &sortCriteria)
{
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;
    
    // Validate sort criteria against whitelist to prevent SQL injection
    QStringList allowedSortColumns = {
        "id_inter", "id_inter asc", "id_inter desc",
        "reference", "reference asc", "reference desc",
        "date_inter", "date_inter asc", "date_inter desc",
        "duree", "duree asc", "duree desc",
        "cout", "cout asc", "cout desc",
        "statut", "statut asc", "statut desc",
        "type", "type asc", "type desc",
        "priorite", "priorite asc", "priorite desc",
        "priority_custom"
    };
    
    QString safeSortCriteria = "id_inter ASC";
    if (!sortCriteria.isEmpty()) {
        QString lowerCrit = sortCriteria.toLower().trimmed();
        bool found = false;
        for (const QString &col : allowedSortColumns) {
            if (col.toLower() == lowerCrit) {
                if (lowerCrit == "priority_custom") {
                    safeSortCriteria = "CASE UPPER(PRIORITE) WHEN 'URGENTE' THEN 1 WHEN 'HAUTE' THEN 2 WHEN 'NORMALE' THEN 3 WHEN 'FAIBLE' THEN 4 ELSE 5 END ASC";
                } else {
                    safeSortCriteria = col;
                }
                found = true;
                break;
            }
        }
        if (!found && !lowerCrit.contains(" ")) {
             for (const QString &col : allowedSortColumns) {
                 if (col.toLower().startsWith(lowerCrit + " ")) {
                     safeSortCriteria = col;
                     break;
                 }
             }
        }
    }
    
    QString queryString = 
        "SELECT ID_INTER, REFERENCE, DATE_INTER, DUREE, COUT, STATUT, TYPE, PRIORITE, ID_BAC, ID_EMP, TECHNICIEN, ADRESSE, DESCRIPT "
        "FROM INTERVENTION ";
    
    if (!searchValue.isEmpty()) {
        if (searchField.isEmpty() || searchField.toLower() == "all") {
            queryString += "WHERE (UPPER(REFERENCE) LIKE '%' || UPPER(:search) || '%' "
                           "OR UPPER(STATUT) LIKE '%' || UPPER(:search) || '%' "
                           "OR UPPER(PRIORITE) LIKE '%' || UPPER(:search) || '%' "
                           "OR TO_CHAR(DATE_INTER, 'YYYY-MM-DD') LIKE '%' || :search || '%') ";
        } else {
            QString colName = searchField.toLower() == "reference" ? "REFERENCE" : searchField;
            if (searchField.toLower() == "date") {
                queryString += "WHERE TO_CHAR(DATE_INTER, 'YYYY-MM-DD') LIKE '%' || :search || '%' ";
            } else {
                queryString += "WHERE UPPER(" + colName + ") LIKE '%' || UPPER(:search) || '%' ";
            }
        }
        query.prepare(queryString + "ORDER BY " + safeSortCriteria);
        query.bindValue(":search", searchValue.trimmed());
    } else {
        query.prepare(queryString + "ORDER BY " + safeSortCriteria);
    }
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
    }
    model->setQuery(std::move(query));
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
