<?php

/*
 * Levert instellingen voor de ESP; vereist de apparaat-API-sleutel en geeft alleen
 * de door de firmware gebruikte instellingen terug.
 */

header('Content-Type: application/json; charset=utf-8');

require_once __DIR__ . '/device_auth.php';

if ($_SERVER['REQUEST_METHOD'] !== 'GET')
{
    http_response_code(405);

    header('Allow: GET');

    echo json_encode([
        'error' => 'Alleen GET is toegestaan'
    ]);

    exit;
}

require_device_api_key();

require_once __DIR__ . '/config_store.php';
require_once __DIR__ . '/database.php';

try
{
    $config = load_app_config();

    $deviceConfig = [
        'co2_good_max' =>
            $config['co2_good_max'],

        'co2_warning_max' =>
            $config['co2_warning_max'],

        'upload_interval' =>
            $config['upload_interval'],

        'config_sync_interval' =>
            $config['config_sync_interval'],

        'audio_enabled' =>
            $config['audio_enabled'],

        'audio_volume' =>
            $config['audio_volume'],

        'audio_track' =>
            $config['audio_track'],

        'display_enabled' =>
            $config['display_enabled'],

        'green_enabled' =>
            $config['green_enabled'],

        'orange_enabled' =>
            $config['orange_enabled'],

        'red_enabled' =>
            $config['red_enabled']
    ];

    $connection = database_connection();
    $statement = $connection->query(
        'SELECT
            id,
            hour,
            minute,
            CASE WHEN enabled THEN 1 ELSE 0 END AS enabled
        FROM alarms
        ORDER BY hour, minute'
    );

    $alarms = [];
    foreach ($statement->fetchAll(PDO::FETCH_ASSOC) as $row)
    {
        $alarms[] = [
            'id' => (int)$row['id'],
            'hour' => (int)$row['hour'],
            'minute' => (int)$row['minute'],
            'enabled' => (bool)$row['enabled']
        ];
    }

    $deviceConfig['alarms'] = $alarms;

    echo json_encode($deviceConfig);
}
catch (Throwable $error)
{
    error_log(
        'Apparaatconfiguratie ophalen mislukt: ' .
        $error->getMessage()
    );

    http_response_code(500);

    echo json_encode([
        'error' => 'Apparaatconfiguratie kon niet worden opgehaald.'
    ]);
}