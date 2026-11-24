<?php

/*
 * Geeft de nieuwste sensormeting terug en gebruikt de uploadinstelling om
 * te bepalen of het apparaat op het dashboard online is.
 */
header('Content-Type: application/json; charset=utf-8');

require_once __DIR__ . '/../settings/web_auth.php';
web_auth_require(true);

require_once __DIR__ . '/../settings/database.php';

try
{
    $connection = database_connection();

    $statement = $connection->query(
        "SELECT
            temperature,
            humidity,
            pressure,
            co2,
            light,
            presence,
            timestamp,
            EXTRACT(EPOCH FROM (CURRENT_TIMESTAMP - timestamp)) AS seconds_since_update
         FROM measurements
         ORDER BY timestamp DESC
         LIMIT 1"
    );

    $measurement = $statement->fetch(PDO::FETCH_ASSOC);

    $response = $measurement ?: [
        'temperature' => null,
        'humidity' => null,
        'pressure' => null,
        'co2' => null,
        'light' => null,
        'presence' => null,
        'timestamp' => null,
        'seconds_since_update' => null,
    ];

    // Trend: nieuwste meting t.o.v. de laatste meting van minstens 30 min eerder.
    $previous = $connection->query(
        "SELECT temperature, humidity, pressure, co2
         FROM measurements
         WHERE timestamp <= (SELECT MAX(timestamp) FROM measurements) - INTERVAL '30 minutes'
         ORDER BY timestamp DESC
         LIMIT 1"
    )->fetch(PDO::FETCH_ASSOC);

    foreach (['temperature', 'humidity', 'pressure', 'co2'] as $field) {
        $response[$field . '_trend'] =
            $previous && $response[$field] !== null && $previous[$field] !== null
                ? $response[$field] <=> $previous[$field]
                : null;
    }

    // Afwijking: CO2 meer dan 300 ppm gestegen t.o.v. de meting van 10 min eerder.
    $earlier = $connection->query(
        "SELECT co2 FROM measurements
         WHERE timestamp <= (SELECT MAX(timestamp) FROM measurements) - INTERVAL '10 minutes'
         ORDER BY timestamp DESC LIMIT 1"
    )->fetchColumn();
    $response['co2_spike'] =
        $earlier !== false && $response['co2'] !== null
        && $response['co2'] - $earlier > 300;

    $secondsSinceUpdate =
        $response['seconds_since_update'] === null
            ? null
            : (int)$response['seconds_since_update'];

    require_once __DIR__ . '/../settings/config_store.php';
    $config = load_app_config();

    $onlineThreshold =
        max(
            30,
            ((int)($config['upload_interval'] ?? 30)) * 3
        );

    $response['device_status'] =
        $secondsSinceUpdate !== null
        && $secondsSinceUpdate >= 0
        && $secondsSinceUpdate <= $onlineThreshold
            ? 'online'
            : ($secondsSinceUpdate === null ? 'unknown' : 'offline');

    $response['seconds_since_update'] =
        $secondsSinceUpdate;

    echo json_encode($response);
}
catch (Throwable $error)
{
    error_log('Laatste meting ophalen mislukt: ' . $error->getMessage());

    http_response_code(500);

    echo json_encode([
        'error' => 'De laatste meting kon niet worden opgehaald.'
    ]);
}