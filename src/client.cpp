#include "client.h"
#include <QSqlError>
#include <QVariant>

namespace {
bool hasModernClientSchema()
{
    QSqlQuery probe;
    return probe.exec("SELECT TELEPHONE, DATE_CREATION_CONTRAT FROM CLIENT WHERE 1=0");
}

[[maybe_unused]] void ensureClientSchemaUpgrade()
{
    if (hasModernClientSchema()) {
        return;
    }

    QSqlQuery q;
    q.exec("ALTER TABLE CLIENT ADD TELEPHONE VARCHAR2(20)");
    q.exec("ALTER TABLE CLIENT ADD DATE_CREATION_CONTRAT DATE");

    if (hasModernClientSchema()) {
        q.exec("UPDATE CLIENT SET DATE_CREATION_CONTRAT = NVL(DATE_CREATION_CONTRAT, SYSDATE)");
    }
}

bool isLegacyClientSchema()
{
    return !hasModernClientSchema();
}
}

Client::Client()
    : m_idClient(0)
{
}

Client::Client(int idClient, const QString &nom, const QString &matricule,
               const QString &email, const QString &typeContrat,
           const QString &telephone, const QString &dateCreationContrat,
           const QString &dateExpiration, int tauxTri)
    : m_idClient(idClient), m_nom(nom), m_matricule(matricule),
      m_email(email), m_typeContrat(typeContrat),
    m_telephone(telephone), m_dateCreationContrat(dateCreationContrat),
    m_dateExpiration(dateExpiration),
      m_tauxTri(tauxTri)
{
}

// ---------- Getters ----------
int Client::getIdClient() const { return m_idClient; }
QString Client::getNom() const { return m_nom; }
QString Client::getMatricule() const { return m_matricule; }
QString Client::getEmail() const { return m_email; }
QString Client::getTypeContrat() const { return m_typeContrat; }
QString Client::getTelephone() const { return m_telephone; }
QString Client::getDateCreationContrat() const { return m_dateCreationContrat; }
QString Client::getDateExpiration() const { return m_dateExpiration; }
int Client::getTauxTri() const { return m_tauxTri; }

// ---------- Setters ----------
void Client::setIdClient(int value) { m_idClient = value; }
void Client::setNom(const QString &value) { m_nom = value; }
void Client::setMatricule(const QString &value) { m_matricule = value; }
void Client::setEmail(const QString &value) { m_email = value; }
void Client::setTypeContrat(const QString &value) { m_typeContrat = value; }
void Client::setTelephone(const QString &value) { m_telephone = value; }
void Client::setDateCreationContrat(const QString &value) { m_dateCreationContrat = value; }
void Client::setDateExpiration(const QString &value) { m_dateExpiration = value; }
void Client::setTauxTri(int value) { m_tauxTri = value; }

// ---------- CRUD ----------
bool Client::ajouter()
{
    QSqlQuery query;
    int nextId = m_idClient;
    if (nextId <= 0) {
        QSqlQuery idQuery;
        if (idQuery.exec("SELECT NVL(MAX(ID_CLIENT), 0) + 1 FROM CLIENT") && idQuery.next()) {
            nextId = idQuery.value(0).toInt();
        } else {
            m_lastError = "Erreur generation ID client.";
            return false;
        }
    }

    const bool legacySchema = isLegacyClientSchema();
    if (legacySchema) {
        query.prepare(
            "INSERT INTO CLIENT (ID_CLIENT, NOM, MATRICULE, EMAIL, TYPE_CONTRAT, STATUT_PAIEMENT, DATE_EXPIRATION_CONTRAT, TAUX_TRI) "
            "VALUES (:id, :nom, :matricule, :email, :type_contrat, :telephone, TO_DATE(:date_expiration, 'YYYY-MM-DD'), :taux_tri)"
        );
    } else {
        query.prepare(
            "INSERT INTO CLIENT (ID_CLIENT, NOM, MATRICULE, EMAIL, TYPE_CONTRAT, TELEPHONE, DATE_CREATION_CONTRAT, DATE_EXPIRATION_CONTRAT, TAUX_TRI) "
            "VALUES (:id, :nom, :matricule, :email, :type_contrat, :telephone, TO_DATE(:date_creation, 'YYYY-MM-DD'), TO_DATE(:date_expiration, 'YYYY-MM-DD'), :taux_tri)"
        );
    }
    query.bindValue(":id", nextId);
    query.bindValue(":nom", m_nom);
    query.bindValue(":matricule", m_matricule);
    query.bindValue(":email", m_email);
    query.bindValue(":type_contrat", m_typeContrat);
    query.bindValue(":telephone", legacySchema ? QString("A_JOUR") : m_telephone);
    if (!legacySchema) {
        query.bindValue(":date_creation", m_dateCreationContrat);
    }
    query.bindValue(":date_expiration", m_dateExpiration);
    query.bindValue(":taux_tri", m_tauxTri);

    if (query.exec()) {
        m_idClient = nextId;
        m_lastError.clear();
        return true;
    }
    m_lastError = query.lastError().text();
    return false;
}

