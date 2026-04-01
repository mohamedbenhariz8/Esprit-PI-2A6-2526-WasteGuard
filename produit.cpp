#include "produit.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QPair>

namespace {
int resolveNextBacId(QString &error)
{
    QSqlQuery q;
    q.prepare("SELECT NVL(MAX(id_bac), 0) + 1 FROM BAC_INTEL");
    if (!q.exec()) {
        error = q.lastError().text();
        return -1;
    }
    if (!q.next() || q.value(0).isNull()) {
        error = "Impossible de generer un identifiant produit.";
        return -1;
    }
    return q.value(0).toInt();
}
}

Produit::Produit()
    : m_idMp(0), m_quantite(0), m_capacite(0), m_prix(0.0), m_capaciteBatterie(10000)
{
}

Produit::Produit(int id_mp, const QString &reference, const QString &nom, int quantite, int capacite, double prix, int capaciteBatterie, const QString &imagePath, const QString &caracteristiques, const QString &localisation)
    : m_idMp(id_mp), m_reference(reference), m_nom(nom), m_quantite(quantite), m_capacite(capacite), m_prix(prix), m_capaciteBatterie(capaciteBatterie), m_imagePath(imagePath), m_caracteristiques(caracteristiques), m_localisation(localisation)
{
}

bool Produit::ajouter()
{
    QSqlQuery query;
    int idToUse = m_idMp;
    if (idToUse <= 0) {
        idToUse = resolveNextBacId(m_lastError);
        if (idToUse <= 0) {
            return false;
        }
    }

    QString locToUse = m_localisation;
    if (locToUse.isEmpty()) {
        locToUse = QString("Stock principal (%1 L)").arg(m_capacite);
    }

    query.prepare(
        "INSERT INTO BAC_INTEL "
        "(id_bac, num_serie, modele, remplissage, localisation_stock, id_commande, prix, stock, capacite_batterie, image_path, caracteristiques) "
        "VALUES (:id_bac, :num_serie, :modele, :remplissage, :localisation_stock, NULL, :prix, :stock, :capacite_batterie, :image_path, :caracteristiques)"
    );

    query.bindValue(":id_bac", idToUse);
    query.bindValue(":num_serie", m_reference);
    query.bindValue(":modele", m_nom);
    query.bindValue(":remplissage", m_capacite);
    query.bindValue(":localisation_stock", locToUse);
    query.bindValue(":prix", m_prix);
    query.bindValue(":stock", m_quantite);
    query.bindValue(":capacite_batterie", m_capaciteBatterie);
    query.bindValue(":image_path", m_imagePath);
    query.bindValue(":caracteristiques", m_caracteristiques);

    const bool ok = query.exec();
    if (ok) {
        m_idMp = idToUse;
        m_lastError.clear();
    } else {
        m_lastError = query.lastError().text();
    }
    return ok;
}

bool Produit::modifier()
{
    QSqlQuery query;
    query.prepare(
        "UPDATE BAC_INTEL SET "
        "num_serie = :num_serie, "
        "modele = :modele, "
        "remplissage = :remplissage, "
        "localisation_stock = :localisation_stock, "
        "prix = :prix, "
        "stock = :stock, "
        "capacite_batterie = :capacite_batterie, "
        "image_path = :image_path, "
        "caracteristiques = :caracteristiques "
        "WHERE id_bac = :id_bac"
    );

    QString locToUse = m_localisation;
    if (locToUse.isEmpty()) {
        locToUse = QString("Stock principal (%1 L)").arg(m_capacite);
    }

    query.bindValue(":id_bac", m_idMp);
    query.bindValue(":num_serie", m_reference);
    query.bindValue(":modele", m_nom);
    query.bindValue(":remplissage", m_capacite);
    query.bindValue(":localisation_stock", locToUse);
    query.bindValue(":prix", m_prix);
    query.bindValue(":stock", m_quantite);
    query.bindValue(":capacite_batterie", m_capaciteBatterie);
    query.bindValue(":image_path", m_imagePath);
    query.bindValue(":caracteristiques", m_caracteristiques);

    const bool ok = query.exec();
    m_lastError = ok ? QString() : query.lastError().text();
    return ok;
}

