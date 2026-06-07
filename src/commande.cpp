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
    : m_id(id), m_idClient(idClient), m_qte(qte), 
      m_prixTotal(prixTotal), m_adresse(""), m_startAdresse(""), m_pathHistory(""), m_dateCommande(dateCommande),
      m_dateLivraison(dateLivraison)
{
    setPriorite(priorite);
    setStatut(statut);
}

int Commande::id() const { return m_id; }
QString Commande::refer() const { return m_refer; }
int Commande::idClient() const { return m_idClient; }
int Commande::qte() const { return m_qte; }
QString Commande::priorite() const { return m_priorite; }
QString Commande::statut() const { return m_statut; }
QDate Commande::dateCommande() const { return m_dateCommande; }
QDate Commande::dateLivraison() const { return m_dateLivraison; }
double Commande::prixTotal() const { return m_prixTotal; }
QString Commande::adresse() const { return m_adresse; }
QString Commande::startAdresse() const { return m_startAdresse; }
QString Commande::pathHistory() const { return m_pathHistory; }

void Commande::setId(int id) { m_id = id; }
void Commande::setRefer(const QString &refer) { m_refer = refer.trimmed(); }
void Commande::setIdClient(int idClient) { m_idClient = idClient; }
void Commande::setQte(int qte) { m_qte = qte; }
void Commande::setPriorite(const QString &priorite) { 
    QString p = priorite.toUpper();
    if (p.contains("URGENT")) m_priorite = "URGENTE";
    else if (p.contains("HAUT")) m_priorite = "HAUTE";
    else if (p.contains("FAIBL")) m_priorite = "FAIBLE";
    else m_priorite = "NORMALE";
}

void Commande::setStatut(const QString &statut) {
    QString s = statut.trimmed().toUpper();
    // Map UI combobox display values to the DB-allowed values (CHK_STATUT constraint):
    // EN_PREPARATION | LIVREE | LIVRAISON_EN_COURS | ANNULEE
    if (s.contains("COURS"))
        m_statut = "LIVRAISON_EN_COURS";
    else if (s.contains("LIVR"))
        m_statut = "LIVREE";
    else if (s.contains("ANNUL"))
        m_statut = "ANNULEE";
    else
        m_statut = "EN_PREPARATION"; // "Confirmation Pendante" and any unknown value
}

void Commande::setDateCommande(const QDate &date) { m_dateCommande = date; }
void Commande::setDateLivraison(const QDate &date) { m_dateLivraison = date; }
void Commande::setPrixTotal(double prix) { m_prixTotal = prix; }
void Commande::setAdresse(const QString &adresse) { m_adresse = adresse; }
void Commande::setStartAdresse(const QString &startAdresse) { m_startAdresse = startAdresse; }
void Commande::setPathHistory(const QString &pathHistory) { m_pathHistory = pathHistory; }

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

    query.prepare("INSERT INTO COMMANDE (ID_COMMANDE, ID_CLIENT, QTE, PRIORITE, STATUT, DATE_COMMANDE, DATE_LIVRAISON, PRIX_TOTAL, ADRESSE, REFER, START_ADRESSE, PATH_HISTORY) "
                  "VALUES (:id, :client, :qte, :prio, :stat, TO_DATE(:dcmd, 'DD-MM-YYYY'), TO_DATE(:dliv, 'DD-MM-YYYY'), :prix, :adresse, :refer, :startAdresse, :pathHistory)");
    query.bindValue(":id", nextId);
    query.bindValue(":client", m_idClient);
    query.bindValue(":qte", m_qte);
    query.bindValue(":prio", m_priorite);
    query.bindValue(":stat", m_statut);
    
    // Use the string versions if available, otherwise fallback to QDate format
    QString dcmd = !m_dateCmdStr.isEmpty() ? m_dateCmdStr : m_dateCommande.toString("dd-MM-yyyy");
    QString dliv = !m_dateLivStr.isEmpty() ? m_dateLivStr : m_dateLivraison.toString("dd-MM-yyyy");
    query.bindValue(":dcmd", dcmd);
    query.bindValue(":dliv", dliv);
    
    query.bindValue(":prix", m_prixTotal);
    query.bindValue(":adresse", m_adresse);
    query.bindValue(":refer", m_refer.trimmed());
    query.bindValue(":startAdresse", m_startAdresse);
    query.bindValue(":pathHistory", m_pathHistory.isEmpty() ? QString("%1").arg(m_startAdresse) : m_pathHistory);

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
                  "DATE_COMMANDE=TO_DATE(:dcmd, 'DD-MM-YYYY'), DATE_LIVRAISON=TO_DATE(:dliv, 'DD-MM-YYYY'), "
                  "PRIX_TOTAL=:prix, ADRESSE=:adresse, REFER=:refer, START_ADRESSE=:startAdresse, PATH_HISTORY=:pathHistory "
                  "WHERE ID_COMMANDE=:id");
    query.bindValue(":id", m_id);
    query.bindValue(":client", m_idClient);
    query.bindValue(":qte", m_qte);
    query.bindValue(":prio", m_priorite);
    query.bindValue(":stat", m_statut);
    
    QString dcmd = !m_dateCmdStr.isEmpty() ? m_dateCmdStr : m_dateCommande.toString("dd-MM-yyyy");
    QString dliv = !m_dateLivStr.isEmpty() ? m_dateLivStr : m_dateLivraison.toString("dd-MM-yyyy");
    query.bindValue(":dcmd", dcmd);
    query.bindValue(":dliv", dliv);

    query.bindValue(":prix", m_prixTotal);
    query.bindValue(":adresse", m_adresse);
    query.bindValue(":refer", m_refer.trimmed());
    query.bindValue(":startAdresse", m_startAdresse);
    query.bindValue(":pathHistory", m_pathHistory);

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

