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

De backend draait op InfinityFree en bestaat uit:

- PHP
- MySQL
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
MySQL Database
    ↓
latest.php / history.php
    ↓
HTML Dashboard
---

# Aansluitschema

## OLED + AHT20 + BMP280
GPIO20 -> SDA
GPIO21 -> SCL

## MH-Z19B
TX -> GPIO16
RX -> GPIO17

## DFPlayer Mini
RX -> GPIO18
TX -> GPIO19

## RGB LED
R -> GPIO25
G -> GPIO26
B -> GPIO27

# RGB Statusindicatie

| Kleur | Betekenis |
|---------|------------|
| Groen | Goede luchtkwaliteit |
| Oranje | Matige luchtkwaliteit |
| Rood | Slechte luchtkwaliteit |

## CO₂-niveaus
|< 800 ppm       Goede luchtkwaliteit
|800-1200 ppm    Matige luchtkwaliteit
|> 1200 ppm      Slechte luchtkwaliteit

---

# Dashboard

Het dashboard toont:

- Actuele temperatuur
- Actuele luchtvochtigheid
- Actuele luchtdruk
- Actuele CO₂-waarde
- Luchtkwaliteitsstatus
- Historische grafieken
- Trends over tijd
- Laatste meetmoment

---
# Auteur

**Ties Berghs**

IoT Project 2026

PXL Hogeschool