bool Produit::supprimer(int id_mp)
{
    QSqlQuery query;
    query.prepare("DELETE FROM BAC_INTEL WHERE id_bac = :id_bac");
    query.bindValue(":id_bac", id_mp);
    const bool ok = query.exec();
    m_lastError = ok ? QString() : query.lastError().text();
    return ok;
}

QSqlQueryModel *Produit::afficher(const QString &searchModel, const QString &sortCriteria)
{
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;
    
    // Validate sort criteria against whitelist to prevent SQL injection
    QStringList allowedSortColumns = {
        "id_bac", "id_bac ASC", "id_bac DESC",
        "modele", "modele ASC", "modele DESC",
        "num_serie", "num_serie ASC", "num_serie DESC",
        "stock", "stock ASC", "stock DESC",
        "prix", "prix ASC", "prix DESC",
        "remplissage", "remplissage ASC", "remplissage DESC",
        "capacite_batterie", "capacite_batterie ASC", "capacite_batterie DESC",
        "localisation_stock", "localisation_stock ASC", "localisation_stock DESC"
    };
    
    QString safeSortCriteria = "id_bac ASC"; // Default safe sort
    if (!sortCriteria.isEmpty() && allowedSortColumns.contains(sortCriteria.trimmed())) {
        safeSortCriteria = sortCriteria.trimmed();
    }
    
    QString queryString = 
        "SELECT "
        "id_bac AS id_mp, "
        "num_serie AS reference, "
        "modele AS nom, "
        "NVL(stock, NVL(ROUND(remplissage), 0)) AS quantite, "
        "NVL(ROUND(remplissage), 100) AS seuil_critique, "
        "NVL(prix, 0) AS prix, "
        "NVL(ROUND(capacite_batterie), 10000) AS capacite_batterie, "
        "image_path, "
        "caracteristiques, "
        "localisation_stock "
        "FROM BAC_INTEL ";
        
    if (!searchModel.isEmpty()) {
        queryString +=
            "WHERE UPPER(modele) LIKE '%' || UPPER(:search) || '%' "
            "   OR UPPER(num_serie) LIKE '%' || UPPER(:search) || '%' ";
    }
    
    queryString += "ORDER BY " + safeSortCriteria;

    query.prepare(queryString);
    if (!searchModel.isEmpty()) {
        query.bindValue(":search", searchModel);
    }
    
    query.exec();
    model->setQuery(std::move(query));
    
    m_lastError = model->lastError().isValid() ? model->lastError().text() : QString();
    return model;
}

bool Produit::findIdByReference(const QString &reference, int &id_mp)
{
    id_mp = -1;
    QSqlQuery query;
    query.prepare("SELECT id_bac FROM BAC_INTEL WHERE num_serie = :reference");
    query.bindValue(":reference", reference);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    if (query.next()) {
        id_mp = query.value(0).toInt();
        m_lastError.clear();
        return true;
    }

    m_lastError = "Produit introuvable.";
    return false;
}

QString Produit::lastError() const { return m_lastError; }

QList<QPair<int, QString>> Produit::fetchAvailableCapteurOptions(int minQty, int idBac, QString *error) const
{
    int minRequired = minQty;
    if (minRequired <= 0) minRequired = 1;

    QSqlQuery query;
    if (idBac > 0) {
        query.prepare(
            "SELECT DISTINCT mp.ID_MP, mp.NOM "
            "FROM MATIERE_PREMIERE mp "
            "LEFT JOIN FABRICATION f ON f.ID_MP = mp.ID_MP AND f.ID_BAC = :id_bac "
            "WHERE NVL(mp.QUANTITE, 0) >= (:minQty * NVL(f.QTE_UTILISE, 1)) "
            "  AND (UPPER(mp.NOM) LIKE 'CAPTUR%' OR UPPER(mp.NOM) LIKE 'CAPTEUR%') "
            "ORDER BY mp.NOM"
        );
        query.bindValue(":id_bac", idBac);
        query.bindValue(":minQty", minRequired);
    } else {
        query.prepare(
            "SELECT DISTINCT ID_MP, NOM "
            "FROM MATIERE_PREMIERE "
            "WHERE NVL(QUANTITE, 0) >= :minQty "
            "  AND (UPPER(NOM) LIKE 'CAPTUR%' OR UPPER(NOM) LIKE 'CAPTEUR%') "
            "ORDER BY NOM"
        );
        query.bindValue(":minQty", minRequired);
    }

    if (!query.exec()) {
        if (error) *error = query.lastError().text();
        return {};
    }

    QList<QPair<int, QString>> results;
    while (query.next()) {
        const int idMp = query.value(0).toInt();
        const QString name = query.value(1).toString().trimmed();
        if (idMp > 0 && !name.isEmpty()) results.append(qMakePair(idMp, name));
    }
    return results;
}

