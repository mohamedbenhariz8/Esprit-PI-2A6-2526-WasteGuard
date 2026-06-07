# Construire le fichier Fritzing source (schema-cablage.fzz)

> Le schéma de câblage est déjà fourni en **SVG éditable** + **PNG**
> ([schema-cablage.svg](schema-cablage.svg) / [schema-cablage.png](schema-cablage.png)),
> ce qui satisfait l'exigence du guide ESPRIT (« Fritzing, KiCad **ou** photo annotée »).
>
> Si vous voulez en plus le `.fzz` natif, voici la recette exacte (~10 min).
> Un `.fzz` ne peut pas être généré de façon fiable hors de l'application
> Fritzing : il référence la base de pièces interne du logiciel.

## 1. Installer / ouvrir Fritzing
- Télécharger : https://fritzing.org/download/ (ou Microsoft Store).
- Nouveau croquis → onglet **Schematic** (ou Breadboard).

## 2. Poser les composants (volet Parts, à droite)
| Composant à chercher | Quantité |
|----------------------|----------|
| ESP32 (DevKit / DevKitC) — onglet **CORE** ou import depuis « Parts » communautaires | 1 |
| Stepper driver A4988 (ou « stepper motor driver ») | 1 |
| Moteur pas-à-pas (Stepper motor, bipolar) | 1 |
| Servo (Servo motor) | 4 |
| RFID-RC522 (MFRC522) | 1 |
| HC-SR04 (Ultrasonic distance sensor) | 4 |
| Capteur IR obstacle (IR sensor / generic 3-pin) | 1 |
| Bouton poussoir (Pushbutton) | 1 |
| OLED 128x32 I2C (SSD1306) | 1 |
| ESP32-CAM (séparé, juste posé pour la doc) | 1 |
| Alimentation 5V (Power supply / barrel jack) | 1 |

> Si une pièce n'existe pas en CORE, cliquez sur l'icône « importer une pièce »
> ou cherchez-la dans l'onglet communautaire (bouton +). L'ESP32 DevKit et le
> RC522 existent comme pièces communautaires.

## 3. Câbler (clic sur une broche → glisser vers l'autre)

Reproduire **exactement** ce brochage (identique au firmware
[updated_arduino_code.ino](../updated_arduino_code/updated_arduino_code.ino)) :

| ESP32 | → vers | Signal |
|-------|--------|--------|
| G14 | A4988 STEP | stepper |
| G27 | A4988 DIR | stepper |
| G15 | Servo 1 (signal) | couvercle |
| G13 | Servo 2 (signal) | couvercle |
| G12 | Servo 3 (signal) | couvercle |
| G25 | Servo 4 (signal) | couvercle |
| G5  | RC522 SDA/SS | SPI |
| G4  | RC522 RST | SPI |
| G18 | RC522 SCK | SPI |
| G19 | RC522 MISO | SPI |
| G23 | RC522 MOSI | SPI |
| G32 | Bouton (→ GND) | INPUT_PULLUP |
| G33 | IR OUT | présence |
| G2  | TRIG des 4 HC-SR04 (commun) | ultrason |
| G34 | HC-SR04 #0 ECHO | bac 0 |
| G35 | HC-SR04 #1 ECHO | bac 1 |
| G36 | HC-SR04 #2 ECHO | bac 2 |
| G39 | HC-SR04 #3 ECHO | bac 3 |
| G21 | OLED SDA | I2C |
| G22 | OLED SCL | I2C |
| 5V  | servos + driver + capteurs (5V externe) | alim |
| 3V3 | RC522 VCC | alim 3.3V |
| GND | **toutes les masses en commun** | masse |

### ⚠️ Rappels (déjà notés sur le schéma)
- G34/35/36/39 = entrée seule → OK pour ECHO.
- Diviseur de tension sur les ECHO si les HC-SR04 sortent du 5V (ESP32 = 3.3V).
- Servos + stepper alimentés en **5V externe**, pas via le régulateur ESP32.
- RC522 alimenté en **3.3V** (pas 5V).

## 4. Exporter
- **Fichier → Enregistrer sous** → `docs/schema-cablage.fzz`
- (Optionnel) **Fichier → Exporter → en image (PNG)** pour mettre à jour le rendu.
