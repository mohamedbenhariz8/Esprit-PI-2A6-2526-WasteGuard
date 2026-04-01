#include "stock.h"
#include <QDate>
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

static void logStockHistoryTxt(int idMp, int quantite) {
    QFile file("stock_history.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << idMp << ";" << QDateTime::currentMSecsSinceEpoch() << ";" << quantite << "\n";
        file.close();
    }
}

Stock::Stock()
    : m_idMp(0),
      m_quantite(0),
      m_seuilCritique(5),
      m_prix(0.0),
      m_dateFabrication(QDate::currentDate()),
      m_dateAchat(QDate::currentDate())
{
}

Stock::Stock(int idMp,
             const QString &reference,
             const QString &nom,
             int quantite,
             int seuilCritique,
             double prix,
             const QString &fournisseurInput,
             const QDate &dateFabrication,
             const QDate &dateAchat)
    : m_idMp(idMp),
      m_reference(reference),
      m_nom(nom),
      m_quantite(quantite),
      m_seuilCritique(seuilCritique),
      m_prix(prix),
      m_fournisseurInput(fournisseurInput),
      m_dateFabrication(dateFabrication),
      m_dateAchat(dateAchat)
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

    bool isInt;
    int parsedId = input.toInt(&isInt); // First declaration here

    // Vérification de l'existence de la table FOURNISSEUR
    QSqlQuery probe;
    if (!probe.exec("SELECT 1 FROM FOURNISSEUR WHERE 1=0")) {
        idFour = isInt ? parsedId : 1;
        nomFour = input;
        m_lastError.clear();
        return true;
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

    // FIXED: Removed 'bool' and 'const int' because they were already declared above.
    // We just reuse the values already stored in 'isInt' and 'parsedId'.
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
        nomFour = m_fournisseurInput.trimmed();
    }

    const int seuil = (m_seuilCritique > 0) ? m_seuilCritique : qMax(5, m_quantite / 5);

    QSqlQuery q;
    q.prepare("INSERT INTO MATIERE_PREMIERE (ID_MP, REFERENCE, NOM, QUANTITE, PRIX, SEUIL_CRITIQUE, DATE_FABRICATION, DATE_ACHAT, NUM_FOUR, NOM_FOUR, EMAIL_FOUR) "
        "VALUES (:ID_MP, :REFERENCE, :NOM, :QUANTITE, :PRIX, :SEUIL_CRITIQUE, :DATE_FABRICATION, :DATE_ACHAT, :NUM_FOUR, :NOM_FOUR, :EMAIL_FOUR)");

    q.bindValue(":ID_MP", idMp);
    q.bindValue(":REFERENCE", m_reference.trimmed());
    q.bindValue(":NOM", m_nom.trimmed());
    q.bindValue(":QUANTITE", m_quantite);
    q.bindValue(":PRIX", m_prix);
    q.bindValue(":SEUIL_CRITIQUE", seuil);
    q.bindValue(":DATE_FABRICATION", m_dateFabrication);
    q.bindValue(":DATE_ACHAT", m_dateAchat);
    q.bindValue(":NUM_FOUR", idFour > 0 ? idFour : 1);
    q.bindValue(":NOM_FOUR", nomFour);
    q.bindValue(":EMAIL_FOUR", m_emailFourInput);

    const bool ok = q.exec();
    if (ok) logStockHistoryTxt(idMp, m_quantite);
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
        nomFour = m_fournisseurInput.trimmed();
    }

    const int seuil = (m_seuilCritique > 0) ? m_seuilCritique : qMax(5, m_quantite / 5);

    QSqlQuery q;
    q.prepare("UPDATE MATIERE_PREMIERE SET NOM=:NOM, QUANTITE=:QUANTITE, PRIX=:PRIX, SEUIL_CRITIQUE=:SEUIL_CRITIQUE, DATE_FABRICATION=:DATE_FABRICATION, DATE_ACHAT=:DATE_ACHAT, NUM_FOUR=:NUM_FOUR, NOM_FOUR=:NOM_FOUR, EMAIL_FOUR=:EMAIL_FOUR WHERE REFERENCE=:REFERENCE");

    q.bindValue(":NOM", m_nom.trimmed());
    q.bindValue(":QUANTITE", m_quantite);
    q.bindValue(":PRIX", m_prix);
    q.bindValue(":SEUIL_CRITIQUE", seuil);
    q.bindValue(":DATE_FABRICATION", m_dateFabrication);
    q.bindValue(":DATE_ACHAT", m_dateAchat);
    q.bindValue(":NUM_FOUR", idFour > 0 ? idFour : 1);
    q.bindValue(":NOM_FOUR", nomFour);
    q.bindValue(":EMAIL_FOUR", m_emailFourInput);
    q.bindValue(":REFERENCE", m_reference.trimmed());

    const bool ok = q.exec();
    if (ok) logStockHistoryTxt(m_idMp, m_quantite);
    m_lastError = ok ? QString() : q.lastError().text();
    return ok;
}

bool Stock::supprimer(int idMp)
{
    QSqlQuery q;
    // Supprimer les enregistrements enfants pour eviter l'erreur de contrainte de clef etrangere
    q.prepare("DELETE FROM INTERVENTION_MATIERE WHERE ID_MP = :id");
    q.bindValue(":id", idMp);
    q.exec();

    q.prepare("DELETE FROM MATIERE_PREMIERE WHERE id_mp = :id");
    q.bindValue(":id", idMp);
    const bool ok = q.exec();
    m_lastError = ok ? QString() : q.lastError().text();
    return ok;
}

QSqlQueryModel *Stock::afficher()
{
    auto *model = new QSqlQueryModel();
    model->setQuery("SELECT ID_MP, REFERENCE, NOM, QUANTITE, SEUIL_CRITIQUE, PRIX, DATE_FABRICATION, DATE_ACHAT, NOM_FOUR, EMAIL_FOUR "
                "FROM MATIERE_PREMIERE");

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
QString Stock::emailFourInput() const { return m_emailFourInput; }
QDate Stock::dateFabrication() const { return m_dateFabrication; }
QDate Stock::dateAchat() const { return m_dateAchat; }

void Stock::setIdMp(int value) { m_idMp = value; }
void Stock::setReference(const QString &value) { m_reference = value; }
void Stock::setNom(const QString &value) { m_nom = value; }
void Stock::setQuantite(int value) { m_quantite = value; }
void Stock::setSeuilCritique(int value) { m_seuilCritique = value; }
void Stock::setPrix(double value) { m_prix = value; }
void Stock::setFournisseurInput(const QString &value) { m_fournisseurInput = value; }
void Stock::setEmailFourInput(const QString &value) { m_emailFourInput = value; }
void Stock::setDateFabrication(const QDate &value) { m_dateFabrication = value; }
void Stock::setDateAchat(const QDate &value) { m_dateAchat = value; }
