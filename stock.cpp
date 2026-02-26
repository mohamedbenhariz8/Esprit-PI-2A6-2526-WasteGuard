#include "stock.h"

#include <QDate>
#include <QSqlError>
#include <QSqlQuery>

Stock::Stock()
    : m_idMp(0),
      m_quantite(0),
      m_seuilCritique(5),
      m_prix(0.0)
{
}

Stock::Stock(int idMp,
             const QString &reference,
             const QString &nom,
             int quantite,
             int seuilCritique,
             double prix,
             const QString &fournisseurInput)
    : m_idMp(idMp),
      m_reference(reference),
      m_nom(nom),
      m_quantite(quantite),
      m_seuilCritique(seuilCritique),
      m_prix(prix),
      m_fournisseurInput(fournisseurInput)
{
}

int Stock::resolveNextMpId()
{
    QSqlQuery q;
    q.prepare("SELECT NVL(MAX(id_mp), 0) + 1 FROM MATIERE_PREMIERE");
    if (!q.exec()) {
        m_lastError = q.lastError().text();
        return -1;
    }
    if (!q.next() || q.value(0).isNull()) {
        m_lastError = "Impossible de generer un id pour MATIERE_PREMIERE.";
        return -1;
    }
    return q.value(0).toInt();
}

int Stock::resolveNextFournisseurId()
{
    QSqlQuery q;
    q.prepare("SELECT NVL(MAX(id_four), 0) + 1 FROM FOURNISSEUR");
    if (!q.exec()) {
        m_lastError = q.lastError().text();
        return -1;
    }
    if (!q.next() || q.value(0).isNull()) {
        m_lastError = "Impossible de generer un id fournisseur.";
        return -1;
    }
    return q.value(0).toInt();
}

bool Stock::resolveFournisseurIdFromInput(const QString &inputRaw, int &idFour, QString &nomFour)
{
    const QString input = inputRaw.trimmed();
    if (input.isEmpty()) {
        m_lastError = "Le fournisseur est obligatoire.";
        return false;
    }

    auto byId = [&](int id) -> bool {
        QSqlQuery q;
        q.prepare("SELECT id_four, nom FROM FOURNISSEUR WHERE id_four = :id");
        q.bindValue(":id", id);
        if (!q.exec()) {
            m_lastError = q.lastError().text();
            return false;
        }
        if (!q.next()) {
            m_lastError = "ID fournisseur introuvable.";
            return false;
        }
        idFour = q.value(0).toInt();
        nomFour = q.value(1).toString().trimmed();
        m_lastError.clear();
        return true;
    };

    bool isInt = false;
    const int parsedId = input.toInt(&isInt);
    if (isInt) {
        return byId(parsedId);
    }

    auto byNameQuery = [&](const QString &sql, const QString &val) -> bool {
        QSqlQuery q;
        q.prepare(sql);
        q.bindValue(":nom", val);
        if (!q.exec()) {
            m_lastError = q.lastError().text();
            return false;
        }
        if (!q.next()) {
            return false;
        }
        idFour = q.value(0).toInt();
        nomFour = q.value(1).toString().trimmed();
        m_lastError.clear();
        return true;
    };

    if (byNameQuery("SELECT id_four, nom FROM FOURNISSEUR WHERE UPPER(nom)=UPPER(:nom)", input)) {
        return true;
    }

    if (byNameQuery("SELECT id_four, nom FROM FOURNISSEUR WHERE UPPER(nom) LIKE UPPER(:nom) ORDER BY id_four", input + "%")) {
        return true;
    }

    const int newId = resolveNextFournisseurId();
    if (newId <= 0) return false;

    QSqlQuery ins;
    ins.prepare("INSERT INTO FOURNISSEUR (id_four, nom, email, telephone) VALUES (:id, :nom, NULL, NULL)");
    ins.bindValue(":id", newId);
    ins.bindValue(":nom", input);
    if (!ins.exec()) {
        m_lastError = ins.lastError().text();
        return false;
    }

    idFour = newId;
    nomFour = input;
    m_lastError.clear();
    return true;
}

