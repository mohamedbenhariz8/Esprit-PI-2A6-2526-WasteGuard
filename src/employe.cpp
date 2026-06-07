#include "employe.h"
#include "connection.h"
#include <QSqlError>
#include <QVariant>
#include <QDateTime>
#include <QDate>

namespace {
bool isDisconnectedSessionError(const QString &errorText)
{
    const QString upper = errorText.toUpper();
    return upper.contains("ORA-03114")
        || upper.contains("ORA-03113")
        || upper.contains("ORA-01012")
        || upper.contains("NOT CONNECTED TO ORACLE")
        || upper.contains("CONNECTION IS BROKEN")
        || upper.contains("COMMUNICATION LINK FAILURE");
}

bool pingCurrentSession(QString &errorText)
{
    errorText.clear();
    QSqlQuery ping;
    if (ping.exec("SELECT 1")) {
        return true;
    }
    if (ping.exec("SELECT 1 FROM DUAL")) {
        return true;
    }
    errorText = ping.lastError().text();
    return false;
}

bool ensureSessionAlive(QString &errorText)
{
    errorText.clear();

    Connection *connection = Connection::instance();
    if (!connection) {
        errorText = "Connexion base introuvable.";
        return false;
    }

    if (!connection->isOpen()) {
        if (!connection->createConnect()) {
            errorText = connection->lastError();
            return false;
        }
        return true;
    }

    QString pingError;
    if (pingCurrentSession(pingError)) {
        return true;
    }

    if (!isDisconnectedSessionError(pingError)) {
        errorText = pingError;
        return false;
    }

    connection->closeConnection();
    if (!connection->createConnect()) {
        errorText = connection->lastError();
        return false;
    }

    if (!pingCurrentSession(pingError)) {
        errorText = pingError;
        return false;
    }

    return true;
}

bool tryDeleteEmployeeRow(int idEmp, QString &errorText)
{
    errorText.clear();

    QSqlQuery query;
    query.prepare("DELETE FROM EMPLOYE WHERE id_emp = :id_emp");
    query.bindValue(":id_emp", idEmp);
    if (query.exec()) {
        return true;
    }

    errorText = query.lastError().text();
    return false;
}

void bindPhotoValue(QSqlQuery &query, const QByteArray &photo)
{
    if (photo.isEmpty()) {
        query.bindValue(":photo", QVariant());
    } else {
        query.bindValue(":photo", photo);
    }
}

bool columnExists(const QString &columnName)
{
    if (columnName.trimmed().isEmpty()) return false;
    QSqlQuery probe;
    const QString sql = QString("SELECT %1 FROM EMPLOYE WHERE 1=0").arg(columnName.trimmed());
    if (probe.exec(sql)) {
        return true;
    }
    return false;
}

bool ensureSalaryColumnExists(QString &errorText)
{
    errorText.clear();
    if (columnExists("SALAIRE")) {
        return true;
    }

    QSqlQuery alter;
    if (!alter.exec("ALTER TABLE EMPLOYE ADD (SALAIRE NUMBER(12,3) DEFAULT 1200 CHECK (SALAIRE >= 0))")) {
        // If another session created it in parallel, continue.
        if (!columnExists("SALAIRE")) {
            errorText = alter.lastError().text();
            return false;
        }
    }

    QSqlQuery fixNulls;
    if (!fixNulls.exec("UPDATE EMPLOYE SET SALAIRE = 1200 WHERE SALAIRE IS NULL")) {
        // non-blocking
    }
    return true;
}

bool ensureRfidColumnExists(QString &errorText)
{
    errorText.clear();
    if (columnExists("RFID")) {
        return true;
    }

    QSqlQuery alter;
    if (!alter.exec("ALTER TABLE EMPLOYE ADD (RFID VARCHAR2(64))")) {
        if (!columnExists("RFID")) {
            errorText = alter.lastError().text();
            qWarning() << "[EMPLOYE] ALTER ADD RFID failed:" << errorText;
            return false;
        }
    }
    qInfo() << "[EMPLOYE] Colonne RFID disponible.";
    return true;
}

bool ensureLastPointageColumnExists(QString &errorText)
{
    errorText.clear();
    if (columnExists("LAST_POINTAGE_DATE")) {
        return true;
    }

    QSqlQuery alter;
    if (!alter.exec("ALTER TABLE EMPLOYE ADD (LAST_POINTAGE_DATE DATE)")) {
        if (!columnExists("LAST_POINTAGE_DATE")) {
            errorText = alter.lastError().text();
            qWarning() << "[EMPLOYE] ALTER ADD LAST_POINTAGE_DATE failed:" << errorText;
            return false;
        }
    }
    qInfo() << "[EMPLOYE] Colonne LAST_POINTAGE_DATE disponible.";
    return true;
}

bool ensurePointageColumnsExist(QString &errorText)
{
    errorText.clear();
    // LAST_POINTAGE_TYPE: 'ENTREE' or 'SORTIE'
    if (!columnExists("LAST_POINTAGE_TYPE")) {
        QSqlQuery a;
        if (!a.exec("ALTER TABLE EMPLOYE ADD (LAST_POINTAGE_TYPE VARCHAR2(10))")) {
            if (!columnExists("LAST_POINTAGE_TYPE")) {
                errorText = a.lastError().text();
                qWarning() << "[EMPLOYE] ALTER ADD LAST_POINTAGE_TYPE failed:" << errorText;
                return false;
            }
        }
        qInfo() << "[EMPLOYE] Colonne LAST_POINTAGE_TYPE creee.";
    }
    // LAST_ENTRY_TIME: timestamp of last clock-in
    if (!columnExists("LAST_ENTRY_TIME")) {
        QSqlQuery a;
        if (!a.exec("ALTER TABLE EMPLOYE ADD (LAST_ENTRY_TIME DATE)")) {
            if (!columnExists("LAST_ENTRY_TIME")) {
                errorText = a.lastError().text();
                qWarning() << "[EMPLOYE] ALTER ADD LAST_ENTRY_TIME failed:" << errorText;
                return false;
            }
        }
        qInfo() << "[EMPLOYE] Colonne LAST_ENTRY_TIME creee.";
    }
    // TOTAL_HOURS_TODAY: accumulated work hours for the day
    if (!columnExists("TOTAL_HOURS_TODAY")) {
        QSqlQuery a;
        if (!a.exec("ALTER TABLE EMPLOYE ADD (TOTAL_HOURS_TODAY NUMBER(12,6) DEFAULT 0)")) {
            if (!columnExists("TOTAL_HOURS_TODAY")) {
                errorText = a.lastError().text();
                qWarning() << "[EMPLOYE] ALTER ADD TOTAL_HOURS_TODAY failed:" << errorText;
                return false;
            }
        }
        QSqlQuery fix;
        fix.exec("UPDATE EMPLOYE SET TOTAL_HOURS_TODAY = 0 WHERE TOTAL_HOURS_TODAY IS NULL");
        qInfo() << "[EMPLOYE] Colonne TOTAL_HOURS_TODAY creee.";
    } else {
        // Upgrade precision for existing column to fix truncation of seconds
        QSqlQuery m;
        m.exec("ALTER TABLE EMPLOYE MODIFY (TOTAL_HOURS_TODAY NUMBER(12,6))");
    }
    // TOTAL_HOURS_MONTH: accumulated work hours for the month
    if (!columnExists("TOTAL_HOURS_MONTH")) {
        QSqlQuery a;
        if (!a.exec("ALTER TABLE EMPLOYE ADD (TOTAL_HOURS_MONTH NUMBER(12,6) DEFAULT 0)")) {
            if (!columnExists("TOTAL_HOURS_MONTH")) {
                errorText = a.lastError().text();
                qWarning() << "[EMPLOYE] ALTER ADD TOTAL_HOURS_MONTH failed:" << errorText;
                return false;
            }
        }
        QSqlQuery fix;
        fix.exec("UPDATE EMPLOYE SET TOTAL_HOURS_MONTH = 0 WHERE TOTAL_HOURS_MONTH IS NULL");
        qInfo() << "[EMPLOYE] Colonne TOTAL_HOURS_MONTH creee.";
    }
    return true;
}
}

