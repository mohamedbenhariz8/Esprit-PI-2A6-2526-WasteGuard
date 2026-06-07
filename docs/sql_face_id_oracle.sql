-- Face ID schema update for EMPLOYE (Oracle)
-- Safe to rerun: each ALTER is wrapped and ignores "column already exists" (ORA-01430).

BEGIN
    EXECUTE IMMEDIATE 'ALTER TABLE EMPLOYE ADD (photo BLOB)';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -1430 THEN RAISE; END IF;
END;
/

BEGIN
    EXECUTE IMMEDIATE 'ALTER TABLE EMPLOYE ADD (face_template CLOB)';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -1430 THEN RAISE; END IF;
END;
/

BEGIN
    EXECUTE IMMEDIATE 'ALTER TABLE EMPLOYE ADD (face_template_updated_at DATE)';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -1430 THEN RAISE; END IF;
END;
/

BEGIN
    EXECUTE IMMEDIATE 'ALTER TABLE EMPLOYE ADD (face_enabled NUMBER(1) DEFAULT 1)';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -1430 THEN RAISE; END IF;
END;
/

UPDATE EMPLOYE SET face_enabled = 1 WHERE face_enabled IS NULL;
COMMIT;

-- Optional: disable Face ID for selected employees
-- UPDATE EMPLOYE SET face_enabled = 0 WHERE matricule = 'EMP-001';
-- COMMIT;
