#include "labibchat.h"

#include <QDate>
#include <QDateTime>
#include <QMetaType>
#include <QRegularExpression>
#include <QSqlQuery>
#include <QSqlError>
#include <QSet>
#include <QStringList>
#include <QTime>
#include <QVariant>

namespace {

QString normalize(QString text)
{
    text = text.normalized(QString::NormalizationForm_D);
    text.remove(QRegularExpression("[\\x{0300}-\\x{036f}]"));
    text = text.toLower();
    text.replace(QRegularExpression("[^a-z0-9 ]"), " ");
    text.replace(QRegularExpression("\\s+"), " ");
    text.replace(QRegularExpression("\\bamployes\\b"), "employes");
    text.replace(QRegularExpression("\\bamploye\\b"), "employe");
    // Graphie anglaise -> francais (collegue international)
    text.replace(QRegularExpression("\\bemployees\\b"), "employes");
    text.replace(QRegularExpression("\\bemployee\\b"), "employe");
    return text.trimmed();
}

bool hasAny(const QString &text, const QStringList &tokens)
{
    for (const QString &t : tokens) {
        if (text.contains(t)) return true;
    }
    return false;
}

int scalarInt(const QString &sql, int fallback = 0)
{
    QSqlQuery q;
    if (q.exec(sql) && q.next()) {
        return q.value(0).toInt();
    }
    return fallback;
}

double scalarDouble(const QString &sql, double fallback = 0.0)
{
    QSqlQuery q;
    if (q.exec(sql) && q.next()) {
        return q.value(0).toDouble();
    }
    return fallback;
}

QStringList scalarList(const QString &sql, int maxRows = 8)
{
    QStringList rows;
    QSqlQuery q;
    if (q.exec(sql)) {
        while (q.next() && rows.size() < maxRows) {
            rows << q.value(0).toString().trimmed();
        }
    }
    return rows;
}

QString greetingByHour()
{
    const int h = QTime::currentTime().hour();
    if (h < 6)  return "Bonsoir";
    if (h < 12) return "Bonjour";
    if (h < 18) return "Bon apr\xC3\xA8s-midi";
    return "Bonsoir";
}

QString badge(const QString &text, const QString &bg, const QString &fg = "#ffffff")
{
    return QString("<span style='background:%1;color:%2;padding:1px 6px;"
                   "border-radius:4px;font-weight:700;font-size:11px;'>%3</span>")
        .arg(bg, fg, text);
}

QString line(const QString &emoji, const QString &content)
{
    return QString("%1 %2<br>").arg(emoji, content);
}

// ─────────────────────────────────────────────────────────────────────
// Indicateurs métier (live DB)
// ─────────────────────────────────────────────────────────────────────

struct LiveStats {
    int clients = 0;
    int employes = 0;
    int empDispo = 0;
    int empMission = 0;
    int empConge = 0;
    int produits = 0;
    int bacs = 0;
    int bacsPleins = 0;            // remplissage >= 80
    int commandes = 0;
    int commandesEnPrep = 0;
    int commandesLivrees = 0;
    int interventions = 0;
    int interventionsEnCours = 0;
    int stockCritique = 0;
    double caTotal = 0.0;
    double salaireMasse = 0.0;
};

LiveStats fetchStats()
{
    LiveStats s;
    s.clients              = scalarInt("SELECT COUNT(*) FROM CLIENT");
    s.employes             = scalarInt("SELECT COUNT(*) FROM EMPLOYE");
    s.empDispo             = scalarInt("SELECT COUNT(*) FROM EMPLOYE WHERE UPPER(NVL(DISPONIBILITE,'X'))='DISPONIBLE'");
    s.empMission           = scalarInt("SELECT COUNT(*) FROM EMPLOYE WHERE UPPER(NVL(DISPONIBILITE,'X')) IN ('EN_INTERVENTION','INDISPONIBLE')");
    s.empConge             = scalarInt("SELECT COUNT(*) FROM EMPLOYE WHERE UPPER(NVL(DISPONIBILITE,'X')) LIKE 'CONGE%' OR UPPER(NVL(DISPONIBILITE,'X')) LIKE 'EN_CONGE%'");
    s.produits             = scalarInt("SELECT COUNT(*) FROM MATIERE_PREMIERE");
    s.bacs                 = scalarInt("SELECT COUNT(*) FROM BAC_INTEL");
    s.bacsPleins           = scalarInt("SELECT COUNT(*) FROM BAC_INTEL WHERE NVL(REMPLISSAGE,0) >= 80");
    s.commandes            = scalarInt("SELECT COUNT(*) FROM COMMANDE");
    s.commandesEnPrep      = scalarInt("SELECT COUNT(*) FROM COMMANDE WHERE UPPER(NVL(STATUT,'X'))='EN_PREPARATION'");
    s.commandesLivrees     = scalarInt("SELECT COUNT(*) FROM COMMANDE WHERE UPPER(NVL(STATUT,'X'))='LIVREE'");
    s.interventions        = scalarInt("SELECT COUNT(*) FROM INTERVENTION");
    s.interventionsEnCours = scalarInt("SELECT COUNT(*) FROM INTERVENTION WHERE UPPER(NVL(STATUT,'X')) IN ('EN_COURS','EN_ATTENTE')");
    s.stockCritique        = scalarInt("SELECT COUNT(*) FROM MATIERE_PREMIERE WHERE QUANTITE <= SEUIL_CRITIQUE");
    s.caTotal              = scalarDouble("SELECT NVL(SUM(PRIX_TOTAL),0) FROM COMMANDE WHERE UPPER(NVL(STATUT,'X')) IN ('LIVREE','INSTALLEE')");
    s.salaireMasse         = scalarDouble("SELECT NVL(SUM(SALAIRE),0) FROM EMPLOYE");
    return s;
}

// ─────────────────────────────────────────────────────────────────────
// Réponses spécialisées
// ─────────────────────────────────────────────────────────────────────

QString answerDailyReport(const QString &operatorName)
{
    const LiveStats s = fetchStats();
    const QString who = operatorName.isEmpty() ? "ch\xC3\xA9" "f" : operatorName;

    QString html;
    html += QString("<b>Rapport \xC3\xA9" "clair, %1 \xF0\x9F\x9A\x80</b><br>").arg(who);
    html += QString("Aujourd'hui chez WasteGuard, voici l'\xC3\xA9" "tat des troupes :<br><br>");
    html += line("\xF0\x9F\x91\xA5", QString("<b>%1</b> employ\xC3\xA9" "s &mdash; %2 dispo, %3 en mission, %4 en cong\xC3\xA9")
                                      .arg(s.employes).arg(s.empDispo).arg(s.empMission).arg(s.empConge));
    html += line("\xF0\x9F\x91\xA4", QString("<b>%1</b> clients enregistr\xC3\xA9" "s").arg(s.clients));
    html += line("\xF0\x9F\x97\x91\xEF\xB8\x8F", QString("<b>%1</b> bacs intelligents (%2 pleins \xE2\x89\xA5 80%%)")
                                      .arg(s.bacs).arg(s.bacsPleins));
    html += line("\xF0\x9F\x93\xA6", QString("<b>%1</b> commandes (%2 en pr\xC3\xA9" "paration, %3 livr\xC3\xA9" "es)")
                                      .arg(s.commandes).arg(s.commandesEnPrep).arg(s.commandesLivrees));
    html += line("\xF0\x9F\x94\xA7", QString("<b>%1</b> interventions actives sur le terrain")
                                      .arg(s.interventionsEnCours));
    if (s.stockCritique > 0) {
        html += line("\xE2\x9A\xA0\xEF\xB8\x8F", QString("%1 &mdash; <b>%2</b> mati\xC3\xA8re(s) en seuil critique, on doit r\xC3\xA9" "agir vite !")
                                      .arg(badge("STOCK", "#dc2626")).arg(s.stockCritique));
    } else {
        html += line("\xE2\x9C\x85", "Niveaux de stock OK, rien de critique.");
    }
    html += QString("<br>\xF0\x9F\x92\xB0 Chiffre d'affaires livr\xC3\xA9 : <b>%1 DT</b>").arg(s.caTotal, 0, 'f', 2);
    html += "<br><i>Si tu veux un focus sur un module, dis-moi : <b>clients</b>, <b>stock</b>, <b>employ\xC3\xA9" "s</b>, <b>commandes</b> ou <b>interventions</b>.</i>";
    return html;
}

QString answerStockAlerts()
{
    QSqlQuery q;
    QStringList critiques;
    if (q.exec("SELECT NOM, QUANTITE, SEUIL_CRITIQUE, REFERENCE "
               "FROM MATIERE_PREMIERE WHERE QUANTITE <= SEUIL_CRITIQUE "
               "ORDER BY (SEUIL_CRITIQUE - QUANTITE) DESC")) {
        while (q.next() && critiques.size() < 10) {
            critiques << QString("<b>%1</b> (R\xC3\xA9" "f %2) : %3 / seuil %4")
                .arg(q.value(0).toString().trimmed(),
                     q.value(3).toString().trimmed())
                .arg(q.value(1).toInt())
                .arg(q.value(2).toInt());
        }
    }

    if (critiques.isEmpty()) {
        return "\xE2\x9C\x85 <b>Aucun stock critique</b>, ch\xC3\xA9" "f. "
               "Tous nos approvisionnements sont au-dessus des seuils. "
               "On peut se concentrer sur la production sans stress \xF0\x9F\x92\xAA.";
    }

    QString html = QString("\xE2\x9A\xA0\xEF\xB8\x8F %1 <b>Mati\xC3\xA8res en alerte</b> :<br><br>")
        .arg(badge("URGENT", "#dc2626"));
    html += "\xE2\x80\xA2 " + critiques.join("<br>\xE2\x80\xA2 ");
    html += "<br><br><i>Je propose qu'on lance un r\xC3\xA9" "approvisionnement avant la fin de la semaine. Tu veux que je te g\xC3\xA9n\xC3\xA8re la liste fournisseur ?</i>";
    return html;
}

QString answerClients()
{
    const int total = scalarInt("SELECT COUNT(*) FROM CLIENT");
    const int retard = scalarInt("SELECT COUNT(*) FROM CLIENT WHERE STATUT_PAIEMENT='EN_RETARD'");
    const int suspendu = scalarInt("SELECT COUNT(*) FROM CLIENT WHERE STATUT_PAIEMENT='SUSPENDU'");
    const int ajour = scalarInt("SELECT COUNT(*) FROM CLIENT WHERE STATUT_PAIEMENT='A_JOUR'");
    const int expSoon = scalarInt("SELECT COUNT(*) FROM CLIENT WHERE DATE_EXPIRATION_CONTRAT IS NOT NULL "
                                  "AND DATE_EXPIRATION_CONTRAT BETWEEN SYSDATE AND SYSDATE+30");

    const QStringList topRetard = scalarList(
        "SELECT NOM FROM CLIENT WHERE STATUT_PAIEMENT='EN_RETARD' "
        "AND ROWNUM <= 5");

    QString html = QString("\xF0\x9F\x91\xA4 <b>Portefeuille clients</b> &mdash; %1 actifs<br><br>").arg(total);
    html += line("\xE2\x9C\x85", QString("<b>%1</b> \xC3\xA0 jour").arg(ajour));
    html += line("\xE2\x8F\xB0", QString("<b>%1</b> en retard de paiement").arg(retard));
    html += line("\xE2\x9B\x94", QString("<b>%1</b> suspendus").arg(suspendu));
    html += line("\xF0\x9F\x93\x85", QString("<b>%1</b> contrats expirent dans 30 jours").arg(expSoon));

    if (!topRetard.isEmpty()) {
        html += "<br><b>\xC3\x80 relancer en priorit\xC3\xA9 :</b><br>\xE2\x80\xA2 ";
        html += topRetard.join("<br>\xE2\x80\xA2 ");
    }

    if (retard > 0 || expSoon > 0) {
        html += "<br><br><i>Je peux pr\xC3\xA9parer les emails de relance si tu valides, c'est cash flow direct \xF0\x9F\x92\xB0.</i>";
    }
    return html;
}

QString answerEmployes()
{
    const LiveStats s = fetchStats();
    const QStringList topSpec = scalarList(
        "SELECT SPECIALITE || ' (' || COUNT(*) || ')' "
        "FROM EMPLOYE WHERE SPECIALITE IS NOT NULL "
        "GROUP BY SPECIALITE ORDER BY COUNT(*) DESC", 5);

    QString html = QString("\xF0\x9F\x91\xA5 <b>\xC3\x89quipe WasteGuard</b> &mdash; %1 collaborateurs<br><br>").arg(s.employes);
    html += line("\xF0\x9F\x9F\xA2", QString("<b>%1</b> disponibles").arg(s.empDispo));
    html += line("\xF0\x9F\x9F\xA1", QString("<b>%1</b> en mission/intervention").arg(s.empMission));
    html += line("\xF0\x9F\x8F\x96\xEF\xB8\x8F", QString("<b>%1</b> en cong\xC3\xA9").arg(s.empConge));
    html += line("\xF0\x9F\x92\xB6", QString("Masse salariale : <b>%1 DT</b>").arg(s.salaireMasse, 0, 'f', 2));

    if (!topSpec.isEmpty()) {
        html += "<br><b>R\xC3\xA9partition des sp\xC3\xA9" "cialit\xC3\xA9" "s :</b><br>\xE2\x80\xA2 ";
        html += topSpec.join("<br>\xE2\x80\xA2 ");
    }
    if (s.empDispo == 0 && s.employes > 0) {
        html += "<br><br>\xE2\x9A\xA0\xEF\xB8\x8F <i>Aucun employ\xC3\xA9 disponible imm\xC3\xA9" "diatement &mdash; \xC3\xA0 surveiller si une nouvelle intervention tombe.</i>";
    }
    return html;
}

QString answerCommandes()
{
    const LiveStats s = fetchStats();
    const int urgentes = scalarInt("SELECT COUNT(*) FROM COMMANDE WHERE PRIORITE IN ('HAUTE','URGENTE') AND UPPER(NVL(STATUT,'X'))<>'LIVREE'");
    const int annulees = scalarInt("SELECT COUNT(*) FROM COMMANDE WHERE UPPER(NVL(STATUT,'X'))='ANNULEE'");

    QString html = QString("\xF0\x9F\x93\xA6 <b>Commandes</b> &mdash; %1 au total<br><br>").arg(s.commandes);
    html += line("\xF0\x9F\x9A\x9A", QString("<b>%1</b> livr\xC3\xA9" "es").arg(s.commandesLivrees));
    html += line("\xF0\x9F\x93\x8B", QString("<b>%1</b> en pr\xC3\xA9paration").arg(s.commandesEnPrep));
    html += line("\xF0\x9F\x9A\xA8", QString("<b>%1</b> prioritaires/urgentes en cours").arg(urgentes));
    html += line("\xE2\x9D\x8C", QString("<b>%1</b> annul\xC3\xA9" "es").arg(annulees));
    html += QString("<br>\xF0\x9F\x92\xB0 CA livr\xC3\xA9 : <b>%1 DT</b>").arg(s.caTotal, 0, 'f', 2);

    if (urgentes > 0) {
        html += "<br><br><i>On a des urgences sur le feu, je suivrais ces commandes en priorit\xC3\xA9 ce matin \xF0\x9F\x94\xA5.</i>";
    }
    return html;
}

QString answerInterventions()
{
    const LiveStats s = fetchStats();
    const int urgentes = scalarInt("SELECT COUNT(*) FROM INTERVENTION WHERE PRIORITE IN ('HAUTE','URGENTE') AND UPPER(NVL(STATUT,'X'))='EN_COURS'");
    const double cout = scalarDouble("SELECT NVL(SUM(COUT),0) FROM INTERVENTION");

    QString html = QString("\xF0\x9F\x94\xA7 <b>Maintenance &amp; interventions</b><br><br>");
    html += line("\xF0\x9F\x93\x8C", QString("<b>%1</b> interventions enregistr\xC3\xA9" "es").arg(s.interventions));
    html += line("\xE2\x9A\x99\xEF\xB8\x8F", QString("<b>%1</b> actives (en cours / en attente)").arg(s.interventionsEnCours));
    html += line("\xF0\x9F\x9A\xA8", QString("<b>%1</b> urgentes en cours").arg(urgentes));
    html += line("\xF0\x9F\x92\xB0", QString("Co\xC3\xBBt cumul\xC3\xA9 : <b>%1 DT</b>").arg(cout, 0, 'f', 2));

    if (urgentes > 0) {
        html += "<br><br>\xF0\x9F\x94\xA5 <i>Faut envoyer une \xC3\xA9quipe sur les urgences avant qu'on perde un client.</i>";
    } else {
        html += "<br><br>\xE2\x9C\x85 <i>Pas d'urgence en intervention, on tient bien le terrain.</i>";
    }
    return html;
}

QString answerBacs()
{
    const LiveStats s = fetchStats();
    const int presquePlein = scalarInt("SELECT COUNT(*) FROM BAC_INTEL WHERE NVL(REMPLISSAGE,0) BETWEEN 60 AND 79");
    const int satures      = scalarInt("SELECT COUNT(*) FROM BAC_INTEL WHERE NVL(REMPLISSAGE,0) >= 90");

    QString html = QString("\xF0\x9F\x97\x91\xEF\xB8\x8F <b>Parc de bacs intelligents</b> &mdash; %1 d\xC3\xA9ploy\xC3\xA9" "s<br><br>").arg(s.bacs);
    html += line("\xF0\x9F\x9F\xA2", QString("<b>%1</b> sous 60%% (sains)").arg(s.bacs - s.bacsPleins - presquePlein));
    html += line("\xF0\x9F\x9F\xA1", QString("<b>%1</b> entre 60-79%%").arg(presquePlein));
    html += line("\xF0\x9F\x9F\xA0", QString("<b>%1</b> entre 80-89%% (\xC3\xA0 collecter)").arg(s.bacsPleins - satures));
    html += line("\xF0\x9F\x94\xB4", QString("<b>%1</b> satur\xC3\xA9" "s \xE2\x89\xA5 90%%").arg(satures));

    if (satures > 0) {
        html += "<br><i>On doit organiser une tourn\xC3\xA9" "e de collecte aujourd'hui sur les bacs satur\xC3\xA9" "s.</i>";
    }
    return html;
}

QString answerWhoAmI(const QString &operatorName, bool isAdmin)
{
    Q_UNUSED(isAdmin);
    const QString who = operatorName.isEmpty() ? "coll\xC3\xA8gue" : operatorName;
    return QString(
        "\xF0\x9F\xA6\x8A <b>Salut %1, c'est Labib !</b><br><br>"
        "Je suis ton bras droit num\xC3\xA9rique chez <b>WasteGuard</b>. "
        "J'aime cette boite, elle r\xC3\xA9volutionne la collecte intelligente des d\xC3\xA9" "chets, "
        "et je suis l\xC3\xA0 24/7 pour qu'on tienne la cadence ensemble.<br><br>"
        "<b>Ce que je sais faire :</b><br>"
        "\xE2\x80\xA2 Lire la base en temps r\xC3\xA9" "el (clients, employ\xC3\xA9" "s, stock, bacs, commandes, maintenance)<br>"
        "\xE2\x80\xA2 Te sortir un rapport \xC3\xA9" "clair sur n'importe quel module<br>"
        "\xE2\x80\xA2 D\xC3\xA9tecter les alertes critiques et te proposer des actions<br>"
        "\xE2\x80\xA2 R\xC3\xA9pondre \xC3\xA0 tes questions m\xC3\xA9tier en fran\xC3\xA7" "ais<br><br>"
        "Demande-moi par exemple : <i>\xC2\xAB rapport du jour \xC2\xBB</i>, "
        "<i>\xC2\xAB alertes stock \xC2\xBB</i>, <i>\xC2\xAB \xC3\xA9quipe \xC2\xBB</i> ou "
        "<i>\xC2\xAB combien de clients \xC2\xBB</i>. On y va ? \xF0\x9F\x9A\x80"
    ).arg(who);
}

QString answerThanks()
{
    static const QStringList replies = {
        "Avec plaisir, ch\xC3\xA9" "f \xF0\x9F\x92\xAA. On forme une bonne \xC3\xA9quipe.",
        "Toujours l\xC3\xA0 pour WasteGuard \xF0\x9F\xA6\x8A.",
        "Pas de souci, on continue \xC3\xA0 faire avancer la boite \xF0\x9F\x9A\x80.",
        "C'est moi qui te remercie, ensemble on est plus efficaces."
    };
    return replies.at(QDateTime::currentSecsSinceEpoch() % replies.size());
}

QString answerGreeting(const QString &operatorName)
{
    const QString who = operatorName.isEmpty() ? "coll\xC3\xA8gue" : operatorName;
    return QString("%1 %2 \xF0\x9F\xA6\x8A ! Pr\xC3\xAAt \xC3\xA0 attaquer la journ\xC3\xA9" "e ? "
                   "Demande-moi un <i>rapport du jour</i> ou tape <i>aide</i> pour voir ce que je peux faire.")
        .arg(greetingByHour(), who);
}

QString answerHelp()
{
    return
        "\xF0\x9F\xA6\x8A <b>Voici tout ce que tu peux me demander, coll\xC3\xA8gue :</b><br><br>"
        "\xE2\x80\xA2 <b>rapport du jour</b> &mdash; vue d'ensemble live<br>"
        "\xE2\x80\xA2 <b>alertes stock</b> &mdash; mati\xC3\xA8res en seuil critique<br>"
        "\xE2\x80\xA2 <b>clients</b> &mdash; portefeuille, retards, expirations<br>"
        "\xE2\x80\xA2 <b>employ\xC3\xA9" "s / \xC3\xA9quipe</b> &mdash; effectif, dispo, sp\xC3\xA9" "cialit\xC3\xA9" "s<br>"
        "\xE2\x80\xA2 <b>commandes</b> &mdash; statut, urgences, CA<br>"
        "\xE2\x80\xA2 <b>interventions / maintenance</b> &mdash; terrain et co\xC3\xBBts<br>"
        "\xE2\x80\xA2 <b>bacs</b> &mdash; remplissage du parc IoT<br>"
        "\xE2\x80\xA2 <b>combien de [clients/employ\xC3\xA9" "s/bacs/...]</b><br>"
        "\xE2\x80\xA2 <b>qui es-tu</b> &mdash; pour faire connaissance<br><br>"
        "Bref, parle-moi comme \xC3\xA0 un coll\xC3\xA8gue. Je suis branch\xC3\xA9 sur la base, "
        "donc mes chiffres sont toujours frais \xF0\x9F\x94\xA5.";
}

QString answerCount(const QString &n)
{
    if (hasAny(n, {"client"}))                    return QString("On a actuellement <b>%1</b> clients dans la base \xF0\x9F\x91\xA4.").arg(scalarInt("SELECT COUNT(*) FROM CLIENT"));
    if (hasAny(n, {"employe", "salarie", "equipe"})) return QString("Notre \xC3\xA9quipe compte <b>%1</b> collaborateurs \xF0\x9F\x91\xA5.").arg(scalarInt("SELECT COUNT(*) FROM EMPLOYE"));
    if (hasAny(n, {"bac", "iot", "poubelle"}))    return QString("On a <b>%1</b> bacs intelligents d\xC3\xA9ploy\xC3\xA9" "s \xF0\x9F\x97\x91\xEF\xB8\x8F.").arg(scalarInt("SELECT COUNT(*) FROM BAC_INTEL"));
    if (hasAny(n, {"commande", "order"}))          return QString("On a <b>%1</b> commandes au total \xF0\x9F\x93\xA6.").arg(scalarInt("SELECT COUNT(*) FROM COMMANDE"));
    if (hasAny(n, {"intervention", "maintenance"})) return QString("<b>%1</b> interventions enregistr\xC3\xA9" "es \xF0\x9F\x94\xA7.").arg(scalarInt("SELECT COUNT(*) FROM INTERVENTION"));
    if (hasAny(n, {"matiere", "produit", "stock"})) return QStringLiteral("Le magasin contient <b>%1</b> références matière première 📦.").arg(scalarInt("SELECT COUNT(*) FROM MATIERE_PREMIERE"));
    return QString();
}

// ─────────────────────────────────────────────────────────────────────
// Moteur générique : détecte entité + filtre + agrégation + projection
// et construit une requête SQL Oracle en lecture seule.
// ─────────────────────────────────────────────────────────────────────

struct EntityDef {
    QString key;
    QString table;
    QString labelCol;          // colonne "nom"
    QStringList searchCols;    // colonnes à LIKE-chercher pour le free-text
    QStringList numericCols;   // colonnes numériques connues (pour AVG/SUM/MAX/MIN)
    QString plural;            // affichage
    QString emoji;
};

QList<EntityDef> entityCatalog()
{
    return {
        {"employe",      "EMPLOYE",          "NOM",
            {"NOM","EMAIL","CIN","SPECIALITE","MATRICULE"},
            {"SALAIRE","TOTAL_REWARD","TOTAL_EXPERIENCE","TOTAL_LEAVE_DAYS"},
            "employes", "\xF0\x9F\x91\xA5"},
        {"client",       "CLIENT",           "NOM",
            {"NOM","EMAIL","MATRICULE","TYPE_CONTRAT"},
            {"TAUX_TRI"},
            "clients", "\xF0\x9F\x91\xA4"},
        {"matiere",      "MATIERE_PREMIERE", "NOM",
            {"NOM","REFERENCE","NOM_FOUR","EMAIL_FOUR"},
            {"QUANTITE","SEUIL_CRITIQUE","PRIX"},
            "mati\xC3\xA8res premi\xC3\xA8res", "\xF0\x9F\x93\xA6"},
        {"produit",      "PRODUITS",         "MODEL",
            {"MODEL","TYPE","ALLEE"},
            {"QUANTITE","PRIX_UNITAIRE","CAPACITE"},
            "produits", "\xF0\x9F\x93\x8B"},
        {"bac",          "BAC_INTEL",        "MODELE",
            {"NUM_SERIE","MODELE","LOCALISATION_STOCK","CARACTERISTIQUES"},
            {"REMPLISSAGE","PRIX","STOCK","CAPACITE_BATTERIE"},
            "bacs intelligents", "\xF0\x9F\x97\x91\xEF\xB8\x8F"},
        {"commande",     "COMMANDE",         "ID_COMMANDE",
            {"ADRESSE","STATUT","PRIORITE"},
            {"QTE","PRIX_TOTAL"},
            "commandes", "\xF0\x9F\x93\xA6"},
        {"intervention", "INTERVENTION",     "REFERENCE",
            {"REFERENCE","TYPE","STATUT","PRIORITE"},
            {"DUREE","COUT","MAIN_OEUVRE"},
            "interventions", "\xF0\x9F\x94\xA7"}
    };
}

EntityDef entityForKey(const QString &k)
{
    for (const auto &e : entityCatalog()) if (e.key == k) return e;
    return {};
}

QString detectEntityKey(const QString &n)
{
    if (hasAny(n, {"intervention", "interventions", "panne", "reparation"})) return "intervention";
    if (hasAny(n, {"commande", "commandes", "vente", "ventes", "facture"})) return "commande";
    if (hasAny(n, {"bac", "bacs", "iot", "capteur", "poubelle", "remplissage"})) return "bac";
    if (hasAny(n, {"client", "clients", "abonne", "abonnes", "contrat"})) return "client";
    // Produit fini (catalogue PRODUITS) — ex: "le composant le plus en stock"
    if (hasAny(n, {"produit", "produits", "composant", "composants", "piece", "pieces",
                   "article", "articles", "model", "modele", "rayon", "allee"})) return "produit";
    if (hasAny(n, {"matiere", "matieres", "magasin", "inventaire", "ressource", "fournisseur"})) return "matiere";
    // mots-clés liés au métier "employé" : titres, rôles, RH
    if (hasAny(n, {"employe","employes","salarie","salaries","personnel","equipe","rh",
                   "technicien","techniciens","admin","admins","responsable","responsables",
                   "operateur","operateurs","superviseur","manager","chef","chauffeur"})) return "employe";
    if (hasAny(n, {"stock"})) return "produit";

    // Heuristique : un nom de colonne implique son entité.
    if (hasAny(n, {"salaire","salaires","cin","specialite","disponibilite","matricule"})) return "employe";
    if (hasAny(n, {"paiement","tri","expiration"})) return "client";
    if (hasAny(n, {"seuil","critique"})) return "matiere";
    if (hasAny(n, {"quantite","prix unitaire","capacite produit","allee","rayon"})) return "produit";
    if (hasAny(n, {"localisation","capacite batterie","autonomie"})) return "bac";
    if (hasAny(n, {"livraison","priorite"})) return "commande";

    return QString();
}

struct FilterClause {
    QString sql;
    QString display;
};

QList<FilterClause> detectFilters(const QString &n, const QString &entityKey)
{
    QList<FilterClause> out;
    if (entityKey == "employe") {
        if (hasAny(n, {"admin"}))            out << FilterClause{"UPPER(NVL(SPECIALITE,'')) LIKE '%ADMIN%'", "sp\xC3\xA9" "cialit\xC3\xA9 Admin"};
        if (hasAny(n, {"responsable"}))      out << FilterClause{"UPPER(NVL(SPECIALITE,'')) LIKE '%RESPONSABLE%'", "Responsable"};
        if (hasAny(n, {"technicien"}))       out << FilterClause{"UPPER(NVL(SPECIALITE,'')) LIKE '%TECHNICIEN%'", "Technicien"};
        if (hasAny(n, {"superviseur"}))      out << FilterClause{"UPPER(NVL(SPECIALITE,'')) LIKE '%SUPERVISEUR%'", "Superviseur"};
        if (hasAny(n, {"chauffeur"}))        out << FilterClause{"UPPER(NVL(SPECIALITE,'')) LIKE '%CHAUFFEUR%'", "Chauffeur"};
        if (hasAny(n, {"operateur"}))        out << FilterClause{"UPPER(NVL(SPECIALITE,'')) LIKE '%OPERATEUR%'", "Op\xC3\xA9rateur"};
        if (hasAny(n, {"disponible","dispo"})) out << FilterClause{"UPPER(NVL(DISPONIBILITE,''))='DISPONIBLE'", "disponibles"};
        if (hasAny(n, {"en mission","en intervention","en_intervention","mission"}))
                                              out << FilterClause{"UPPER(NVL(DISPONIBILITE,'')) IN ('EN_INTERVENTION','INDISPONIBLE')", "en mission"};
        if (hasAny(n, {"conge","conges"}))   out << FilterClause{"UPPER(NVL(DISPONIBILITE,'')) LIKE 'CONGE%' OR UPPER(NVL(DISPONIBILITE,'')) LIKE 'EN_CONGE%'", "en cong\xC3\xA9"};
        if (hasAny(n, {"absent","absente","absents","absentes","indisponible"}))
                                              out << FilterClause{"UPPER(NVL(DISPONIBILITE,'')) NOT IN ('DISPONIBLE') OR DISPONIBILITE IS NULL", "non disponibles"};
    } else if (entityKey == "client") {
        if (hasAny(n, {"en retard","retard","impayes","impaye"})) out << FilterClause{"STATUT_PAIEMENT='EN_RETARD'", "en retard de paiement"};
        if (hasAny(n, {"suspendu"}))         out << FilterClause{"STATUT_PAIEMENT='SUSPENDU'", "suspendus"};
        if (hasAny(n, {"a jour"}))           out << FilterClause{"STATUT_PAIEMENT='A_JOUR'", "\xC3\xA0 jour"};
        if (hasAny(n, {"expire","expiration","bientot"})) out << FilterClause{"DATE_EXPIRATION_CONTRAT IS NOT NULL AND DATE_EXPIRATION_CONTRAT BETWEEN SYSDATE AND SYSDATE+30", "expirent dans 30 jours"};
        if (hasAny(n, {"expires"}))          out << FilterClause{"DATE_EXPIRATION_CONTRAT < SYSDATE", "contrats d\xC3\xA9j\xC3\xA0 expir\xC3\xA9" "s"};
    } else if (entityKey == "commande") {
        if (hasAny(n, {"urgente","urgent","urgentes","haute"})) out << FilterClause{"PRIORITE IN ('HAUTE','URGENTE')", "prioritaires"};
        if (hasAny(n, {"livree","livrees","livre"})) out << FilterClause{"UPPER(STATUT)='LIVREE'", "livr\xC3\xA9" "es"};
        if (hasAny(n, {"installee","installees"})) out << FilterClause{"UPPER(STATUT)='INSTALLEE'", "install\xC3\xA9" "es"};
        if (hasAny(n, {"preparation","en preparation","en cours"})) out << FilterClause{"UPPER(STATUT)='EN_PREPARATION'", "en pr\xC3\xA9paration"};
        if (hasAny(n, {"annulee","annulees","annule"})) out << FilterClause{"UPPER(STATUT)='ANNULEE'", "annul\xC3\xA9" "es"};
    } else if (entityKey == "intervention") {
        if (hasAny(n, {"en cours"}))         out << FilterClause{"UPPER(STATUT)='EN_COURS'", "en cours"};
        if (hasAny(n, {"terminee","terminees","fini","finies"})) out << FilterClause{"UPPER(STATUT)='TERMINEE'", "termin\xC3\xA9" "es"};
        if (hasAny(n, {"en attente","attente"})) out << FilterClause{"UPPER(STATUT)='EN_ATTENTE'", "en attente"};
        if (hasAny(n, {"annulee","annulees"})) out << FilterClause{"UPPER(STATUT)='ANNULEE'", "annul\xC3\xA9" "es"};
        if (hasAny(n, {"urgente","urgent","urgentes","haute"})) out << FilterClause{"PRIORITE IN ('HAUTE','URGENTE')", "urgentes"};
    } else if (entityKey == "matiere") {
        if (hasAny(n, {"critique","seuil","rupture","manque","manquant"})) out << FilterClause{"QUANTITE <= SEUIL_CRITIQUE", "en seuil critique"};
        if (hasAny(n, {"epuise","zero","vide"})) out << FilterClause{"NVL(QUANTITE,0)=0", "\xC3\xA9puis\xC3\xA9" "es"};
    } else if (entityKey == "produit") {
        if (hasAny(n, {"epuise","rupture","zero","vide"})) out << FilterClause{"NVL(QUANTITE,0)=0", "\xC3\xA9puis\xC3\xA9s"};
        if (hasAny(n, {"faible","critique","manque","manquant"})) out << FilterClause{"NVL(QUANTITE,0) <= 5", "stock faible"};
    } else if (entityKey == "bac") {
        if (hasAny(n, {"plein","sature","rempli","saturee"})) out << FilterClause{"NVL(REMPLISSAGE,0) >= 80", "presque pleins"};
        if (hasAny(n, {"vide","vides"})) out << FilterClause{"NVL(REMPLISSAGE,0) <= 20", "vides ou quasi vides"};
    }
    return out;
}

// Stopwords partagés pour le filtrage du free-text. Ils incluent les noms
// d'entités, les rôles, les colonnes et les verbes/prépositions usuels.
QStringList freeTextStops()
{
    return {
        // articles, prépositions, pronoms
        "le","la","les","un","une","des","du","de","au","aux","ce","cet","cette",
        "et","ou","en","dans","sur","par","pour","avec","mon","ma","mes","ton","ta","tes",
        "qui","est","sont","etre","ete","etais","que","tu","toi","votre","vos","ok","svp","stp",
        // noms d'entités
        "client","clients","employe","employes","salarie","salaries","produit","produits",
        "commande","commandes","intervention","interventions","bac","bacs",
        "matiere","matieres","stock","stocks","composant","composants","piece","pieces",
        "article","articles","entite","entites",
        // rôles / statuts / filtres déjà gérés ailleurs
        "admin","admins","responsable","technicien","superviseur","chauffeur","operateur",
        "disponible","dispo","mission","conge","conges","absent","absente","absents","absentes",
        "indisponible","retard","suspendu","urgente","urgent","haute",
        "livree","livrees","installee","preparation","annulee","critique","plein","vide",
        "rempli","sature","epuise","faible","manque","manquant","rupture",
        // colonnes / agrégations / actions
        "salaire","salaires","prix","quantite","email","mail","matricule","cin","statut",
        "contrat","telephone","date","duree","cout","priorite","capacite","remplissage",
        "specialite","poste","role","metier","fournisseur","reference","ref","localisation",
        "emplacement","montant","total","ca","chiffre","seuil","fin",
        "combien","nombre","count","moyen","moyenne","max","min","plus","top","somme",
        "lister","liste","afficher","montrer","trouver","cherche","cherches","cherchez",
        "donner","donne","moi","veux","peux","peut","sait","savoir",
        "nommer","nomme","appelle","appele","appelez",
        // verbes d'invocation conversationnels (a ne pas filtrer comme nom propre)
        "parler","parle","parles","parlez","parlons","parlais",
        "dire","dis","disent","dites","raconte","racontes","racontez","raconter",
        "explique","expliques","expliquez","expliquer","expliquemoi",
        "rapport","rapporte","rapportez","informe","informes","informer",
        "questionne","question","questions",
        // adjectifs comparatifs qui ne doivent pas servir de filtre LIKE
        "cher","chere","chers","cheres","eleve","elevee","haut","haute","bas","basse",
        "grand","grande","petit","petite","moins"
    };
}

bool freeTextLooksLikeComparatorNoise(const QString &freeText)
{
    if (freeText.trimmed().isEmpty()) return false;
    static const QSet<QString> noise = {
        "cher","chere","chers","cheres","plus","moins","maximum","minimum",
        "max","min","eleve","elevee","haut","haute","bas","basse",
        "grand","grande","petit","petite","prix","cout",
        "moindre","faible","gros","important","top"
    };

    const QStringList words = freeText.split(' ', Qt::SkipEmptyParts);
    if (words.isEmpty()) return false;
    for (const QString &w : words) {
        if (!noise.contains(w)) return false;
    }
    return true;
}

QString extractFreeText(const QString &n)
{
    static const QStringList stops = freeTextStops();

    // 1) Tentative dirigée : "... <preposition> <texte>"
    static const QRegularExpression re(
        "(?:\\bde\\b|\\bdu\\b|\\bdes\\b|\\bpour\\b|\\bavec\\b|\\bnomme\\b|\\bappelle\\b|"
        "\\bappele\\w*\\b|\\bnomm\\w*\\b)\\s+([a-z0-9 @._-]{2,60})");
    auto m = re.match(n);
    if (m.hasMatch()) {
        const QString t = m.captured(1).trimmed();
        QStringList kept;
        for (const QString &w : t.split(' ', Qt::SkipEmptyParts)) {
            if (!stops.contains(w) && w.size() >= 2) kept << w;
        }
        const QString joined = kept.join(' ').trimmed();
        if (!joined.isEmpty()) return joined;
    }

    // 2) Repli large : tout token de 3+ char qui n'est pas un stopword.
    QStringList kept;
    for (const QString &w : n.split(' ', Qt::SkipEmptyParts)) {
        if (!stops.contains(w) && w.size() >= 3) kept << w;
    }
    return kept.join(' ').trimmed();
}

QString detectAggregation(const QString &n)
{
    if (hasAny(n, {"combien","nombre","count","total nombre"})) return "COUNT";
    if (hasAny(n, {"moyen","moyenne","average","avg"}))         return "AVG";
    // MIN avant MAX pour eviter qu'une expression comme "le moins" soit
    // happee par un futur token "le plus" partagé.
    if (hasAny(n, {"minimum","plus bas","plus petit","moins cher","plus vide",
                   "le moins","moins de","moindre","plus faible","plus rare",
                   "le plus faible","le plus bas","le plus petit","le plus vide","min"})) return "MIN";
    if (hasAny(n, {"maximum","plus eleve","plus haut","plus cher","plus grand",
                   "plus rempli","plus gros","plus important","plus eleves",
                   "le plus","top","max"})) return "MAX";
    if (hasAny(n, {"somme","total","sum","cumule","cumul"}))    return "SUM";
    return QString();
}

QString detectNumericColumn(const QString &n, const EntityDef &e)
{
    // Mappage mots → colonnes selon l'entité.
    if (e.key == "employe") {
        if (hasAny(n, {"salaire","paie","remuneration"})) return "SALAIRE";
        if (hasAny(n, {"recompense","reward","prime"}))  return "TOTAL_REWARD";
        if (hasAny(n, {"experience","xp"}))              return "TOTAL_EXPERIENCE";
        if (hasAny(n, {"conge","conges","jours"}))       return "TOTAL_LEAVE_DAYS";
    } else if (e.key == "client") {
        if (hasAny(n, {"tri","taux"}))                   return "TAUX_TRI";
    } else if (e.key == "matiere") {
        if (hasAny(n, {"quantite","qte","qty","stock"})) return "QUANTITE";
        if (hasAny(n, {"seuil"}))                        return "SEUIL_CRITIQUE";
        if (hasAny(n, {"prix","cout","cost"}))           return "PRIX";
    } else if (e.key == "produit") {
        if (hasAny(n, {"quantite","qte","qty","stock"})) return "QUANTITE";
        if (hasAny(n, {"prix","cout","cost","unitaire"})) return "PRIX_UNITAIRE";
        if (hasAny(n, {"capacite"}))                     return "CAPACITE";
    } else if (e.key == "bac") {
        if (hasAny(n, {"remplissage","taux","fill"}))    return "REMPLISSAGE";
        if (hasAny(n, {"prix","cout"}))                  return "PRIX";
        if (hasAny(n, {"stock","quantite"}))             return "STOCK";
        if (hasAny(n, {"batterie","autonomie"}))         return "CAPACITE_BATTERIE";
    } else if (e.key == "commande") {
        if (hasAny(n, {"prix","montant","total","ca","chiffre"})) return "PRIX_TOTAL";
        if (hasAny(n, {"quantite","qte","qty"}))                  return "QTE";
    } else if (e.key == "intervention") {
        if (hasAny(n, {"cout","prix","montant","facture"}))       return "COUT";
        if (hasAny(n, {"duree","heures","heure","time"}))         return "DUREE";
        if (hasAny(n, {"main oeuvre","mo","main d oeuvre"}))      return "MAIN_OEUVRE";
    }
    if (!e.numericCols.isEmpty()) return e.numericCols.first();
    return QString();
}

QStringList detectProjection(const QString &n, const EntityDef &e)
{
    QStringList cols;
    if (e.key == "employe") {
        if (hasAny(n, {"email","mail"}))             cols << "EMAIL";
        if (hasAny(n, {"matricule","code"}))         cols << "MATRICULE";
        if (hasAny(n, {"cin"}))                      cols << "CIN";
        if (hasAny(n, {"specialite","poste","role","metier"})) cols << "SPECIALITE";
        if (hasAny(n, {"salaire","paie"}))           cols << "SALAIRE";
        if (hasAny(n, {"disponibilite","statut","dispo"})) cols << "DISPONIBILITE";
    } else if (e.key == "client") {
        if (hasAny(n, {"email","mail"}))             cols << "EMAIL";
        if (hasAny(n, {"matricule","code"}))         cols << "MATRICULE";
        if (hasAny(n, {"contrat","type"}))           cols << "TYPE_CONTRAT";
        if (hasAny(n, {"paiement","statut"}))        cols << "STATUT_PAIEMENT";
        if (hasAny(n, {"expiration","fin"}))         cols << "DATE_EXPIRATION_CONTRAT";
        if (hasAny(n, {"taux","tri"}))               cols << "TAUX_TRI";
    } else if (e.key == "matiere") {
        if (hasAny(n, {"reference","ref"}))          cols << "REFERENCE";
        if (hasAny(n, {"quantite","qte","stock"}))   cols << "QUANTITE";
        if (hasAny(n, {"seuil"}))                    cols << "SEUIL_CRITIQUE";
        if (hasAny(n, {"prix","cout"}))              cols << "PRIX";
        if (hasAny(n, {"fournisseur","supplier"}))   cols << "NOM_FOUR";
    } else if (e.key == "produit") {
        if (hasAny(n, {"type","categorie"}))         cols << "TYPE";
        if (hasAny(n, {"quantite","qte","stock"}))   cols << "QUANTITE";
        if (hasAny(n, {"prix","cout","unitaire"}))   cols << "PRIX_UNITAIRE";
        if (hasAny(n, {"capacite"}))                 cols << "CAPACITE";
        if (hasAny(n, {"allee","rayon","emplacement","localisation"})) cols << "ALLEE";
    } else if (e.key == "bac") {
        if (hasAny(n, {"serie","numero"}))           cols << "NUM_SERIE";
        if (hasAny(n, {"remplissage","taux"}))       cols << "REMPLISSAGE";
        if (hasAny(n, {"localisation","emplacement","ou","ou est"})) cols << "LOCALISATION_STOCK";
        if (hasAny(n, {"prix"}))                     cols << "PRIX";
        if (hasAny(n, {"stock"}))                    cols << "STOCK";
    } else if (e.key == "commande") {
        if (hasAny(n, {"prix","montant","total"}))   cols << "PRIX_TOTAL";
        if (hasAny(n, {"quantite","qte"}))           cols << "QTE";
        if (hasAny(n, {"statut"}))                   cols << "STATUT";
        if (hasAny(n, {"priorite"}))                 cols << "PRIORITE";
        if (hasAny(n, {"date","commande"}))          cols << "DATE_COMMANDE";
        if (hasAny(n, {"livraison"}))                cols << "DATE_LIVRAISON";
        if (hasAny(n, {"adresse"}))                  cols << "ADRESSE";
    } else if (e.key == "intervention") {
        if (hasAny(n, {"reference","ref"}))          cols << "REFERENCE";
        if (hasAny(n, {"type"}))                     cols << "TYPE";
        if (hasAny(n, {"statut"}))                   cols << "STATUT";
        if (hasAny(n, {"priorite"}))                 cols << "PRIORITE";
        if (hasAny(n, {"cout","prix"}))              cols << "COUT";
        if (hasAny(n, {"duree"}))                    cols << "DUREE";
        if (hasAny(n, {"date"}))                     cols << "DATE_INTER";
    }
    return cols;
}

QString formatCellValue(const QVariant &v)
{
    if (v.isNull()) return "<i>(vide)</i>";
    const int t = v.typeId();
    if (t == QMetaType::Double || t == QMetaType::Float) {
        return QString::number(v.toDouble(), 'f', 2);
    }
    if (t == QMetaType::QDate) {
        return v.toDate().toString("dd/MM/yyyy");
    }
    if (t == QMetaType::QDateTime) {
        return v.toDateTime().toString("dd/MM/yyyy HH:mm");
    }
    return v.toString().trimmed();
}

// Indique si la question contient des indices "spécifiques" (filtre, agrégation,
// projection ou nom propre) — auquel cas le moteur générique passe en priorité
// devant les rapports génériques par module.
bool questionLooksSpecific(const QString &n)
{
    if (n.isEmpty()) return false;
    if (!detectAggregation(n).isEmpty()) {
        // une agrégation seule sur "client/employe/..." reste générique → on
        // privilégie le moteur uniquement si on a aussi un filtre ou une cible.
    }
    // Filtres connus
    static const QStringList filterMarkers = {
        "admin","responsable","technicien","superviseur","chauffeur","operateur",
        "disponible","dispo","en mission","en intervention","conge","conges",
        "absent","absente","absents","absentes","indisponible",
        "en retard","retard","suspendu","a jour","expire","expiration","expires",
        "urgente","urgent","urgentes","haute","livree","livrees",
        "installee","preparation","annulee","en cours","terminee","en attente",
        "critique","seuil","rupture","manque","manquant","epuise","faible",
        "plein","sature","rempli","vide",
        "le moins","le plus","plus gros","plus faible","moindre","plus important"
    };
    if (hasAny(n, filterMarkers)) return true;
    // Projection : email, prix, salaire, etc.
    static const QStringList projMarkers = {
        "email","mail","matricule","cin","specialite","poste","role",
        "salaire","paie","disponibilite","statut","contrat","paiement",
        "expiration","taux","tri","quantite","seuil","prix","cout","fournisseur",
        "serie","localisation","emplacement","priorite","duree","reference",
        "livraison","adresse","capacite","batterie"
    };
    if (hasAny(n, projMarkers)) return true;
    // Capture d'un nom propre / valeur libre via la regex extractFreeText
    if (!extractFreeText(n).isEmpty()) return true;
    // Verbes d'action ciblés
    if (hasAny(n, {"nommer","nomme","nomm","appelle","trouver","cherche","cherches",
                   "qui est","qui sont","donner","donne moi","liste les",
                   "afficher","montre"})) return true;
    return false;
}

QString smartDbAnswerImpl(const QString &userMessage)
{
    const QString n = normalize(userMessage);
    if (n.isEmpty()) return QString();

    QString entityKey = detectEntityKey(n);
    if (entityKey.isEmpty()) {
        // Pas d'entité explicite → on cherche le nom libre dans toutes les
        // entités et on choisit celle qui matche.
        const QString ft = extractFreeText(n);
        if (ft.isEmpty()) return QString();
        for (const auto &cand : entityCatalog()) {
            QStringList likeParts;
            for (const QString &c : cand.searchCols) {
                likeParts << QString("UPPER(NVL(%1,'')) LIKE :ft").arg(c);
            }
            QSqlQuery q;
            q.prepare(QString("SELECT COUNT(*) FROM %1 WHERE %2")
                          .arg(cand.table, likeParts.join(" OR ")));
            q.bindValue(":ft", "%" + ft.toUpper() + "%");
            if (q.exec() && q.next() && q.value(0).toInt() > 0) {
                entityKey = cand.key;
                break;
            }
        }
        if (entityKey.isEmpty()) return QString();
    }

    const EntityDef e = entityForKey(entityKey);
    if (e.table.isEmpty()) return QString();

    const QString agg = detectAggregation(n);
    QList<FilterClause> filters = detectFilters(n, entityKey);
    QString freeText = extractFreeText(n);
    if (!freeText.isEmpty() && freeTextLooksLikeComparatorNoise(freeText)) {
        freeText.clear();
    }
    // Quand une agregation (MAX/MIN/AVG/SUM/COUNT) est demandee, le residu de
    // free-text est presque toujours du bruit conversationnel ("parler moi de",
    // "dis moi", "donne moi"...) qui produirait un filtre LIKE absurde sur
    // l'agregat. On le drop pour laisser l'agregation porter sur tous les
    // enregistrements (sauf si un filtre metier explicite a deja matche).
    if (!agg.isEmpty()) {
        freeText.clear();
    }
    QStringList projection = detectProjection(n, e);

    // ─── Construction de la clause WHERE ───
    QStringList whereParts;
    QStringList displayFilters;
    for (const auto &f : filters) {
        whereParts << QString("(%1)").arg(f.sql);
        displayFilters << f.display;
    }
    QString freeBindKey;
    if (!freeText.isEmpty() && !e.searchCols.isEmpty()) {
        QStringList likeParts;
        for (const QString &c : e.searchCols) {
            likeParts << QString("UPPER(NVL(%1,'')) LIKE :ftext").arg(c);
        }
        whereParts << QString("(%1)").arg(likeParts.join(" OR "));
        freeBindKey = QStringLiteral(":ftext");
        displayFilters << QString("contenant \"%1\"").arg(freeText);
    }
    const QString whereSql = whereParts.isEmpty()
        ? QString()
        : (" WHERE " + whereParts.join(" AND "));
    const QString filterPretty = displayFilters.isEmpty()
        ? QString()
        : QString(" (%1)").arg(displayFilters.join(", "));

    // ─── 1) Agrégation simple (COUNT/AVG/MIN/MAX/SUM) ───
    if (agg == "COUNT") {
        const QString sql = QString("SELECT COUNT(*) FROM %1%2").arg(e.table, whereSql);
        QSqlQuery q;
        q.prepare(sql);
        if (!freeBindKey.isEmpty()) q.bindValue(freeBindKey, "%" + freeText.toUpper() + "%");
        if (!q.exec() || !q.next()) return QString();
        const int total = q.value(0).toInt();
        return QString("%1 <b>%2</b> %3%4 dans la base.")
            .arg(e.emoji).arg(total).arg(e.plural, filterPretty);
    }

    if (agg == "AVG" || agg == "MIN" || agg == "MAX" || agg == "SUM") {
        QString col = detectNumericColumn(n, e);
        if (col.isEmpty()) return QString();

        if ((agg == "MAX" || agg == "MIN") && !e.labelCol.isEmpty()) {
            // "qui gagne le plus" / "produit le plus cher" → ramener la ligne entière
            const QString order = (agg == "MAX") ? "DESC" : "ASC";
            const QString nonNullSql = whereSql.isEmpty()
                ? QString(" WHERE %1 IS NOT NULL").arg(col)
                : QString(" AND %1 IS NOT NULL").arg(col);
            const QString baseSql = QString(
                "SELECT %1, %2 FROM %3%4%5 ORDER BY %2 %6")
                .arg(e.labelCol, col, e.table, whereSql, nonNullSql, order);
            const QString sql = QString("SELECT * FROM (%1) WHERE ROWNUM <= 1").arg(baseSql);
            QSqlQuery q;
            q.prepare(sql);
            if (!freeBindKey.isEmpty()) q.bindValue(freeBindKey, "%" + freeText.toUpper() + "%");
            if (q.exec() && q.next()) {
                const QString lbl = q.value(0).toString().trimmed();
                const QString val = formatCellValue(q.value(1));
                return QString("%1 %2 sur <b>%3</b> &mdash; <b>%4</b> avec <b>%5</b>%6.")
                    .arg(e.emoji,
                         agg == "MAX" ? "Maximum" : "Minimum",
                         col.toLower(),
                         lbl, val, filterPretty);
            }
        }

        const QString aggSql = QString("SELECT %1(%2) FROM %3%4").arg(agg, col, e.table, whereSql);
        QSqlQuery q;
        q.prepare(aggSql);
        if (!freeBindKey.isEmpty()) q.bindValue(freeBindKey, "%" + freeText.toUpper() + "%");
        if (!q.exec() || !q.next()) return QString();
        const QVariant v = q.value(0);
        const QString val = formatCellValue(v);
        QString verb;
        if (agg == "AVG") verb = "moyenne";
        if (agg == "SUM") verb = "total";
        if (agg == "MIN") verb = "minimum";
        if (agg == "MAX") verb = "maximum";
        return QString("%1 <b>%2</b> de <i>%3</i> sur les %4%5 : <b>%6</b>.")
            .arg(e.emoji, verb, col.toLower(), e.plural, filterPretty, val);
    }

    // ─── 2) Lookup / liste / projection ───
    QStringList selectCols;
    selectCols << e.labelCol;
    for (const QString &c : projection) {
        if (!selectCols.contains(c)) selectCols << c;
    }
    if (selectCols.size() == 1 && !projection.isEmpty()) {
        selectCols = projection;
    }

    const QString baseSql = QString("SELECT %1 FROM %2%3")
                                .arg(selectCols.join(", "), e.table, whereSql);
    const QString sql = QString("SELECT * FROM (%1) WHERE ROWNUM <= 25").arg(baseSql);
    QSqlQuery q;
    q.prepare(sql);
    if (!freeBindKey.isEmpty()) q.bindValue(freeBindKey, "%" + freeText.toUpper() + "%");
    if (!q.exec()) return QString();

    QStringList rowsHtml;
    while (q.next() && rowsHtml.size() < 25) {
        QStringList parts;
        for (int i = 0; i < selectCols.size(); ++i) {
            const QString name = selectCols.at(i).toLower();
            const QString val  = formatCellValue(q.value(i));
            if (i == 0) {
                parts << QString("<b>%1</b>").arg(val);
            } else {
                parts << QString("<i>%1</i>: %2").arg(name, val);
            }
        }
        rowsHtml << "\xE2\x80\xA2 " + parts.join(" \xE2\x80\x94 ");
    }

    if (rowsHtml.isEmpty()) {
        return QStringLiteral("%1 Aucun résultat pour les %2%3. "
                              "Précise un nom, un statut ou une spécialité si tu veux que je creuse 🔍.")
            .arg(e.emoji, e.plural, filterPretty);
    }

    QString html = QString("%1 R\xC3\xA9sultats <b>%2</b>%3 (%4 ligne%5) :<br><br>")
        .arg(e.emoji, e.plural, filterPretty)
        .arg(rowsHtml.size())
        .arg(rowsHtml.size() > 1 ? "s" : "");
    html += rowsHtml.join("<br>");

    if (rowsHtml.size() == 25) {
        html += "<br><br><i>(R\xC3\xA9sultats limit\xC3\xA9" "s aux 25 premi\xC3\xA8res lignes &mdash; affine ta question si besoin.)</i>";
    }
    return html;
}

} // namespace

