#include "commande.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

Commande::Commande()
    : m_id(0), m_idClient(0), m_qte(0), m_prixTotal(0.0)
{
}

Commande::Commande(int id, int idClient, int qte, const QString &priorite,
                   const QString &statut, const QDate &dateCommande,
                   const QDate &dateLivraison, double prixTotal)
    : m_id(id), m_idClient(idClient), m_qte(qte), m_priorite(priorite),
      m_statut(statut), m_dateCommande(dateCommande),
      m_dateLivraison(dateLivraison), m_prixTotal(prixTotal), m_adresse("")
{
}

int Commande::id() const { return m_id; }
int Commande::idClient() const { return m_idClient; }
int Commande::qte() const { return m_qte; }
QString Commande::priorite() const { return m_priorite; }
QString Commande::statut() const { return m_statut; }
QDate Commande::dateCommande() const { return m_dateCommande; }
QDate Commande::dateLivraison() const { return m_dateLivraison; }
double Commande::prixTotal() const { return m_prixTotal; }
QString Commande::adresse() const { return m_adresse; }

void Commande::setId(int id) { m_id = id; }
void Commande::setIdClient(int idClient) { m_idClient = idClient; }
void Commande::setQte(int qte) { m_qte = qte; }
void Commande::setPriorite(const QString &priorite) { m_priorite = priorite; }
void Commande::setStatut(const QString &statut) { m_statut = statut; }
void Commande::setDateCommande(const QDate &date) { m_dateCommande = date; }
void Commande::setDateLivraison(const QDate &date) { m_dateLivraison = date; }
void Commande::setPrixTotal(double prix) { m_prixTotal = prix; }
void Commande::setAdresse(const QString &adresse) { m_adresse = adresse; }

bool Commande::ajouter()
{
    QSqlQuery query;
    int nextId = m_id;
    if (nextId <= 0) {
        QSqlQuery idQuery;
        if (idQuery.exec("SELECT NVL(MAX(ID_COMMANDE), 0) + 1 FROM COMMANDE") && idQuery.next()) {
            nextId = idQuery.value(0).toInt();
        } else {
            m_lastError = "Erreur generation ID.";
            return false;
        }
    }

    query.prepare("INSERT INTO COMMANDE (ID_COMMANDE, ID_CLIENT, QTE, PRIORITE, STATUT, DATE_COMMANDE, DATE_LIVRAISON, PRIX_TOTAL, ADRESSE) "
                  "VALUES (:id, :client, :qte, :prio, :stat, :dcmd, :dliv, :prix, :adresse)");
    query.bindValue(":id", nextId);
    query.bindValue(":client", m_idClient);
    query.bindValue(":qte", m_qte);
    query.bindValue(":prio", m_priorite);
    query.bindValue(":stat", m_statut);
    query.bindValue(":dcmd", m_dateCommande);
    query.bindValue(":dliv", m_dateLivraison);
    query.bindValue(":prix", m_prixTotal);
    query.bindValue(":adresse", m_adresse);

    if (query.exec()) {
        m_id = nextId;
        return true;
    }
    m_lastError = query.lastError().text();
    return false;
}

bool Commande::modifier()
{
    QSqlQuery query;
    query.prepare("UPDATE COMMANDE SET ID_CLIENT=:client, QTE=:qte, PRIORITE=:prio, STATUT=:stat, "
                  "DATE_COMMANDE=:dcmd, DATE_LIVRAISON=:dliv, PRIX_TOTAL=:prix, ADRESSE=:adresse "
                  "WHERE ID_COMMANDE=:id");
    query.bindValue(":id", m_id);
    query.bindValue(":client", m_idClient);
    query.bindValue(":qte", m_qte);
    query.bindValue(":prio", m_priorite);
    query.bindValue(":stat", m_statut);
    query.bindValue(":dcmd", m_dateCommande);
    query.bindValue(":dliv", m_dateLivraison);
    query.bindValue(":prix", m_prixTotal);
    query.bindValue(":adresse", m_adresse);

    if (query.exec()) return true;
    m_lastError = query.lastError().text();
    return false;
}

bool Commande::supprimer(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM COMMANDE WHERE ID_COMMANDE = :id");
    query.bindValue(":id", id);
    if (query.exec()) return true;
    m_lastError = query.lastError().text();
    return false;
}

QSqlQueryModel *Commande::afficher()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    // Joining with CLIENT to show Matricule
    model->setQuery("SELECT c.ID_COMMANDE, cl.MATRICULE, c.QTE, c.PRIORITE, c.STATUT, c.DATE_COMMANDE, c.DATE_LIVRAISON, c.PRIX_TOTAL, c.ID_CLIENT, c.ADRESSE "
                    "FROM COMMANDE c "
                    "LEFT JOIN CLIENT cl ON c.ID_CLIENT = cl.ID_CLIENT "
                    "ORDER BY c.ID_COMMANDE");
    if (model->lastError().isValid()) {
        m_lastError = model->lastError().text();
    }
    return model;
}

int Commande::findClientByMatricule(const QString &matricule)
{
    QSqlQuery query;
    // Uses UPPER() to match regardless of how the data was typed into Oracle
    query.prepare("SELECT ID_CLIENT FROM CLIENT WHERE UPPER(MATRICULE) = UPPER(:mat)");
    query.bindValue(":mat", matricule.trimmed());

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

QString Commande::getClientMatricule(int idClient)
{
    QSqlQuery query;
    query.prepare("SELECT MATRICULE FROM CLIENT WHERE ID_CLIENT = :id");
    query.bindValue(":id", idClient);
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return QString();
}

QString Commande::lastError() const
{
    return m_lastError;
}