Employe::Employe()
{
    id_emp = 0;
    salaire = 1200;
}

Employe::Employe(int id_emp, QString matricule, QString cin, QString nom,
                 QString email, QString specialite, QString disponibilite,
                 int salaire, const QByteArray &photo)
{
    this->id_emp = id_emp;
    this->matricule = matricule;
    this->cin = cin;
    this->nom = nom;
    this->email = email;
    this->specialite = specialite;
    this->disponibilite = disponibilite;
    this->salaire = salaire;
    this->photo = photo;
}

Employe::Employe(int id_emp, QString matricule, QString cin, QString nom,
                 QString email, QString specialite, QString disponibilite,
                 const QByteArray &photo)
{
    this->id_emp = id_emp;
    this->matricule = matricule;
    this->cin = cin;
    this->nom = nom;
    this->email = email;
    this->specialite = specialite;
    this->disponibilite = disponibilite;
    this->salaire = 1200;
    this->photo = photo;
}


int Employe::getIdEmp() const { return id_emp; }
QString Employe::getMatricule() const { return matricule; }
QString Employe::getCin() const { return cin; }
QString Employe::getNom() const { return nom; }
QString Employe::getEmail() const { return email; }
QString Employe::getSpecialite() const { return specialite; }
QString Employe::getDisponibilite() const { return disponibilite; }
int Employe::getSalaire() const { return salaire; }
QByteArray Employe::getPhoto() const { return photo; }
QString Employe::getRfid() const { return rfid; }


