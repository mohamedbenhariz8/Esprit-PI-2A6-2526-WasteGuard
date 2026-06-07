#include "produit.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDate>
#include <QStringList>
#include <QPair>
#include <QMap>
#include <QVector>

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

QString generateProduitBarcode(int idBac)
{
    const int safeId = qMax(1, idBac);
    const QString body12 = QString("2%1%2")
        .arg(QDate::currentDate().toString("yyMMdd"))
        .arg(safeId, 5, 10, QChar('0'));

    int sumOdd = 0;
    int sumEven = 0;
    for (int i = 0; i < body12.size(); ++i) {
        const int digit = body12.mid(i, 1).toInt();
        if ((i % 2) == 0) sumOdd += digit;
        else sumEven += digit;
    }
    const int total = sumOdd + (sumEven * 3);
    const int checkDigit = (10 - (total % 10)) % 10;
    return body12 + QString::number(checkDigit);
}

bool fetchLotReferenceById(QSqlDatabase &db, int idBac, QString &reference, QString &error)
{
    QSqlQuery q(db);
    q.prepare("SELECT num_serie FROM BAC_INTEL WHERE id_bac = :id_bac");
    q.bindValue(":id_bac", idBac);
    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    if (!q.next()) {
        error = "Produit introuvable.";
        return false;
    }

    reference = q.value(0).toString().trimmed();
    if (reference.isEmpty()) {
        error = "Reference lot invalide.";
        return false;
    }
    return true;
}

QList<int> fetchLotIds(QSqlDatabase &db, const QString &reference, QString &error)
{
    QSqlQuery q(db);
    q.prepare(
        "SELECT id_bac "
        "FROM BAC_INTEL "
        "WHERE num_serie = :reference "
        "ORDER BY NVL(num_unite, 0), id_bac"
    );
    q.bindValue(":reference", reference);
    if (!q.exec()) {
        error = q.lastError().text();
        return {};
    }

    QList<int> ids;
    while (q.next()) {
        const int id = q.value(0).toInt();
        if (id > 0) ids.append(id);
    }
    return ids;
}

bool lotReferenceExistsElsewhere(QSqlDatabase &db,
                                 const QString &targetReference,
                                 const QString &currentReference,
                                 QString &error)
{
    QSqlQuery q(db);
    q.prepare(
        "SELECT COUNT(*) "
        "FROM BAC_INTEL "
        "WHERE num_serie = :target "
        "  AND num_serie <> :current"
    );
    q.bindValue(":target", targetReference);
    q.bindValue(":current", currentReference);
    if (!q.exec() || !q.next()) {
        error = q.lastError().text();
        return false;
    }
    return q.value(0).toInt() > 0;
}

bool hasLinkedRows(QSqlDatabase &db, const QString &tableName, int idBac, QString &error)
{
    QSqlQuery q(db);
    q.prepare(QString("SELECT COUNT(*) FROM %1 WHERE ID_BAC = :id_bac").arg(tableName));
    q.bindValue(":id_bac", idBac);
    if (!q.exec() || !q.next()) {
        error = q.lastError().text();
        return false;
    }
    return q.value(0).toInt() > 0;
}

bool deleteFabricationRow(QSqlDatabase &db, int idBac, QString &error)
{
    QSqlQuery q(db);
    q.prepare("DELETE FROM FABRICATION WHERE ID_BAC = :id_bac");
    q.bindValue(":id_bac", idBac);
    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    return true;
}

