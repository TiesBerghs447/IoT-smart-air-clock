# Smart Air Clock

Een slimme IoT-klok die de luchtkwaliteit van een slaapkamer monitort en gebruikers helpt een gezondere slaapomgeving te creëren.

---

# Probleemstelling

Veel mensen slapen in ruimtes met een slechte luchtkwaliteit zonder dit te beseffen. Een te hoge CO₂-concentratie, een oncomfortabele temperatuur of een onjuiste luchtvochtigheid kunnen een negatieve invloed hebben op de slaapkwaliteit, concentratie en het algemene welzijn. Omdat deze factoren niet zichtbaar zijn, wordt er vaak te laat geventileerd of bijgestuurd.

De Smart Air Clock meet continu de omgevingsfactoren in een slaapkamer en informeert de gebruiker via een OLED-display, RGB-statusled, geluidsmeldingen en een webdashboard. Hierdoor kan de gebruiker tijdig actie ondernemen om een gezondere slaapomgeving te creëren.

---

# Doelgroep

- Studenten
- Gezinnen
- Kantoorwerkers
- Iedereen die zijn slaapkwaliteit wil verbeteren
- Gebruikers van slaapkamers, studentenkamers en appartementen

---

# Onderzoeksvraag

> Hoe kunnen gebruikers eenvoudig geïnformeerd worden over de luchtkwaliteit, temperatuur en luchtvochtigheid van een slaapkamer zodat zij hun slaapomgeving kunnen optimaliseren?

---

# Functionaliteiten

## Lokale functies

- Weergave van tijd op OLED-display
- Meting van temperatuur
- Meting van luchtvochtigheid
- Meting van luchtdruk
- Meting van CO₂-concentratie
- RGB-statusindicator
- Instelbare wekker
- Geluidsmeldingen via luidspreker
- Bediening via drukknoppen

## IoT-functies

- MQTT-communicatie
- Raspberry Pi backend
- Historische opslag van meetgegevens
- Live webdashboard
- Historische trends en grafieken
- Centrale verwerking van sensorgegevens

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
| Raspberry Pi | Backend server |

---

# Software

## ESP32-S3

De ESP32-S3 verzamelt alle sensorgegevens, verwerkt deze lokaal en verstuurt ze via MQTT naar de Raspberry Pi.

## Raspberry Pi

De Raspberry Pi verzorgt:

- MQTT Broker
- Node-RED
- Opslag van meetgegevens
- Hosting van het dashboard

## Webdashboard

Het dashboard wordt ontwikkeld in:

- HTML
- CSS
- JavaScript

Hiermee kunnen gebruikers zowel actuele als historische gegevens bekijken via een webbrowser.

# Aansluitschema

## OLED + AHT20 + BMP280

```text
GPIO20 -> SDA
GPIO21 -> SCL
```

## MH-Z19B

```text
TX -> GPIO16
RX -> GPIO17
```

## DFPlayer Mini

```text
RX -> GPIO18
TX -> GPIO19
```

## RGB LED

```text
R -> GPIO25
G -> GPIO26
B -> GPIO27
```

---

# RGB Statusindicatie

| Kleur | Betekenis |
|---------|------------|
| Groen | Goede luchtkwaliteit |
| Oranje | Matige luchtkwaliteit |
| Rood | Slechte luchtkwaliteit |

## CO₂-niveaus

```text
< 800 ppm       Goede luchtkwaliteit
800-1200 ppm    Matige luchtkwaliteit
> 1200 ppm      Ventileren aanbevolen
```

---

# Dashboard

Het dashboard toont:

- Actuele temperatuur
- Actuele luchtvochtigheid
- Actuele luchtdruk
- Actuele CO₂-waarde
- Historische grafieken
- Trends over tijd
- Status van de luchtkwaliteit
- Laatste meetmoment

Toegang verloopt via een webbrowser op hetzelfde netwerk.

# Auteur

**Ties Berghs**

IoT Project 2026

PXL Hogeschool