void Employe::setIdEmp(int value) { id_emp = value; }
void Employe::setMatricule(const QString &value) { matricule = value; }
void Employe::setCin(const QString &value) { cin = value; }
void Employe::setNom(const QString &value) { nom = value; }
void Employe::setEmail(const QString &value) { email = value; }
void Employe::setSpecialite(const QString &value) { specialite = value; }
void Employe::setDisponibilite(const QString &value) { disponibilite = value; }
void Employe::setSalaire(int value) { salaire = value; }
void Employe::setPhoto(const QByteArray &value) { photo = value; }
void Employe::setRfid(const QString &value) { rfid = value.trimmed().toUpper(); }


bool Employe::ajouter()
{
    QString sessionError;
    if (!ensureSessionAlive(sessionError)) {
        m_lastError = sessionError;
        return false;
    }

    QString salarySchemaError;
    const bool hasSalaryColumn = ensureSalaryColumnExists(salarySchemaError);
    if (!hasSalaryColumn) {
        m_lastError = salarySchemaError.isEmpty() ? "Colonne SALAIRE introuvable." : salarySchemaError;
        return false;
    }
    const bool hasPhotoColumn = columnExists("PHOTO");
    QString rfidSchemaError;
    const bool hasRfidColumn = ensureRfidColumnExists(rfidSchemaError);
    if (!hasRfidColumn) {
        qWarning() << "[EMPLOYE.ajouter] Colonne RFID indisponible:" << rfidSchemaError;
        if (!rfid.trimmed().isEmpty()) {
            m_lastError = QString("Impossible d'enregistrer le badge RFID: colonne absente. "
                                  "Demandez a un DBA d'executer:\n"
                                  "ALTER TABLE EMPLOYE ADD (RFID VARCHAR2(64) NULL);\n\n"
                                  "Detail: %1").arg(rfidSchemaError);
            return false;
        }
    }

    QSqlQuery query;
    QString sql;
    if (id_emp > 0) {
        sql = "INSERT INTO EMPLOYE (id_emp, matricule, cin, nom, email, specialite, disponibilite, salaire";
        if (hasPhotoColumn) sql += ", photo";
        if (hasRfidColumn) sql += ", rfid";
        sql += ") VALUES (:id_emp, :matricule, :cin, :nom, :email, :specialite, :disponibilite, :salaire";
        if (hasPhotoColumn) sql += ", :photo";
        if (hasRfidColumn) sql += ", :rfid";
        sql += ")";
        query.prepare(sql);
        query.bindValue(":id_emp", id_emp);
    } else {
        sql = "INSERT INTO EMPLOYE (matricule, cin, nom, email, specialite, disponibilite, salaire";
        if (hasPhotoColumn) sql += ", photo";
        if (hasRfidColumn) sql += ", rfid";
        sql += ") VALUES (:matricule, :cin, :nom, :email, :specialite, :disponibilite, :salaire";
        if (hasPhotoColumn) sql += ", :photo";
        if (hasRfidColumn) sql += ", :rfid";
        sql += ")";
        query.prepare(sql);
    }
    query.bindValue(":matricule", matricule);
    query.bindValue(":cin", cin);
    query.bindValue(":nom", nom);
    query.bindValue(":email", email);
    query.bindValue(":specialite", specialite);
    query.bindValue(":disponibilite", disponibilite);
    query.bindValue(":salaire", qMax(0, salaire));
    if (hasPhotoColumn) {
        bindPhotoValue(query, photo);
    }
    if (hasRfidColumn) {
        if (rfid.trimmed().isEmpty()) {
            query.bindValue(":rfid", QVariant());
        } else {
            query.bindValue(":rfid", rfid.trimmed().toUpper());
        }
    }

    const bool ok = query.exec();
    m_lastError = ok ? QString() : query.lastError().text();
    return ok;
}


