#include "labibassistant.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include <QRegularExpression>
#include <QProcess>
#include <QStandardPaths>
#include <QProcessEnvironment>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QDir>
#include <QRandomGenerator>
#include <QToolButton>
#include <QStyle>
#include <QGuiApplication>
#include <QClipboard>
#include <QTextDocument>
#include <QKeyEvent>
#include <QApplication>
#include <QScrollBar>
#include <QFrame>
#include <QColor>

namespace {

QString normalizeIntentText(QString text)
{
    text.replace("Ã©", "e");
    text.replace("Ã¨", "e");
    text.replace("Ãª", "e");
    text.replace("Ã«", "e");
    text.replace("Ã ", "a");
    text.replace("Ã¢", "a");
    text.replace("Ã®", "i");
    text.replace("Ã¯", "i");
    text.replace("Ã´", "o");
    text.replace("Ã¶", "o");
    text.replace("Ã¹", "u");
    text.replace("Ã»", "u");
    text.replace("Ã¼", "u");
    text.replace("Ã§", "c");
    text.replace("ã©", "e");
    text.replace("ã¨", "e");
    text.replace("ãª", "e");
    text.replace("ã«", "e");
    text.replace("ã ", "a");
    text.replace("ã¢", "a");
    text.replace("ã®", "i");
    text.replace("ã¯", "i");
    text.replace("ã´", "o");
    text.replace("ã¶", "o");
    text.replace("ã¹", "u");
    text.replace("ã»", "u");
    text.replace("ã¼", "u");
    text.replace("ã§", "c");
    text = text.normalized(QString::NormalizationForm_D);
    text.remove(QRegularExpression("[\\x{0300}-\\x{036f}]"));
    text = text.toLower();
    text.replace(QRegularExpression("[^a-z0-9 ]"), " ");
    text.replace(QRegularExpression("\\s+"), " ");
    return text.trimmed();
}

bool containsAny(const QString &text, const QStringList &tokens)
{
    for (const QString &t : tokens) {
        if (text.contains(t)) {
            return true;
        }
    }
    return false;
}

QString canonicalModuleName(const QString &raw)
{
    const QString n = normalizeIntentText(raw);
    if (containsAny(n, {"client", "clients", "customer"})) return "Client";
    if (containsAny(n, {"employe", "employes", "employee", "rh", "ressource humaine"})) return "Employe";
    if (containsAny(n, {"produit", "produits", "product"})) return "Produit";
    if (containsAny(n, {"maintenance", "intervention", "interventions", "panne"})) return "Maintenance";
    if (containsAny(n, {"commande", "commandes", "order"})) return "Commande";
    if (containsAny(n, {"stock", "inventaire", "magasin"})) return "Stock";
    return QString();
}

[[maybe_unused]] QString moduleOverview(const QString &module)
{
    if (module == "Client") {
        return "Module Clients: ajout/modification/suivi des contrats, emails, matricules, dates d'expiration et statut de paiement.";
    }
    if (module == "Employe") {
        return "Module Employes: gestion du personnel, CIN/email, specialite, disponibilite, salaire et suivi RH.";
    }
    if (module == "Produit") {
        return "Module Produits: references, type, capacite, prix, quantite et alertes de stock.";
    }
    if (module == "Maintenance") {
        return "Module Maintenance: interventions, reference, date, cout, priorite, statut, technicien et details terrain.";
    }
    if (module == "Commande") {
        return "Module Commandes: creation et suivi des commandes, quantites, statut, livraison et facturation.";
    }
    if (module == "Stock") {
        return "Module Stock: suivi intelligent des niveaux, entrees/sorties et previsions.";
    }
    return "WasteGuard couvre les modules Clients, Employes, Produits, Maintenance, Commandes et Stock.";
}

[[maybe_unused]] QString buildStepGuide(const QString &module)
{
    if (module == "Client") {
        return "Etapes pour ajouter un client dans WasteGuard:<br>"
               "1) Ouvrir le module <b>Clients</b>.<br>"
               "2) Cliquer sur <b>Nouveau / Ajouter</b>.<br>"
               "3) Renseigner: Nom, Matricule, Email, Type contrat, Telephone, Date creation et Date expiration.<br>"
               "4) Verifier l'unicite du matricule et la validite des dates.<br>"
               "5) Cliquer <b>Enregistrer</b> puis controler l'apparition dans le tableau.<br>"
               "6) Optionnel: exporter le contrat PDF pour archivage.";
    }
    if (module == "Maintenance") {
        return "Etapes pour ajouter une intervention:<br>"
               "1) Ouvrir le module <b>Maintenance</b>.<br>"
               "2) Cliquer sur <b>Ajouter intervention</b>.<br>"
               "3) Saisir Reference, Date, Duree, Cout, Priorite, Type, Statut et ID bac.<br>"
               "4) Ajouter technicien/adresse/description si disponibles.<br>"
               "5) Enregistrer puis verifier le statut dans le tableau des interventions.";
    }
    if (module == "Employe") {
        return "Etapes pour ajouter un employe:<br>"
               "1) Aller a <b>Employes</b> puis cliquer <b>Nouveau</b>.<br>"
               "2) Completer Nom, Matricule, CIN, Email, Specialite et disponibilite.<br>"
               "3) Enregistrer et verifier la ligne dans le tableau.<br>"
               "4) Optionnel: generer fiche de paie ou affecter une intervention.";
    }
    if (module == "Produit") {
        return "Etapes pour ajouter un produit:<br>"
               "1) Ouvrir le module <b>Produits</b>.<br>"
               "2) Cliquer sur <b>Ajouter Produit</b>.<br>"
               "3) Renseigner Model, Type, Capacite, Prix unitaire, Quantite et Allee.<br>"
               "4) Verifier les champs numeriques (prix/quantite/capacite).<br>"
               "5) Cliquer <b>Enregistrer</b> puis verifier la mise a jour du stock.";
    }
    if (module == "Commande") {
        return "Etapes pour ajouter une commande:<br>"
               "1) Ouvrir le module <b>Commandes</b>.<br>"
               "2) Cliquer sur <b>Nouvelle Commande</b>.<br>"
               "3) Selectionner le client/produit et saisir la quantite.<br>"
               "4) Verifier le montant, le statut et la date de livraison.<br>"
               "5) Enregistrer puis generer la facture PDF si necessaire.";
    }
    if (module == "Stock") {
        return "Etapes pour gerer le stock:<br>"
               "1) Ouvrir le module <b>Stock</b>.<br>"
               "2) Identifier le produit concerne (reference/model).<br>"
               "3) Choisir l'operation: entree, sortie ou ajustement.<br>"
               "4) Saisir la quantite et confirmer la mise a jour.<br>"
               "5) Verifier les alertes de seuil et l'historique des mouvements.";
    }
    return "Je peux te donner les etapes detaillees pour: ajout client, ajout employe, ajout intervention, gestion produit, commande et stock.";
}

[[maybe_unused]] QString buildActionGuide(const QString &module, const QString &action)
{
    if (action == "modifier") {
        return QString("Etapes pour modifier dans le module <b>%1</b>:<br>")
               .arg(module)
               + "1) Ouvrir le module concerne et rechercher l'enregistrement.<br>"
                 "2) Selectionner la ligne puis cliquer <b>Modifier</b>.<br>"
                 "3) Mettre a jour les champs necessaires.<br>"
                 "4) Valider et verifier l'historique/statut apres sauvegarde.";
    }
    if (action == "supprimer") {
        return QString("Etapes pour supprimer dans le module <b>%1</b>:<br>")
               .arg(module)
               + "1) Rechercher l'enregistrement cible.<br>"
                 "2) Verifier qu'il ne bloque aucune dependance (commande/intervention/stock).<br>"
                 "3) Cliquer <b>Supprimer</b> et confirmer.<br>"
                 "4) Controler que la suppression est bien refletee dans le tableau.";
    }
    if (action == "analyser") {
        return QString("Etapes pour analyser le module <b>%1</b>:<br>")
               .arg(module)
               + "1) Filtrer la periode et les statuts importants.<br>"
                 "2) Identifier les indicateurs cles (quantite, cout, delai, statut).<br>"
                 "3) Comparer les tendances et anomalies.<br>"
                 "4) Exporter le resultat (PDF/rapport) si necessaire.";
    }

    return buildStepGuide(module);
}

QString extractPdfTextHeuristicSimple(const QByteArray &pdfBytes)
{
    if (pdfBytes.isEmpty()) return QString();
    QString raw = QString::fromLatin1(pdfBytes);
    QStringList chunks;

    const QRegularExpression parenRe(R"(\(([^\\\)]{4,260})\))");
    auto it = parenRe.globalMatch(raw);
    while (it.hasNext()) {
        QString c = it.next().captured(1).trimmed();
        c.replace("\\n", " ");
        c.replace("\\r", " ");
        c.replace("\\t", " ");
        if (c.size() >= 4 && !chunks.contains(c)) {
            chunks << c;
        }
    }

    if (chunks.isEmpty()) {
        const QRegularExpression textRunRe(R"(([\p{L}0-9][\p{L}0-9 ,.;:/()'"+\-]{18,240}))");
        auto it2 = textRunRe.globalMatch(raw);
        while (it2.hasNext()) {
            const QString c = it2.next().captured(1).trimmed();
            if (c.size() >= 18 && !chunks.contains(c)) {
                chunks << c;
            }
            if (chunks.size() > 120) break;
        }
    }

    return chunks.join("\n").simplified();
}

QString readPdfByPdftotext(const QString &pdfPath)
{
    const QString exe = QStandardPaths::findExecutable("pdftotext");
    if (exe.isEmpty()) return QString();

    QProcess process;
    process.setProgram(exe);
    process.setArguments(QStringList() << "-layout" << pdfPath << "-");
    process.start();
    if (!process.waitForStarted(1500)) return QString();
    if (!process.waitForFinished(15000)) {
        process.kill();
        process.waitForFinished(700);
        return QString();
    }

    const QString out = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    if (!out.isEmpty()) return out;
    return QString::fromUtf8(process.readAllStandardError()).trimmed();
}

QString extractValue(const QString &text, const QStringList &labels)
{
    for (const QString &label : labels) {
        const QString escaped = QRegularExpression::escape(label);
        const QRegularExpression re(
            QString("(?:^|\\n|\\r|,)\\s*%1\\s*(?::|=|\\-|\\s)\\s*([^\\n\\r,;]{2,120})")
                .arg(escaped),
            QRegularExpression::CaseInsensitiveOption);
        const QRegularExpressionMatch m = re.match(text);
        if (m.hasMatch()) {
            return m.captured(1).trimmed();
        }

        // Fallback: label can appear in the middle of a line, e.g. "Nom du client : ...".
        const QRegularExpression reLoose(
            QString("\\b%1\\b[^\\n\\r:;=\\-]{0,40}(?::|=|\\-)\\s*([^\\n\\r,;]{2,120})")
                .arg(escaped),
            QRegularExpression::CaseInsensitiveOption);
        const QRegularExpressionMatch mLoose = reLoose.match(text);
        if (mLoose.hasMatch()) {
            return mLoose.captured(1).trimmed();
        }
    }
    return QString();
}

