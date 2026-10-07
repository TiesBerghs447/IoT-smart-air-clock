<?php

header('Content-Type: application/json; charset=utf-8');
require_once __DIR__ . '/../settings/database.php';

try
{
    $connection = database_connection();

    $statement = $connection->query(
        'SELECT temperature, humidity, pressure, co2, timestamp
         FROM measurements
         ORDER BY timestamp DESC
         LIMIT 50'
    );

    $measurements = $statement->fetchAll(PDO::FETCH_ASSOC);

    echo json_encode(array_reverse($measurements));
}
catch (Throwable $error)
{
    http_response_code(500);
    echo json_encode([
        'error' => $error->getMessage()
    ]);
}