bool Stock::ajouter()
{
    const int idMp = (m_idMp > 0) ? m_idMp : resolveNextMpId();
    if (idMp <= 0) return false;

    int idFour = -1;
    QString nomFour;
    if (!resolveFournisseurIdFromInput(m_fournisseurInput, idFour, nomFour)) {
        return false;
    }

    const int seuil = (m_seuilCritique > 0) ? m_seuilCritique : qMax(5, m_quantite / 5);

    QSqlQuery q;
    q.prepare(
        "INSERT INTO MATIERE_PREMIERE "
        "(id_mp, reference, nom, quantite, seuil_critique, prix, date_fabrication, date_achat, id_four) "
        "VALUES (:id_mp, :reference, :nom, :quantite, :seuil, :prix, :df, :da, :id_four)");

    q.bindValue(":id_mp", idMp);
    q.bindValue(":reference", m_reference.trimmed());
    q.bindValue(":nom", m_nom.trimmed());
    q.bindValue(":quantite", m_quantite);
    q.bindValue(":seuil", seuil);
    q.bindValue(":prix", m_prix);
    q.bindValue(":df", QDate::currentDate());
    q.bindValue(":da", QDate::currentDate());
    q.bindValue(":id_four", idFour);

    const bool ok = q.exec();
    m_lastError = ok ? QString() : q.lastError().text();
    return ok;
}

bool Stock::modifier()
{
    if (m_idMp <= 0) {
        m_lastError = "ID MATIERE_PREMIERE invalide pour la modification.";
        return false;
    }

    int idFour = -1;
    QString nomFour;
    if (!resolveFournisseurIdFromInput(m_fournisseurInput, idFour, nomFour)) {
        return false;
    }

    const int seuil = (m_seuilCritique > 0) ? m_seuilCritique : qMax(5, m_quantite / 5);

    QSqlQuery q;
    q.prepare(
        "UPDATE MATIERE_PREMIERE SET "
        "reference=:reference, nom=:nom, quantite=:quantite, seuil_critique=:seuil, "
        "prix=:prix, date_achat=:da, id_four=:id_four "
        "WHERE id_mp=:id_mp");

    q.bindValue(":id_mp", m_idMp);
    q.bindValue(":reference", m_reference.trimmed());
    q.bindValue(":nom", m_nom.trimmed());
    q.bindValue(":quantite", m_quantite);
    q.bindValue(":seuil", seuil);
    q.bindValue(":prix", m_prix);
    q.bindValue(":da", QDate::currentDate());
    q.bindValue(":id_four", idFour);

    const bool ok = q.exec();
    m_lastError = ok ? QString() : q.lastError().text();
    return ok;
}

bool Stock::supprimer(int idMp)
{
    QSqlQuery q;
    q.prepare("DELETE FROM MATIERE_PREMIERE WHERE id_mp = :id");
    q.bindValue(":id", idMp);
    const bool ok = q.exec();
    m_lastError = ok ? QString() : q.lastError().text();
    return ok;
}

QSqlQueryModel *Stock::afficher()
{
    auto *model = new QSqlQueryModel();
    model->setQuery(
        "SELECT "
        "m.id_mp, m.reference, m.nom, m.quantite, m.seuil_critique, m.prix, m.id_four, "
        "NVL(f.nom, '') AS fournisseur_nom "
        "FROM MATIERE_PREMIERE m "
        "LEFT JOIN FOURNISSEUR f ON f.id_four = m.id_four "
        "ORDER BY m.id_mp");

    m_lastError = model->lastError().isValid() ? model->lastError().text() : QString();
    return model;
}

bool Stock::findIdByReference(const QString &reference, int &idMp)
{
    idMp = -1;
    QSqlQuery q;
    q.prepare("SELECT id_mp FROM MATIERE_PREMIERE WHERE reference = :reference");
    q.bindValue(":reference", reference.trimmed());

    if (!q.exec()) {
        m_lastError = q.lastError().text();
        return false;
    }

    if (!q.next()) {
        m_lastError = "Reference introuvable.";
        return false;
    }

    idMp = q.value(0).toInt();
    m_lastError.clear();
    return true;
}

QString Stock::lastError() const { return m_lastError; }

int Stock::idMp() const { return m_idMp; }
QString Stock::reference() const { return m_reference; }
QString Stock::nom() const { return m_nom; }
int Stock::quantite() const { return m_quantite; }
int Stock::seuilCritique() const { return m_seuilCritique; }
double Stock::prix() const { return m_prix; }
QString Stock::fournisseurInput() const { return m_fournisseurInput; }

void Stock::setIdMp(int value) { m_idMp = value; }
void Stock::setReference(const QString &value) { m_reference = value; }
void Stock::setNom(const QString &value) { m_nom = value; }
void Stock::setQuantite(int value) { m_quantite = value; }
void Stock::setSeuilCritique(int value) { m_seuilCritique = value; }
void Stock::setPrix(double value) { m_prix = value; }
void Stock::setFournisseurInput(const QString &value) { m_fournisseurInput = value; }