bool deleteBacRow(QSqlDatabase &db, int idBac, QString &error)
{
    QSqlQuery q(db);
    q.prepare("DELETE FROM BAC_INTEL WHERE ID_BAC = :id_bac");
    q.bindValue(":id_bac", idBac);
    if (!q.exec()) {
        error = q.lastError().text();
        return false;
    }
    return true;
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
    const int qtyToInsert = m_quantite;
    if (qtyToInsert <= 0) {
        m_lastError = "La quantite doit etre superieure a 0.";
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database();
    const bool useTx = db.isValid() && db.isOpen();
    if (useTx && !db.transaction()) {
        m_lastError = "Impossible de demarrer la transaction.";
        return false;
    }

    int idToUse = m_idMp;
    if (idToUse <= 0) {
        idToUse = resolveNextBacId(m_lastError);
        if (idToUse <= 0) {
            if (useTx) db.rollback();
            return false;
        }
    }

    if (m_reference.trimmed().isEmpty()) {
        m_reference = generateProduitBarcode(idToUse);
    }

    QString locToUse = m_localisation;
    if (locToUse.isEmpty()) {
        locToUse = QString("Stock principal (%1 L)").arg(m_capacite);
    }

    // Keep one logical lot per barcode inside the product module.
    QSqlQuery lotCheck(db);
    lotCheck.prepare("SELECT COUNT(*) FROM BAC_INTEL WHERE num_serie = :num_serie");
    lotCheck.bindValue(":num_serie", m_reference);
    if (!lotCheck.exec() || !lotCheck.next()) {
        if (useTx) db.rollback();
        m_lastError = lotCheck.lastError().text();
        return false;
    }
    if (lotCheck.value(0).toInt() > 0) {
        if (useTx) db.rollback();
        m_lastError = "Cette reference existe deja. Utilisez la modification pour ajuster la quantite.";
        return false;
    }

    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO BAC_INTEL "
        "(id_bac, num_serie, num_unite, statut_bac, modele, remplissage, localisation_stock, id_commande, prix, stock, capacite_batterie, image_path, caracteristiques) "
        "VALUES (:id_bac, :num_serie, :num_unite, :statut_bac, :modele, :remplissage, :localisation_stock, NULL, :prix, :stock, :capacite_batterie, :image_path, :caracteristiques)"
    );

    const int firstId = idToUse;
    for (int unit = 1; unit <= qtyToInsert; ++unit) {
        query.bindValue(":id_bac", idToUse++);
        query.bindValue(":num_serie", m_reference);
        query.bindValue(":num_unite", unit);
        query.bindValue(":statut_bac", "EN_ATTENTE");
        query.bindValue(":modele", m_nom);
        query.bindValue(":remplissage", m_capacite);
        query.bindValue(":localisation_stock", locToUse);
        query.bindValue(":prix", m_prix);
        query.bindValue(":stock", 1);
        query.bindValue(":capacite_batterie", m_capaciteBatterie);
        query.bindValue(":image_path", m_imagePath);
        query.bindValue(":caracteristiques", m_caracteristiques);
        if (!query.exec()) {
            if (useTx) db.rollback();
            m_lastError = query.lastError().text();
            return false;
        }
    }

    if (useTx && !db.commit()) {
        m_lastError = "Impossible de valider la transaction.";
        return false;
    }

    m_idMp = firstId;
    m_lastError.clear();
    return true;
}

