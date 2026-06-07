#ifndef LABIBCHAT_H
#define LABIBCHAT_H

#include <QString>
#include <QStringList>

// Labib — votre collègue WasteGuard ambitieux et passionné.
// Ces fonctions construisent ses réponses et son message d'accueil.
// Toutes les réponses sont retournées en HTML (compatibles QListWidgetItem en mode RichText).

namespace Labib {

// Message de bienvenue personnel quand on ouvre le tableau de bord.
QString welcomeMessage(const QString &operatorName = QString(),
                       bool isAdmin = true);

// Réponse principale : Labib analyse la question et interroge la base si nécessaire.
QString reply(const QString &userMessage,
              const QString &operatorName = QString(),
              bool isAdmin = true);

// Variante : retourne une chaîne vide si Labib n'a PAS de réponse spécifique
// (utile pour laisser un autre moteur (Qwen, GPT…) prendre le relais).
QString replyIfMatched(const QString &userMessage,
                       const QString &operatorName = QString(),
                       bool isAdmin = true);

// Moteur générique : détecte entité + filtre + agrégation + projection,
// construit une requête SQL en lecture, l'exécute et formate la réponse.
// Retourne une chaîne vide si la question n'est pas interprétable.
QString smartDbAnswer(const QString &userMessage);

// Convertit les marqueurs Markdown courants (**gras**, *italique*, sauts de ligne)
// en HTML léger pour qu'ils s'affichent correctement dans une bulle de chat.
QString markdownToHtml(const QString &raw);

// Liste de suggestions rapides à afficher comme aide contextuelle.
QStringList quickSuggestions();

// Petit nom signé "Labib" + horodatage HH:mm
QString signature();

}

#endif // LABIBCHAT_H
