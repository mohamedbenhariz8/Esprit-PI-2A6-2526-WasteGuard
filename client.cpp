#include "client.h"
#include <QSqlError>
#include <QVariant>

Client::Client()
    : m_idClient(0)
{
}

Client::Client(int idClient, const QString &nom, const QString &matricule,
               const QString &email, const QString &typeContrat,
               const QString &statutPaiement)
    : m_idClient(idClient), m_nom(nom), m_matricule(matricule),
      m_email(email), m_typeContrat(typeContrat),
      m_statutPaiement(statutPaiement)
{
}

// ---------- Getters ----------
int Client::getIdClient() const { return m_idClient; }
QString Client::getNom() const { return m_nom; }
QString Client::getMatricule() const { return m_matricule; }
QString Client::getEmail() const { return m_email; }
QString Client::getTypeContrat() const { return m_typeContrat; }
QString Client::getStatutPaiement() const { return m_statutPaiement; }

// ---------- Setters ----------
void Client::setIdClient(int value) { m_idClient = value; }
void Client::setNom(const QString &value) { m_nom = value; }
void Client::setMatricule(const QString &value) { m_matricule = value; }
void Client::setEmail(const QString &value) { m_email = value; }
void Client::setTypeContrat(const QString &value) { m_typeContrat = value; }
void Client::setStatutPaiement(const QString &value) { m_statutPaiement = value; }

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

    query.prepare(
        "INSERT INTO CLIENT (ID_CLIENT, NOM, MATRICULE, EMAIL, TYPE_CONTRAT, STATUT_PAIEMENT) "
        "VALUES (:id, :nom, :matricule, :email, :type_contrat, :statut_paiement)"
    );
    query.bindValue(":id", nextId);
    query.bindValue(":nom", m_nom);
    query.bindValue(":matricule", m_matricule);
    query.bindValue(":email", m_email);
    query.bindValue(":type_contrat", m_typeContrat);
    query.bindValue(":statut_paiement", m_statutPaiement);

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
    query.prepare(
        "UPDATE CLIENT SET "
        "NOM = :nom, MATRICULE = :matricule, EMAIL = :email, "
        "TYPE_CONTRAT = :type_contrat, STATUT_PAIEMENT = :statut_paiement "
        "WHERE ID_CLIENT = :id"
    );
    query.bindValue(":id", m_idClient);
    query.bindValue(":nom", m_nom);
    query.bindValue(":matricule", m_matricule);
    query.bindValue(":email", m_email);
    query.bindValue(":type_contrat", m_typeContrat);
    query.bindValue(":statut_paiement", m_statutPaiement);

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

QSqlQueryModel *Client::afficher()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(
        "SELECT ID_CLIENT, MATRICULE, NOM, EMAIL, TYPE_CONTRAT, STATUT_PAIEMENT "
        "FROM CLIENT ORDER BY ID_CLIENT"
    );
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
