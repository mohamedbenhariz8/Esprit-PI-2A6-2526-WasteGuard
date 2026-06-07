# Liste du matériel (BOM — Bill of Materials)

> Obligatoire pour les projets Embarqué & IoT (guide ESPRIT).
> Remplacez les `?` par les références exactes de vos composants.

| # | Composant | Référence / Modèle | Quantité | Rôle dans WasteGuard |
|---|-----------|--------------------|----------|----------------------|
| 1 | Carte microcontrôleur | ESP32 DevKit v1 (38 pins) | 1 | Cerveau du bac : capteurs, moteurs, OLED, série USB |
| 2 | Caméra | ESP32-CAM (OV2640) | 1 | Capture de l'image du déchet pour classification IA |
| 3 | Moteur pas-à-pas | NEMA 17 (?) | 1 | Carrousel 4 bacs (rotation 90°) |
| 4 | Driver stepper | A4988 / DRV8825 (?) | 1 | Pilotage du moteur pas-à-pas |
| 5 | Servomoteur | MG90S / SG90 (?) | 4 | Ouverture/fermeture du couvercle (GPIO 15,13,12,25) |
| 6 | Lecteur RFID | MFRC522 (RC522) | 1 | Badge technicien (SPI : SS=5, RST=4) |
| 7 | Capteur de distance | HC-SR04 | 4 | Niveau de remplissage par bac (ECHO 34,35,36,39) |
| 8 | Capteur de présence | Module IR obstacle | 1 | Détection d'approche (GPIO 33) |
| 9 | Écran | OLED SSD1306 128x32 I2C | 1 | Visage animé "Labib" (SDA=21, SCL=22) |
| 10 | Bouton poussoir | Push button | 1 | Déclenche le cycle (GPIO 32) |
| 11 | Alimentation | ? (5V / ?A) | 1 | Alimentation servos + stepper + ESP32 |
| 12 | Divers | Câbles Dupont, breadboard/PCB, résistances | — | Câblage |

## Brochage ESP32 (résumé)

| Fonction | GPIO |
|----------|------|
| Stepper STEP / DIR | 14 / 27 |
| Servos couvercle (x4) | 15, 13, 12, 25 |
| RFID SS / RST (SPI SCK18 MISO19 MOSI23) | 5 / 4 |
| Bouton cycle (INPUT_PULLUP) | 32 |
| Capteur IR présence | 33 |
| Ultrason TRIG (partagé) | 2 |
| Ultrason ECHO (x4) | 34, 35, 36, 39 |
| OLED I2C SDA / SCL | 21 / 22 |

> Le brochage de référence se trouve dans
> [updated_arduino_code/updated_arduino_code.ino](../updated_arduino_code/updated_arduino_code.ino).
