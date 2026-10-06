<?php

header('Content-Type: application/json; charset=utf-8');
require_once __DIR__ . '/../settings/database.php';

try {
    $connection = database_connection();
    $result = $connection->query(
        'SELECT temperature, humidity, pressure, co2, timestamp,
            TIMESTAMPDIFF(SECOND, timestamp, CURRENT_TIMESTAMP) AS seconds_since_update
         FROM measurements
         ORDER BY timestamp DESC
         LIMIT 1'
    );

    if (!$result) {
        throw new RuntimeException('Latest measurement query failed');
    }

    $measurement = $result->fetch_assoc();
    $response = $measurement ?: [
        'temperature' => null,
        'humidity' => null,
        'pressure' => null,
        'co2' => null,
        'timestamp' => null,
        'seconds_since_update' => null,
    ];

    $secondsSinceUpdate = $response['seconds_since_update'] === null
        ? null
        : (int) $response['seconds_since_update'];
    $config = include __DIR__ . '/../settings/config.php';
    $onlineThreshold = max(30, ((int) ($config['upload_interval'] ?? 30)) * 3);

    $response['device_status'] = $secondsSinceUpdate !== null
        && $secondsSinceUpdate >= 0
        && $secondsSinceUpdate <= $onlineThreshold
        ? 'online'
        : ($secondsSinceUpdate === null ? 'unknown' : 'offline');
    $response['seconds_since_update'] = $secondsSinceUpdate;

    echo json_encode($response);
    $connection->close();
} catch (Throwable $error) {
    http_response_code(500);
    echo json_encode(['error' => 'Metingen ophalen mislukt']);
}
/*
} catch (Throwable $error) {
    http_response_code(500);
    echo json_encode(['error' => 'Metingen ophalen mislukt']);
}*/