QStringList Produit::fetchAvailableCapteurFeatures(int minQty, int idBac, QString *error) const
{
    const auto options = fetchAvailableCapteurOptions(minQty, idBac, error);
    QStringList names;
    names.reserve(options.size());
    for (const auto &opt : options) {
        names << opt.second;
    }
    return names;
}

bool Produit::syncFabricationForBac(int idBac, const QList<int> &idMps, int qteUtiliseDefault)
{
    if (idBac <= 0) {
        m_lastError = "ID bac invalide pour fabrication.";
        return false;
    }

    QSqlQuery del;
    del.prepare("DELETE FROM FABRICATION WHERE ID_BAC = :id_bac");
    del.bindValue(":id_bac", idBac);
    if (!del.exec()) {
        m_lastError = del.lastError().text();
        return false;
    }

    if (idMps.isEmpty()) {
        m_lastError.clear();
        return true;
    }

    QSqlQuery ins;
    ins.prepare(
        "INSERT INTO FABRICATION (ID_MP, ID_BAC, QTE_UTILISE) "
        "VALUES (:id_mp, :id_bac, :qte)"
    );

    const int qte = qteUtiliseDefault > 0 ? qteUtiliseDefault : 1;
    for (int idMp : idMps) {
        if (idMp <= 0) continue;
        ins.bindValue(":id_mp", idMp);
        ins.bindValue(":id_bac", idBac);
        ins.bindValue(":qte", qte);
        if (!ins.exec()) {
            m_lastError = ins.lastError().text();
            return false;
        }
    }

    m_lastError.clear();
    return true;
}

bool Produit::syncFabricationForBacByNames(int idBac, const QStringList &capteurNames, int qteUtiliseDefault)
{
    QList<int> ids;
    ids.reserve(capteurNames.size());
    QSqlQuery idQuery;
    for (const QString &raw : capteurNames) {
        const QString name = raw.trimmed();
        if (name.isEmpty()) continue;
        idQuery.prepare("SELECT MIN(ID_MP) FROM MATIERE_PREMIERE WHERE UPPER(TRIM(NOM)) = UPPER(:nom)");
        idQuery.bindValue(":nom", name);
        if (idQuery.exec() && idQuery.next() && !idQuery.value(0).isNull()) {
            ids.append(idQuery.value(0).toInt());
        }
    }
    return syncFabricationForBac(idBac, ids, qteUtiliseDefault);
}

