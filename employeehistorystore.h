#ifndef EMPLOYEEHISTORYSTORE_H
#define EMPLOYEEHISTORYSTORE_H

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QVector>
#include <QMutex>

// Persistance JSON de l'historique des conges et missions des employes.
// Remplace les anciennes tables EMPLOYEE_LEAVE_REQUEST et MISSION_AFFECTATION.
// Singleton thread-safe (mutex), stockage dans data/employee_leaves.json
// et data/employee_missions.json a cote de l'executable.
class EmployeeHistoryStore
{
public:
    struct LeaveRecord {
        int       idLeave        = 0;
        int       idEmp          = 0;
        QString   email;
        QString   reasonText;
        QDate     startDate;
        QDate     endDate;
        int       durationDays   = 0;
        QString   attachmentPath;
        QString   attachmentType;
        QString   aiDecision;
        double    aiConfidence   = 0.0;
        QString   aiSummary;
        QString   status;        // EN_ATTENTE_ADMIN / APPROVED / REJECTED
        QDateTime createdAt;
    };

    struct MissionRecord {
        int       idAffect       = 0;
        int       idEmp          = 0;
        QString   missionText;
        double    score          = 0.0;
        QDateTime assignedAt;
        QString   status;        // EN_ATTENTE / TERMINEE
        QDateTime completedAt;
        double    rewardAmount   = 0.0;
        int       experienceGain = 0;
    };

    static EmployeeHistoryStore &instance();

    // === Conges ===
    bool                  addLeave(LeaveRecord &rec, QString &errorText);
    QVector<LeaveRecord>  leavesForEmployee(int idEmp);
    QVector<LeaveRecord>  allLeaves();

    // === Missions ===
    bool                   addMission(MissionRecord &rec, QString &errorText);
    QVector<MissionRecord> missionsForEmployee(int idEmp);
    QVector<MissionRecord> missionsForEmployeeByEmail(const QString &email);
    bool                   markMissionDone(int idAffect,
                                           double rewardAmount,
                                           int experienceGain,
                                           int &outIdEmp,
                                           QString &outMissionText,
                                           double &outScore,
                                           bool &alreadyDone,
                                           QString &errorText);
    bool                   getMission(int idAffect, MissionRecord &out);

private:
    EmployeeHistoryStore();
    Q_DISABLE_COPY(EmployeeHistoryStore)

    QString leavesPath() const;
    QString missionsPath() const;

    bool ensureLoaded(QString &errorText);
    bool loadLeaves(QString &errorText);
    bool loadMissions(QString &errorText);
    bool saveLeaves(QString &errorText);
    bool saveMissions(QString &errorText);

    QVector<LeaveRecord>   m_leaves;
    QVector<MissionRecord> m_missions;
    int   m_nextLeaveId   = 1;
    int   m_nextMissionId = 1;
    bool  m_loaded        = false;
    QMutex m_mutex;
};

#endif // EMPLOYEEHISTORYSTORE_H