QJsonObject extractRecordFromDocument(const QString &rawText, const QString &module)
{
    QJsonObject record;
    if (rawText.trimmed().isEmpty()) return record;

    if (module == "Client") {
        const QString nom = extractValue(rawText, {"nom", "name", "raison sociale", "client"});
        const QString matricule = extractValue(rawText, {"matricule", "code client", "id client"});
        const QString email = extractValue(rawText, {"email", "mail", "e-mail"});
        const QString typeContrat = extractValue(rawText, {"type contrat", "contrat", "abonnement"});
        const QString dateExp = extractValue(rawText, {"date expiration", "expiration", "date fin"});
        const QString statut = extractValue(rawText, {"statut paiement", "paiement", "statut"});

        if (!nom.isEmpty()) record.insert("NOM", nom);
        if (!matricule.isEmpty()) record.insert("MATRICULE", matricule);
        if (!email.isEmpty()) record.insert("EMAIL", email);
        if (!typeContrat.isEmpty()) record.insert("TYPE_CONTRAT", typeContrat);
        if (!dateExp.isEmpty()) record.insert("DATE_EXPIRATION_CONTRAT", dateExp);
        if (!statut.isEmpty()) record.insert("STATUT_PAIEMENT", statut);
    } else if (module == "Maintenance") {
        const QString reference = extractValue(rawText, {"reference", "ref intervention", "numero"});
        const QString type = extractValue(rawText, {"type", "type intervention"});
        const QString priorite = extractValue(rawText, {"priorite", "urgence"});
        const QString statut = extractValue(rawText, {"statut", "etat"});
        const QString technicien = extractValue(rawText, {"technicien", "agent"});
        const QString cout = extractValue(rawText, {"cout", "cost", "montant"});
        const QString duree = extractValue(rawText, {"duree", "duration", "temps"});
        const QString idBac = extractValue(rawText, {"id bac", "bac", "id_bac"});

        if (!reference.isEmpty()) record.insert("REFERENCE", reference);
        if (!type.isEmpty()) record.insert("TYPE", type);
        if (!priorite.isEmpty()) record.insert("PRIORITE", priorite);
        if (!statut.isEmpty()) record.insert("STATUT", statut);
        if (!technicien.isEmpty()) record.insert("TECHNICIEN", technicien);
        if (!cout.isEmpty()) record.insert("COUT", cout);
        if (!duree.isEmpty()) record.insert("DUREE", duree);
        if (!idBac.isEmpty()) record.insert("ID_BAC", idBac);
    } else if (module == "Employe") {
        const QString nom = extractValue(rawText, {"nom", "name"});
        const QString email = extractValue(rawText, {"email", "mail", "e-mail"});
        const QString cin = extractValue(rawText, {"cin", "identifiant"});
        const QString specialite = extractValue(rawText, {"specialite", "poste", "role"});
        const QString salaire = extractValue(rawText, {"salaire", "salary"});
        const QString statut = extractValue(rawText, {"statut", "disponibilite"});

        if (!nom.isEmpty()) record.insert("NOM", nom);
        if (!email.isEmpty()) record.insert("EMAIL", email);
        if (!cin.isEmpty()) record.insert("CIN", cin);
        if (!specialite.isEmpty()) record.insert("SPECIALITE", specialite);
        if (!salaire.isEmpty()) record.insert("SALAIRE", salaire);
        if (!statut.isEmpty()) record.insert("STATUT", statut);
    } else if (module == "Produit") {
        const QString model = extractValue(rawText, {"model", "modele", "nom produit"});
        const QString type = extractValue(rawText, {"type"});
        const QString capacite = extractValue(rawText, {"capacite"});
        const QString prix = extractValue(rawText, {"prix", "prix unitaire", "cout"});
        const QString quantite = extractValue(rawText, {"quantite", "qte", "stock"});
        const QString allee = extractValue(rawText, {"allee", "rayon"});

        if (!model.isEmpty()) record.insert("MODEL", model);
        if (!type.isEmpty()) record.insert("TYPE", type);
        if (!capacite.isEmpty()) record.insert("CAPACITE", capacite);
        if (!prix.isEmpty()) record.insert("PRIX_UNITAIRE", prix);
        if (!quantite.isEmpty()) record.insert("QUANTITE", quantite);
        if (!allee.isEmpty()) record.insert("ALLEE", allee);
    } else if (module == "Commande") {
        const QString refer = extractValue(rawText, {"reference", "ref", "code commande"});
        const QString qte = extractValue(rawText, {"quantite", "qte"});
        const QString statut = extractValue(rawText, {"statut"});
        const QString prix = extractValue(rawText, {"prix total", "montant", "total"});

        if (!refer.isEmpty()) record.insert("REFER", refer);
        if (!qte.isEmpty()) record.insert("QTE", qte);
        if (!statut.isEmpty()) record.insert("STATUT", statut);
        if (!prix.isEmpty()) record.insert("PRIX_TOTAL", prix);
    } else if (module == "Stock") {
        const QString modele = extractValue(rawText, {"model", "modele"});
        const QString stock = extractValue(rawText, {"stock", "quantite", "qte"});
        const QString remplissage = extractValue(rawText, {"remplissage", "taux"});
        const QString prix = extractValue(rawText, {"prix", "valeur"});
        const QString localisation = extractValue(rawText, {"localisation", "emplacement"});

        if (!modele.isEmpty()) record.insert("MODELE", modele);
        if (!stock.isEmpty()) record.insert("STOCK", stock);
        if (!remplissage.isEmpty()) record.insert("REMPLISSAGE", remplissage);
        if (!prix.isEmpty()) record.insert("PRIX", prix);
        if (!localisation.isEmpty()) record.insert("LOCALISATION_STOCK", localisation);
    }

    return record;
}

QList<QJsonObject> extractRecordsFromDocument(const QString &rawText, const QString &module)
{
    QList<QJsonObject> out;
    if (rawText.trimmed().isEmpty()) return out;

    const QString normalized = rawText;
    const QStringList blocks = normalized.split(QRegularExpression("\\r?\\n\\s*\\r?\\n+"), Qt::SkipEmptyParts);

    // First pass: blank-line separated blocks (most invoices/forms exports).
    if (blocks.size() > 1) {
        for (const QString &block : blocks) {
            QJsonObject rec = extractRecordFromDocument(block, module);
            if (rec.isEmpty()) {
                const QStringList lines = block.split(QRegularExpression("\\r?\\n"), Qt::SkipEmptyParts);
                for (const QString &line : lines) {
                    const int c = line.indexOf(':');
                    const int e = line.indexOf('=');
                    int sep = -1;
                    if (c >= 0 && e >= 0) sep = qMin(c, e);
                    else sep = qMax(c, e);
                    if (sep > 0) {
                        const QString k = line.left(sep).trimmed();
                        const QString v = line.mid(sep + 1).trimmed();
                        if (!k.isEmpty() && !v.isEmpty()) rec.insert(k, v);
                    }
                }
            }
            if (!rec.isEmpty()) out.append(rec);
        }
    }

    // Second pass: line-stream segmentation for dense PDFs without blank separators.
    if (out.isEmpty()) {
        QString current;
        const QStringList lines = normalized.split(QRegularExpression("\\r?\\n"), Qt::SkipEmptyParts);
        const auto startsNewRecord = [&](const QString &line) {
            const QString n = normalizeIntentText(line);
            if (module == "Client") {
                return n.contains("nom") || n.contains("raison sociale") || n.contains("matricule");
            }
            if (module == "Employe") {
                return n.contains("nom") || n.contains("cin");
            }
            if (module == "Produit") {
                return n.contains("model") || n.contains("modele") || n.contains("type");
            }
            if (module == "Maintenance") {
                return n.contains("reference") || n.contains("type") || n.contains("priorite");
            }
            return n.contains("reference") || n.contains("nom");
        };

        for (const QString &line : lines) {
            if (line.trimmed().isEmpty()) continue;
            if (!current.isEmpty() && startsNewRecord(line)) {
                QJsonObject rec = extractRecordFromDocument(current, module);
                if (!rec.isEmpty()) out.append(rec);
                current.clear();
            }
            if (!current.isEmpty()) current += "\n";
            current += line;
        }
        if (!current.isEmpty()) {
            QJsonObject rec = extractRecordFromDocument(current, module);
            if (!rec.isEmpty()) out.append(rec);
        }
    }

    // Final fallback: whole-document single extraction.
    if (out.isEmpty()) {
        const QJsonObject rec = extractRecordFromDocument(rawText, module);
        if (!rec.isEmpty()) out.append(rec);
    }

    return out;
}

struct LocalQwenConfig {
    QString endpointBase;
    QString model;
    QString apiKey;
    bool valid = false;
};

QString trimTrailingSlash(QString s)
{
    while (s.endsWith('/')) s.chop(1);
    return s;
}

QString detectInstalledQwenModel()
{
    QProcess p;
    p.setProgram("ollama");
    p.setArguments(QStringList() << "list");
    p.start();
    if (!p.waitForStarted(1200)) return QString();
    if (!p.waitForFinished(2500)) {
        p.kill();
        p.waitForFinished(300);
        return QString();
    }

    const QString out = QString::fromUtf8(p.readAllStandardOutput());
    const QStringList lines = out.split(QRegularExpression("\\r?\\n"), Qt::SkipEmptyParts);
    for (const QString &lineRaw : lines) {
        const QString line = lineRaw.trimmed();
        if (line.isEmpty()) continue;
        if (line.startsWith("NAME", Qt::CaseInsensitive)) continue;

        const QStringList cols = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (cols.isEmpty()) continue;
        const QString modelName = cols.first().trimmed();
        if (modelName.contains("qwen", Qt::CaseInsensitive)) {
            return modelName;
        }
    }
    return QString();
}