bool Produit::modifier()
{
    if (m_idMp <= 0) {
        m_lastError = "ID produit invalide.";
        return false;
    }
    if (m_quantite <= 0) {
        m_lastError = "La quantite doit etre superieure a 0.";
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database();
    const bool useTx = db.isValid() && db.isOpen();
    if (useTx && !db.transaction()) {
        m_lastError = "Impossible de demarrer la transaction.";
        return false;
    }

    QString currentReference;
    if (!fetchLotReferenceById(db, m_idMp, currentReference, m_lastError)) {
        if (useTx) db.rollback();
        return false;
    }

    const QString targetReference = m_reference.trimmed().isEmpty() ? currentReference : m_reference.trimmed();
    if (targetReference != currentReference) {
        QString existsError;
        const bool existsElsewhere = lotReferenceExistsElsewhere(db, targetReference, currentReference, existsError);
        if (!existsError.isEmpty()) {
            if (useTx) db.rollback();
            m_lastError = existsError;
            return false;
        }
        if (existsElsewhere) {
            if (useTx) db.rollback();
            m_lastError = "La nouvelle reference existe deja pour un autre lot.";
            return false;
        }
    }

    QString idsError;
    QList<int> lotIds = fetchLotIds(db, currentReference, idsError);
    if (!idsError.isEmpty()) {
        if (useTx) db.rollback();
        m_lastError = idsError;
        return false;
    }
    if (lotIds.isEmpty()) {
        if (useTx) db.rollback();
        m_lastError = "Aucun bac trouve pour cette reference.";
        return false;
    }

    const int currentQty = lotIds.size();
    const int targetQty = m_quantite;

    // Shrink: remove trailing rows only if they are not referenced elsewhere.
    if (targetQty < currentQty) {
        QVector<int> removableIds;
        removableIds.reserve(currentQty - targetQty);
        for (int i = lotIds.size() - 1; i >= 0 && removableIds.size() < (currentQty - targetQty); --i) {
            const int candidateId = lotIds.at(i);
            QString depErr;
            const bool inCommande = hasLinkedRows(db, "COMMANDE_BAC", candidateId, depErr);
            if (!depErr.isEmpty()) {
                if (useTx) db.rollback();
                m_lastError = depErr;
                return false;
            }
            const bool inIntervention = hasLinkedRows(db, "INTERVENTION", candidateId, depErr);
            if (!depErr.isEmpty()) {
                if (useTx) db.rollback();
                m_lastError = depErr;
                return false;
            }
            if (!inCommande && !inIntervention) {
                removableIds.push_back(candidateId);
            }
        }

        if (removableIds.size() < (currentQty - targetQty)) {
            if (useTx) db.rollback();
            m_lastError = "Impossible de reduire la quantite: certains bacs sont deja utilises dans des commandes/interventions.";
            return false;
        }

        for (int idBac : removableIds) {
            if (!deleteFabricationRow(db, idBac, m_lastError) || !deleteBacRow(db, idBac, m_lastError)) {
                if (useTx) db.rollback();
                return false;
            }
        }
    }

    // Expand: append new physical rows.
    if (targetQty > currentQty) {
        QString idError;
        int nextId = resolveNextBacId(idError);
        if (nextId <= 0) {
            if (useTx) db.rollback();
            m_lastError = idError;
            return false;
        }

        QString locToUse = m_localisation;
        if (locToUse.isEmpty()) {
            locToUse = QString("Stock principal (%1 L)").arg(m_capacite);
        }

        QSqlQuery ins(db);
        ins.prepare(
            "INSERT INTO BAC_INTEL "
            "(id_bac, num_serie, num_unite, statut_bac, modele, remplissage, localisation_stock, id_commande, prix, stock, capacite_batterie, image_path, caracteristiques) "
            "VALUES (:id_bac, :num_serie, :num_unite, :statut_bac, :modele, :remplissage, :localisation_stock, NULL, :prix, :stock, :capacite_batterie, :image_path, :caracteristiques)"
        );

        for (int unit = currentQty + 1; unit <= targetQty; ++unit) {
            ins.bindValue(":id_bac", nextId++);
            ins.bindValue(":num_serie", currentReference);
            ins.bindValue(":num_unite", unit);
            ins.bindValue(":statut_bac", "EN_ATTENTE");
            ins.bindValue(":modele", m_nom);
            ins.bindValue(":remplissage", m_capacite);
            ins.bindValue(":localisation_stock", locToUse);
            ins.bindValue(":prix", m_prix);
            ins.bindValue(":stock", 1);
            ins.bindValue(":capacite_batterie", m_capaciteBatterie);
            ins.bindValue(":image_path", m_imagePath);
            ins.bindValue(":caracteristiques", m_caracteristiques);
            if (!ins.exec()) {
                if (useTx) db.rollback();
                m_lastError = ins.lastError().text();
                return false;
            }
        }
    }

    QString refreshIdsError;
    lotIds = fetchLotIds(db, currentReference, refreshIdsError);
    if (!refreshIdsError.isEmpty()) {
        if (useTx) db.rollback();
        m_lastError = refreshIdsError;
        return false;
    }

    QString locToUse = m_localisation;
    if (locToUse.isEmpty()) {
        locToUse = QString("Stock principal (%1 L)").arg(m_capacite);
    }

    QSqlQuery upd(db);
    upd.prepare(
        "UPDATE BAC_INTEL SET "
        "num_serie = :num_serie, "
        "num_unite = :num_unite, "
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

    for (int i = 0; i < lotIds.size(); ++i) {
        const int idBac = lotIds.at(i);
        upd.bindValue(":id_bac", idBac);
        upd.bindValue(":num_serie", targetReference);
        upd.bindValue(":num_unite", i + 1);
        upd.bindValue(":modele", m_nom);
        upd.bindValue(":remplissage", m_capacite);
        upd.bindValue(":localisation_stock", locToUse);
        upd.bindValue(":prix", m_prix);
        upd.bindValue(":stock", 1);
        upd.bindValue(":capacite_batterie", m_capaciteBatterie);
        upd.bindValue(":image_path", m_imagePath);
        upd.bindValue(":caracteristiques", m_caracteristiques);
        if (!upd.exec()) {
            if (useTx) db.rollback();
            m_lastError = upd.lastError().text();
            return false;
        }
    }

    if (useTx && !db.commit()) {
        m_lastError = "Impossible de valider la transaction.";
        return false;
    }

    if (!lotIds.isEmpty()) {
        m_idMp = lotIds.first();
    }
    m_reference = targetReference;
    m_lastError.clear();
    return true;
}

bool Produit::supprimer(int id_mp)
{
    if (id_mp <= 0) {
        m_lastError = "ID produit invalide.";
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database();
    const bool useTx = db.isValid() && db.isOpen();
    if (useTx && !db.transaction()) {
        m_lastError = "Impossible de demarrer la transaction.";
        return false;
    }

    QString lotReference;
    if (!fetchLotReferenceById(db, id_mp, lotReference, m_lastError)) {
        if (useTx) db.rollback();
        return false;
    }

    QString idsError;
    const QList<int> lotIds = fetchLotIds(db, lotReference, idsError);
    if (!idsError.isEmpty()) {
        if (useTx) db.rollback();
        m_lastError = idsError;
        return false;
    }
    if (lotIds.isEmpty()) {
        if (useTx) db.rollback();
        m_lastError = "Aucun bac trouve pour cette reference.";
        return false;
    }

    for (int idBac : lotIds) {
        QString depErr;
        const bool inCommande = hasLinkedRows(db, "COMMANDE_BAC", idBac, depErr);
        if (!depErr.isEmpty()) {
            if (useTx) db.rollback();
            m_lastError = depErr;
            return false;
        }
        const bool inIntervention = hasLinkedRows(db, "INTERVENTION", idBac, depErr);
        if (!depErr.isEmpty()) {
            if (useTx) db.rollback();
            m_lastError = depErr;
            return false;
        }
        if (inCommande || inIntervention) {
            if (useTx) db.rollback();
            m_lastError = "Suppression impossible: au moins un bac du lot est deja utilise dans une commande/intervention.";
            return false;
        }
    }

    for (int idBac : lotIds) {
        if (!deleteFabricationRow(db, idBac, m_lastError) || !deleteBacRow(db, idBac, m_lastError)) {
            if (useTx) db.rollback();
            return false;
        }
    }

    if (useTx && !db.commit()) {
        m_lastError = "Impossible de valider la transaction.";
        return false;
    }

    m_lastError.clear();
    return true;
}

QSqlQueryModel *Produit::afficher(const QString &searchModel, const QString &sortCriteria)
{
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;

    // Validate sort criteria against whitelist to prevent SQL injection.
    const QMap<QString, QString> sortMap = {
        {"id_bac", "id_mp ASC"},
        {"id_bac ASC", "id_mp ASC"},
        {"id_bac DESC", "id_mp DESC"},
        {"modele", "nom ASC"},
        {"modele ASC", "nom ASC"},
        {"modele DESC", "nom DESC"},
        {"num_serie", "reference ASC"},
        {"num_serie ASC", "reference ASC"},
        {"num_serie DESC", "reference DESC"},
        {"stock", "quantite ASC"},
        {"stock ASC", "quantite ASC"},
        {"stock DESC", "quantite DESC"},
        {"prix", "prix ASC"},
        {"prix ASC", "prix ASC"},
        {"prix DESC", "prix DESC"},
        {"remplissage", "seuil_critique ASC"},
        {"remplissage ASC", "seuil_critique ASC"},
        {"remplissage DESC", "seuil_critique DESC"},
        {"capacite_batterie", "capacite_batterie ASC"},
        {"capacite_batterie ASC", "capacite_batterie ASC"},
        {"capacite_batterie DESC", "capacite_batterie DESC"},
        {"localisation_stock", "localisation_stock ASC"},
        {"localisation_stock ASC", "localisation_stock ASC"},
        {"localisation_stock DESC", "localisation_stock DESC"}
    };

    QString safeSortCriteria = "id_mp ASC";
    const QString sortKey = sortCriteria.trimmed();
    if (!sortKey.isEmpty() && sortMap.contains(sortKey)) {
        safeSortCriteria = sortMap.value(sortKey);
    }

    QString queryString =
        "SELECT "
        "MIN(id_bac) AS id_mp, "
        "num_serie AS reference, "
        "MIN(modele) AS nom, "
        "COUNT(*) AS quantite, "
        "NVL(ROUND(AVG(NVL(remplissage, 0))), 0) AS seuil_critique, "
        "NVL(AVG(NVL(prix, 0)), 0) AS prix, "
        "NVL(ROUND(AVG(NVL(capacite_batterie, 10000))), 10000) AS capacite_batterie, "
        "MAX(image_path) AS image_path, "
        "MAX(caracteristiques) AS caracteristiques, "
        "MAX(localisation_stock) AS localisation_stock "
        "FROM BAC_INTEL ";

    if (!searchModel.isEmpty()) {
        queryString +=
            "WHERE UPPER(modele) LIKE '%' || UPPER(:search) || '%' "
            "   OR UPPER(num_serie) LIKE '%' || UPPER(:search) || '%' ";
    }

    queryString += "GROUP BY num_serie ";
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
    query.prepare("SELECT MIN(id_bac) FROM BAC_INTEL WHERE num_serie = :reference");
    query.bindValue(":reference", reference);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    if (query.next() && !query.value(0).isNull()) {
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
