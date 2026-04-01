#include "client.h"
#include <QSqlError>
#include <QVariant>

Client::Client()
    : m_idClient(0)
{
}

Client::Client(int idClient, const QString &nom, const QString &matricule,
               const QString &email, const QString &typeContrat,
               const QString &statutPaiement, const QString &dateExpiration,
               int tauxTri)
    : m_idClient(idClient), m_nom(nom), m_matricule(matricule),
      m_email(email), m_typeContrat(typeContrat),
      m_statutPaiement(statutPaiement), m_dateExpiration(dateExpiration),
      m_tauxTri(tauxTri)
{
}

// ---------- Getters ----------
int Client::getIdClient() const { return m_idClient; }
QString Client::getNom() const { return m_nom; }
QString Client::getMatricule() const { return m_matricule; }
QString Client::getEmail() const { return m_email; }
QString Client::getTypeContrat() const { return m_typeContrat; }
QString Client::getStatutPaiement() const { return m_statutPaiement; }
QString Client::getDateExpiration() const { return m_dateExpiration; }
int Client::getTauxTri() const { return m_tauxTri; }

// ---------- Setters ----------
void Client::setIdClient(int value) { m_idClient = value; }
void Client::setNom(const QString &value) { m_nom = value; }
void Client::setMatricule(const QString &value) { m_matricule = value; }
void Client::setEmail(const QString &value) { m_email = value; }
void Client::setTypeContrat(const QString &value) { m_typeContrat = value; }
void Client::setStatutPaiement(const QString &value) { m_statutPaiement = value; }
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

    QString statutCode = m_statutPaiement;
    if (statutCode == "Payé" || statutCode == "A_JOUR") statutCode = "A_JOUR";
    else if (statutCode == "En Retard" || statutCode == "EN_RETARD") statutCode = "EN_RETARD";
    else if (statutCode == "En Attente" || statutCode == "SUSPENDU") statutCode = "SUSPENDU";
    else statutCode = "A_JOUR"; // Default

    query.prepare(
        "INSERT INTO CLIENT (ID_CLIENT, NOM, MATRICULE, EMAIL, TYPE_CONTRAT, STATUT_PAIEMENT, DATE_EXPIRATION_CONTRAT, TAUX_TRI) "
        "VALUES (:id, :nom, :matricule, :email, :type_contrat, :statut_paiement, TO_DATE(:date_expiration, 'YYYY-MM-DD'), :taux_tri)"
    );
    query.bindValue(":id", nextId);
    query.bindValue(":nom", m_nom);
    query.bindValue(":matricule", m_matricule);
    query.bindValue(":email", m_email);
    query.bindValue(":type_contrat", m_typeContrat);
    query.bindValue(":statut_paiement", statutCode);
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
    QString statutCode = m_statutPaiement;
    if (statutCode == "Payé" || statutCode == "A_JOUR") statutCode = "A_JOUR";
    else if (statutCode == "En Retard" || statutCode == "EN_RETARD") statutCode = "EN_RETARD";
    else if (statutCode == "En Attente" || statutCode == "SUSPENDU") statutCode = "SUSPENDU";
    else statutCode = "A_JOUR"; // Default

    query.prepare(
        "UPDATE CLIENT SET "
        "NOM = :nom, MATRICULE = :matricule, EMAIL = :email, "
        "TYPE_CONTRAT = :type_contrat, STATUT_PAIEMENT = :statut_paiement, "
        "DATE_EXPIRATION_CONTRAT = TO_DATE(:date_expiration, 'YYYY-MM-DD'), "
        "TAUX_TRI = :taux_tri "
        "WHERE ID_CLIENT = :id"
    );
    query.bindValue(":id", m_idClient);
    query.bindValue(":nom", m_nom);
    query.bindValue(":matricule", m_matricule);
    query.bindValue(":email", m_email);
    query.bindValue(":type_contrat", m_typeContrat);
    query.bindValue(":statut_paiement", statutCode);
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
    
    // Validate sort criteria against whitelist to prevent SQL injection
    QStringList allowedSortColumns = {
        "id_client", "id_client ASC", "id_client DESC",
        "matricule", "matricule ASC", "matricule DESC",
        "nom", "nom ASC", "nom DESC",
        "email", "email ASC", "email DESC",
        "type_contrat", "type_contrat ASC", "type_contrat DESC",
        "statut_paiement", "statut_paiement ASC", "statut_paiement DESC",
        "date_expiration_contrat", "date_expiration_contrat ASC", "date_expiration_contrat DESC",
        "taux_tri", "taux_tri ASC", "taux_tri DESC"
    };
    
    QString safeSortCriteria = "id_client ASC";
    if (!sortCriteria.isEmpty() && allowedSortColumns.contains(sortCriteria.trimmed().toLower())) {
        safeSortCriteria = sortCriteria.trimmed();
    }
    
    // Validate search field
    QStringList validSearchFields = {"matricule", "nom", "email", "type_contrat", "statut_paiement", "taux_tri"};
    
    QString queryString = 
        "SELECT ID_CLIENT, MATRICULE, NOM, EMAIL, TYPE_CONTRAT, TO_CHAR(DATE_EXPIRATION_CONTRAT, 'YYYY-MM-DD'), "
        "CASE STATUT_PAIEMENT "
        "  WHEN 'A_JOUR' THEN 'Payé' "
        "  WHEN 'EN_RETARD' THEN 'En Retard' "
        "  WHEN 'SUSPENDU' THEN 'En Attente' "
        "  ELSE STATUT_PAIEMENT "
        "END AS STATUT_TEXT, TAUX_TRI "
        "FROM CLIENT ";
    
    if (!searchValue.isEmpty() && validSearchFields.contains(searchField.toLower())) {
        QString mappedValue = searchValue;
        if (searchField.toLower() == "statut_paiement") {
             QString sv = searchValue.toLower();
             if (sv.contains("pay") || sv.contains("aj") || sv.contains("jour")) mappedValue = "A_JOUR";
             else if (sv.contains("retard")) mappedValue = "EN_RETARD";
             else if (sv.contains("suspend") || sv.contains("attent")) mappedValue = "SUSPENDU";
        }
        queryString += "WHERE UPPER(" + searchField + ") LIKE '%' || UPPER(:search) || '%' ";
        query.prepare(queryString + "ORDER BY " + safeSortCriteria);
        query.bindValue(":search", mappedValue);
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