LocalQwenConfig resolveLocalQwenConfig()
{
    LocalQwenConfig cfg;
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    QString base = env.value("WASTEGUARD_LOCAL_QWEN_URL").trimmed();
    if (base.isEmpty()) {
        base = env.value("OLLAMA_HOST").trimmed();
    }
    if (base.isEmpty()) {
        base = "http://127.0.0.1:11434";
    }
    if (!base.startsWith("http://") && !base.startsWith("https://")) {
        base = "http://" + base;
    }
    cfg.endpointBase = trimTrailingSlash(base);

    cfg.model = env.value("WASTEGUARD_LOCAL_QWEN_MODEL").trimmed();
    if (cfg.model.isEmpty()) {
        cfg.model = env.value("OLLAMA_MODEL").trimmed();
    }
    if (cfg.model.isEmpty()) {
        cfg.model = detectInstalledQwenModel();
    }
    if (cfg.model.isEmpty()) {
        cfg.model = "qwen2.5:7b-instruct";
    }

    cfg.apiKey = env.value("WASTEGUARD_LOCAL_QWEN_API_KEY").trimmed();
    cfg.valid = !cfg.endpointBase.isEmpty() && !cfg.model.isEmpty();
    return cfg;
}

QString responseTextFromOpenAiJson(const QJsonObject &obj)
{
    const QJsonArray choices = obj.value("choices").toArray();
    if (choices.isEmpty()) return QString();
    const QJsonObject msg = choices.first().toObject().value("message").toObject();
    return msg.value("content").toString().trimmed();
}

QString responseTextFromOllamaJson(const QJsonObject &obj)
{
    const QString nested = obj.value("message").toObject().value("content").toString().trimmed();
    if (!nested.isEmpty()) return nested;
    return obj.value("response").toString().trimmed();
}

QString postAssistantRequest(const QUrl &url, const QJsonObject &payload, const QString &apiKey, int timeoutMs)
{
    if (!url.isValid()) return QString();

    QNetworkAccessManager network;
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if (!apiKey.trimmed().isEmpty()) {
        req.setRawHeader("Authorization", QString("Bearer %1").arg(apiKey).toUtf8());
    }

    QNetworkReply *reply = network.post(req, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    QEventLoop loop;
    QTimer timeout;
    timeout.setSingleShot(true);

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit);
    timeout.start(timeoutMs);
    loop.exec();

    QString out;
    if (timeout.isActive() && reply->error() == QNetworkReply::NoError) {
        const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isObject()) {
            out = responseTextFromOpenAiJson(doc.object());
            if (out.isEmpty()) {
                out = responseTextFromOllamaJson(doc.object());
            }
        }
    } else if (!timeout.isActive()) {
        reply->abort();
    }

    reply->deleteLater();
    return out;
}

QString queryLocalQwenAssistant(const QString &userMessage, const QString &module)
{
    const LocalQwenConfig cfg = resolveLocalQwenConfig();
    if (!cfg.valid) return QString();

    const QString moduleCtx = module.trimmed().isEmpty() ? "non precise" : module.trimmed();
    const QString systemPrompt =
        "Tu es Labib, assistant personnel interne de l'application WasteGuard. "
        "Tu dois aider uniquement sur l'application WasteGuard (modules, workflows, donnees, import, erreurs, utilisation). "
        "Refuse poliment les demandes hors sujet (culture generale, actualites, etc.) et recentre vers l'app. "
        "Reponds en francais, concret, et avec etapes numerotees quand il faut. "
        "N'invente pas de fonctionnalites non presentes. "
        "Module courant: " + moduleCtx + ".";

    QJsonArray messages;
    messages.append(QJsonObject{{"role", "system"}, {"content", systemPrompt}});
    messages.append(QJsonObject{{"role", "user"}, {"content", userMessage}});

    QString base = trimTrailingSlash(cfg.endpointBase);
    const bool hasV1 = base.endsWith("/v1");
    const QString openAiUrl = hasV1 ? (base + "/chat/completions") : (base + "/v1/chat/completions");
    QJsonObject openAiPayload{
        {"model", cfg.model},
        {"messages", messages},
        {"temperature", 0.2}
    };

    QString out = postAssistantRequest(QUrl(openAiUrl), openAiPayload, cfg.apiKey, 16000);
    if (!out.isEmpty()) {
        return out;
    }

    QString nativeBase = base;
    if (nativeBase.endsWith("/v1")) {
        nativeBase.chop(3);
    }
    nativeBase = trimTrailingSlash(nativeBase);
    const QJsonObject ollamaPayload{
        {"model", cfg.model},
        {"messages", messages},
        {"stream", false}
    };
    out = postAssistantRequest(QUrl(nativeBase + "/api/chat"), ollamaPayload, QString(), 16000);
    return out;
}

QString moduleTableName(const QString &module)
{
    if (module == "Client") return "CLIENT";
    if (module == "Employe") return "EMPLOYE";
    if (module == "Produit") return "PRODUITS";
    if (module == "Maintenance") return "INTERVENTION";
    if (module == "Commande") return "COMMANDE";
    if (module == "Stock") return "BAC_INTEL";
    return QString();
}

QString modulePrimaryLabelColumn(const QString &module)
{
    if (module == "Client") return "NOM";
    if (module == "Employe") return "NOM";
    if (module == "Produit") return "MODEL";
    if (module == "Maintenance") return "REFERENCE";
    if (module == "Commande") return "REFER";
    if (module == "Stock") return "MODELE";
    return QString();
}

QStringList moduleSearchColumns(const QString &module)
{
    if (module == "Client") return {"NOM", "MATRICULE", "EMAIL"};
    if (module == "Employe") return {"NOM", "EMAIL", "CIN"};
    if (module == "Produit") return {"MODEL", "TYPE", "ALLEE"};
    if (module == "Maintenance") return {"REFERENCE", "TYPE", "STATUT"};
    if (module == "Commande") return {"REFER", "STATUT"};
    if (module == "Stock") return {"MODELE", "LOCALISATION_STOCK", "TYPE"};
    return {};
}

QString fieldColumnForQuestion(const QString &module, const QString &normalizedQuestion)
{
    const QString n = normalizedQuestion;
    if (module == "Client") {
        if (containsAny(n, {"nom"})) return "NOM";
        if (containsAny(n, {"email", "mail"})) return "EMAIL";
        if (containsAny(n, {"matricule", "code"})) return "MATRICULE";
        if (containsAny(n, {"contrat", "type contrat"})) return "TYPE_CONTRAT";
        if (containsAny(n, {"paiement", "statut"})) return "STATUT_PAIEMENT";
        if (containsAny(n, {"telephone", "tel"})) return "TELEPHONE";
    } else if (module == "Employe") {
        if (containsAny(n, {"nom"})) return "NOM";
        if (containsAny(n, {"email", "mail"})) return "EMAIL";
        if (containsAny(n, {"cin"})) return "CIN";
        if (containsAny(n, {"specialite"})) return "SPECIALITE";
        if (containsAny(n, {"salaire"})) return "SALAIRE";
        if (containsAny(n, {"statut", "disponibilite"})) return "STATUT";
    } else if (module == "Produit") {
        if (containsAny(n, {"model", "modele", "nom"})) return "MODEL";
        if (containsAny(n, {"type"})) return "TYPE";
        if (containsAny(n, {"capacite"})) return "CAPACITE";
        if (containsAny(n, {"prix", "cout"})) return "PRIX_UNITAIRE";
        if (containsAny(n, {"quantite", "qte", "stock"})) return "QUANTITE";
        if (containsAny(n, {"allee", "rayon"})) return "ALLEE";
    } else if (module == "Maintenance") {
        if (containsAny(n, {"reference", "ref"})) return "REFERENCE";
        if (containsAny(n, {"type"})) return "TYPE";
        if (containsAny(n, {"priorite"})) return "PRIORITE";
        if (containsAny(n, {"statut"})) return "STATUT";
        if (containsAny(n, {"cout", "prix"})) return "COUT";
        if (containsAny(n, {"duree"})) return "DUREE";
        if (containsAny(n, {"id bac", "bac"})) return "ID_BAC";
    } else if (module == "Commande") {
        if (containsAny(n, {"reference", "ref"})) return "REFER";
        if (containsAny(n, {"quantite", "qte"})) return "QTE";
        if (containsAny(n, {"prix", "montant", "total"})) return "PRIX_TOTAL";
        if (containsAny(n, {"statut"})) return "STATUT";
        if (containsAny(n, {"date"})) return "DATE_COMMANDE";
    } else if (module == "Stock") {
        if (containsAny(n, {"model", "modele", "nom"})) return "MODELE";
        if (containsAny(n, {"stock", "quantite", "qte"})) return "STOCK";
        if (containsAny(n, {"remplissage", "taux"})) return "REMPLISSAGE";
        if (containsAny(n, {"prix", "valeur"})) return "PRIX";
        if (containsAny(n, {"emplacement", "localisation"})) return "LOCALISATION_STOCK";
        if (containsAny(n, {"type"})) return "TYPE";
    }
    return QString();
}

QString extractTargetToken(const QString &normalizedQuestion)
{
    const QRegularExpression re("(?:de|du|des|pour|client|produit|employe|commande|intervention|stock)\\s+([a-z0-9@._\\- ]{2,80})");
    const QRegularExpressionMatch m = re.match(normalizedQuestion);
    if (!m.hasMatch()) return QString();
    QString t = m.captured(1).trimmed();
    t.remove(QRegularExpression("\\b(le|la|les|un|une|ce|cet|cette|des)\\b"));
    t = t.simplified();
    if (t.size() < 2) return QString();
    return t;
}

QString normalizeDateForOracle(const QString &raw)
{
    const QString s = raw.trimmed();
    if (s.isEmpty()) return QString("2026-12-31");

    QDate d = QDate::fromString(s, "yyyy-MM-dd");
    if (!d.isValid()) d = QDate::fromString(s, "dd/MM/yyyy");
    if (!d.isValid()) d = QDate::fromString(s, "d/M/yyyy");
    if (!d.isValid()) d = QDate::fromString(s, "dd-MM-yyyy");
    if (!d.isValid()) d = QDate::fromString(s, "d-M-yyyy");
    if (!d.isValid()) d = QDate::fromString(s, "MM/dd/yyyy");

    if (!d.isValid()) return QString("2026-12-31");
    return d.toString("yyyy-MM-dd");
}

QString autoClientMatricule()
{
    QSqlQuery q;
    if (q.exec("SELECT NVL(MAX(TO_NUMBER(REGEXP_SUBSTR(MATRICULE, '[0-9]+$'))), 0) + 1 FROM CLIENT WHERE UPPER(MATRICULE) LIKE 'CL-%'") && q.next()) {
        const int next = q.value(0).toInt();
        return QString("CL-%1").arg(next, 6, 10, QChar('0'));
    }

    // Fallback when DB query cannot compute the next sequential value.
    const QString ts = QDateTime::currentDateTime().toString("yyMMddhhmmss");
    return QString("CL-%1").arg(ts);
}

