#include "employeehistorystore.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>
#include <QSaveFile>

namespace {

QString dateToString(const QDate &d)
{
    return d.isValid() ? d.toString(Qt::ISODate) : QString();
}

QDate dateFromString(const QString &s)
{
    if (s.trimmed().isEmpty()) return QDate();
    return QDate::fromString(s, Qt::ISODate);
}

QString dateTimeToString(const QDateTime &dt)
{
    return dt.isValid() ? dt.toString(Qt::ISODateWithMs) : QString();
}

QDateTime dateTimeFromString(const QString &s)
{
    if (s.trimmed().isEmpty()) return QDateTime();
    QDateTime dt = QDateTime::fromString(s, Qt::ISODateWithMs);
    if (!dt.isValid()) dt = QDateTime::fromString(s, Qt::ISODate);
    return dt;
}

QJsonObject leaveToJson(const EmployeeHistoryStore::LeaveRecord &r)
{
    QJsonObject o;
    o.insert("id_leave",        r.idLeave);
    o.insert("id_emp",          r.idEmp);
    o.insert("email",           r.email);
    o.insert("reason_text",     r.reasonText);
    o.insert("start_date",      dateToString(r.startDate));
    o.insert("end_date",        dateToString(r.endDate));
    o.insert("duration_days",   r.durationDays);
    o.insert("attachment_path", r.attachmentPath);
    o.insert("attachment_type", r.attachmentType);
    o.insert("ai_decision",     r.aiDecision);
    o.insert("ai_confidence",   r.aiConfidence);
    o.insert("ai_summary",      r.aiSummary);
    o.insert("status",          r.status);
    o.insert("created_at",      dateTimeToString(r.createdAt));
    return o;
}

EmployeeHistoryStore::LeaveRecord leaveFromJson(const QJsonObject &o)
{
    EmployeeHistoryStore::LeaveRecord r;
    r.idLeave        = o.value("id_leave").toInt();
    r.idEmp          = o.value("id_emp").toInt();
    r.email          = o.value("email").toString();
    r.reasonText     = o.value("reason_text").toString();
    r.startDate      = dateFromString(o.value("start_date").toString());
    r.endDate        = dateFromString(o.value("end_date").toString());
    r.durationDays   = o.value("duration_days").toInt();
    r.attachmentPath = o.value("attachment_path").toString();
    r.attachmentType = o.value("attachment_type").toString();
    r.aiDecision     = o.value("ai_decision").toString();
    r.aiConfidence   = o.value("ai_confidence").toDouble();
    r.aiSummary      = o.value("ai_summary").toString();
    r.status         = o.value("status").toString();
    r.createdAt      = dateTimeFromString(o.value("created_at").toString());
    return r;
}

QJsonObject missionToJson(const EmployeeHistoryStore::MissionRecord &r)
{
    QJsonObject o;
    o.insert("id_affect",       r.idAffect);
    o.insert("id_emp",          r.idEmp);
    o.insert("mission_text",    r.missionText);
    o.insert("score",           r.score);
    o.insert("assigned_at",     dateTimeToString(r.assignedAt));
    o.insert("status",          r.status);
    o.insert("completed_at",    dateTimeToString(r.completedAt));
    o.insert("reward_amount",   r.rewardAmount);
    o.insert("experience_gain", r.experienceGain);
    return o;
}

EmployeeHistoryStore::MissionRecord missionFromJson(const QJsonObject &o)
{
    EmployeeHistoryStore::MissionRecord r;
    r.idAffect       = o.value("id_affect").toInt();
    r.idEmp          = o.value("id_emp").toInt();
    r.missionText    = o.value("mission_text").toString();
    r.score          = o.value("score").toDouble();
    r.assignedAt     = dateTimeFromString(o.value("assigned_at").toString());
    r.status         = o.value("status").toString();
    if (r.status.isEmpty()) r.status = "EN_ATTENTE";
    r.completedAt    = dateTimeFromString(o.value("completed_at").toString());
    r.rewardAmount   = o.value("reward_amount").toDouble();
    r.experienceGain = o.value("experience_gain").toInt();
    return r;
}

bool writeJsonAtomic(const QString &path, const QJsonDocument &doc, QString &errorText)
{
    QFileInfo fi(path);
    QDir dir = fi.absoluteDir();
    if (!dir.exists() && !dir.mkpath(".")) {
        errorText = QString("Impossible de creer le dossier %1").arg(dir.absolutePath());
        return false;
    }
    QSaveFile out(path);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        errorText = out.errorString();
        return false;
    }
    out.write(doc.toJson(QJsonDocument::Indented));
    if (!out.commit()) {
        errorText = out.errorString();
        return false;
    }
    return true;
}

} // namespace

