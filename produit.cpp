#include "produit.h"

#include <QSqlError>
#include <QSqlQuery>

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
    m_lastError = ok ? QString() : query.lastError().text();
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
        queryString += "WHERE UPPER(modele) LIKE '%' || UPPER(:search) || '%' ";
    }
    
    queryString += "ORDER BY " + sortCriteria;

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