bool Produit::consumeMatiereForFabrication(int idBac, int produitQty)
{
    if (idBac <= 0 || produitQty <= 0) {
        m_lastError.clear();
        return true;
    }

    QSqlDatabase db = QSqlDatabase::database();
    const bool useTx = db.isValid() && db.isOpen();
    if (useTx && !db.transaction()) {
        m_lastError = "Impossible de demarrer la transaction.";
        return false;
    }

    QSqlQuery countQ(db);
    countQ.prepare("SELECT COUNT(*) FROM FABRICATION WHERE ID_BAC = :id_bac");
    countQ.bindValue(":id_bac", idBac);
    if (!countQ.exec() || !countQ.next()) {
        if (useTx) db.rollback();
        m_lastError = countQ.lastError().text();
        return false;
    }
    if (countQ.value(0).toInt() == 0) {
        if (useTx) db.rollback();
        m_lastError = "Aucune fabrication associee au produit.";
        return false;
    }

    QSqlQuery check(db);
    check.prepare(
        "SELECT mp.NOM, NVL(mp.QUANTITE, 0) "
        "FROM MATIERE_PREMIERE mp "
        "JOIN FABRICATION f ON f.ID_MP = mp.ID_MP "
        "WHERE f.ID_BAC = :id_bac "
        "  AND NVL(mp.QUANTITE, 0) < :qty"
    );
    check.bindValue(":id_bac", idBac);
    check.bindValue(":qty", produitQty);

    if (!check.exec()) {
        if (useTx) db.rollback();
        m_lastError = check.lastError().text();
        return false;
    }

    if (check.next()) {
        const QString nom = check.value(0).toString().trimmed();
        const double dispo = check.value(1).toDouble();
        if (useTx) db.rollback();
        m_lastError = QString("Stock insuffisant pour %1 (dispo: %2, besoin: %3).")
                          .arg(nom)
                          .arg(dispo)
                          .arg(produitQty);
        return false;
    }

    QSqlQuery upd(db);
    upd.prepare(
        "UPDATE MATIERE_PREMIERE mp "
        "SET mp.QUANTITE = NVL(mp.QUANTITE, 0) - :qty "
        "WHERE EXISTS (SELECT 1 FROM FABRICATION f WHERE f.ID_MP = mp.ID_MP AND f.ID_BAC = :id_bac)"
    );
    upd.bindValue(":qty", produitQty);
    upd.bindValue(":id_bac", idBac);

    if (!upd.exec()) {
        if (useTx) db.rollback();
        m_lastError = upd.lastError().text();
        return false;
    }

    if (useTx && !db.commit()) {
        m_lastError = "Impossible de valider la transaction.";
        return false;
    }

    m_lastError.clear();
    return true;
}

bool Produit::restoreMatiereForFabrication(int idBac, int produitQty)
{
    if (idBac <= 0 || produitQty <= 0) {
        m_lastError.clear();
        return true;
    }

    QSqlDatabase db = QSqlDatabase::database();
    const bool useTx = db.isValid() && db.isOpen();
    if (useTx && !db.transaction()) {
        m_lastError = "Impossible de demarrer la transaction.";
        return false;
    }

    QSqlQuery upd(db);
    upd.prepare(
        "UPDATE MATIERE_PREMIERE mp "
        "SET mp.QUANTITE = NVL(mp.QUANTITE, 0) + :qty "
        "WHERE EXISTS (SELECT 1 FROM FABRICATION f WHERE f.ID_MP = mp.ID_MP AND f.ID_BAC = :id_bac)"
    );
    upd.bindValue(":qty", produitQty);
    upd.bindValue(":id_bac", idBac);

    if (!upd.exec()) {
        if (useTx) db.rollback();
        m_lastError = upd.lastError().text();
        return false;
    }

    if (useTx && !db.commit()) {
        m_lastError = "Impossible de valider la transaction.";
        return false;
    }

    m_lastError.clear();
    return true;
}

int Produit::getIdMp() const { return m_idMp; }
QString Produit::getReference() const { return m_reference; }
QString Produit::getNom() const { return m_nom; }
int Produit::getQuantite() const { return m_quantite; }
int Produit::getCapacite() const { return m_capacite; }
double Produit::getPrix() const { return m_prix; }
int Produit::getCapaciteBatterie() const { return m_capaciteBatterie; }

void Produit::setIdMp(int value) { m_idMp = value; }
void Produit::setReference(const QString &value) { m_reference = value; }
void Produit::setNom(const QString &value) { m_nom = value; }
void Produit::setQuantite(int value) { m_quantite = value; }
void Produit::setCapacite(int value) { m_capacite = value; }
void Produit::setPrix(double value) { m_prix = value; }
void Produit::setCapaciteBatterie(int value) { m_capaciteBatterie = value; }
void Produit::setImagePath(const QString &value) { m_imagePath = value; }
void Produit::setCaracteristiques(const QString &value) { m_caracteristiques = value; }
void Produit::setLocalisation(const QString &value) { m_localisation = value; }

QString Produit::getImagePath() const { return m_imagePath; }
QString Produit::getCaracteristiques() const { return m_caracteristiques; }
QString Produit::getLocalisation() const { return m_localisation; }