EmployeeHistoryStore &EmployeeHistoryStore::instance()
{
    static EmployeeHistoryStore s;
    return s;
}

EmployeeHistoryStore::EmployeeHistoryStore() = default;

QString EmployeeHistoryStore::leavesPath() const
{
    return QDir(QCoreApplication::applicationDirPath())
        .filePath("data/employee_leaves.json");
}

QString EmployeeHistoryStore::missionsPath() const
{
    return QDir(QCoreApplication::applicationDirPath())
        .filePath("data/employee_missions.json");
}

bool EmployeeHistoryStore::ensureLoaded(QString &errorText)
{
    if (m_loaded) return true;
    if (!loadLeaves(errorText))   return false;
    if (!loadMissions(errorText)) return false;
    m_loaded = true;
    return true;
}

bool EmployeeHistoryStore::loadLeaves(QString &errorText)
{
    m_leaves.clear();
    m_nextLeaveId = 1;

    QFile f(leavesPath());
    if (!f.exists()) return true;
    if (!f.open(QIODevice::ReadOnly)) {
        errorText = f.errorString();
        return false;
    }
    const QByteArray raw = f.readAll();
    f.close();
    if (raw.trimmed().isEmpty()) return true;

    QJsonParseError perr{};
    const QJsonDocument doc = QJsonDocument::fromJson(raw, &perr);
    if (perr.error != QJsonParseError::NoError) {
        errorText = QString("JSON conges illisible: %1").arg(perr.errorString());
        return false;
    }
    const QJsonArray arr = doc.isArray() ? doc.array() : doc.object().value("leaves").toArray();
    for (const QJsonValue &v : arr) {
        if (!v.isObject()) continue;
        LeaveRecord r = leaveFromJson(v.toObject());
        if (r.idLeave >= m_nextLeaveId) m_nextLeaveId = r.idLeave + 1;
        m_leaves.append(r);
    }
    return true;
}

bool EmployeeHistoryStore::loadMissions(QString &errorText)
{
    m_missions.clear();
    m_nextMissionId = 1;

    QFile f(missionsPath());
    if (!f.exists()) return true;
    if (!f.open(QIODevice::ReadOnly)) {
        errorText = f.errorString();
        return false;
    }
    const QByteArray raw = f.readAll();
    f.close();
    if (raw.trimmed().isEmpty()) return true;

    QJsonParseError perr{};
    const QJsonDocument doc = QJsonDocument::fromJson(raw, &perr);
    if (perr.error != QJsonParseError::NoError) {
        errorText = QString("JSON missions illisible: %1").arg(perr.errorString());
        return false;
    }
    const QJsonArray arr = doc.isArray() ? doc.array() : doc.object().value("missions").toArray();
    for (const QJsonValue &v : arr) {
        if (!v.isObject()) continue;
        MissionRecord r = missionFromJson(v.toObject());
        if (r.idAffect >= m_nextMissionId) m_nextMissionId = r.idAffect + 1;
        m_missions.append(r);
    }
    return true;
}

bool EmployeeHistoryStore::saveLeaves(QString &errorText)
{
    QJsonArray arr;
    for (const LeaveRecord &r : m_leaves) arr.append(leaveToJson(r));
    return writeJsonAtomic(leavesPath(), QJsonDocument(arr), errorText);
}

bool EmployeeHistoryStore::saveMissions(QString &errorText)
{
    QJsonArray arr;
    for (const MissionRecord &r : m_missions) arr.append(missionToJson(r));
    return writeJsonAtomic(missionsPath(), QJsonDocument(arr), errorText);
}