QJsonObject normalizeImportedRecordForModule(const QJsonObject &input, const QString &module)
{
    if (input.isEmpty()) return input;

    QJsonObject out;
    const auto setIf = [&](const QString &key, const QString &value) {
        const QString v = value.trimmed();
        if (!v.isEmpty() && !out.contains(key)) {
            out.insert(key, v);
        }
    };

    for (auto it = input.begin(); it != input.end(); ++it) {
        const QString key = it.key().trimmed();
        const QString val = it.value().toVariant().toString().trimmed();
        if (val.isEmpty()) continue;

        const QString nk = normalizeIntentText(key);

        if (module == "Client") {
            if (containsAny(nk, {"nom", "name", "raison sociale", "client"})) setIf("NOM", val);
            else if (containsAny(nk, {"matricule", "code client", "id client", "code"})) setIf("MATRICULE", val);
            else if (containsAny(nk, {"email", "mail", "e mail"})) setIf("EMAIL", val);
            else if (containsAny(nk, {"contrat", "abonnement", "type contrat"})) setIf("TYPE_CONTRAT", val);
            else if (containsAny(nk, {"expiration", "date fin", "date expiration"})) setIf("DATE_EXPIRATION_CONTRAT", val);
            else if (containsAny(nk, {"paiement", "statut"})) setIf("STATUT_PAIEMENT", val);
            else if (containsAny(nk, {"telephone", "tel"})) setIf("TELEPHONE", val);
        } else if (module == "Employe") {
            if (containsAny(nk, {"nom", "name"})) setIf("NOM", val);
            else if (containsAny(nk, {"email", "mail"})) setIf("EMAIL", val);
            else if (containsAny(nk, {"cin"})) setIf("CIN", val);
            else if (containsAny(nk, {"specialite", "poste", "role"})) setIf("SPECIALITE", val);
            else if (containsAny(nk, {"salaire", "salary"})) setIf("SALAIRE", val);
            else if (containsAny(nk, {"statut", "disponibilite"})) setIf("STATUT", val);
        } else if (module == "Produit") {
            if (containsAny(nk, {"model", "modele", "nom produit", "produit"})) setIf("MODEL", val);
            else if (containsAny(nk, {"type"})) setIf("TYPE", val);
            else if (containsAny(nk, {"capacite"})) setIf("CAPACITE", val);
            else if (containsAny(nk, {"prix", "cout", "prix unitaire"})) setIf("PRIX_UNITAIRE", val);
            else if (containsAny(nk, {"quantite", "qte", "stock"})) setIf("QUANTITE", val);
            else if (containsAny(nk, {"allee", "rayon"})) setIf("ALLEE", val);
        } else if (module == "Maintenance") {
            if (containsAny(nk, {"reference", "ref", "numero"})) setIf("REFERENCE", val);
            else if (containsAny(nk, {"type"})) setIf("TYPE", val);
            else if (containsAny(nk, {"priorite", "urgence"})) setIf("PRIORITE", val);
            else if (containsAny(nk, {"statut", "etat"})) setIf("STATUT", val);
            else if (containsAny(nk, {"cout", "prix"})) setIf("COUT", val);
            else if (containsAny(nk, {"duree", "temps"})) setIf("DUREE", val);
            else if (containsAny(nk, {"id bac", "bac"})) setIf("ID_BAC", val);
        } else if (module == "Commande") {
            if (containsAny(nk, {"reference", "ref"})) setIf("REFER", val);
            else if (containsAny(nk, {"quantite", "qte"})) setIf("QTE", val);
            else if (containsAny(nk, {"prix", "montant", "total"})) setIf("PRIX_TOTAL", val);
            else if (containsAny(nk, {"statut"})) setIf("STATUT", val);
            else if (containsAny(nk, {"date"})) setIf("DATE_COMMANDE", val);
        } else if (module == "Stock") {
            if (containsAny(nk, {"model", "modele", "produit"})) setIf("MODELE", val);
            else if (containsAny(nk, {"stock", "quantite", "qte"})) setIf("STOCK", val);
            else if (containsAny(nk, {"remplissage", "taux"})) setIf("REMPLISSAGE", val);
            else if (containsAny(nk, {"prix", "valeur"})) setIf("PRIX", val);
            else if (containsAny(nk, {"localisation", "emplacement"})) setIf("LOCALISATION_STOCK", val);
            else if (containsAny(nk, {"type"})) setIf("TYPE", val);
        }

        // Keep exact schema keys if already present in the input.
        setIf(key.toUpper(), val);
    }

    // If normalization produced nothing, keep original payload.
    return out.isEmpty() ? input : out;
}

QString validationFailureReason(const QJsonObject &record, const QString &module)
{
    if (module == "Client") {
        return "Champs requis Client manquants (NOM ou MATRICULE).";
    }
    if (module == "Employe") {
        return "Champs requis Employe manquants (NOM et EMAIL).";
    }
    if (module == "Produit") {
        return "Champs requis Produit manquants (MODEL et TYPE).";
    }
    if (module == "Maintenance") {
        return "Champs requis Maintenance manquants (REFERENCE/TYPE/STATUT).";
    }
    Q_UNUSED(record);
    return "Validation impossible sur cet enregistrement.";
}

QString answerDataQuestion(const QString &userMessage, const QString &module)
{
    const QString table = moduleTableName(module);
    if (table.isEmpty()) return QString();

    const QString normalized = normalizeIntentText(userMessage);
    const bool asksCount = containsAny(normalized, {"combien", "nombre", "total"});
    const bool asksList = containsAny(normalized, {"liste", "affiche", "montre", "tous", "toutes"});
    const QString fieldCol = fieldColumnForQuestion(module, normalized);

    if (asksCount && fieldCol.isEmpty()) {
        QSqlQuery q;
        if (q.exec(QString("SELECT COUNT(*) FROM %1").arg(table)) && q.next()) {
            return QString("Il y a actuellement <b>%1</b> enregistrements dans le module <b>%2</b>.")
                .arg(q.value(0).toInt())
                .arg(module);
        }
        return QString("Je n'arrive pas a recuperer le total pour le module %1.").arg(module);
    }

    if (asksList && fieldCol.isEmpty()) {
        const QString labelCol = modulePrimaryLabelColumn(module);
        if (labelCol.isEmpty()) return QString();
        QSqlQuery q;
        const QString sql = QString("SELECT %1 FROM %2 WHERE %1 IS NOT NULL FETCH FIRST 10 ROWS ONLY").arg(labelCol, table);
        if (q.exec(sql)) {
            QStringList rows;
            while (q.next()) {
                rows << q.value(0).toString().trimmed();
            }
            if (!rows.isEmpty()) {
                return QString("Top elements (%1):<br>- %2").arg(module, rows.join("<br>- "));
            }
        }
        return QString("Aucune donnee listee pour le module %1.").arg(module);
    }

    if (!fieldCol.isEmpty()) {
        QString target = extractTargetToken(normalized);
        const QStringList searchCols = moduleSearchColumns(module);

        if (target.isEmpty()) {
            return QString("Je peux repondre, mais precise la cible. Exemple: <i>quel est l'email du client ahmed</i>.");
        }

        if (searchCols.isEmpty()) return QString();
        QString where;
        for (int i = 0; i < searchCols.size(); ++i) {
            if (i) where += " OR ";
            where += QString("UPPER(%1) LIKE :needle").arg(searchCols[i]);
        }

        QSqlQuery q;
        const QString sql = QString("SELECT %1 FROM %2 WHERE (%3) AND %1 IS NOT NULL FETCH FIRST 1 ROWS ONLY")
            .arg(fieldCol, table, where);
        q.prepare(sql);
        q.bindValue(":needle", QString("%") + target.toUpper() + "%");
        if (q.exec() && q.next()) {
            const QString v = q.value(0).toString().trimmed();
            if (!v.isEmpty()) {
                return QString("Valeur de <b>%1</b> pour <b>%2</b> : <b>%3</b>")
                    .arg(fieldCol, target, v.toHtmlEscaped());
            }
        }
        return QString("Je n'ai pas trouve cette information pour <b>%1</b> dans le module <b>%2</b>.")
            .arg(target, module);
    }

    return QString();
}

QList<QJsonObject> parseDelimitedText(const QString &text, const QChar delimiter)
{
    QList<QJsonObject> records;
    const QStringList lines = text.split(QRegularExpression("\\r?\\n"), Qt::SkipEmptyParts);
    if (lines.size() < 2) return records;

    QStringList headers = lines.first().split(delimiter, Qt::KeepEmptyParts);
    for (QString &h : headers) h = h.trimmed();

    for (int li = 1; li < lines.size(); ++li) {
        const QStringList vals = lines[li].split(delimiter, Qt::KeepEmptyParts);
        if (vals.isEmpty()) continue;
        QJsonObject row;
        const int n = qMin(headers.size(), vals.size());
        for (int i = 0; i < n; ++i) {
            if (!headers[i].isEmpty()) {
                row.insert(headers[i], vals[i].trimmed());
            }
        }
        if (!row.isEmpty()) records.append(row);
    }
    return records;
}

QList<QJsonObject> parseExcelByPython(const QString &xlsxPath)
{
    QList<QJsonObject> records;
    const QString py = QStandardPaths::findExecutable("python");
    if (py.isEmpty()) return records;

    const QString code =
        "import json,sys\n"
        "p=sys.argv[1]\n"
        "try:\n"
        " import openpyxl\n"
        " wb=openpyxl.load_workbook(p, data_only=True)\n"
        " ws=wb.active\n"
        " rows=list(ws.iter_rows(values_only=True))\n"
        " if not rows: print('[]'); sys.exit(0)\n"
        " def score_header(r):\n"
        "  vals=[str(c).strip() for c in r if c is not None and str(c).strip()]\n"
        "  if len(vals)<2: return -1\n"
        "  return len(vals)\n"
        " hdr_idx=0; best=-1\n"
        " for i in range(min(25,len(rows))):\n"
        "  sc=score_header(rows[i])\n"
        "  if sc>best: best=sc; hdr_idx=i\n"
        " headers=[str(c).strip() if c is not None else '' for c in rows[hdr_idx]]\n"
        " out=[]\n"
        " for r in rows[hdr_idx+1:]:\n"
        "  obj={}\n"
        "  for i,v in enumerate(r):\n"
        "   if i < len(headers) and headers[i]: obj[headers[i]]='' if v is None else str(v).strip()\n"
        "  if obj: out.append(obj)\n"
        " print(json.dumps(out, ensure_ascii=False))\n"
        "except Exception:\n"
        " print('[]')\n";

    QProcess p;
    p.setProgram(py);
    p.setArguments(QStringList() << "-c" << code << xlsxPath);
    p.start();
    if (!p.waitForStarted(1500)) return records;
    if (!p.waitForFinished(20000)) {
        p.kill();
        p.waitForFinished(500);
        return records;
    }

    const QByteArray out = p.readAllStandardOutput().trimmed();
    QJsonParseError err{};
    const QJsonDocument doc = QJsonDocument::fromJson(out, &err);
    if (err.error != QJsonParseError::NoError || !doc.isArray()) return records;

    for (const QJsonValue &v : doc.array()) {
        if (v.isObject()) records.append(v.toObject());
    }
    return records;
}