bool Employe::supprimer(int id_emp)
{
    QString lastSqlError;

    for (int attempt = 0; attempt < 2; ++attempt) {
        QString sessionError;
        if (!ensureSessionAlive(sessionError)) {
            m_lastError = sessionError;
            return false;
        }

        if (tryDeleteEmployeeRow(id_emp, lastSqlError)) {
            m_lastError.clear();
            return true;
        }

        if (attempt == 0 && isDisconnectedSessionError(lastSqlError)) {
            continue;
        }

        m_lastError = lastSqlError;
        return false;
    }

    m_lastError = lastSqlError.isEmpty() ? "Suppression employe impossible." : lastSqlError;
    return false;
}


bool Employe::modifier()
{
    QString sessionError;
    if (!ensureSessionAlive(sessionError)) {
        m_lastError = sessionError;
        return false;
    }

    QString salarySchemaError;
    const bool hasSalaryColumn = ensureSalaryColumnExists(salarySchemaError);
    if (!hasSalaryColumn) {
        m_lastError = salarySchemaError.isEmpty() ? "Colonne SALAIRE introuvable." : salarySchemaError;
        return false;
    }
    const bool hasPhotoColumn = columnExists("PHOTO");
    QString rfidSchemaError;
    const bool hasRfidColumn = ensureRfidColumnExists(rfidSchemaError);
    if (!hasRfidColumn) {
        qWarning() << "[EMPLOYE.modifier] Colonne RFID indisponible:" << rfidSchemaError;
        if (!rfid.trimmed().isEmpty()) {
            m_lastError = QString("Impossible d'enregistrer le badge RFID: colonne absente. "
                                  "Demandez a un DBA d'executer:\n"
                                  "ALTER TABLE EMPLOYE ADD (RFID VARCHAR2(64) NULL);\n\n"
                                  "Detail: %1").arg(rfidSchemaError);
            return false;
        }
    }

    QSqlQuery query;
    QString sql = "UPDATE EMPLOYE SET "
                  "matricule=:matricule, cin=:cin, nom=:nom, email=:email, "
                  "specialite=:specialite, disponibilite=:disponibilite, salaire=:salaire";
    if (hasPhotoColumn) sql += ", photo=:photo";
    if (hasRfidColumn) sql += ", rfid=:rfid";
    sql += " WHERE id_emp=:id_emp";
    query.prepare(sql);

    query.bindValue(":id_emp", id_emp);
    query.bindValue(":matricule", matricule);
    query.bindValue(":cin", cin);
    query.bindValue(":nom", nom);
    query.bindValue(":email", email);
    query.bindValue(":specialite", specialite);
    query.bindValue(":disponibilite", disponibilite);
    query.bindValue(":salaire", qMax(0, salaire));
    if (hasPhotoColumn) {
        bindPhotoValue(query, photo);
    }
    if (hasRfidColumn) {
        if (rfid.trimmed().isEmpty()) {
            query.bindValue(":rfid", QVariant());
        } else {
            query.bindValue(":rfid", rfid.trimmed().toUpper());
        }
    }

    const bool ok = query.exec();
    m_lastError = ok ? QString() : query.lastError().text();
    return ok;
}

