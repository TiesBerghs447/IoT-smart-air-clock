<?php

require_once __DIR__ . '/../settings/database.php';

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    header('Allow: POST');
    exit('Alleen POST is toegestaan');
}

$limits = [
    'temperature' => [-50, 100],
    'humidity' => [0, 100],
    'pressure' => [300, 1200],
    'co2' => [0, 10000]
];
$values = [];

foreach ($limits as $name => [$minimum, $maximum]) {
    if (!isset($_POST[$name]) || !is_numeric($_POST[$name])) {
        http_response_code(400);
        exit("Ongeldige of ontbrekende waarde: $name");
    }

    $value = (float) $_POST[$name];
    if (!is_finite($value) || $value < $minimum || $value > $maximum) {
        http_response_code(400);
        exit("Waarde buiten bereik: $name");
    }

    $values[$name] = $value;
}

try {
    $connection = database_connection();
    $statement = $connection->prepare(
        'INSERT INTO measurements (temperature, humidity, pressure, co2) VALUES (?, ?, ?, ?)'
    );

    if (!$statement) {
        throw new RuntimeException('Insert preparation failed');
    }

    $temperature = $values['temperature'];
    $humidity = $values['humidity'];
    $pressure = $values['pressure'];
    $co2 = (int) $values['co2'];
    $statement->bind_param('dddi', $temperature, $humidity, $pressure, $co2);

    if (!$statement->execute()) {
        throw new RuntimeException('Insert failed');
    }

    echo 'OK';
    $statement->close();
    $connection->close();
} catch (Throwable $error) {
    http_response_code(500);
    exit('Opslaan mislukt');
}