QList<QJsonObject> parseExcelByPowerShell(const QString &xlsxPath)
{
    QList<QJsonObject> records;
    const QString script =
        "$ErrorActionPreference='SilentlyContinue';"
        "$p=$args[0];"
        "$e=$null; $w=$null;"
        "try {"
        " $e=New-Object -ComObject Excel.Application;"
        " $e.Visible=$false;"
        " $e.DisplayAlerts=$false;"
        " $w=$e.Workbooks.Open($p);"
        " $s=$w.Worksheets.Item(1);"
        " $u=$s.UsedRange;"
        " $r=$u.Rows.Count; $c=$u.Columns.Count;"
        " if($r -lt 1 -or $c -lt 1){ Write-Output '[]'; exit 0 }"
        " $bestRow=1; $bestScore=-1;"
        " for($i=1;$i -le [Math]::Min(25,$r);$i++){"
        "  $score=0;"
        "  for($j=1;$j -le $c;$j++){ $t=[string]$u.Cells.Item($i,$j).Text; if($t -and $t.Trim().Length -gt 0){ $score++ } }"
        "  if($score -gt $bestScore){ $bestScore=$score; $bestRow=$i }"
        " }"
        " $h=@(); for($j=1;$j -le $c;$j++){ $h += [string]$u.Cells.Item($bestRow,$j).Text }"
        " $arr=@();"
        " for($i=$bestRow+1;$i -le $r;$i++){"
        "  $o=[ordered]@{};"
        "  for($j=1;$j -le $c;$j++){ if($h[$j-1]){ $o[$h[$j-1]]=[string]$u.Cells.Item($i,$j).Text } }"
        "  if($o.Count -gt 0){ $arr += [pscustomobject]$o }"
        " }"
        " $arr | ConvertTo-Json -Depth 5 -Compress"
        "} catch { Write-Output '[]' }"
        "finally { if($w){$w.Close($false)}; if($e){$e.Quit()} }";

    QProcess p;
    p.setProgram("powershell");
    p.setArguments(QStringList() << "-NoProfile" << "-ExecutionPolicy" << "Bypass" << "-Command" << script << xlsxPath);
    p.start();
    if (!p.waitForStarted(1500)) return records;
    if (!p.waitForFinished(25000)) {
        p.kill();
        p.waitForFinished(500);
        return records;
    }

    QByteArray out = p.readAllStandardOutput().trimmed();
    if (out.isEmpty()) return records;
    if (out.startsWith("{")) out = "[" + out + "]";
    QJsonParseError err{};
    const QJsonDocument doc = QJsonDocument::fromJson(out, &err);
    if (err.error != QJsonParseError::NoError || !doc.isArray()) return records;

    for (const QJsonValue &v : doc.array()) {
        if (v.isObject()) records.append(v.toObject());
    }
    return records;
}

} // namespace

LabibAssistant::LabibAssistant(QWidget *parent)
    : QWidget(parent), m_selectedModule("Client")
{
    setupUI();
    setupConnections();
}

LabibAssistant::~LabibAssistant()
{
}