QSqlQueryModel *Commande::afficher(const QString &searchField, const QString &searchValue, const QString &sortCriteria)
{
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;
    
    // Validate sort criteria against whitelist to prevent SQL injection
    QStringList allowedSortColumns = {
        "c.id_commande", "c.id_commande asc", "c.id_commande desc",
        "c.refer", "c.refer asc", "c.refer desc",
        "cl.matricule", "cl.matricule asc", "cl.matricule desc",
        "c.qte", "c.qte asc", "c.qte desc",
        "c.priorite", "c.priorite asc", "c.priorite desc",
        "c.statut", "c.statut asc", "c.statut desc",
        "c.date_commande", "c.date_commande asc", "c.date_commande desc",
        "c.date_livraison", "c.date_livraison asc", "c.date_livraison desc",
        "c.prix_total", "c.prix_total asc", "c.prix_total desc"
    };
    
    QString safeSortCriteria = "c.id_commande ASC";
    if (!sortCriteria.isEmpty()) {
        QString lowerCrit = sortCriteria.toLower().trimmed();
        bool found = false;
        for (const QString &col : allowedSortColumns) {
            if (col.toLower() == lowerCrit) {
                safeSortCriteria = col;
                found = true;
                break;
            }
        }
        if (!found && !lowerCrit.contains(" ")) {
             // allow e.g. "c.refer" even if no asc/desc
             for (const QString &col : allowedSortColumns) {
                 if (col.toLower().startsWith(lowerCrit + " ")) {
                     safeSortCriteria = col;
                     break;
                 }
             }
        }
    }
    
    QString queryString = 
        "SELECT c.REFER, cl.MATRICULE, c.QTE, c.PRIORITE, c.STATUT, c.DATE_COMMANDE, c.DATE_LIVRAISON, c.PRIX_TOTAL, c.ID_CLIENT, c.ADRESSE, c.ID_COMMANDE, c.START_ADRESSE, c.PATH_HISTORY "
        "FROM COMMANDE c "
        "LEFT JOIN CLIENT cl ON c.ID_CLIENT = cl.ID_CLIENT ";
    
    if (!searchValue.isEmpty()) {
        if (searchField.isEmpty() || searchField.toLower() == "all") {
            queryString += "WHERE (UPPER(c.REFER) LIKE '%' || UPPER(:search) || '%' "
                           "OR UPPER(c.PRIORITE) LIKE '%' || UPPER(:search) || '%' "
                           "OR UPPER(c.STATUT) LIKE '%' || UPPER(:search) || '%' "
                           "OR UPPER(c.ADRESSE) LIKE '%' || UPPER(:search) || '%' "
                           "OR UPPER(cl.MATRICULE) LIKE '%' || UPPER(:search) || '%') ";
        } else {
            QString searchColumn = "c." + searchField;
            if (searchField.toLower() == "matricule") searchColumn = "cl.matricule";
            else if (searchField.toLower() == "reference") searchColumn = "c.refer";
            queryString += "WHERE UPPER(" + searchColumn + ") LIKE '%' || UPPER(:search) || '%' ";
        }
        query.prepare(queryString + "ORDER BY " + safeSortCriteria);
        query.bindValue(":search", searchValue.trimmed());
    } else {
        query.prepare(queryString + "ORDER BY " + safeSortCriteria);
    }
    
    query.exec();
    model->setQuery(std::move(query));
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
