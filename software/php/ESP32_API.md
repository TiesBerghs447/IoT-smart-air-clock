# ESP32 API

Configureer op de PHP-host een omgevingsvariabele `ESP_API_KEY` met minimaal 32 willekeurige tekens. Als de host geen eigen omgevingsvariabelen ondersteunt, zet de sleutel in `settings/config.php` bij `device_api_key`. De API weigert requests zolang geen geldige sleutel is ingesteld. Gebruik dezelfde sleutel in de firmware en stuur hem mee als `X-API-Key`; verstuur requests uitsluitend over HTTPS. De browserconfiguratie geeft de sleutel niet terug.

## Metingen verzenden

`POST /data/save_data.php` verwacht `application/x-www-form-urlencoded` met `temperature`, `humidity`, `pressure` en `co2`. Succes geeft HTTP 200 met `{"status":"ok"}`. Ongeldige waarden geven HTTP 400; een ongeldige sleutel geeft HTTP 401.

## Instellingen ophalen

`GET /settings/get_device_config.php` vereist dezelfde `X-API-Key`-header. Het JSON-antwoord bevat de CO2-grenzen, upload- en synchronisatie-intervallen, audio-instellingen inclusief `audio_track`, alarmtijd en hardware-uitgangen. Haal deze configuratie op bij het opstarten en daarna volgens `config_sync_interval`.

De MP3-keuze is een tracknummer van 1 t/m 10; de SD-kaartbestanden en de DFPlayer-bibliotheek moeten met die nummering overeenkomen. De status op het dashboard is gebaseerd op de laatste opgeslagen meting. Hij wordt offline zodra die ouder is dan drie uploadintervallen, met een minimumgrens van 30 seconden.

De PHP-code configureert de server-API, maar bevat geen ESP-firmware. Voor de firmware is nog bevestiging nodig of het doelbord echt een ESP32-S3 is of de PSoC 6 Pioneer Kit; dit zijn verschillende platforms en SDK's.