void LabibAssistant::setupUI()
{
    setWindowTitle("Labib AI Assistant - WasteGuard Intelligence");
    setWindowIcon(QIcon(":/login_logo.png"));
    resize(900, 680);
    setMinimumSize(520, 520);
    
    // Professional Color Palette
    const QString PRIMARY_COLOR = "#0f2b4c";      // Deep Blue
    const QString SECONDARY_COLOR = "#2a5298";    // Medium Blue
    const QString ACCENT_COLOR = "#27ae60";       // Green
    const QString SUCCESS_COLOR = "#059669";      // Bright Green
    const QString WARNING_COLOR = "#f59e0b";      // Amber
    const QString ERROR_COLOR = "#dc2626";        // Red
    const QString TEXT_PRIMARY = "#1e293b";       // Dark Gray
    const QString TEXT_SECONDARY = "#64748b";     // Medium Gray
    const QString BG_LIGHT = "#f8fafc";           // Light Gray
    const QString BG_WHITE = "#ffffff";           // White
    const QString BORDER_COLOR = "#e2e8f0";       // Light Border
    
    QString stylesheet = QString(
        "QWidget { background: %1; }"
        "QTextEdit { background: %6; border: 1px solid %7; border-radius: 12px; padding: 12px; font-size: 13px; color: %2; }"
        "QLineEdit { background: %6; border: 1px solid %7; border-radius: 8px; padding: 10px; font-size: 13px; color: %4; }"
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %1, stop:1 %2); color: white; border: none; border-radius: 10px; padding: 12px 18px; font-weight: 700; font-size: 13px; }"
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %2, stop:1 %1); }"
        "QPushButton:pressed { transform: scale(0.98); }"
        "QTableWidget { background: %6; border: 1px solid %7; border-radius: 8px; gridline-color: %7; }"
        "QHeaderView::section { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 %1, stop:1 %2); color: white; padding: 8px; font-weight: 700; border: none; }"
        "QListWidget { background: %6; border: 1px solid %7; border-radius: 8px; color: %4; }"
        "QListWidget::item { color: %4; }"
        "QListWidget::item:selected { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %1, stop:1 %2); color: white; font-weight: 700; }"
        "QLabel { color: %2; }"
        )
        .arg(PRIMARY_COLOR, SECONDARY_COLOR, BG_LIGHT, TEXT_PRIMARY, TEXT_SECONDARY, BG_WHITE, BORDER_COLOR);
    
    setStyleSheet(stylesheet);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(14);
    
    // ===== PROFESSIONAL HEADER =====
    QFrame *headerFrame = new QFrame();
    headerFrame->setMinimumHeight(72);
    headerFrame->setStyleSheet(
        QString("QFrame { "
                "background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %1, stop:1 %2); "
                "border-radius: 16px; border: 2px solid %2; }")
        .arg(PRIMARY_COLOR, SECONDARY_COLOR, ACCENT_COLOR));
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(16, 10, 16, 10);
    headerLayout->setSpacing(12);
    
    // Logo/Icon Area
    QLabel *iconLabel = new QLabel();
    const QPixmap mascotPixmap(":/login_logo.png");
    if (!mascotPixmap.isNull()) {
        iconLabel->setPixmap(mascotPixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        iconLabel->setText("AI");
        iconLabel->setStyleSheet("font-size: 34px;");
    }
    headerLayout->addWidget(iconLabel);
    
    // Title & Description
    QVBoxLayout *titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(2);
    
    QLabel *titleLabel = new QLabel("Labib AI Assistant");
    titleLabel->setStyleSheet("color: white; font-size: 18px; font-weight: 900; letter-spacing: 0.2px;");
    titleLayout->addWidget(titleLabel);
    
    QLabel *subtitleLabel = new QLabel("Assistant Intelligent pour la Gestion Optimale des Dechets");
    subtitleLabel->setStyleSheet("color: #bfdbfe; font-size: 10px; font-weight: 600;");
    titleLayout->addWidget(subtitleLabel);
    
    headerLayout->addLayout(titleLayout, 1);
    
    // Status Badge
    QLabel *statusLabel = new QLabel("EN LIGNE");
    statusLabel->setStyleSheet(QString(
        "color: %1; font-weight: 700; font-size: 10px; "
        "background: rgba(255, 255, 255, 0.2); border-radius: 20px; "
        "padding: 4px 10px;").arg(ACCENT_COLOR));
    headerLayout->addWidget(statusLabel, 0, Qt::AlignRight | Qt::AlignVCenter);
    auto *headerShadow = new QGraphicsDropShadowEffect(this);
    headerShadow->setBlurRadius(18);
    headerShadow->setOffset(0, 4);
    headerShadow->setColor(QColor(15, 43, 76, 52));
    headerFrame->setGraphicsEffect(headerShadow);
    
    mainLayout->addWidget(headerFrame);
    
    // ===== MODULE BAR (TOP) =====
    QFrame *moduleBar = new QFrame();
    moduleBar->setStyleSheet(
        "QFrame { background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #163b6a, stop:1 #0f2b4c); border: 1px solid #2d5f9a; border-radius: 12px; }");
    QVBoxLayout *moduleBarLayout = new QVBoxLayout(moduleBar);
    moduleBarLayout->setContentsMargins(10, 7, 10, 7);
    moduleBarLayout->setSpacing(4);

    QLabel *moduleLbl = new QLabel("Modules");
    moduleLbl->setStyleSheet("font-weight: 700; color: #e2ecff; font-size: 12px;");
    moduleBarLayout->addWidget(moduleLbl);

    m_moduleSelector = new QListWidget();
    m_moduleSelector->addItems({"Clients", "Employes", "Produits", "Maintenance", "Commandes", "Stock"});
    m_moduleSelector->setFlow(QListView::LeftToRight);
    m_moduleSelector->setWrapping(true);
    m_moduleSelector->setResizeMode(QListView::Adjust);
    m_moduleSelector->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_moduleSelector->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_moduleSelector->setFixedHeight(60);
    m_moduleSelector->setCurrentRow(0);
    m_moduleSelector->setStyleSheet(
        "QListWidget { background: rgba(255,255,255,0.08); border: 1px solid rgba(173,205,255,0.35); border-radius: 8px; }"
        "QListWidget::item { padding: 6px 8px; color: #f1f6ff; border-radius: 6px; }"
        "QListWidget::item:selected { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #2a5ea0, stop:1 #1d4b84); color: #ffffff; font-weight: 700; }");
    moduleBarLayout->addWidget(m_moduleSelector);
    auto *moduleShadow = new QGraphicsDropShadowEffect(this);
    moduleShadow->setBlurRadius(18);
    moduleShadow->setOffset(0, 4);
    moduleShadow->setColor(QColor(15, 43, 76, 36));
    moduleBar->setGraphicsEffect(moduleShadow);
    mainLayout->addWidget(moduleBar);

    // ===== MAIN CONTENT LAYOUT =====
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(14);
    
    // --- CENTER PANEL: Chat + Input ---
    QFrame *centerPanel = new QFrame();
    centerPanel->setStyleSheet(QString(
        "QFrame { background: %1; border: 2px solid %2; border-radius: 14px; }")
        .arg(BG_WHITE, BORDER_COLOR));
    QVBoxLayout *centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setContentsMargins(14, 14, 14, 14);
    centerLayout->setSpacing(12);
    
    QLabel *chatLbl = new QLabel("Conversation Intelligente");
    chatLbl->setStyleSheet(QString("font-weight: 700; color: %1; font-size: 13px;").arg(PRIMARY_COLOR));
    centerLayout->addWidget(chatLbl);

    // Smart stats card for proactive assistant insights.
    QFrame *statsFrame = new QFrame();
    statsFrame->setStyleSheet(
        "QFrame { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 rgba(15,43,76,0.05), stop:1 rgba(42,82,152,0.08)); border: 1px solid rgba(42,82,152,0.12); border-radius: 12px; }");
    auto *statsLayout = new QHBoxLayout(statsFrame);
    statsLayout->setContentsMargins(12, 10, 12, 10);
    statsLayout->setSpacing(10);
    m_statsLabel = new QLabel();
    m_statsLabel->setWordWrap(true);
    m_statsLabel->setStyleSheet("color: #24415f; font-size: 11px; font-weight: 600;");
    statsLayout->addWidget(m_statsLabel, 1);
    centerLayout->addWidget(statsFrame);
    auto *statsShadow = new QGraphicsDropShadowEffect(this);
    statsShadow->setBlurRadius(18);
    statsShadow->setOffset(0, 4);
    statsShadow->setColor(QColor(15, 43, 76, 26));
    statsFrame->setGraphicsEffect(statsShadow);

    // Professional quick options to streamline common assistant actions.
    QHBoxLayout *assistantToolsLayout = new QHBoxLayout();
    assistantToolsLayout->setSpacing(4);

    QToolButton *newChatBtn = new QToolButton();
    newChatBtn->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    newChatBtn->setToolTip("Nouveau chat");
    newChatBtn->setAutoRaise(true);

    QToolButton *copyLastBtn = new QToolButton();
    copyLastBtn->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    copyLastBtn->setToolTip("Copier la derniere reponse");
    copyLastBtn->setAutoRaise(true);

    QToolButton *guideBtn = new QToolButton();
    guideBtn->setIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));
    guideBtn->setToolTip("Guide rapide du module");
    guideBtn->setAutoRaise(true);

    assistantToolsLayout->addWidget(newChatBtn);
    assistantToolsLayout->addWidget(copyLastBtn);
    assistantToolsLayout->addWidget(guideBtn);
    assistantToolsLayout->addStretch();
    centerLayout->addLayout(assistantToolsLayout);
    
    m_chatHistory = new QTextEdit();
    m_chatHistory->setReadOnly(true);
    m_chatHistory->setStyleSheet(QString(
        "QTextEdit { background: %1; border: 1px solid %2; border-radius: 10px; padding: 12px; color: %3; }"
        "QScrollBar:vertical { background: transparent; width: 10px; margin: 8px 0; }"
        "QScrollBar::handle:vertical { background: rgba(42,82,152,0.45); border-radius: 5px; min-height: 28px; }"
        "QScrollBar::handle:vertical:hover { background: rgba(42,82,152,0.70); }")
        .arg(BG_LIGHT, BORDER_COLOR, TEXT_PRIMARY));
    centerLayout->addWidget(m_chatHistory);

    m_typingLabel = new QLabel("Labib ecrit...");
    m_typingLabel->setStyleSheet("color: #5f7ea8; font-size: 11px; font-style: italic; padding-left: 2px;");
    m_typingLabel->hide();
    centerLayout->addWidget(m_typingLabel);
    m_typingTimer = new QTimer(this);
    m_typingTimer->setInterval(350);
    connect(m_typingTimer, &QTimer::timeout, this, [this]() {
        m_typingStep = (m_typingStep + 1) % 4;
        m_typingLabel->setText(QString("Labib ecrit%1").arg(QString(m_typingStep, '.')));
    });
    
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(10);
    m_userInput = new QTextEdit();
    m_userInput->setPlaceholderText("Tapez votre question ou demande...");
    m_userInput->setAcceptRichText(false);
    m_userInput->setLineWrapMode(QTextEdit::WidgetWidth);
    m_userInput->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_userInput->setMinimumHeight(58);
    m_userInput->setMaximumHeight(118);
    m_userInput->setStyleSheet(QString(
        "QTextEdit { background: %1; border: 2px solid %2; border-radius: 10px; padding: 9px 12px; "
        "font-size: 13px; color: %3; } QTextEdit:focus { border: 2px solid %4; }").arg(BG_LIGHT, BORDER_COLOR, TEXT_PRIMARY, SECONDARY_COLOR));
    m_userInput->installEventFilter(this);
    
    m_sendButton = new QPushButton();
    m_sendButton->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
    m_sendButton->setToolTip("Envoyer");
    m_sendButton->setMinimumWidth(40);
    m_sendButton->setMaximumWidth(44);
    m_sendButton->setMinimumHeight(38);
    m_sendButton->setStyleSheet(QString(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %1, stop:1 %2); "
        "color: white; border: none; border-radius: 10px; "
        "padding: 6px; font-weight: 700; font-size: 11px; } "
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %2, stop:1 %1); } "
        "QPushButton:pressed { opacity: 0.8; }").arg(SECONDARY_COLOR, PRIMARY_COLOR));
    m_sendButton->setIconSize(QSize(18, 18));
    
    QPushButton *quickImportButton = new QPushButton();
    quickImportButton->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    quickImportButton->setToolTip("Importer");
    quickImportButton->setMinimumWidth(40);
    quickImportButton->setMaximumWidth(44);
    quickImportButton->setMinimumHeight(38);
    quickImportButton->setStyleSheet(QString(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %1, stop:1 %2); "
        "color: white; border: none; border-radius: 10px; "
        "padding: 6px; font-weight: 700; font-size: 11px; } "
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %2, stop:1 %1); }")
        .arg(SUCCESS_COLOR, ACCENT_COLOR));
    quickImportButton->setIconSize(QSize(18, 18));

    QPushButton *quickProcessButton = new QPushButton();
    quickProcessButton->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    quickProcessButton->setToolTip("Traiter");
    quickProcessButton->setMinimumWidth(40);
    quickProcessButton->setMaximumWidth(44);
    quickProcessButton->setMinimumHeight(38);
    quickProcessButton->setEnabled(false);
    quickProcessButton->setStyleSheet(QString(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %1, stop:1 %2); "
        "color: white; border: none; border-radius: 10px; "
        "padding: 6px; font-weight: 700; font-size: 11px; } "
        "QPushButton:disabled { background: #cbd5e1; color: #94a3b8; } "
        "QPushButton:hover:enabled { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %2, stop:1 %1); }")
        .arg(WARNING_COLOR, ACCENT_COLOR));
    quickProcessButton->setIconSize(QSize(18, 18));

    connect(m_userInput, &QTextEdit::textChanged, this, [this]() {
        const int minH = 58;
        const int maxH = 118;
        const int docH = static_cast<int>(m_userInput->document()->size().height()) + 22;
        m_userInput->setFixedHeight(qBound(minH, docH, maxH));
    });

    inputLayout->addWidget(m_userInput, 1);
    inputLayout->addWidget(quickImportButton);
    inputLayout->addWidget(quickProcessButton);
    inputLayout->addWidget(m_sendButton);
    centerLayout->addLayout(inputLayout);

    connect(quickImportButton, &QPushButton::clicked, this, [this, quickProcessButton]() {
        onImportFile();
        quickProcessButton->setEnabled(m_processButton && m_processButton->isEnabled());
    });
    connect(quickProcessButton, &QPushButton::clicked, this, [this, quickProcessButton]() {
        onProcessImportedData();
        quickProcessButton->setEnabled(false);
    });

    connect(newChatBtn, &QToolButton::clicked, this, [this]() {
        stopTypingIndicator();
        m_chatHistory->clear();
        m_chatHistory->setText(
            "<div style='color: #0f2b4c; font-weight: bold; font-size: 15px; margin-bottom: 12px;'>"
            "Nouveau chat Labib</div>"
            "<div style='color: #64748b; font-size: 12px;'>"
            "Posez votre question ou utilisez les actions rapides.</div>");
    });

    connect(copyLastBtn, &QToolButton::clicked, this, [this]() {
        if (!m_lastAssistantHtml.trimmed().isEmpty()) {
            QTextDocument doc;
            doc.setHtml(m_lastAssistantHtml);
            QGuiApplication::clipboard()->setText(doc.toPlainText().trimmed());
        }
    });

    connect(guideBtn, &QToolButton::clicked, this, [this]() {
        const QString module = m_selectedModule.isEmpty() ? QString("Client") : m_selectedModule;
        m_userInput->setPlainText(QString("Donne-moi un guide etapes par etapes pour le module %1").arg(module));
        m_userInput->setFocus();
    });

    // Hidden action buttons kept for existing signal wiring and state propagation.
    m_importButton = new QPushButton(this);
    m_processButton = new QPushButton(this);
    m_importButton->hide();
    m_processButton->hide();
    m_processButton->setEnabled(false);
    
    contentLayout->addWidget(centerPanel, 1);
    m_importPreview = nullptr;
    
    mainLayout->addLayout(contentLayout, 1);
    
    // Professional Welcome Message
    m_chatHistory->setText(
        "<div style='color: #0f2b4c; font-weight: bold; font-size: 15px; margin-bottom: 12px;'>"
        "Bienvenue dans Labib AI Assistant!</div>"
        "<div style='color: #64748b; font-size: 12px; line-height: 1.6;'>"
        "<b>Je suis votre assistant intelligent pour WasteGuard.</b><br><br>"
        "Je peux vous aider avec:<br>"
        "- <b>Questions</b> - Posez n'importe quelle question sur le systeme<br>"
        "- <b>Importation</b> - Importer des fichiers CSV ou Excel<br>"
        "- <b>Operations groupees</b> - Ajouter/modifier des donnees en masse<br>"
        "- <b>Analyse</b> - Analyser et resumer les donnees<br><br>"
        "<i>Commencez par selectionner un module et posez votre question...</i></div>");

    refreshSmartStats();
}

void LabibAssistant::setupConnections()
{
    connect(m_sendButton, &QPushButton::clicked, this, &LabibAssistant::onSendMessage);
    connect(m_importButton, &QPushButton::clicked, this, &LabibAssistant::onImportFile);
    connect(m_processButton, &QPushButton::clicked, this, &LabibAssistant::onProcessImportedData);
    connect(m_moduleSelector, &QListWidget::itemSelectionChanged, this, [this]() {
        if (m_moduleSelector->currentItem()) {
            m_selectedModule = canonicalModuleName(m_moduleSelector->currentItem()->text());
            if (m_selectedModule.isEmpty()) {
                m_selectedModule = "Client";
            }
        }
    });

    if (m_moduleSelector->currentItem()) {
        m_selectedModule = canonicalModuleName(m_moduleSelector->currentItem()->text());
        if (m_selectedModule.isEmpty()) {
            m_selectedModule = "Client";
        }
    }
}

