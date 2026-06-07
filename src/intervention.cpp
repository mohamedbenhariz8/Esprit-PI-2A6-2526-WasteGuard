#include "intervention.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QVariant>

namespace {

bool ensureInterventionExtendedColumnsExist(QString &errorText)
{
    errorText.clear();

    QSqlDatabase db = QSqlDatabase::database();
    const QString driver = db.driverName().toUpper();
    const bool isOracleLikeDriver =
        driver.contains("QOCI") ||
        driver.contains("OCI") ||
        driver.contains("QODBC") ||
        driver.contains("ODBC");

    struct ColumnSpec {
        const char *name;
        const char *oracleType;
        const char *sqliteType;
    };

    const ColumnSpec columns[] = {
        {"TECHNICIEN", "VARCHAR2(200)", "TEXT"},
        {"ADRESSE", "VARCHAR2(300)", "TEXT"},
        {"DESCRIPT", "VARCHAR2(1000)", "TEXT"},
        {"MAIN_OEUVRE", "NUMBER(12,3)", "REAL"},
        {"PHOTO_AVANT", "VARCHAR2(1000)", "TEXT"},
        {"PHOTO_APRES", "VARCHAR2(1000)", "TEXT"}
    };

    auto columnExists = [&](const QString &columnName) -> bool {
        QSqlQuery q;
        q.prepare(
            "SELECT COUNT(*) "
            "FROM USER_TAB_COLUMNS "
            "WHERE TABLE_NAME = 'INTERVENTION' AND COLUMN_NAME = :col");
        q.bindValue(":col", columnName.toUpper());
        if (q.exec() && q.next()) {
            return q.value(0).toInt() > 0;
        }

        QSqlQuery sqliteQuery;
        if (sqliteQuery.exec("PRAGMA table_info(INTERVENTION)")) {
            while (sqliteQuery.next()) {
                if (sqliteQuery.value(1).toString().trimmed().compare(columnName, Qt::CaseInsensitive) == 0) {
                    return true;
                }
            }
        }

        return false;
    };

    for (const ColumnSpec &column : columns) {
        const QString columnName = QString::fromLatin1(column.name);
        if (columnExists(columnName)) {
            continue;
        }

        const QString columnType = isOracleLikeDriver
            ? QString::fromLatin1(column.oracleType)
            : QString::fromLatin1(column.sqliteType);

        QSqlQuery alter;
        const QString sql = isOracleLikeDriver
            ? QString("ALTER TABLE INTERVENTION ADD (%1 %2)").arg(columnName, columnType)
            : QString("ALTER TABLE INTERVENTION ADD COLUMN %1 %2").arg(columnName, columnType);
        if (!alter.exec(sql)) {
            const QString dbError = alter.lastError().text();
            if (!columnExists(columnName)) {
                errorText = dbError.isEmpty()
                    ? QString("Impossible d'ajouter la colonne %1 a INTERVENTION.").arg(columnName)
                    : dbError;
                return false;
            }
        }
    }

    return true;
}

}

Intervention::Intervention()
    : m_idInter(0), m_duree(0.0), m_cout(0.0), m_mainOeuvre(0.0), m_idBac(0)
{
}

Intervention::Intervention(int idInter, const QDate &dateInter, const QString &reference,
                           double duree, double cout, double mainOeuvre, const QString &statut,
                           const QString &type, const QString &priorite, int idBac,
                   const QString &technicien, const QString &adresse, const QString &descript,
                   const QString &photoAvant, const QString &photoApres)
    : m_idInter(idInter), m_dateInter(dateInter), m_reference(reference),
      m_duree(duree), m_cout(cout), m_mainOeuvre(mainOeuvre), m_statut(statut),
      m_type(type), m_priorite(priorite), m_idBac(idBac),
    m_technicien(technicien), m_adresse(adresse), m_descript(descript),
    m_photoAvant(photoAvant), m_photoApres(photoApres)
{
}

// ---------- Getters ----------
int Intervention::getIdInter() const { return m_idInter; }
QDate Intervention::getDateInter() const { return m_dateInter; }
QString Intervention::getReference() const { return m_reference; }
double Intervention::getDuree() const { return m_duree; }
double Intervention::getCout() const { return m_cout; }
double Intervention::getMainOeuvre() const { return m_mainOeuvre; }
QString Intervention::getStatut() const { return m_statut; }
QString Intervention::getType() const { return m_type; }
QString Intervention::getPriorite() const { return m_priorite; }
int Intervention::getIdBac() const { return m_idBac; }
QString Intervention::getTechnicien() const { return m_technicien; }
QString Intervention::getAdresse() const { return m_adresse; }
QString Intervention::getDescript() const { return m_descript; }
QString Intervention::getPhotoAvant() const { return m_photoAvant; }
QString Intervention::getPhotoApres() const { return m_photoApres; }