QSqlQueryModel *Employe::afficher()
{
    QString sessionError;
    if (!ensureSessionAlive(sessionError)) {
        QSqlQueryModel *failedModel = new QSqlQueryModel();
        QSqlQuery failedQuery;
        failedQuery.exec("SELECT id_emp FROM EMPLOYE WHERE 1=0");
        failedModel->setQuery(std::move(failedQuery));
        m_lastError = sessionError;
        return failedModel;
    }

    QSqlQueryModel *model = new QSqlQueryModel();
    QString salarySchemaError;
    ensureSalaryColumnExists(salarySchemaError);
    QString rfidSchemaError;
    ensureRfidColumnExists(rfidSchemaError);
    // Avoid fetching BLOB photo data in the generic startup model. On some Oracle/ODBC
    // setups, loading LOB columns here is unstable and can crash the process.
    QString sql = "SELECT id_emp, matricule, cin, nom, email, specialite, disponibilite, ";
    if (columnExists("SALAIRE")) {
        sql += "NVL(salaire, 1200) AS salaire";
    } else {
        sql += "1200 AS salaire";
    }
    if (columnExists("RFID")) {
        sql += ", rfid ";
    } else {
        sql += ", NULL AS rfid ";
    }
    sql += "FROM EMPLOYE";
    model->setQuery(sql);
    if (model->lastError().isValid()) {
        m_lastError = model->lastError().text();
    } else {
        m_lastError = salarySchemaError;
    }
    return model;
}