void LabibAssistant::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    m_userInput->setFocus();
}

void LabibAssistant::appendChatBubble(const QString &message, bool fromUser, bool allowHtml)
{
    const QString bubbleColor = fromUser ? "#1d4b84" : "#f7fbff";
    const QString textColor = fromUser ? "#ffffff" : "#10273d";
    const QString borderColor = fromUser ? "rgba(29,75,132,0.18)" : "rgba(42,82,152,0.14)";
    const QString align = fromUser ? "right" : "left";
    const QString content = allowHtml ? message : message.toHtmlEscaped();
    m_chatHistory->append(QString(
        "<div style='text-align:%1; margin: 8px 0;'>"
        "<span style='display:inline-block; max-width:84%%; background:%2; color:%3; padding:11px 13px;"
        " border-radius:16px; border:1px solid %4; box-shadow:0 6px 18px rgba(15,43,76,0.08); line-height:1.45;'>%5</span>"
        "</div>")
        .arg(align, bubbleColor, textColor, borderColor, content));
    scrollChatToBottom();
}

void LabibAssistant::scrollChatToBottom()
{
    if (!m_chatHistory) return;
    QScrollBar *bar = m_chatHistory->verticalScrollBar();
    if (bar) {
        bar->setValue(bar->maximum());
    }
}

void LabibAssistant::refreshSmartStats()
{
    if (!m_statsLabel) return;

    int clients = 0;
    int products = 0;
    int lowStock = 0;

    QSqlQuery q1;
    if (q1.exec("SELECT COUNT(*) FROM CLIENT") && q1.next()) clients = q1.value(0).toInt();
    QSqlQuery q2;
    if (q2.exec("SELECT COUNT(*) FROM PRODUITS") && q2.next()) products = q2.value(0).toInt();
    QSqlQuery q3;
    if (q3.exec("SELECT COUNT(*) FROM BAC_INTEL WHERE NVL(STOCK,0) <= 10 OR NVL(REMPLISSAGE,0) <= 20") && q3.next()) lowStock = q3.value(0).toInt();

    m_statsLabel->setText(QString(
        "<b>%1</b> clients enregistres &nbsp;&nbsp; - &nbsp;&nbsp; <b>%2</b> produits &nbsp;&nbsp; - &nbsp;&nbsp; "
        "<span style='color:#c2410c; font-weight:700;'>%3</span> alertes stock bas")
        .arg(clients).arg(products).arg(lowStock));
}

void LabibAssistant::startTypingIndicator()
{
    if (!m_typingLabel || !m_typingTimer) return;
    m_typingStep = 0;
    m_typingLabel->setText("Labib ecrit...");
    m_typingLabel->show();
    m_typingTimer->start();
    qApp->processEvents();
}

void LabibAssistant::stopTypingIndicator()
{
    if (m_typingTimer) m_typingTimer->stop();
    if (m_typingLabel) m_typingLabel->hide();
}

bool LabibAssistant::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_userInput && event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            if (keyEvent->modifiers() == Qt::NoModifier) {
                onSendMessage();
                return true;
            }

            if (keyEvent->modifiers() == Qt::ShiftModifier) {
                return false;
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

void LabibAssistant::onSendMessage()
{
    QString userMessage = m_userInput->toPlainText().trimmed();
    if (userMessage.isEmpty()) return;
    
    // Display user message
    appendChatBubble(QString("<b>Vous :</b> %1").arg(userMessage.toHtmlEscaped()), true, true);
    
    m_userInput->clear();
    startTypingIndicator();
    
    // Generate AI response
    qApp->processEvents();
    handleAiResponse(userMessage);
}

void LabibAssistant::handleAiResponse(const QString &userMessage)
{
    QString response = generateAiResponse(userMessage);
    displayAiAnswer(response);
}

QString LabibAssistant::generateAiResponse(const QString &userMessage)
{
    const QString normalized = normalizeIntentText(userMessage);

    const QString explicitModuleMention = canonicalModuleName(normalized);
    QString mentionedModule = explicitModuleMention;
    if (mentionedModule.isEmpty()) {
        mentionedModule = m_selectedModule;
    }

    const bool asksDataField = containsAny(normalized, {
        "quel", "quelle", "nom", "email", "matricule", "cin", "prix", "stock", "quantite", "statut", "combien", "liste"
    });

    if (asksDataField || normalized.contains("client") || normalized.contains("produit") || normalized.contains("employe") || normalized.contains("stock")) {
        const QString dataAnswer = answerDataQuestion(userMessage, mentionedModule);
        if (!dataAnswer.isEmpty()) {
            return dataAnswer;
        }
    }

    const bool appScopedQuestion = containsAny(normalized, {
        "wasteguard", "module", "client", "employe", "produit", "maintenance",
        "commande", "stock", "intervention", "table", "base", "donnee",
        "import", "pdf", "csv", "excel", "dashboard", "interface", "formulaire"
    });
    const bool genericAssistantIntent = containsAny(normalized, {
        "aide", "help", "assistant", "comment", "procedure", "explique", "detail"
    });

    const bool hasExplicitModuleInPrompt = !explicitModuleMention.isEmpty();
    if (!appScopedQuestion && !hasExplicitModuleInPrompt) {
        if (genericAssistantIntent) {
            return "Je suis limite a WasteGuard. "
                   "Precise le module (Clients, Employes, Produits, Maintenance, Commandes ou Stock) "
                   "et je t'aiderai pas a pas.";
        }
        return "Je suis l'assistant personnel de l'application WasteGuard uniquement. "
               "Pose-moi une question sur un module, un workflow, une donnee ou une erreur de l'app.";
    }

    QString localAnswer = queryLocalQwenAssistant(userMessage, mentionedModule);
    if (!localAnswer.trimmed().isEmpty()) {
        localAnswer = localAnswer.toHtmlEscaped();
        localAnswer.replace("\n", "<br>");
        return localAnswer;
    }

    return "Qwen local est indisponible pour le moment. "
           "Verifie que le serveur local est actif (ex: Ollama) et que le modele Qwen est installe, "
           "puis reessaie.";
}

void LabibAssistant::displayAiAnswer(const QString &answer)
{
    stopTypingIndicator();
    m_lastAssistantHtml = answer;
    appendChatBubble(QString("<b>Labib :</b> %1").arg(answer), false, true);
    refreshSmartStats();
}

void LabibAssistant::onImportFile()
{
    if (m_moduleSelector && m_moduleSelector->currentItem()) {
        const QString current = canonicalModuleName(m_moduleSelector->currentItem()->text());
        if (!current.isEmpty()) {
            m_selectedModule = current;
        }
    }

    QString filePath = QFileDialog::getOpenFileName(this,
        "Importer Fichier", "",
        "CSV Files (*.csv);;PDF Files (*.pdf);;JSON Files (*.json);;Excel Files (*.xlsx);;All Files (*)");
    
    if (filePath.isEmpty()) return;
    
    m_importedFilePath = filePath;
    m_importedRecords = parseImportedFile(filePath);
    
    if (m_importedRecords.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucune donnee exploitable trouvee. Pour Excel, verifiez que le fichier contient une premiere ligne d'en-tetes.");
        return;
    }
    
    m_processButton->setEnabled(true);
    displayAiAnswer(QString("Fichier importe avec succes! %1 enregistrements trouves. "
                           "Cliquez sur 'Traiter' pour continuer.").arg(m_importedRecords.size()));
    
    showImportPreview(m_importedRecords);
}

QList<QJsonObject> LabibAssistant::parseImportedFile(const QString &filePath)
{
    QList<QJsonObject> records;
    
    if (filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return records;
        }

        const QString text = QString::fromUtf8(file.readAll());
        file.close();
        records = parseDelimitedText(text, ',');
        if (records.isEmpty()) {
            records = parseDelimitedText(text, ';');
        }
    } 
    else if (filePath.endsWith(".json", Qt::CaseInsensitive)) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            return records;
        }
        
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        
        if (doc.isArray()) {
            for (const QJsonValue &val : doc.array()) {
                if (val.isObject()) {
                    records.append(val.toObject());
                }
            }
        }
    }
    else if (filePath.endsWith(".xlsx", Qt::CaseInsensitive) || filePath.endsWith(".xls", Qt::CaseInsensitive)) {
        records = parseExcelByPython(filePath);
        if (records.isEmpty()) {
            records = parseExcelByPowerShell(filePath);
        }
    }
    else if (filePath.endsWith(".pdf", Qt::CaseInsensitive)) {
        QString text = readPdfByPdftotext(filePath);
        if (text.isEmpty()) {
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly)) {
                text = extractPdfTextHeuristicSimple(file.readAll());
                file.close();
            }
        }

        if (!text.isEmpty()) {
            records = extractRecordsFromDocument(text, m_selectedModule);

            if (records.isEmpty()) {
                QJsonObject record;
                const QStringList lines = text.split(QRegularExpression("\\r?\\n"), Qt::SkipEmptyParts);
                for (const QString &line : lines) {
                    const int c = line.indexOf(':');
                    const int e = line.indexOf('=');
                    int sep = -1;
                    if (c >= 0 && e >= 0) sep = qMin(c, e);
                    else sep = qMax(c, e);
                    if (sep > 0) {
                        const QString k = line.left(sep).trimmed();
                        const QString v = line.mid(sep + 1).trimmed();
                        if (!k.isEmpty() && !v.isEmpty()) {
                            record.insert(k, v);
                        }
                    }
                }
                if (!record.isEmpty()) {
                    records.append(record);
                }
            }
        }
    }
    
    return records;
}

void LabibAssistant::showImportPreview(const QList<QJsonObject> &records)
{
    if (!m_importPreview) return;
    m_importPreview->setRowCount(0);
    
    if (records.isEmpty()) return;
    
    const QJsonObject &firstRecord = records.first();
    int row = 0;
    
    for (auto it = firstRecord.begin(); it != firstRecord.end(); ++it) {
        m_importPreview->insertRow(row);
        m_importPreview->setItem(row, 0, new QTableWidgetItem(it.key()));
        m_importPreview->setItem(row, 1, new QTableWidgetItem(it.value().toString()));
        row++;
    }
}

void LabibAssistant::onProcessImportedData()
{
    if (m_moduleSelector && m_moduleSelector->currentItem()) {
        const QString current = canonicalModuleName(m_moduleSelector->currentItem()->text());
        if (!current.isEmpty()) {
            m_selectedModule = current;
        }
    }

    if (m_importedRecords.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucune donnee a traiter.");
        return;
    }
    
    processBulkImport(m_importedRecords);
    m_processButton->setEnabled(false);
}

