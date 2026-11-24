<!-- Documentatie voor de ESP API en voor het blijvend opslaan van instellingen op Render. -->

# ESP32 API

Configureer op de PHP-host een omgevingsvariabele `ESP_API_KEY` met minimaal 32 willekeurige tekens. De sleutel staat bewust niet in de broncode. De API weigert requests zolang geen geldige sleutel is ingesteld. Gebruik dezelfde sleutel in de firmware en stuur hem mee als `X-API-Key`; verstuur requests uitsluitend over HTTPS. De browserconfiguratie geeft de sleutel niet terug.

## Website-login

De website gebruikt de Render-omgevingsvariabelen `user` en `passcode` voor de loginpagina. Omgevingsvariabelen zijn hoofdlettergevoelig; gebruik exact deze namen. Als een waarde ontbreekt, weigert de login en toont de site een configuratiefout. De dashboard-, alarm-, historie- en instellingenpagina's en de bijbehorende browser-API's vereisen een ingelogde sessie. De ESP32-endpoints blijven hun eigen `ESP_API_KEY`-controle gebruiken.

## Metingen verzenden

`POST /data/save_data.php` verwacht `application/x-www-form-urlencoded` met `temperature`, `humidity`, `pressure`, `co2`, `light` (geheel getal van 0 t/m 4095) en `presence` (0 of 1). Succes geeft HTTP 200 met `{"status":"ok"}`. Optioneel: `timestamp` (Unix-tijd) voor metingen uit de offline buffer; max. 7 dagen oud en niet in de toekomst. Ongeldige waarden geven HTTP 400; een ongeldige sleutel geeft HTTP 401.

## Alarm opslaan vanaf het toestel

`POST /settings/save_alarm.php` met `hour`, `minute`, `enabled` (0/1), `soundID` en optioneel `id`. Antwoord: `{"id": n}`. Met `id` wordt het alarm bijgewerkt; zonder `id` wordt een bestaand alarm op hetzelfde tijdstip bijgewerkt of een nieuw aangemaakt. HTTP 409 = al 5 alarmen.

## LED-commando ophalen

`GET /data/get_commando.php` vereist ook `X-API-Key`.

De tabel `measurements` moet de kolom `light` bevatten. De bestaande kolom `presence` kan als `integer`/`smallint` blijven staan. Voer eenmalig uit op de PostgreSQL-database als `light` nog ontbreekt:

```sql
ALTER TABLE measurements ADD COLUMN IF NOT EXISTS light integer;
```

Oude meetrijen blijven voor `light` leeg (`NULL`); nieuwe uploads bevatten beide waarden. De historiek toont lichtniveau als gemiddelde per tijdvak en aanwezigheid als 0/1 (aanwezig geweest in dat tijdvak).

## Instellingen ophalen

`GET /settings/get_device_config.php` vereist dezelfde `X-API-Key`-header. Het JSON-antwoord bevat de CO2-grenzen, upload- en synchronisatie-intervallen, audio-instellingen inclusief `audio_track`, hardware-uitgangen en een `alarms`-array met voor elk alarm `hour`, `minute`, `enabled` en `soundID`. Alarmgeluiden 1 (wake-up), 2 (Rickroll), 4 (Star Destroyer), 5 (vogels) en 7 (piano) kunnen op de alarmenpagina worden gekozen. Geluiden 3 (batterijwaarschuwing) en 6 (CO2-waarschuwing) zijn waarschuwingen en staan daarom niet in deze keuzelijst. Wijzigingen worden in PostgreSQL opgeslagen en zijn op het apparaat actief na de volgende configuratie-update. De firmware haalt deze configuratie op bij het opstarten en daarna volgens `config_sync_interval` (standaard 60 seconden); na een firmware-update moet de ESP opnieuw geflasht worden om dit synchronisatiegedrag te gebruiken.

`refresh_interval` bepaalt hoe vaak het dashboard nieuwe metingen ophaalt. Het dashboard haalt de instelling tijdens het verversen opnieuw op, zodat een opgeslagen wijziging zonder handmatig herladen van de dashboardpagina actief wordt.

## Instellingen opslaan op Render

De webinstellingen worden opgeslagen als JSON, niet door `config.php` te herschrijven. Lokaal staat het bestand in `settings/config.json`. Voor instellingen die bewaard moeten blijven na een Render-herstart of nieuwe deployment, koppel je een Render Persistent Disk (bijvoorbeeld op `/var/data`) en stel je de omgevingsvariabele `SETTINGS_FILE` in op `/var/data/settings.json`. De map moet schrijfbaar zijn voor de PHP/Apache-gebruiker. Zonder Persistent Disk is het bestand op Render tijdelijk en kunnen opgeslagen wijzigingen verloren gaan bij een herstart of deployment.

Het JSON-bestand bevat geen ESP API-sleutel. Die blijft in de omgevingsvariabele `ESP_API_KEY` of de serverconfiguratie.

De MP3-keuze is een tracknummer van 1 t/m 10; de SD-kaartbestanden en de DFPlayer-bibliotheek moeten met die nummering overeenkomen. De status op het dashboard is gebaseerd op de laatste opgeslagen meting. Hij wordt offline zodra die ouder is dan drie uploadintervallen, met een minimumgrens van 30 seconden.

De PHP-code configureert de server-API, maar bevat geen ESP-firmware. Voor de firmware is nog bevestiging nodig of het doelbord echt een ESP32-S3 is of de PSoC 6 Pioneer Kit; dit zijn verschillende platforms en SDK's.
