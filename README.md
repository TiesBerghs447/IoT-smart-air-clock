# Smart Air Clock

Een slimme IoT-klok die de luchtkwaliteit van een slaapkamer monitort en gebruikers helpt een gezondere slaapomgeving te creëren.

## Probleemstelling

Veel mensen slapen in ruimtes met een slechte luchtkwaliteit zonder dit te beseffen. Een te hoge CO₂-concentratie, een oncomfortabele temperatuur of een onjuiste luchtvochtigheid kunnen een negatieve invloed hebben op de slaapkwaliteit. Omdat deze factoren niet zichtbaar zijn, wordt er vaak te laat geventileerd of bijgestuurd.

De Smart Air Clock meet continu de omgevingsfactoren in een slaapkamer en informeert de gebruiker via een display, RGB-led, dashboard en geluidsmeldingen.

## Doelgroep

- Studenten
- Gezinnen
- Kantoorwerkers
- Iedereen die zijn slaapkwaliteit wil verbeteren

## Onderzoeksvraag

Hoe kunnen gebruikers eenvoudig geïnformeerd worden over de luchtkwaliteit, temperatuur en luchtvochtigheid van een slaapkamer zodat zij hun slaapomgeving kunnen optimaliseren?

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

## IoT-functies

- MQTT communicatie
- Raspberry Pi backend
- Historische opslag van meetgegevens
- Dashboard met live gegevens
- Dashboard met historische gegevens

---

# Hardware

| Component | Functie |
|------------|----------|
| ESP32-S3 | Edge device |
| AHT20 | Temperatuur- en vochtigheidsmeting |
| BMP280 | Luchtdrukmeting |
| MH-Z19B | CO₂-sensor |
| OLED SSD1306 | Weergave tijd en metingen |
| RGB LED | Visuele statusindicator |
| DFPlayer Mini (YX5200) | Audio afspelen |
| 2W 8Ω Speaker | Geluidsuitvoer |
| Drukknoppen | Instellen klok en wekker |
| Raspberry Pi | Backend server |

---
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

# Statusweergave RGB LED

| Kleur | Betekenis |
|---------|------------|
| Groen | Goede luchtkwaliteit |
| Oranje | Matige luchtkwaliteit |
| Rood | Slechte luchtkwaliteit |

# Dashboard(nog in overleg)

Het Grafana-dashboard toont:

- Actuele temperatuur
- Actuele luchtvochtigheid
- Actuele luchtdruk
- Actuele CO₂-waarde
- Historische grafieken
- Trends over tijd

---
# Auteur

Ties Berghs

IoT Project 2026

PXL Hogeschool
``