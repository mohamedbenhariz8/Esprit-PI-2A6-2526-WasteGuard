# WasteGuard

> **Système intelligent de gestion des déchets** — application desktop Qt/C++ couplée à une base Oracle, à des capteurs IoT (Arduino + ESP32-CAM), à de l'IA locale (chatbot, vision, voix) et à un serveur de tracking GPS.

---

## Table des matières

- [Vision du projet](#vision-du-projet)
- [Architecture générale](#architecture-générale)
- [Stack technique](#stack-technique)
- [Schéma de base de données](#schéma-de-base-de-données)
- [Modules métier](#modules-métier)
  - [1. Client](#1-client)
  - [2. Employé](#2-employé)
  - [3. Produit (catalogue PRODUITS)](#3-produit-catalogue-produits)
  - [4. Bac intelligent (BAC_INTEL)](#4-bac-intelligent-bac_intel)
  - [5. Stock / Matière première](#5-stock--matière-première)
  - [6. Commande](#6-commande)
  - [7. Intervention / Maintenance](#7-intervention--maintenance)
  - [8. Fabrication](#8-fabrication)
- [Intégrations transversales](#intégrations-transversales)
  - [IA Labib (chatbot DB-aware)](#ia-labib-chatbot-db-aware)
  - [Vision IA (BacDetector + classification déchets)](#vision-ia-bacdetector--classification-déchets)
  - [Reconnaissance & synthèse vocale](#reconnaissance--synthèse-vocale)
  - [Visualiseur 3D (TripoSR)](#visualiseur-3d-triposr)
  - [Génération vidéo produit](#génération-vidéo-produit)
  - [IoT Arduino + ESP32-CAM](#iot-arduino--esp32-cam)
  - [Cartes & tracking livraison](#cartes--tracking-livraison)
  - [Notifications email](#notifications-email)
  - [Thèmes dynamiques](#thèmes-dynamiques)
  - [Accessibilité WCAG](#accessibilité-wcag)
  - [Mascotte Labib (UI vivante)](#mascotte-labib-ui-vivante)
- [Dashboards & statistiques](#dashboards--statistiques)
- [Robustesse & sécurité](#robustesse--sécurité)
- [Structure du dépôt](#structure-du-dépôt)
- [Build & lancement](#build--lancement)

---

## Vision du projet

WasteGuard équipe une entreprise de collecte/gestion de déchets avec :

- une **gestion CRUD complète** sur 8 entités métier (clients, employés, produits, bacs, commandes, interventions, matières premières, fabrication) ;
- un **réseau de bacs intelligents IoT** (Arduino + capteurs + ESP32-CAM) qui classifient automatiquement les déchets et signalent leur état de remplissage ;
- un **collègue numérique conversationnel** baptisé *Labib* qui répond en langage naturel à des questions métier en lisant la base Oracle en direct ;
- un **suivi de livraison GPS temps réel** (Qt Quick + OSM + serveur Python + ngrok) ;
- des **outils internes avancés** : reconstruction 3D de produits depuis une photo, génération vidéo marketing, dictée vocale, notifications email automatiques, gestion fine de l'accessibilité.

---

## Architecture générale

```
┌─────────────────────────────────────────────────────────────────────┐
│                       Application desktop Qt 6                      │
│  ┌──────────────┐  ┌─────────────┐  ┌──────────┐  ┌─────────────┐   │
│  │ Modules CRUD │  │ Dashboards  │  │ Mascotte │  │ Dock Labib  │   │
│  │ (8 entités)  │  │  + Charts   │  │ animée   │  │   (chat)    │   │
│  └──────┬───────┘  └──────┬──────┘  └────┬─────┘  └──────┬──────┘   │
│         │                 │              │               │           │
│         └─────────────────┴──────┬───────┴───────────────┘           │
│                                  │                                   │
│                     ┌────────────┴─────────────┐                     │
│                     │        QtSql / Oracle    │                     │
│                     └────────────┬─────────────┘                     │
└──────────────────────────────────┼───────────────────────────────────┘
                                   │
              ┌────────────────────┼─────────────────────┐
              │                    │                     │
   ┌──────────▼─────────┐  ┌───────▼────────┐  ┌─────────▼──────────┐
   │   Oracle DB        │  │ Python workers │  │ Node.js (vidéo)    │
   │ CLIENT/EMPLOYE/... │  │ classify_serv  │  │ génération marketing│
   └────────────────────┘  │ voice_engine   │  └────────────────────┘
                           │ ai_worker      │
                           │ tracking_serv  │
                           └───────┬────────┘
                                   │
                ┌──────────────────┼────────────────────┐
                │                  │                    │
       ┌────────▼─────┐  ┌─────────▼─────────┐  ┌───────▼──────────┐
       │ Arduino Uno  │  │  ESP32-CAM        │  │ ngrok / OSM      │
       │ + RFID + PIR │  │  (image RGB565)   │  │ tracking public  │
       │ + servos     │  └───────────────────┘  └──────────────────┘
       │ + HC-SR04    │
       │ + OLED       │
       └──────────────┘
```

L'application desktop est l'orchestrateur : elle parle à Oracle pour les données, à des sous-processus Python pour l'IA/voix, à un Node.js pour la vidéo, et au firmware Arduino via QSerialPort.

---

## Stack technique

| Couche | Technologies |
|---|---|
| **UI desktop** | Qt 6.x (Widgets + Quick/QML), QtCharts, QtMultimedia, QtPositioning, QtLocation |
| **Données** | Oracle SQL (driver ODBC), fallback SQLite, QSettings (config locale) |
| **IoT** | Qt SerialPort, Arduino C (Uno + MFRC522 + MG90S/SG90 + HC-SR04 + OLED + PIR + buzzer + stepper), ESP32-CAM |
| **IA texte** | Modèle Qwen local via Ollama (HTTP `/v1/chat/completions`), heuristiques fixes côté Qt (`labibchat.cpp`) |
| **IA vision** | YOLO ultralytics (si dispo) + heuristiques OpenCV + HuggingFace Transformers (`classify_service.py`) |
| **IA voix** | `speech_recognition` (FR-FR) + `pyttsx3`, sélection micro intelligente (évite loopback) |
| **Reconstruction 3D** | TripoSR (script Python externe), viewer OpenGL custom |
| **Vidéo** | Script Node.js externe piloté en QProcess |
| **Maps** | QtLocation + OpenStreetMap, RouteModel pour itinéraires |
| **Tracking GPS** | Serveur HTTP Python (Flask/stdlib + CORS), ngrok pour URL publique |
| **Email** | SMTP (SSL/TLS) via `EmailNotificationManager` |
| **Build** | qmake (`WasteGuard.pro`), Qt Creator |

---

## Schéma de base de données

```sql
CLIENT (ID_CLIENT, NOM, MATRICULE [UNIQUE], EMAIL, TELEPHONE,
        TYPE_CONTRAT, DATE_CREATION_CONTRAT, DATE_EXPIRATION_CONTRAT,
        STATUT_PAIEMENT [CHK A_JOUR|EN_RETARD|SUSPENDU], TAUX_TRI [CHK 0-100])

EMPLOYE (ID_EMP, MATRICULE, CIN [UNIQUE], NOM, EMAIL [UNIQUE], SPECIALITE,
         SALAIRE [CHK >=0], DISPONIBILITE, RFID_BADGE,
         CURRENT_LEAVE_STATUS, TOTAL_LEAVE_DAYS, CURRENT_MISSION_TEXT,
         TOTAL_EXPERIENCE, TOTAL_REWARD,
         LAST_POINTAGE_TYPE, LAST_ENTRY_TIME, TOTAL_HOURS_TODAY,
         TOTAL_HOURS_MONTH, FACE_TEMPLATE [CLOB], PHOTO [BLOB])

PRODUITS (ID_PRODUIT, MODEL, TYPE, CAPACITE, PRIX_UNITAIRE,
          QUANTITE, ALLEE, BARCODE [EAN-13])

MATIERE_PREMIERE (ID_MP, REFERENCE, NOM, QUANTITE [CHK >=0],
                  SEUIL_CRITIQUE [CHK >=0], PRIX [CHK >=0],
                  DATE_FABRICATION, DATE_ACHAT, NOM_FOUR, EMAIL_FOUR)

BAC_INTEL (ID_BAC, NUM_SERIE, MODELE, REMPLISSAGE [CHK 0-100],
           LOCALISATION_STOCK, ID_CLIENT [FK], ID_COMMANDE [FK],
           PRIX, STOCK, CAPACITE_BATTERIE, CARACTERISTIQUES)

COMMANDE (ID_COMMANDE, ID_CLIENT [FK], QTE [CHK >0], PRIORITE
          [CHK FAIBLE|NORMALE|HAUTE|URGENTE],
          STATUT [CHK EN_PREPARATION|LIVREE|INSTALLEE|ANNULEE],
          DATE_COMMANDE, DATE_LIVRAISON, PRIX_TOTAL, ADRESSE,
          START_ADRESSE, PATH_HISTORY)

COMMANDE_BAC (ID_COMMANDE [FK CASCADE], ID_BAC [FK CASCADE], QTE, PRIX_UNITAIRE)

INTERVENTION (ID_INTER, ID_BAC [FK], DATE_INTER, REFERENCE, DUREE, COUT,
              MAIN_OEUVRE, STATUT, TYPE, PRIORITE,
              PHOTO_AVANT, PHOTO_APRES, TECHNICIEN, ADRESSE, DESCRIPTION)

EFFECTUATION (ID_INTER [FK], ID_EMP [FK])     -- équipe multi-techniciens
INTERVENTION_MATIERE (ID_INTER, ID_MP, QTE_UTILISEE)
FABRICATION (ID_MP, ID_BAC, QTE_UTILISE)
```

**Relations clés**
- `COMMANDE.ID_CLIENT → CLIENT.ID_CLIENT`
- `BAC_INTEL.ID_CLIENT → CLIENT.ID_CLIENT`
- `COMMANDE_BAC` lie commandes ↔ bacs (CASCADE)
- `INTERVENTION.ID_BAC → BAC_INTEL.ID_BAC`
- `EFFECTUATION` lie interventions ↔ employés
- `FABRICATION` lie matières premières ↔ bacs produits

---

## Modules métier

### 1. Client

📁 `client.cpp`, `client.h`

**CRUD avancé**
- Matricule **UNIQUE** (contrainte Oracle), validation à l'insertion.
- Recherche multi-champs : matricule, nom, email, type contrat, téléphone, date création.
- Tri sécurisé : whitelist `allowedSortColumns` côté C++ pour bloquer l'injection SQL via les en-têtes de colonnes cliquables.
- `findIdByMatricule()` : lookup bidirectionnel rapide pour les jointures côté UI.

**Logique métier**
- **Taux de tri** (`TAUX_TRI`) : score qualité de tri du client, contraint `CHECK BETWEEN 0 AND 100`.
- **Cycle de vie du contrat** : date de création + date d'expiration + statut paiement (`A_JOUR / EN_RETARD / SUSPENDU`) verrouillés par `CHECK` constraint.
- **Migration de schéma à chaud** : si `TELEPHONE` ou `DATE_CREATION_CONTRAT` manquent, l'app les ajoute via `ALTER TABLE` dynamique au démarrage — pas de migration manuelle requise.

**Intégrations**
- Notifications email auto avant expiration de contrat (J-30, J-7, J-0).
- Lien direct vers les commandes du client et les bacs déployés chez lui.

---

### 2. Employé

📁 `employe.cpp`, `employe.h`, `employeehistorystore.cpp`

**CRUD résilient**
- **Détection de session Oracle morte** (`ORA-03114`, `ORA-01012`) avec **reconnexion automatique** transparente avant retry.
- Photo employé en colonne `BLOB`.
- Contraintes UNIQUE sur `CIN` et `EMAIL`.

**Pointage temps réel**
- `pointById()` orchestre le workflow complet `ENTREE → SORTIE → ENTREE…` avec transitions automatiques.
- Colonnes auto-créées si absentes : `LAST_POINTAGE_TYPE`, `LAST_ENTRY_TIME`, `TOTAL_HOURS_TODAY`, `TOTAL_HOURS_MONTH`.
- Calcul live : `computeTodayHours()` accumule l'historique du jour + la session ouverte en cours.
- `resetDailyAttendance()` : reset nocturne (statut → `INDISPONIBLE`).

**RFID**
- `findIdByRfid()` : lecture badge sans UI (déclenchée par l'Arduino, pas par un clic).
- Stockage badge normalisé (trim + uppercase) pour matcher quel que soit le formatage de la lecture.

**RH avancé**
- **Historique JSON externe** (`employeehistorystore.cpp`) : congés (normal/maladie), missions, récompenses, expérience cumulée.
- Sync bidirectionnelle Oracle ↔ fichier JSON.
- États disponibilité : `DISPONIBLE`, `INDISPONIBLE`, `EN_INTERVENTION`, `CONGE_NORMALE`, `CONGE_MALADIE`, `EN_CONGE`, `EN_CONGE_MALADIE`.
- **Template biométrique** : colonne `FACE_TEMPLATE` (CLOB) prête pour intégration reconnaissance faciale.

**Salaire**
- `SALAIRE` avec `CHECK >=0`, défaut 1200, recalcul en fonction des heures cumulées du mois.

---

### 3. Produit (catalogue PRODUITS)

📁 `produit.cpp`, `produit.h`

**Génération barcode EAN-13 automatique**
- `generateProduitBarcode()` : composé `date | id | checksum` selon norme EAN-13 (12 chiffres + 1 contrôle).

**Sync fabrication ↔ stock**
- `syncFabricationForBac()` / `syncFabricationForBacByNames()` : à chaque modif d'un bac fabriqué, recalcule la consommation matière dans `FABRICATION`.
- `consumeMatiereForFabrication()` / `restoreMatiereForFabrication()` : décrémente / restaure les quantités de matière première en cohérence avec la production.

**Recherche**
- `findIdByReference()` rapide par référence catalogue.
- Filtre/tri sur `MODEL`, `TYPE`, `ALLEE`, `QUANTITE`, `PRIX_UNITAIRE`, `CAPACITE`.

---

### 4. Bac intelligent (BAC_INTEL)

📁 `mainwindow.cpp` (intégration UI), `produit.cpp` (sync), `bacdetector.cpp` (vision IoT)

**Métadonnées physiques**
- `REMPLISSAGE` (0-100%, `CHECK`), mis à jour en direct via le capteur ultrason HC-SR04.
- `LOCALISATION_STOCK` : référence position dans l'entrepôt (3D warehouse view, voir [Cartes](#cartes--tracking-livraison)).
- `CAPACITE_BATTERIE` : tracking IoT pour anticiper les remplacements.
- Lien client (`ID_CLIENT`) + lien commande (`ID_COMMANDE`).

**Workflow IoT**
- 4 compartiments futurs (papier / plastique / métal / verre / général) avec rotation automatique du couvercle.
- États : `EN_ATTENTE`, `EN_SERVICE`, `A_VIDER`, `EN_PANNE`.
- Déclenchement automatique sur détection PIR → ouverture servo → classification image → routage vers le bon compartiment.

---

### 5. Stock / Matière première

📁 `stock.cpp`, `stock.h`

**Seuils critiques**
- `SEUIL_CRITIQUE` configurable par MP, alerte rouge dans le dashboard quand `QUANTITE <= SEUIL_CRITIQUE`.
- `CHECK` constraints sur `QUANTITE >= 0`, `SEUIL_CRITIQUE >= 0`, `PRIX >= 0`.

**Traçabilité**
- **Historique persistant** : chaque mouvement de stock écrit une ligne dans `stock_history.txt` (timestamp + delta + raison).
- Dates `DATE_FABRICATION` et `DATE_ACHAT` pour suivre péremption / âge.

**Fournisseurs dynamiques**
- Lookup tolérant : par ID ou par nom (case-insensitive, prefix match).
- Création auto si fournisseur inconnu : `resolveNextFournisseurId()`.
- `EMAIL_FOUR` persistant pour relance auto.

---

### 6. Commande

📁 `commande.cpp`, `commande.h`

**Workflow d'état**
- Statuts contraints : `EN_PREPARATION`, `LIVREE`, `INSTALLEE`, `ANNULEE`.
- **Transcodage UI ↔ DB** : l'UI affiche `LIVRAISON_EN_COURS` mais persiste `LIVREE` pour respecter `CHK_STATUT`.
- Priorité : `FAIBLE / NORMALE / HAUTE / URGENTE` (`CHECK` constraint).
- `QTE > 0` (`CHECK`), barcode auto sur la référence.

**Tracking livraison**
- Colonnes `START_ADRESSE` (point de départ) + `PATH_HISTORY` (polyline GPS, points séparés par `;`).
- Connexion live à `MapWidget.qml` (Qt Quick + OSM) qui affiche la position GPS en temps réel.
- Le serveur Python `tracking_server.py` reçoit les pings du livreur et expose `/get_location` pour le client web.

**Liaison bacs**
- Table pivot `COMMANDE_BAC` (cascade ON DELETE) : une commande peut contenir plusieurs bacs avec quantités et prix unitaires distincts.

---

### 7. Intervention / Maintenance

📁 `intervention.cpp`, `intervention.h`, `maintenancestatusdelegate.cpp`, `bacdetector.cpp`, `repairdialog.cpp`

**Statuts pilotés par IA vision**
Le `BacDetector` capture **2 photos** (avant + après réparation) et classifie automatiquement → la transition de statut est calculée :

| Avant | Après | Statut résultant | Couleur UI |
|---|---|---|---|
| `MAINTENANCE` | `READY` | **TERMINEE** (réparé) | 🟢 Vert |
| `MAINTENANCE` | `MAINTENANCE` | **ANNULEE** (échec) | 🔴 Rouge |
| `READY` | `MAINTENANCE` | **EN_ATTENTE** (nouveau pb) | 🟠 Orange |
| autres | autres | **EN_COURS** | 🔵 Bleu |

**Coûts décomposés**
- `DUREE` (heures), `COUT` (pièces), `MAIN_OEUVRE` (labour) → calcul automatique du coût total.

**Photos & équipe**
- `PHOTO_AVANT` / `PHOTO_APRES` (chemins locaux) intégrés à l'historique.
- Table `EFFECTUATION` permet plusieurs techniciens par intervention.
- Table `INTERVENTION_MATIERE` consomme automatiquement les matières premières utilisées (décrément du stock).

**Delegate visuel custom**
- `maintenancestatusdelegate.cpp` peint chaque ligne avec une pastille colorée animée selon le statut, plus lisible qu'une simple cellule texte.

---

### 8. Fabrication

📁 Logique répartie dans `produit.cpp`

**Table pivot** `FABRICATION (ID_MP, ID_BAC, QTE_UTILISE)`.

**Synchronisation automatique**
À chaque création/modification d'un bac fabriqué, l'app :
1. recalcule la nomenclature (BOM) à partir des matières premières associées,
2. décrémente `MATIERE_PREMIERE.QUANTITE` du `QTE_UTILISE`,
3. log dans `stock_history.txt`,
4. déclenche une alerte si une MP descend sous son seuil critique.

---

## Intégrations transversales

### IA Labib (chatbot DB-aware)

📁 `labibassistant.cpp`, `labibassistant.h`, `labibchat.cpp`, `labibchat.h`, `labibmediacircle.cpp`

**Pipeline de réponse à 4 niveaux**
1. **`Labib::replyIfMatched`** — règles fixes (salutations, aide, présentation, rapport du jour, alertes stock, comptages directs).
2. **Moteur smart DB générique** (`smartDbAnswerImpl`) — détecte entité + filtre + agrégation + projection en français naturel, construit un SQL en lecture seule, l'exécute et formate la réponse.
3. **`answerSqlQuestionWithQwen`** — délègue à un Qwen local (Ollama) pour générer du SQL sur questions plus complexes, avec validation de sécurité (`isSafeReadOnlySql`) interdisant tout DDL/DML.
4. **Fallback Qwen conversationnel** pour questions hors SQL.

**Détection en français naturel** (`labibchat.cpp`)
- Entités : `client`, `employe`, `produit`, `composant`, `bac`, `commande`, `intervention`, `matiere`.
- Agrégations : `MIN` (« le moins », « moindre », « plus faible »), `MAX` (« le plus », « plus gros »), `AVG`, `SUM`, `COUNT`.
- Filtres métier : `en congé`, `absent`, `en mission`, `en retard de paiement`, `contrat expire bientôt`, `seuil critique`, `bac plein`, etc.
- Projections : email, salaire, taux tri, statut, priorité, prix, etc.

**Exemples de questions traitées sans Qwen**
- *« donne-moi le composant avec le moins de quantité en stock »* → `SELECT MODEL, QUANTITE FROM PRODUITS WHERE QUANTITE IS NOT NULL ORDER BY QUANTITE ASC` (limite 1).
- *« combien d'employés sont en congé ? »* → `SELECT COUNT(*) FROM EMPLOYE WHERE DISPONIBILITE LIKE 'CONGE%'`.
- *« qui est l'employé qui gagne le plus ? »* → `MAX(SALAIRE)` + ramène la ligne complète.
- *« commandes urgentes en cours »* → filtre `PRIORITE IN ('HAUTE','URGENTE') AND STATUT = 'EN_PREPARATION'`.

**Sécurité SQL**
- `isSafeReadOnlySql()` : reject toute requête contenant `INSERT/UPDATE/DELETE/MERGE/DROP/ALTER/CREATE/TRUNCATE/BEGIN/EXECUTE`, plus pas de `;`, ni de commentaires.
- Wrap automatique avec `WHERE ROWNUM <= 50` pour Oracle 11g (pas de `LIMIT`/`FETCH FIRST`).

**Import en masse**
- Bulle « Importer fichier » accepte CSV, JSON, Excel (xlsx), PDF.
- Parse + normalise les clés selon le module sélectionné (Clients, Employés, Produits, Maintenance, Commandes, Stock).
- Preview en table avant validation, puis insertion batch transactionnelle.

**UI vivante**
- Dock latéral sombre type ChatGPT, bulles iMessage (utilisateur vert WasteGuard à droite, Labib gris foncé à gauche).
- Avatar circulaire `LabibMediaCircle` qui joue `download2.mp4` (idle, 1 fois) et bascule sur `download3.mp4` (typing, 1 fois) — l'affichage de la réponse **attend la fin de la vidéo** pour ne jamais la couper (signal `playbackEnded`).

---

### Vision IA (BacDetector + classification déchets)

📁 `bacdetector.cpp`, `bacdetector.h`, `ai_worker.py`, `classify_service.py`, `classify_single.py`

**`bacdetector.cpp` — workflow 2 phases**
- Pipeline asynchrone (`QProcess` + tempfile) : photo `AVANT` → classification → photo `APRES` → classification → décision statut.
- Inférence en cascade : YOLO (ultralytics) si dispo, sinon OpenCV (luminosité, variance, edges), sinon PIL (ratio blanc ≥ 82% → `UNRELATED` pour rejeter les documents).
- Résultats : `READY`, `MAINTENANCE`, `UNRELATED`.

**`classify_service.py` — daemon persistant**
- Pipeline HuggingFace Transformers chargée **une seule fois** en RAM (gain de latence énorme vs script one-shot).
- Lit sur stdin : `CLASSIFY <path>`, `URL <esp32_cam_url>`, `QUIT`.
- Convertit le RGB565 brut de l'ESP32-CAM (160×120) → PIL → resize 224×224 → modèle.
- Mapping labels modèle → 4 bins métier : `PLASTIC` / `METAL` / `GLASS` / `GENERAL_WASTE`.

---

### Reconnaissance & synthèse vocale

📁 `voiceassistant.cpp`, `voiceassistant.h`, `voice_engine.py`

**Reconnaissance** (`speech_recognition`)
- Langue FR-FR, timeout 10 s, phrase limit 15 s.
- **Sélection micro intelligente** : `_is_loopback_like()` filtre les périphériques « Stereo Mix » / loopback qui captureraient le son du PC au lieu du micro.
- Override possible via variable d'environnement `WG_VOICE_MIC_DEVICE`.

**Synthèse** (`pyttsx3`)
- Engine init mis en cache (instanciation lente).
- Lecture des réponses Labib à voix haute si activé.

**Wrapper Qt**
- `voiceassistant.cpp` lance `voice_engine.py` via `QProcess` + `QLocalSocket` pour la communication bidirectionnelle (commandes / résultats).
- Cas d'usage : dicter une commande client, demander une intervention, interroger Labib oralement.

---

### Visualiseur 3D (TripoSR)

📁 `triposr3ddialog.cpp`, `viewer3ddialog.cpp`, `modelviewerwidget.cpp`

**TripoSR (image 2D → mesh 3D)**
- Détection automatique du chemin TripoSR (`C:/Users/*/Desktop/triposr/TripoSR`).
- Lance le script Python TripoSR via `QProcess`, suit la progression, parse la sortie.
- Génère un `.obj` chargé directement dans le viewer OpenGL intégré.
- UI : barre de progression live, console log colorée, `QElapsedTimer` pour mesurer la durée.

**Viewer OpenGL custom**
- `modelviewerwidget.cpp` : rendu OBJ, rotation à la souris, zoom à la molette, lighting basique.
- Sert aussi à pré-visualiser un produit du catalogue PRODUITS avant commande.

---

### Génération vidéo produit

📁 `videogenerationdialog.cpp`, `videogenerationdialog.h`

- Récupère depuis le catalogue PRODUITS : nom, référence, capacité, batterie, features (liste libre), image principale.
- Lance un script **Node.js** externe (chemin via env `ELI_VIDEO_SCRIPT` ou fallback intégré) qui assemble une vidéo produit (animation armoire / livraison) au format MP4.
- UI : barre de progression live, log streaming, mode « overlay » pour automation (sans interaction utilisateur).

---

### IoT Arduino + ESP32-CAM

📁 `arduino.cpp`, `arduino.h`, `arduinowidget.cpp`, `bacdetector.cpp`, `updated_arduino_code/updated_arduino_code.ino`

**Firmware Arduino Uno** (`updated_arduino_code.ino`)

| Composant | Pin | Rôle |
|---|---|---|
| MFRC522 (RFID) | SS=10 | Lecture badge employé |
| Servo MG90S × 2 | D2, D6 | Couvercles bac |
| Servo SG90 | D5 | Servo auxiliaire |
| HC-SR04 | TRIG=D3, ECHO=D4 | Mesure profondeur (remplissage) |
| Stepper + DIR | D7, D8 | Rotation des compartiments |
| PIR | A0 | Détection mouvement (réveil bac) |
| Buzzer actif | D9 | Alerte sonore |
| OLED I2C | A4 (SDA), A5 (SCL) | UI mascotte « Labib » (yeux clignotants, sourire, cœur) |

**Cycle autonome**
1. PIR détecte un mouvement → bac sort de veille.
2. App envoie `CLASSIFY` sur le port série.
3. Arduino ouvre le couvercle (servo) → attend 1 s → ferme → mesure la profondeur (HC-SR04).
4. App lit l'image ESP32-CAM, classifie via `classify_service.py`, choisit le compartiment.
5. Stepper fait tourner le bac sur le bon compartiment, dépose le déchet.
6. Mise à jour `BAC_INTEL.REMPLISSAGE` en base.

**Wrapper Qt**
- `QSerialPort` @ 9600 baud.
- Auto-détection Uno (USB VID=9025, PID=67).
- `connect_arduino_excluding()` permet de discriminer un 2e Arduino (contrôleur servos séparé).

---

### Cartes & tracking livraison

📁 `stockmapwidget.cpp`, `assets/MapWidget.qml`, `tracking_server.py`, `tracking.html`

**Vue 3D entrepôt** (`stockmapwidget.cpp`)
- Bi-mode : First-Person (immersif) ou Top-Down isométrique.
- Grille 6 colonnes (A-F) × 5 rangées × 3 niveaux = **90 emplacements adressables**.
- Particules + glow animés (`QPropertyAnimation`).
- Click sur emplacement → coordonnée capturée (utile pour assigner un bac à un slot).
- HUD + minimap avec mascotte Labib en repère positionnel.

**Carte livraison** (`MapWidget.qml`)
- Qt Quick + QtLocation + tuiles **OpenStreetMap** (pas de clé API).
- `PositionSource` (QtPositioning) pour le GPS live.
- `RouteModel` calcule l'itinéraire optimal départ → adresse de livraison.
- Polyline historique reconstituée depuis `COMMANDE.PATH_HISTORY` (séparateur `;`).

**Serveur Python** (`tracking_server.py`)
- HTTP sur port 5050, CORS activé.
- `POST /update_location` `{lat, lng}` : le mobile du livreur ping sa position.
- `GET /get_location` : le client web (`tracking.html`) la récupère.
- Persiste dans `delivery_tracking.json`.
- **ngrok** : URL publique stockée dans `ngrok_url.txt` pour partager le lien au client final sans ouvrir de port.

---

### Notifications email

📁 `emailnotificationmanager.cpp`, `emailnotificationmanager.h`

**Déclencheurs**
- Expiration contrat client (J-30, J-7, J-0).
- Confirmation création de commande au client.
- Relance impayé / facture.
- Alerte interne stock critique → email fournisseur (utilise `MATIERE_PREMIERE.EMAIL_FOUR`).

**Configuration**
- SMTP serveur, port, sender, mot de passe (stockés en `QSettings`).
- Support SSL/TLS.
- Templates HTML générés dynamiquement avec branding WasteGuard.

---

### Thèmes dynamiques

📁 `thememanager.cpp`, `thememanager.h`

**Modes**
- **Default** (clair, palette métier).
- **Dark** (sombre, contraste élevé).
- **Custom** : palette éditable rôle par rôle (bgApp, bgCard, border, text, primary, danger, accent, sidebar…).

**Persistence** : `QSettings` → `config.ini`, appliquée à tout l'arbre de widgets via stylesheet généré dynamiquement.

---

### Accessibilité WCAG

📁 `accessibilityhelper.cpp`, `mainwindow_accessibility_impl.cpp`

- **Contraste élevé** : force palette sombre + bordures épaissies sur tous les widgets.
- **Zoom texte** 50 % – 200 % via `setTextZoom()` (rebuild stylesheet).
- **Navigation clavier** : tab order custom, raccourcis flèches dans les tableaux.
- **Noms accessibles** (`setAccessibleName/Description`) sur tous les widgets interactifs.
- **Persistence** des préférences (contraste, zoom, position fenêtre) entre sessions.

---

### Mascotte Labib (UI vivante)

📁 `mascotwidget.cpp`, `labibmediacircle.cpp`, `floatingaiwidget.cpp`

**`MascotWidget`** — mascotte vectorielle (pas de bitmap)
- Dessinée avec `QPainter` + gradients : entièrement scalable.
- Animations : clignements, regard qui suit, expressions (idle / loupe / bloc-notes).
- Eye cover factor 0-1 pour blink.

**`LabibMediaCircle`** — avatar vidéo circulaire
- Lecteur vidéo (`QMediaPlayer`) clipé en cercle avec liseré blanc + halo vert.
- API : `tryLoadAny(paths)` pour fallback multi-fichiers, `setLoopCount(int)` pour 1-shot ou infini, signal `playbackEnded` pour synchroniser avec le chat.
- Utilisé dans le header du dock Labib (mp4 idle/typing) et dans le bouton flottant global.

**`FloatingAIWidget`** — bouton flottant draggable
- Mascotte 78×78 toujours visible, draggable n'importe où.
- Click → ouvre le dock Labib.
- Contient sa propre fenêtre de chat alternative (Mistral 7B via API HuggingFace) si le dock principal n'est pas dispo.

---

## Dashboards & statistiques

📁 `mainwindow.cpp`, `mainwindow_setupStatistics.cpp`

- **KPIs temps réel** affichés en haut du dashboard : clients, employés, bacs, commandes, alertes stock, interventions en cours.
- **QtCharts** (barres + camemberts) pour :
  - Effectif par disponibilité (Disponibles / En mission / Absents).
  - Commandes par priorité.
  - Stock par seuil critique.
  - Taux de paiement clients.
  - Intervention par statut.
- Refresh à chaque ouverture de page + sur événement IoT (changement remplissage bac, nouvelle commande, etc.).

---

## Robustesse & sécurité

- **Reconnexion Oracle automatique** sur session morte (employe.cpp pattern réplicable).
- **Migration de schéma à chaud** : ajout de colonnes via `ALTER TABLE` au démarrage si manquantes (CLIENT.TELEPHONE, EMPLOYE.SALAIRE, etc.).
- **Validation SQL injection** : whitelists pour les colonnes/sort dynamiques, requêtes paramétrées (`bindValue`) ailleurs.
- **Tri & limite Oracle 11g** : wrap automatique `WHERE ROWNUM <= N` (pas de `LIMIT` natif).
- **Asynchronisme** : `QProcess` pour Python/Node, `QNetworkAccessManager` pour HTTP, `QSerialPort` pour Arduino → l'UI ne bloque jamais.
- **Persistance multi-canal** : Oracle pour le métier, JSON pour l'historique RH (`employeehistorystore`), txt pour l'historique stock, `QSettings` pour la config UI.
- **Gestion d'erreurs métier** : chaque classe expose `lastError()` + propagation en messageBox côté UI.

---

## Structure du dépôt

```
Esprit-PI-2A6-2526-WasteGuard/
├── README.md
├── .gitignore
├── requirements.txt                  # Dépendances Python (versions fixées)
├── .env.example                      # Modèle de configuration
├── docs/                             # Documentation (schémas, guides, SQL)
├── demo/                             # Démonstration (vidéo + captures)
├── firmware/
│   └── updated_arduino_code.ino      # Firmware Arduino Uno / ESP32
└── src/                              # Code Qt / C++ + workers Python
    ├── WasteGuard.pro                # Projet qmake
    ├── main.cpp                      # Login + bootstrap mascotte
    ├── mainwindow.{cpp,h,ui}         # Orchestrateur principal (~26k lignes)
    ├── mainwindow_setupStatistics.cpp    # Charts dashboard
    ├── mainwindow_accessibility_impl.cpp # Hooks WCAG
│
│ ── Modules métier ──
├── client.{cpp,h}
├── employe.{cpp,h}
├── employeehistorystore.{cpp,h}      # JSON RH
├── produit.{cpp,h}
├── stock.{cpp,h}
├── commande.{cpp,h}
├── intervention.{cpp,h}
├── connection.{cpp,h}                # Oracle singleton
│
│ ── IA & assistant ──
├── labibassistant.{cpp,h}            # Dock chat
├── labibchat.{cpp,h}                 # Moteur smart DB
├── labibmediacircle.{cpp,h}          # Avatar vidéo
├── voiceassistant.{cpp,h}
├── floatingaiwidget.{cpp,h}
├── mascotwidget.{cpp,h}
│
│ ── IoT & vision ──
├── arduino.{cpp,h}
├── arduinowidget.{cpp,h}
├── bacdetector.{cpp,h}
├── bacstatusdialog.{cpp,h}
├── repairdialog.{cpp,h}
│   (firmware Arduino/ESP32 -> ../firmware/updated_arduino_code.ino)
│
│ ── 3D & vidéo ──
├── triposr3ddialog.{cpp,h}
├── viewer3ddialog.{cpp,h}
├── modelviewerwidget.{cpp,h}
├── videogenerationdialog.{cpp,h}
│
│ ── Maps & tracking ──
├── stockmapwidget.{cpp,h}            # Vue 3D entrepôt
├── assets/MapWidget.qml              # Carte livraison
├── tracking_server.py                # Serveur GPS
├── tracking.html                     # Client web
├── ngrok_url.txt
├── delivery_tracking.json
│
│ ── Misc ──
├── thememanager.{cpp,h}
├── accessibilityhelper.{cpp,h}
├── emailnotificationmanager.{cpp,h}
├── maintenancestatusdelegate.{cpp,h}
│
│ ── Médias ──
├── download.mp4                      # Mascotte flottante
├── download2.mp4                     # Avatar idle
└── download3.mp4                     # Avatar typing
```

---

## Build & lancement

**Pré-requis**
- **OS testé** : Windows 11 (build principal). Compilable sous Linux (Qt 6 + ODBC).
- **Compilateur** : MSVC 2022 (Windows) ou g++ 11+ / MinGW. **Standard C++17**.
- Qt 6.x (Widgets, Quick, Sql, SerialPort, Network, Multimedia, Location, Positioning, Charts).
- Driver ODBC Oracle (Instant Client) installé et configuré sur la machine.
- **Python 3.11** — `pip install -r requirements.txt` (voir [requirements.txt](requirements.txt)).
- Node.js (uniquement pour la génération vidéo).
- Optionnel : Ollama avec modèle Qwen 2.5 chargé (`ollama pull qwen2.5:7b-instruct`) pour l'IA Labib avancée.
- Optionnel : ngrok pour exposer le tracking GPS.

**Configuration (avant le premier lancement)**

Copiez [.env.example](.env.example) en `.env` et renseignez vos valeurs (identifiants
Oracle, ports série, clés API…). Les identifiants de base de données sont lus depuis
les variables d'environnement `WASTEGUARD_DB_NAME` / `WASTEGUARD_DB_USER` /
`WASTEGUARD_DB_PASSWORD` — **aucun mot de passe n'est stocké dans le code**.

**Base de données**

Le projet s'appuie sur une instance **Oracle** (schéma `WASTEGUARD`). Renseignez les
identifiants via les variables d'environnement ci-dessus.

Au démarrage, l'application applique des **migrations de schéma à chaud**
(ajout de colonnes manquantes via `ALTER TABLE`) — aucune action manuelle requise.

**Matériel IoT** : la liste complète des composants et le brochage ESP32 sont dans
[docs/liste-materiel.md](docs/liste-materiel.md). Le firmware est dans
[firmware/](firmware/) (téléverser via l'IDE Arduino / `arduino-cli`).

**Modèle IA (vision déchets)** : le modèle YOLO entraîné n'est **pas** versionné
(règle GitHub 100 Mo). Renseignez `WASTE_MODEL_URL` dans `.env` ou téléchargez-le
depuis : [Hugging Face Repository](https://huggingface.co/kendrickfff/waste-classification-yolov8-ken).

## Démo

> 📹 Démonstration vidéo (obligatoire pour un projet IoT) — voir le dossier
> [demo/](demo/). Lien en ligne : [Démonstration YouTube](https://youtu.be/IlRb5DY20Wg).

**Variables d'environnement utiles**

| Variable | Rôle |
|---|---|
| `WASTEGUARD_LOCAL_QWEN_URL` | Endpoint Ollama (défaut `http://127.0.0.1:11434`) |
| `WASTEGUARD_LOCAL_QWEN_MODEL` | Modèle (défaut détection auto, sinon `qwen2.5:7b-instruct`) |
| `WG_VOICE_MIC_DEVICE` | Micro à utiliser pour la voix (force le device si plusieurs) |
| `ELI_VIDEO_SCRIPT` | Chemin du script Node.js de génération vidéo |
| `OLLAMA_HOST` / `OLLAMA_MODEL` | Fallback si les vars `WASTEGUARD_*` ne sont pas définies |

**Compilation**

```bash
cd src
qmake WasteGuard.pro
make            # Linux / mingw32-make sur Windows
```

ou ouvrir `src/WasteGuard.pro` dans **Qt Creator** et builder en **Release**.

**Lancement**

L'exe se trouve dans `release/` ou `build-WasteGuard-*/release/`. Au premier lancement :
1. Connexion Oracle vérifiée + migrations de schéma à chaud.
2. Login (mascotte Labib animée).
3. Tableau de bord principal + dock Labib accessible via le bouton flottant.

---

## Auteurs

Ce projet a été réalisé par les étudiants de la classe **2A6** (Année universitaire 2025-2026) sous la supervision de l'enseignante **Soumaya Nheri** :

* **Gaied Rayen** — 2A6
* **Ben Hariz Mohamed** — 2A6
* **Mokhtar Ahmed** — 2A6
* **Elomar Roudi** — 2A6
* **Ghassen Tounsi** — 2A6
* **Bakini Ahmed** — 2A6

Projet pédagogique WasteGuard — gestion intelligente des déchets.