bool Employe::findIdByMatricule(const QString &matricule, int &id_emp)
{
    id_emp = -1;

    QString sessionError;
    if (!ensureSessionAlive(sessionError)) {
        m_lastError = sessionError;
        return false;
    }

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

bool Employe::findIdByRfid(const QString &rfidTag, int &id_emp)
{
    id_emp = -1;

    QString sessionError;
    if (!ensureSessionAlive(sessionError)) {
        m_lastError = sessionError;
        return false;
    }

    if (!columnExists("RFID")) {
        QString rfidSchemaError;
        if (!ensureRfidColumnExists(rfidSchemaError)) {
            m_lastError = rfidSchemaError.isEmpty() ? "Colonne RFID introuvable." : rfidSchemaError;
            return false;
        }
    }

    QSqlQuery query;
    query.prepare("SELECT id_emp FROM EMPLOYE WHERE UPPER(TRIM(rfid)) = UPPER(TRIM(:rfid))");
    query.bindValue(":rfid", rfidTag.trimmed().toUpper());

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    if (query.next()) {
        id_emp = query.value(0).toInt();
        m_lastError.clear();
        return true;
    }

    m_lastError = "Aucun employe associe a ce badge RFID.";
    return false;
}

bool Employe::resetDailyAttendance()
{
    QString sessionError;
    if (!ensureSessionAlive(sessionError)) {
        m_lastError = sessionError;
        return false;
    }

    QString pointageSchemaError;
    ensureLastPointageColumnExists(pointageSchemaError);
    ensurePointageColumnsExist(pointageSchemaError);

    QSqlQuery query;
    // At the start of every day every employee is marked as unavailable
    // (INDISPONIBLE). They become available (DISPONIBLE) only once they
    // present their RFID badge on the pointage reader.
    // Also reset the pointage tracking columns for the new day.
    QString updateSql = "UPDATE EMPLOYE SET disponibilite = 'INDISPONIBLE', "
                        "LAST_POINTAGE_TYPE = NULL, "
                        "LAST_ENTRY_TIME = NULL, "
                        "TOTAL_HOURS_TODAY = 0";
    if (QDate::currentDate().day() == 1) {
        updateSql += ", TOTAL_HOURS_MONTH = 0";
    }

    if (!query.exec(updateSql)) {
        m_lastError = query.lastError().text();
        return false;
    }
    m_lastError.clear();
    return true;
}

bool Employe::markPresentById(int id_emp)
{
    // Kept for backward compatibility: unconditional "present" update, without
    // the once-per-day guard. Most callers should use pointById() instead.
    QString sessionError;
    if (!ensureSessionAlive(sessionError)) {
        m_lastError = sessionError;
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE EMPLOYE SET disponibilite = 'DISPONIBLE' WHERE id_emp = :id");
    query.bindValue(":id", id_emp);
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    m_lastError.clear();
    return true;
}

Employe::PointageResult Employe::pointById(int id_emp)
{
    QString sessionError;
    if (!ensureSessionAlive(sessionError)) {
        m_lastError = sessionError;
        return PointageError;
    }

    QString schemaError;
    if (!ensureLastPointageColumnExists(schemaError)) {
        m_lastError = schemaError.isEmpty()
                          ? QStringLiteral("Colonne LAST_POINTAGE_DATE introuvable.")
                          : schemaError;
        return PointageError;
    }
    if (!ensurePointageColumnsExist(schemaError)) {
        m_lastError = schemaError.isEmpty()
                          ? QStringLiteral("Colonnes pointage introuvables.")
                          : schemaError;
        return PointageError;
    }

    // Check the last pointage type for today.
    // If NULL or 'SORTIE' -> this is an ENTREE (clock in)
    // If 'ENTREE' -> this is a SORTIE (clock out)
    QSqlQuery check;
    check.prepare(
        "SELECT NVL(LAST_POINTAGE_TYPE, 'NONE'), "
        "       LAST_ENTRY_TIME, "
        "       NVL(TOTAL_HOURS_TODAY, 0), "
        "       LAST_POINTAGE_DATE, "
        "       NVL(TOTAL_HOURS_MONTH, 0) "
        "FROM EMPLOYE WHERE id_emp = :id");
    check.bindValue(":id", id_emp);
    if (!check.exec()) {
        m_lastError = check.lastError().text();
        return PointageError;
    }
    if (!check.next()) {
        m_lastError = QStringLiteral("Employe introuvable.");
        return PointageError;
    }

    const QString lastType = check.value(0).toString().trimmed().toUpper();
    const QDateTime lastEntry = check.value(1).toDateTime();
    double totalHours = check.value(2).toDouble();
    const QDateTime lastPointageDate = check.value(3).toDateTime();
    double totalMonthHours = check.value(4).toDouble();

    // If lastPointageDate is from a previous day, treat as fresh start
    bool freshDay = true;
    bool freshMonth = false;
    if (lastPointageDate.isValid()) {
        freshDay = (lastPointageDate.date() != QDate::currentDate());
        freshMonth = (lastPointageDate.date().month() != QDate::currentDate().month() ||
                      lastPointageDate.date().year() != QDate::currentDate().year());
    }

    if (freshDay || lastType == "NONE" || lastType == "SORTIE") {
        // ── ENTREE (clock in) ──
        if (freshDay) {
            totalHours = 0.0;  // Reset hours for new day
        }
        if (freshMonth || QDate::currentDate().day() == 1) {
            totalMonthHours = 0.0;
        }
        QSqlQuery update;
        update.prepare(
            "UPDATE EMPLOYE "
            "SET disponibilite = 'DISPONIBLE', "
            "    last_pointage_date = SYSDATE, "
            "    LAST_POINTAGE_TYPE = 'ENTREE', "
            "    LAST_ENTRY_TIME = SYSDATE"
            + QString(freshDay ? ", TOTAL_HOURS_TODAY = 0" : "")
            + QString(freshMonth || QDate::currentDate().day() == 1 ? ", TOTAL_HOURS_MONTH = 0" : "") +
            " WHERE id_emp = :id");
        update.bindValue(":id", id_emp);
        if (!update.exec()) {
            m_lastError = update.lastError().text();
            return PointageError;
        }
        m_lastError.clear();
        return PointageEntree;
    } else {
        // ── SORTIE (clock out) ──
        // Calculate hours worked in this session
        double sessionHours = 0.0;
        if (lastEntry.isValid()) {
            qint64 secsWorked = lastEntry.secsTo(QDateTime::currentDateTime());
            if (secsWorked < 0) secsWorked = 0;
            sessionHours = static_cast<double>(secsWorked) / 3600.0;
        }
        totalHours += sessionHours;
        totalMonthHours += sessionHours;

        QSqlQuery update;
        update.prepare(
            "UPDATE EMPLOYE "
            "SET disponibilite = 'INDISPONIBLE', "
            "    last_pointage_date = SYSDATE, "
            "    LAST_POINTAGE_TYPE = 'SORTIE', "
            "    LAST_ENTRY_TIME = NULL, "
            "    TOTAL_HOURS_TODAY = :hours, "
            "    TOTAL_HOURS_MONTH = :mhours "
            "WHERE id_emp = :id");
        update.bindValue(":hours", totalHours);
        update.bindValue(":mhours", totalMonthHours);
        update.bindValue(":id", id_emp);
        if (!update.exec()) {
            m_lastError = update.lastError().text();
            return PointageError;
        }
        m_lastError.clear();
        return PointageSortie;
    }
}

double Employe::computeTodayHours(int id_emp)
{
    QString sessionError;
    if (!ensureSessionAlive(sessionError)) {
        return 0.0;
    }

    QSqlQuery q;
    q.prepare(
        "SELECT NVL(TOTAL_HOURS_TODAY, 0), "
        "       LAST_POINTAGE_TYPE, "
        "       LAST_ENTRY_TIME "
        "FROM EMPLOYE WHERE id_emp = :id");
    q.bindValue(":id", id_emp);
    if (!q.exec() || !q.next()) {
        return 0.0;
    }

    double total = q.value(0).toDouble();
    const QString lastType = q.value(1).toString().trimmed().toUpper();
    const QDateTime lastEntry = q.value(2).toDateTime();

    // If currently clocked in, add the ongoing session time
    if (lastType == "ENTREE" && lastEntry.isValid()) {
        qint64 secs = lastEntry.secsTo(QDateTime::currentDateTime());
        if (secs > 0) {
            total += static_cast<double>(secs) / 3600.0;
        }
    }
    return total;
}

double Employe::computeMonthHours(int id_emp)
{
    QString sessionError;
    if (!ensureSessionAlive(sessionError)) {
        return 0.0;
    }

    QSqlQuery q;
    q.prepare(
        "SELECT NVL(TOTAL_HOURS_MONTH, 0), "
        "       LAST_POINTAGE_TYPE, "
        "       LAST_ENTRY_TIME "
        "FROM EMPLOYE WHERE id_emp = :id");
    q.bindValue(":id", id_emp);
    if (!q.exec() || !q.next()) {
        return 0.0;
    }

    double total = q.value(0).toDouble();
    const QString lastType = q.value(1).toString().trimmed().toUpper();
    const QDateTime lastEntry = q.value(2).toDateTime();

    // If currently clocked in, add the ongoing session time
    if (lastType == "ENTREE" && lastEntry.isValid()) {
        qint64 secs = lastEntry.secsTo(QDateTime::currentDateTime());
        if (secs > 0) {
            total += static_cast<double>(secs) / 3600.0;
        }
    }
    return total;
}

bool Employe::isCurrentlyPresent(int id_emp)
{
    QString sessionError;
    if (!ensureSessionAlive(sessionError)) {
        return false;
    }

    QSqlQuery q;
    q.prepare(
        "SELECT LAST_POINTAGE_TYPE "
        "FROM EMPLOYE WHERE id_emp = :id");
    q.bindValue(":id", id_emp);
    if (!q.exec() || !q.next()) {
        return false;
    }
    return q.value(0).toString().trimmed().toUpper() == "ENTREE";
}

QString Employe::lastError() const
{
    return m_lastError;
}
