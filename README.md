# Smart Air Clock

Een slimme IoT-klok die de luchtkwaliteit van een slaapkamer monitort en gebruikers helpt een gezondere slaapomgeving te creëren.

---

# Probleemstelling

Veel mensen slapen in ruimtes met een slechte luchtkwaliteit zonder dit te beseffen. Een verhoogde CO₂-concentratie, een oncomfortabele temperatuur of een onjuiste luchtvochtigheid kunnen een negatieve invloed hebben op de slaapkwaliteit. Omdat deze factoren niet zichtbaar zijn, wordt er vaak te laat geventileerd of bijgestuurd.

De Smart Air Clock meet continu de omgevingsfactoren in een slaapkamer en informeert de gebruiker via een OLED-display, RGB-led, geluidsmeldingen en een online dashboard.

---

# Doelgroep

- Studenten
- Gezinnen
- Kantoorwerkers
- Iedereen die zijn slaapkwaliteit wil verbeteren
- Gebruikers van slaapkamers, studentenkamers en appartementen

---

# Onderzoeksvraag

Hoe kan een slim IoT-systeem gebruikers op een eenvoudige en overzichtelijke manier informeren over de luchtkwaliteit van hun slaapkamer, zodat zij hun slaapomgeving kunnen optimaliseren en tijdig kunnen ingrijpen wanneer de luchtkwaliteit verslechtert?

---

# Functionaliteiten

## Lokale functies

- Weergave van tijd op OLED-display
- Temperatuurmeting
- Vochtigheidsmeting
- Luchtdrukmeting
- CO₂-meting
- RGB-statusindicator
- Instelbare wekker
- Geluidsmeldingen via luidspreker
- Bediening via drukknoppen

## IoT-functies

- WiFi-connectiviteit
- HTTP-communicatie
- Online databank
- Historische opslag van meetgegevens
- Live webdashboard
- Historische grafieken en trends

---

# Hardware

| Component | Functie |
|------------|----------|
| ESP32-S3 | Edge device |
| AHT20 | Temperatuur- en vochtigheidsmeting |
| BMP280 | Luchtdrukmeting |
| MH-Z19B | CO₂-sensor |
| OLED SSD1306 | Weergave van tijd en meetwaarden |
| RGB LED | Visuele statusindicator |
| DFPlayer Mini (YX5200) | Audio afspelen |
| 2W 8Ω Speaker | Geluidsuitvoer |
| Drukknoppen | Instellen klok en wekker |

---

# Software

## ESP32-S3

De ESP32-S3 verzamelt sensorgegevens, verwerkt deze lokaal en verstuurt de gegevens via HTTP naar een online database.

## Backend

De backend draait op Render en bestaat uit:

- PHP
- PostgreSQL
- HTTP API

De backend ontvangt meetgegevens, slaat deze op en maakt ze beschikbaar voor het dashboard.

## Dashboard

Het dashboard wordt ontwikkeld in:

- HTML
- CSS
- JavaScript
- Chart.js

Hiermee kunnen gebruikers actuele en historische gegevens bekijken via een webbrowser.

---

# Systeemarchitectuur
Sensoren
    ↓
ESP32-S3
    ↓ HTTP
save_data.php
    ↓
PostgreSQL Database
    ↓
latest.php / history.php
    ↓
HTML Dashboard
---

# Aansluitschema

De onderstaande GPIO-nummers zijn overgenomen uit de firmware in
`software/smart-air-clock/`. Bij de ESP32-S3 zijn de GPIO-pinnen 3,3V-signalen;
sluit geen 5V-signaal rechtstreeks op een GPIO aan.

## I²C-bus: OLED, AHT20 en BMP280

De drie modules delen dezelfde I²C-bus. Verbind SDA met SDA en SCL met SCL op
elke module.

