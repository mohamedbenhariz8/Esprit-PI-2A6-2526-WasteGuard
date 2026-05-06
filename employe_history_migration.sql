-- ============================================================================
-- Migration: regroupement des modules conges & missions dans la table EMPLOYE
-- L'historique detaille est desormais stocke dans :
--   data/employee_leaves.json
--   data/employee_missions.json
-- Seules les colonnes "etat courant" et "agregats" restent en base.
-- ============================================================================

-- 1) Nouvelles colonnes sur EMPLOYE (etat courant + agregats)
ALTER TABLE EMPLOYE ADD (
    CURRENT_LEAVE_STATUS    VARCHAR2(32),
    CURRENT_LEAVE_START     DATE,
    CURRENT_LEAVE_END       DATE,
    CURRENT_LEAVE_REASON    VARCHAR2(500),
    TOTAL_LEAVE_DAYS        NUMBER        DEFAULT 0,

    CURRENT_MISSION_TEXT    VARCHAR2(1000),
    CURRENT_MISSION_SCORE   NUMBER(6,2)   DEFAULT 0,
    CURRENT_MISSION_STATUS  VARCHAR2(20),
    TOTAL_MISSIONS          NUMBER        DEFAULT 0,
    TOTAL_REWARD            NUMBER(10,2)  DEFAULT 0,
    TOTAL_EXPERIENCE        NUMBER        DEFAULT 0
);

-- 2) Suppression des anciennes tables (et de leurs sequences/triggers)
DROP TRIGGER  EMPLOYEE_LEAVE_REQUEST_BI;
DROP SEQUENCE EMPLOYEE_LEAVE_REQUEST_SEQ;
DROP TABLE    EMPLOYEE_LEAVE_REQUEST CASCADE CONSTRAINTS;

DROP TRIGGER  MISSION_AFFECTATION_BI;
DROP SEQUENCE MISSION_AFFECTATION_SEQ;
DROP TABLE    MISSION_AFFECTATION    CASCADE CONSTRAINTS;

COMMIT;