// ---------- Setters ----------
void Intervention::setIdInter(int value) { m_idInter = value; }
void Intervention::setDateInter(const QDate &value) { m_dateInter = value; }
void Intervention::setReference(const QString &value) { m_reference = value; }
void Intervention::setDuree(double value) { m_duree = value; }
void Intervention::setCout(double value) { m_cout = value; }
void Intervention::setMainOeuvre(double value) { m_mainOeuvre = value; }
void Intervention::setStatut(const QString &value) { m_statut = value; }
void Intervention::setType(const QString &value) { m_type = value; }
void Intervention::setPriorite(const QString &value) { m_priorite = value; }
void Intervention::setIdBac(int value) { m_idBac = value; }
void Intervention::setTechnicien(const QString &value) { m_technicien = value; }
void Intervention::setAdresse(const QString &value) { m_adresse = value; }
void Intervention::setDescript(const QString &value) { m_descript = value; }
void Intervention::setPhotoAvant(const QString &value) { m_photoAvant = value; }
void Intervention::setPhotoApres(const QString &value) { m_photoApres = value; }

// ---------- CRUD ----------
bool Intervention::ajouter()
{
    if (!ensureInterventionExtendedColumnsExist(m_lastError)) {
        return false;
    }

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
        "INSERT INTO INTERVENTION (ID_INTER, DATE_INTER, REFERENCE, DUREE, COUT, MAIN_OEUVRE, STATUT, TYPE, PRIORITE, ID_BAC, TECHNICIEN, ADRESSE, DESCRIPT, PHOTO_AVANT, PHOTO_APRES) "
        "VALUES (:id, :date_inter, :reference, :duree, :cout, :main_oeuvre, :statut, :type, :priorite, :id_bac, :technicien, :adresse, :descript, :photo_avant, :photo_apres)"
    );
    query.bindValue(":id", nextId);
    query.bindValue(":date_inter", m_dateInter);
    query.bindValue(":reference", m_reference);
    query.bindValue(":duree", m_duree);
    query.bindValue(":cout", m_cout);
    query.bindValue(":main_oeuvre", m_mainOeuvre);
    query.bindValue(":statut", m_statut);
    query.bindValue(":type", m_type);
    query.bindValue(":priorite", m_priorite);
    query.bindValue(":id_bac", m_idBac);
    query.bindValue(":technicien", m_technicien);
    query.bindValue(":adresse", m_adresse);
    query.bindValue(":descript", m_descript);
    query.bindValue(":photo_avant", m_photoAvant);
    query.bindValue(":photo_apres", m_photoApres);

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
    if (!ensureInterventionExtendedColumnsExist(m_lastError)) {
        return false;
    }

    QSqlQuery query;
    query.prepare(
        "UPDATE INTERVENTION SET "
        "DATE_INTER = :date_inter, REFERENCE = :reference, DUREE = :duree, "
        "COUT = :cout, MAIN_OEUVRE = :main_oeuvre, STATUT = :statut, TYPE = :type, "
        "PRIORITE = :priorite, ID_BAC = :id_bac, "
        "TECHNICIEN = :technicien, ADRESSE = :adresse, DESCRIPT = :descript, "
        "PHOTO_AVANT = :photo_avant, PHOTO_APRES = :photo_apres "
        "WHERE ID_INTER = :id"
    );
    query.bindValue(":id", m_idInter);
    query.bindValue(":date_inter", m_dateInter);
    query.bindValue(":reference", m_reference);
    query.bindValue(":duree", m_duree);
    query.bindValue(":cout", m_cout);
    query.bindValue(":main_oeuvre", m_mainOeuvre);
    query.bindValue(":statut", m_statut);
    query.bindValue(":type", m_type);
    query.bindValue(":priorite", m_priorite);
    query.bindValue(":id_bac", m_idBac);
    query.bindValue(":technicien", m_technicien);
    query.bindValue(":adresse", m_adresse);
    query.bindValue(":descript", m_descript);
    query.bindValue(":photo_avant", m_photoAvant);
    query.bindValue(":photo_apres", m_photoApres);

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
    if (!ensureInterventionExtendedColumnsExist(m_lastError)) {
        auto *failedModel = new QSqlQueryModel();
        return failedModel;
    }

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
        "SELECT ID_INTER, REFERENCE, DATE_INTER, DUREE, COUT, MAIN_OEUVRE, STATUT, TYPE, PRIORITE, ID_BAC, TECHNICIEN, ADRESSE, DESCRIPT, PHOTO_AVANT, PHOTO_APRES "
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