| ESP32-S3 | OLED SSD1306 | AHT20 | BMP280 | Functie / firmware-instelling |
|----------|--------------|-------|--------|-------------------------------|
| GPIO 8   | SDA          | SDA   | SDA    | I²C-data |
| GPIO 9   | SCL          | SCL   | SCL    | I²C-klok |
| GND      | GND          | GND   | GND    | Gemeenschappelijke massa |
| Voeding volgens module | VCC | VCC | VCC | 3.3V |

| Module | I²C-adres in firmware |
|--------|-----------------------|
| OLED SSD1306 | `0x3C` |
| BMP280 | `0x77` |
| AHT20 | Adres wordt door de AHT20-bibliotheek afgehandeld |

## Seriële verbindingen (UART)

De RX- en TX-signalen worden gekruist aangesloten: TX van een module gaat naar
RX van de ESP32-S3, en RX van de module naar TX van de ESP32-S3.

| Module | Modulepin | ESP32-S3-pin | Instelling in firmware |
|--------|-----------|--------------|-------------------------|
| MH-Z19B | TX | GPIO 16 (RX) | UART2, 9600 baud |
| MH-Z19B | RX | GPIO 15 (TX) | UART2, 9600 baud |
| DFPlayer Mini | TX | GPIO 18 (RX) | UART1, 9600 baud |
| DFPlayer Mini | RX | GPIO 17 (TX) | UART1, 9600 baud |
| Beide modules | GND | GND | Gemeenschappelijke massa |
| MH-Z19B | Voeding | 5V |
| DFPlayer Mini | VCC | 5V |
| Luidspreker (2 W, 8 Ω) | Aansluitingen | DFPlayer Mini `SPK1` en `SPK2` | Gebruik de luidsprekeruitgang van de DFPlayer, niet een ESP32-GPIO |

Controleer vóór aansluiting of de UART-signaalniveaus compatibel zijn met de
3,3V-GPIO's van de ESP32-S3. Gebruik zo nodig een geschikte level shifter.

## Drukknoppen

De firmware configureert de knoppen als `INPUT_PULLUP`. Sluit daarom elke knop
tussen de toegewezen GPIO en GND aan; een ingedrukte knop wordt als LOW gelezen.

| Knop | ESP32-S3-pin | Andere aansluiting | Functie |
|------|--------------|--------------------|---------|
| POWER | GPIO 4 | GND | Aan/uit- en menuactie |
| SET | GPIO 5 | GND | Instellen en bevestigen |
| UP | GPIO 6 | GND | Waarde verhogen / navigeren |

## RGB-led

| Ledkanaal | ESP32-S3-pin | Aansluiting |
|-----------|--------------|-------------|
| Rood | GPIO 40 | Via serieweerstand naar het rode kanaal |
| Groen | GPIO 41 | Via serieweerstand naar het groene kanaal |
| Blauw | GPIO 42 | Via serieweerstand naar het blauwe kanaal |
| Gemeenschappelijke pin | — | Aansluiten volgens het type RGB-led (common anode of common cathode) |

# RGB Statusindicatie

| Kleur | Betekenis |
|---------|------------|
| Groen | Goede luchtkwaliteit |
| Geel | Matige luchtkwaliteit |
| Rood | Slechte luchtkwaliteit |

## CO₂-niveaus (standaardwaarden)

| CO₂-concentratie | Kleur | Betekenis |
|------------------|-------|-----------|
| Minder dan 800 ppm | Groen | Goede luchtkwaliteit |
| 800 tot minder dan 1200 ppm | Geel | Matige luchtkwaliteit |
| 1200 ppm of hoger | Rood | Slechte luchtkwaliteit |

De grenswaarden zijn standaard 800 en 1200 ppm en kunnen via de configuratie
worden aangepast.

---

# Dashboard

Het dashboard toont:

- Actuele temperatuur
- Actuele luchtvochtigheid
- Actuele luchtdruk
- Actuele CO₂-waarde
- Luchtkwaliteitsstatus
- Historische grafieken van maximaal de laatste 50 metingen
- Trends over tijd
- Laatste meetmoment
- Online/offline-status van de ESP32

---
# Auteur

**Ties Berghs**

IoT Project 2026

PXL Hogeschool