// =====================================================================
// Conges
// =====================================================================

bool EmployeeHistoryStore::addLeave(LeaveRecord &rec, QString &errorText)
{
    QMutexLocker lock(&m_mutex);
    if (!ensureLoaded(errorText)) return false;

    rec.idLeave = m_nextLeaveId++;
    if (!rec.createdAt.isValid()) rec.createdAt = QDateTime::currentDateTime();
    if (rec.status.trimmed().isEmpty()) rec.status = "EN_ATTENTE_ADMIN";
    m_leaves.append(rec);
    return saveLeaves(errorText);
}

QVector<EmployeeHistoryStore::LeaveRecord>
EmployeeHistoryStore::leavesForEmployee(int idEmp)
{
    QMutexLocker lock(&m_mutex);
    QString err;
    ensureLoaded(err);
    QVector<LeaveRecord> out;
    for (const LeaveRecord &r : m_leaves) {
        if (r.idEmp == idEmp) out.append(r);
    }
    return out;
}

QVector<EmployeeHistoryStore::LeaveRecord>
EmployeeHistoryStore::allLeaves()
{
    QMutexLocker lock(&m_mutex);
    QString err;
    ensureLoaded(err);
    return m_leaves;
}

// =====================================================================
// Missions
// =====================================================================

bool EmployeeHistoryStore::addMission(MissionRecord &rec, QString &errorText)
{
    QMutexLocker lock(&m_mutex);
    if (!ensureLoaded(errorText)) return false;

    rec.idAffect = m_nextMissionId++;
    if (!rec.assignedAt.isValid()) rec.assignedAt = QDateTime::currentDateTime();
    if (rec.status.trimmed().isEmpty()) rec.status = "EN_ATTENTE";
    m_missions.append(rec);
    return saveMissions(errorText);
}

QVector<EmployeeHistoryStore::MissionRecord>
EmployeeHistoryStore::missionsForEmployee(int idEmp)
{
    QMutexLocker lock(&m_mutex);
    QString err;
    ensureLoaded(err);
    QVector<MissionRecord> out;
    for (const MissionRecord &r : m_missions) {
        if (r.idEmp == idEmp) out.append(r);
    }
    return out;
}

QVector<EmployeeHistoryStore::MissionRecord>
EmployeeHistoryStore::missionsForEmployeeByEmail(const QString &email)
{
    Q_UNUSED(email);
    // Resolution email -> id_emp se fait cote SQL (table EMPLOYE).
    // L'appelant doit utiliser missionsForEmployee(idEmp) apres resolution.
    return {};
}

bool EmployeeHistoryStore::getMission(int idAffect, MissionRecord &out)
{
    QMutexLocker lock(&m_mutex);
    QString err;
    ensureLoaded(err);
    for (const MissionRecord &r : m_missions) {
        if (r.idAffect == idAffect) { out = r; return true; }
    }
    return false;
}

bool EmployeeHistoryStore::markMissionDone(int idAffect,
                                           double rewardAmount,
                                           int experienceGain,
                                           int &outIdEmp,
                                           QString &outMissionText,
                                           double &outScore,
                                           bool &alreadyDone,
                                           QString &errorText)
{
    QMutexLocker lock(&m_mutex);
    if (!ensureLoaded(errorText)) return false;

    alreadyDone = false;
    for (MissionRecord &r : m_missions) {
        if (r.idAffect != idAffect) continue;

        outIdEmp       = r.idEmp;
        outMissionText = r.missionText;
        outScore       = r.score;

        const QString s = r.status.trimmed().toUpper();
        if (s == "TERMINEE" || s == "TERMINE" || s == "DONE" || s == "COMPLETED") {
            alreadyDone = true;
            return true;
        }

        r.status         = "TERMINEE";
        r.completedAt    = QDateTime::currentDateTime();
        r.rewardAmount   = rewardAmount;
        r.experienceGain = experienceGain;
        return saveMissions(errorText);
    }

    errorText = "Tache introuvable.";
    return false;
}
