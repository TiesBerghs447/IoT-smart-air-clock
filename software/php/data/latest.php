<?php

header('Content-Type: application/json; charset=utf-8');
require_once __DIR__ . '/../settings/database.php';

try {
    $connection = database_connection();
    $result = $connection->query(
        'SELECT temperature, humidity, pressure, co2, timestamp
         FROM measurements
         ORDER BY timestamp DESC
         LIMIT 1'
    );

    if (!$result) {
        throw new RuntimeException('Latest measurement query failed');
    }

    $measurement = $result->fetch_assoc();
    echo json_encode($measurement ?: [
        'temperature' => null,
        'humidity' => null,
        'pressure' => null,
        'co2' => null,
        'timestamp' => null
    ]);
    $connection->close();
} catch (Throwable $error) {
http_response_code(500);
echo json_encode([
'error' => $error->getMessage()
]);
}
/*
} catch (Throwable $error) {
    http_response_code(500);
    echo json_encode(['error' => 'Metingen ophalen mislukt']);
}*/