void LabibAssistant::processBulkImport(const QList<QJsonObject> &records)
{
    int successCount = 0;
    int errorCount = 0;
    QStringList issues;
    
    for (int i = 0; i < records.size(); ++i) {
        const QJsonObject normalizedRecord = normalizeImportedRecordForModule(records[i], m_selectedModule);
        if (validateRecord(normalizedRecord, m_selectedModule)) {
            if (addRecordToDatabase(normalizedRecord, m_selectedModule)) {
                successCount++;
            } else {
                errorCount++;
                issues << QString("Ligne %1: echec insertion en base.").arg(i + 1);
            }
        } else {
            errorCount++;
            issues << QString("Ligne %1: %2").arg(i + 1).arg(validationFailureReason(normalizedRecord, m_selectedModule));
        }
    }
    
    QString result = QString("Traitement termine!<br>"
                            "Succes: %1<br>"
                            "Erreurs: %2<br><br>"
                            "Les donnees ont ete importees dans le module '%3'.")
                    .arg(successCount).arg(errorCount).arg(m_selectedModule);

    if (!issues.isEmpty()) {
        const int maxIssue = qMin(3, issues.size());
        QStringList top;
        for (int i = 0; i < maxIssue; ++i) top << issues[i];
        result += "<br><br><b>Details erreurs:</b><br>- " + top.join("<br>- ");
    }
    
    displayAiAnswer(result);
    m_importedRecords.clear();
}

bool LabibAssistant::validateRecord(const QJsonObject &record, const QString &module)
{
    const QString normalizedModule = canonicalModuleName(module);

    if (normalizedModule == "Client") {
        return record.contains("NOM") || record.contains("MATRICULE");
    } else if (normalizedModule == "Employe") {
        return record.contains("NOM") && record.contains("EMAIL");
    } else if (normalizedModule == "Produit") {
        return record.contains("MODEL") && record.contains("TYPE");
    } else if (normalizedModule == "Maintenance") {
        return record.contains("REFERENCE") || record.contains("TYPE") || record.contains("STATUT");
    }
    
    return !record.isEmpty();
}

bool LabibAssistant::addRecordToDatabase(const QJsonObject &record, const QString &module)
{
    const QString normalizedModule = canonicalModuleName(module);
    QSqlQuery query;
    
    if (normalizedModule == "Client") {
        query.prepare("INSERT INTO CLIENT (ID_CLIENT, NOM, MATRICULE, EMAIL, TYPE_CONTRAT, DATE_EXPIRATION_CONTRAT, STATUT_PAIEMENT) "
                     "VALUES (seq_client.nextval, :nom, :matricule, :email, :type_contrat, TO_DATE(:date_exp, 'YYYY-MM-DD'), :statut)");
        const QString nom = record["NOM"].toString().trimmed().isEmpty() ? QString("Client Auto") : record["NOM"].toString().trimmed();
        const QString matricule = record["MATRICULE"].toString().trimmed().isEmpty() ? autoClientMatricule() : record["MATRICULE"].toString().trimmed();
        query.bindValue(":nom", nom);
        query.bindValue(":matricule", matricule);
        query.bindValue(":email", record["EMAIL"].toString());
        query.bindValue(":type_contrat", record["TYPE_CONTRAT"].toString("Mensuel"));
        query.bindValue(":date_exp", normalizeDateForOracle(record["DATE_EXPIRATION_CONTRAT"].toString()));
        query.bindValue(":statut", record["STATUT_PAIEMENT"].toString("Paye"));
    } 
    else if (normalizedModule == "Employe") {
        query.prepare("INSERT INTO EMPLOYE (ID_EMPLOYE, NOM, EMAIL, CIN, SPECIALITE, SALAIRE, STATUT) "
                     "VALUES (seq_employe.nextval, :nom, :email, :cin, :specialite, :salaire, :statut)");
        query.bindValue(":nom", record["NOM"].toString());
        query.bindValue(":email", record["EMAIL"].toString());
        query.bindValue(":cin", record["CIN"].toString());
        query.bindValue(":specialite", record["SPECIALITE"].toString("General"));
        query.bindValue(":salaire", record["SALAIRE"].toInt(2000));
        query.bindValue(":statut", record["STATUT"].toString("Disponible"));
    }
    else if (normalizedModule == "Produit") {
        query.prepare("INSERT INTO PRODUITS (ID_PRODUIT, MODEL, TYPE, CAPACITE, PRIX_UNITAIRE, QUANTITE, ALLEE) "
                     "VALUES (seq_produit.nextval, :model, :type, :capacite, :prix, :quantite, :allee)");
        query.bindValue(":model", record["MODEL"].toString());
        query.bindValue(":type", record["TYPE"].toString());
        query.bindValue(":capacite", record["CAPACITE"].toString("100"));
        query.bindValue(":prix", record["PRIX_UNITAIRE"].toDouble(0.0));
        query.bindValue(":quantite", record["QUANTITE"].toInt(0));
        query.bindValue(":allee", record["ALLEE"].toString("A"));
    }
    else if (normalizedModule == "Maintenance") {
        int nextId = 1;
        QSqlQuery idQuery;
        if (idQuery.exec("SELECT NVL(MAX(ID_INTER), 0) + 1 FROM INTERVENTION") && idQuery.next()) {
            nextId = idQuery.value(0).toInt();
        }

        query.prepare("INSERT INTO INTERVENTION (ID_INTER, DATE_INTER, REFERENCE, DUREE, COUT, STATUT, TYPE, PRIORITE, ID_BAC) "
                      "VALUES (:id, SYSDATE, :reference, :duree, :cout, :statut, :type, :priorite, :id_bac)");
        query.bindValue(":id", nextId);
        query.bindValue(":reference", record["REFERENCE"].toString("LABIB-AUTO"));
        query.bindValue(":duree", record["DUREE"].toString().toDouble());
        query.bindValue(":cout", record["COUT"].toString().toDouble());
        query.bindValue(":statut", record["STATUT"].toString("Ouverte"));
        query.bindValue(":type", record["TYPE"].toString("Corrective"));
        query.bindValue(":priorite", record["PRIORITE"].toString("Normale"));
        query.bindValue(":id_bac", record["ID_BAC"].toString().toInt());
    }
    else if (normalizedModule == "Stock") {
        int nextBacId = 1;
        QSqlQuery idQuery;
        if (idQuery.exec("SELECT NVL(MAX(ID_BAC), 0) + 1 FROM BAC_INTEL") && idQuery.next()) {
            nextBacId = idQuery.value(0).toInt();
        }

        const QString modele = record["MODELE"].toString().trimmed().isEmpty()
            ? record["MODEL"].toString().trimmed()
            : record["MODELE"].toString().trimmed();

        const QString numSerie = record["NUM_SERIE"].toString().trimmed().isEmpty()
            ? QString("LB-STK-%1").arg(nextBacId)
            : record["NUM_SERIE"].toString().trimmed();

        QString localisation = record["LOCALISATION_STOCK"].toString().trimmed();
        if (localisation.isEmpty()) {
            localisation = QString("Stock principal");
        }

        const QString type = record["TYPE"].toString().trimmed();
        const QString caracteristiques = type.isEmpty() ? QString("Import Labib") : QString("Type: %1").arg(type);

        query.prepare(
            "INSERT INTO BAC_INTEL "
            "(ID_BAC, NUM_SERIE, MODELE, REMPLISSAGE, LOCALISATION_STOCK, ID_COMMANDE, PRIX, STOCK, CAPACITE_BATTERIE, IMAGE_PATH, CARACTERISTIQUES) "
            "VALUES (:id_bac, :num_serie, :modele, :remplissage, :localisation, NULL, :prix, :stock, :cap_batt, :image_path, :caracteristiques)");

        query.bindValue(":id_bac", nextBacId);
        query.bindValue(":num_serie", numSerie);
        query.bindValue(":modele", modele.isEmpty() ? QString("Bac Auto") : modele);
        query.bindValue(":remplissage", record["REMPLISSAGE"].toString().toDouble());
        query.bindValue(":localisation", localisation);
        query.bindValue(":prix", record["PRIX"].toString().toDouble());
        query.bindValue(":stock", record["STOCK"].toString().toInt());
        query.bindValue(":cap_batt", record["CAPACITE_BATTERIE"].toString().toInt());
        query.bindValue(":image_path", record["IMAGE_PATH"].toString());
        query.bindValue(":caracteristiques", caracteristiques);
    }
    
    if (!query.exec()) {
        const QString err = query.lastError().text();
        // Fallback for environments where sequence-based insert may be unavailable.
        if (normalizedModule == "Client") {
            QSqlQuery fallback;
            fallback.prepare(
                "INSERT INTO CLIENT (ID_CLIENT, NOM, MATRICULE, EMAIL, TYPE_CONTRAT, DATE_EXPIRATION_CONTRAT, STATUT_PAIEMENT) "
                "VALUES ((SELECT NVL(MAX(ID_CLIENT),0)+1 FROM CLIENT), :nom, :matricule, :email, :type_contrat, TO_DATE(:date_exp, 'YYYY-MM-DD'), :statut)");
            fallback.bindValue(":nom", query.boundValue(":nom"));
            fallback.bindValue(":matricule", query.boundValue(":matricule"));
            fallback.bindValue(":email", query.boundValue(":email"));
            fallback.bindValue(":type_contrat", query.boundValue(":type_contrat"));
            fallback.bindValue(":date_exp", query.boundValue(":date_exp"));
            fallback.bindValue(":statut", query.boundValue(":statut"));
            if (fallback.exec()) {
                return true;
            }
            qWarning() << "Database insert error (client seq + fallback):" << err << " / " << fallback.lastError().text();
            return false;
        }

        qWarning() << "Database insert error:" << err;
        return false;
    }
    
    return true;
}

bool LabibAssistant::modifyRecordInDatabase(const QJsonObject &record, const QString &module)
{
    (void)record;
    (void)module;
    // Implementation for modify - similar to add but with UPDATE
    return true;
}

QString LabibAssistant::getModuleDescription(const QString &module)
{
    const QString normalizedModule = canonicalModuleName(module);
    if (normalizedModule == "Client") {
        return "Gestion des clients et contrats";
    } else if (normalizedModule == "Employe") {
        return "Gestion des employes";
    } else if (normalizedModule == "Produit") {
        return "Catalogue de produits";
    } else if (normalizedModule == "Maintenance") {
        return "Suivi des interventions";
    } else if (normalizedModule == "Commande") {
        return "Gestion des commandes";
    } else if (normalizedModule == "Stock") {
        return "Gestion du stock";
    }
    return "";
}
