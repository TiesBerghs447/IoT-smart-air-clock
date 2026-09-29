<?php

header('Content-Type: application/json; charset=utf-8');
require_once __DIR__ . '/../settings/database.php';

try {
    $connection = database_connection();
    $result = $connection->query(
        'SELECT temperature, humidity, pressure, co2, timestamp
         FROM measurements
         ORDER BY timestamp DESC
         LIMIT 50'
    );

    if (!$result) {
        throw new RuntimeException('History query failed');
    }

    $measurements = [];
    while ($row = $result->fetch_assoc()) {
        $measurements[] = $row;
    }

    echo json_encode(array_reverse($measurements));
    $connection->close();
} catch (Throwable $error) {
    http_response_code(500);
    echo json_encode(['error' => 'Historiek ophalen mislukt']);
}