namespace Labib {

QString welcomeMessage(const QString &operatorName, bool isAdmin)
{
    Q_UNUSED(isAdmin);
    const QString who = operatorName.isEmpty() ? "coll\xC3\xA8gue" : operatorName;
    const LiveStats s = fetchStats();

    QString html = QString("\xF0\x9F\xA6\x8A <b>%1 %2 !</b> C'est Labib, votre coll\xC3\xA8gue WasteGuard.<br>")
        .arg(greetingByHour(), who);
    html += "Je suis branch\xC3\xA9 en direct sur la base &mdash; voici la photo du moment :<br>";
    html += QString("\xE2\x80\xA2 %1 clients &middot; %2 employ\xC3\xA9" "s &middot; %3 bacs &middot; %4 commandes<br>")
        .arg(s.clients).arg(s.employes).arg(s.bacs).arg(s.commandes);
    if (s.stockCritique > 0 || s.interventionsEnCours > 0) {
        html += "<b>\xE2\x9A\xA0\xEF\xB8\x8F \xC0 surveiller :</b> ";
        QStringList urgents;
        if (s.stockCritique > 0)        urgents << QString("%1 stock(s) critique(s)").arg(s.stockCritique);
        if (s.interventionsEnCours > 0) urgents << QString("%1 intervention(s) en cours").arg(s.interventionsEnCours);
        html += urgents.join(" &middot; ") + "<br>";
    }
    html += "<i>Tape <b>aide</b> ou <b>rapport du jour</b> pour qu'on attaque \xF0\x9F\x9A\x80</i>";
    return html;
}

QString replyIfMatched(const QString &userMessage, const QString &operatorName, bool isAdmin)
{
    const QString raw = userMessage.trimmed();
    if (raw.isEmpty()) return QString();
    const QString n = normalize(raw);

    // Identité / présentation
    if (hasAny(n, {"qui es tu", "qui es-tu", "qui est labib", "presente toi", "tu es qui"})) {
        return answerWhoAmI(operatorName, isAdmin);
    }

    // Salutations
    if (n == "salut" || n == "bonjour" || n == "hello" || n == "coucou" || n == "hi" || n == "yo"
        || n.startsWith("salut ") || n.startsWith("bonjour ") || n.startsWith("hello ")
        || n.startsWith("bonsoir") ) {
        return answerGreeting(operatorName);
    }

    // Remerciements
    if (hasAny(n, {"merci", "thanks", "thank you", "thx"})) {
        return answerThanks();
    }

    // Aide
    if (hasAny(n, {"aide", "help", "que peux tu faire", "que sais tu faire", "capacites", "fonctionnalites"})) {
        return answerHelp();
    }

    // ─── Moteur DB générique (priorité si la question semble spécifique) ───
    if (questionLooksSpecific(n)) {
        const QString db = smartDbAnswerImpl(userMessage);
        if (!db.isEmpty()) return db;
    }

    // Rapport global / dashboard
    if (hasAny(n, {"rapport", "resume", "synthese", "tableau de bord", "dashboard", "overview", "vue ensemble", "etat global", "etat de la societe", "etat boite"})) {
        return answerDailyReport(operatorName);
    }

    // Stock / matières premières / alertes
    if (hasAny(n, {"alerte", "alertes", "critique", "seuil", "rupture"})
        || (hasAny(n, {"stock", "matiere", "matieres", "produits"}) && hasAny(n, {"alerte", "critique", "rupture", "manque", "seuil"}))) {
        return answerStockAlerts();
    }
    if (hasAny(n, {"stock", "matiere", "matieres", "magasin", "inventaire"})) {
        return answerStockAlerts(); // résumé ciblé sur ce qui chauffe
    }

    // Modules métier
    if (hasAny(n, {"client", "clients", "abonne", "contrat"})) {
        return answerClients();
    }
    if (hasAny(n, {"employe", "employes", "equipe", "rh", "salarie", "personnel"})) {
        return answerEmployes();
    }
    if (hasAny(n, {"commande", "commandes", "order", "vente", "ventes", "ca", "chiffre"})) {
        return answerCommandes();
    }
    if (hasAny(n, {"intervention", "interventions", "maintenance", "panne", "reparation"})) {
        return answerInterventions();
    }
    if (hasAny(n, {"bac", "bacs", "iot", "capteur", "remplissage", "collecte"})) {
        return answerBacs();
    }

    // Comptages directs
    if (hasAny(n, {"combien", "nombre", "total", "count"})) {
        const QString c = answerCount(n);
        if (!c.isEmpty()) return c;
    }

    // Moteur générique : tente d'interpréter n'importe quelle question DB.
    const QString db = smartDbAnswerImpl(userMessage);
    if (!db.isEmpty()) return db;

    return QString();
}

QString smartDbAnswer(const QString &userMessage)
{
    return smartDbAnswerImpl(userMessage);
}

QString reply(const QString &userMessage, const QString &operatorName, bool isAdmin)
{
    const QString matched = replyIfMatched(userMessage, operatorName, isAdmin);
    if (!matched.isEmpty()) return matched;

    // Fallback engageant
    return QString(
        "\xF0\x9F\xA4\x94 J'ai pas tout compris, mais je veux t'aider. "
        "Essaie : <b>rapport du jour</b>, <b>alertes stock</b>, <b>clients</b>, "
        "<b>\xC3\xA9quipe</b>, <b>commandes</b>, <b>interventions</b> ou <b>bacs</b>. "
        "Ou tape <b>aide</b> pour la liste compl\xC3\xA8te. Ensemble on va plus vite \xF0\x9F\x9A\x80"
    );
}

QString markdownToHtml(const QString &raw)
{
    if (raw.isEmpty()) return raw;
    QString out = raw.toHtmlEscaped();

    // Bloc de code ```...``` (basique)
    {
        static const QRegularExpression re(R"(```([\s\S]*?)```)");
        out.replace(re, "<pre style='background:#f4f4f5;border-radius:6px;"
                        "padding:6px 8px;font-family:Consolas,monospace;'>\\1</pre>");
    }
    // Code inline `...`
    {
        static const QRegularExpression re("`([^`\\n]+)`");
        out.replace(re, "<code style='background:#f4f4f5;border-radius:4px;"
                        "padding:1px 4px;font-family:Consolas,monospace;'>\\1</code>");
    }
    // Gras **...**  (avant l'italique pour ne pas être avalé)
    {
        static const QRegularExpression re("\\*\\*([^*\\n][^*]*?)\\*\\*");
        out.replace(re, "<b>\\1</b>");
    }
    // Italique *...*
    {
        static const QRegularExpression re("(?<![*\\w])\\*([^*\\n]+?)\\*(?![*\\w])");
        out.replace(re, "<i>\\1</i>");
    }
    // Titres ### / ## / # en début de ligne
    {
        static const QRegularExpression re("(^|\\n)\\s*###\\s+([^\\n]+)");
        out.replace(re, "\\1<b style='font-size:14px;color:#1a7a42;'>\\2</b>");
    }
    {
        static const QRegularExpression re("(^|\\n)\\s*##\\s+([^\\n]+)");
        out.replace(re, "\\1<b style='font-size:15px;color:#1a7a42;'>\\2</b>");
    }
    {
        static const QRegularExpression re("(^|\\n)\\s*#\\s+([^\\n]+)");
        out.replace(re, "\\1<b style='font-size:16px;color:#1a7a42;'>\\2</b>");
    }
    // Listes (- item / * item) en début de ligne → puce
    {
        static const QRegularExpression re("(^|\\n)\\s*[-\\*]\\s+");
        out.replace(re, "\\1\xE2\x80\xA2 ");
    }
    // Sauts de ligne
    out.replace("\n", "<br>");
    return out;
}

QStringList quickSuggestions()
{
    return {
        "Rapport du jour",
        "Alertes stock",
        "\xC3\x89quipe",
        "Commandes",
        "Interventions",
        "Aide"
    };
}

QString signature()
{
    return QString("Labib \xC2\xB7 %1").arg(QTime::currentTime().toString("HH:mm"));
}

} // namespace Labib