bool Client::modifier()
{
    QSqlQuery query;
    const bool legacySchema = isLegacyClientSchema();
    if (legacySchema) {
        query.prepare(
            "UPDATE CLIENT SET "
            "NOM = :nom, MATRICULE = :matricule, EMAIL = :email, "
            "TYPE_CONTRAT = :type_contrat, STATUT_PAIEMENT = :telephone, "
            "DATE_EXPIRATION_CONTRAT = TO_DATE(:date_expiration, 'YYYY-MM-DD'), "
            "TAUX_TRI = :taux_tri "
            "WHERE ID_CLIENT = :id"
        );
    } else {
        query.prepare(
            "UPDATE CLIENT SET "
            "NOM = :nom, MATRICULE = :matricule, EMAIL = :email, "
            "TYPE_CONTRAT = :type_contrat, TELEPHONE = :telephone, "
            "DATE_CREATION_CONTRAT = TO_DATE(:date_creation, 'YYYY-MM-DD'), "
            "DATE_EXPIRATION_CONTRAT = TO_DATE(:date_expiration, 'YYYY-MM-DD'), "
            "TAUX_TRI = :taux_tri "
            "WHERE ID_CLIENT = :id"
        );
    }
    query.bindValue(":id", m_idClient);
    query.bindValue(":nom", m_nom);
    query.bindValue(":matricule", m_matricule);
    query.bindValue(":email", m_email);
    query.bindValue(":type_contrat", m_typeContrat);
    query.bindValue(":telephone", legacySchema ? QString("A_JOUR") : m_telephone);
    if (!legacySchema) {
        query.bindValue(":date_creation", m_dateCreationContrat);
    }
    query.bindValue(":date_expiration", m_dateExpiration);
    query.bindValue(":taux_tri", m_tauxTri);

    if (query.exec()) {
        m_lastError.clear();
        return true;
    }
    m_lastError = query.lastError().text();
    return false;
}

bool Client::supprimer(int idClient)
{
    QSqlQuery query;
    query.prepare("DELETE FROM CLIENT WHERE ID_CLIENT = :id");
    query.bindValue(":id", idClient);
    if (query.exec()) {
        m_lastError.clear();
        return true;
    }
    m_lastError = query.lastError().text();
    return false;
}

QSqlQueryModel *Client::afficher(const QString &searchField, const QString &searchValue, const QString &sortCriteria)
{
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;
    const bool legacySchema = isLegacyClientSchema();
    
    // Validate sort criteria against whitelist to prevent SQL injection
    QStringList allowedSortColumns = {
        "id_client", "id_client ASC", "id_client DESC",
        "matricule", "matricule ASC", "matricule DESC",
        "nom", "nom ASC", "nom DESC",
        "email", "email ASC", "email DESC",
        "type_contrat", "type_contrat ASC", "type_contrat DESC",
        "telephone", "telephone ASC", "telephone DESC",
        "date_creation_contrat", "date_creation_contrat ASC", "date_creation_contrat DESC",
        "date_expiration_contrat", "date_expiration_contrat ASC", "date_expiration_contrat DESC",
        "taux_tri", "taux_tri ASC", "taux_tri DESC"
    };
    
    QString safeSortCriteria = "id_client ASC";
    if (!sortCriteria.isEmpty() && allowedSortColumns.contains(sortCriteria.trimmed().toLower())) {
        safeSortCriteria = sortCriteria.trimmed();
    }
    if (legacySchema) {
        safeSortCriteria.replace("telephone", "statut_paiement", Qt::CaseInsensitive);
        safeSortCriteria.replace("date_creation_contrat", "date_expiration_contrat", Qt::CaseInsensitive);
    }
    
    // Validate search field
    QStringList validSearchFields = {"matricule", "nom", "email", "type_contrat", "telephone", "date_creation_contrat", "taux_tri"};

    QString queryString;
    if (legacySchema) {
        queryString =
            "SELECT ID_CLIENT, MATRICULE, NOM, EMAIL, TYPE_CONTRAT, '' AS DATE_CREATION, "
            "TO_CHAR(DATE_EXPIRATION_CONTRAT, 'YYYY-MM-DD') AS DATE_EXPIRATION, "
            "CASE WHEN STATUT_PAIEMENT IN ('A_JOUR', 'EN_RETARD', 'SUSPENDU') THEN '' ELSE STATUT_PAIEMENT END AS TELEPHONE, TAUX_TRI "
            "FROM CLIENT ";
    } else {
        queryString =
            "SELECT ID_CLIENT, MATRICULE, NOM, EMAIL, TYPE_CONTRAT, "
            "TO_CHAR(DATE_CREATION_CONTRAT, 'YYYY-MM-DD') AS DATE_CREATION, "
            "TO_CHAR(DATE_EXPIRATION_CONTRAT, 'YYYY-MM-DD') AS DATE_EXPIRATION, "
            "TELEPHONE, TAUX_TRI "
            "FROM CLIENT ";
    }
    
    if (!searchValue.isEmpty() && validSearchFields.contains(searchField.toLower())) {
        QString dbSearchField = searchField.toLower();
        if (legacySchema && dbSearchField == "telephone") {
            dbSearchField = "statut_paiement";
        } else if (legacySchema && dbSearchField == "date_creation_contrat") {
            dbSearchField = "date_expiration_contrat";
        }
        queryString += "WHERE UPPER(" + dbSearchField + ") LIKE '%' || UPPER(:search) || '%' ";
        query.prepare(queryString + "ORDER BY " + safeSortCriteria);
        query.bindValue(":search", searchValue);
    } else {
        query.prepare(queryString + "ORDER BY " + safeSortCriteria);
    }
    
    query.exec();
    model->setQuery(std::move(query));
    m_lastError = model->lastError().isValid() ? model->lastError().text() : QString();
    return model;
}

bool Client::findIdByMatricule(const QString &matricule, int &idClient)
{
    idClient = -1;
    QSqlQuery query;
    query.prepare("SELECT ID_CLIENT FROM CLIENT WHERE UPPER(MATRICULE) = UPPER(:matricule)");
    query.bindValue(":matricule", matricule.trimmed());

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    if (query.next()) {
        idClient = query.value(0).toInt();
        m_lastError.clear();
        return true;
    }

    m_lastError = "Client introuvable.";
    return false;
}

QString Client::lastError() const
{
    return m_lastError;
}
