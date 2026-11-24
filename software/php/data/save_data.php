<?php

// Fouten loggen, niet tonen: foutmeldingen kunnen databasegegevens lekken.
ini_set('display_errors', 0);
error_reporting(E_ALL);

header('Content-Type: application/json; charset=utf-8');

require_once __DIR__ . '/../settings/device_auth.php';

if ($_SERVER['REQUEST_METHOD'] !== 'POST')
{
    http_response_code(405);

    header('Allow: POST');

    echo json_encode([
        'error' => 'Alleen POST is toegestaan'
    ]);

    exit;
}

require_device_api_key();

require_once __DIR__ . '/../settings/database.php';

$limits = [
    'temperature' => [-50, 100],
    'humidity'    => [0, 100],
    'pressure'    => [300, 1200],
    'co2'         => [0, 10000],
    'light'       => [0, 4095],   // ESP32 ADC is 12 bit: 0..4095
    'presence'    => [0, 1]
];

$values = [];

foreach ($limits as $name => [$minimum, $maximum])
{
    if (
        !isset($_POST[$name]) ||
        !is_numeric($_POST[$name])
    )
    {
        http_response_code(400);

        exit(
            json_encode([
                'error' => "Ongeldige of ontbrekende waarde: $name"
            ])
        );
    }

    $value = (float) $_POST[$name];

    if (
        !is_finite($value) ||
        $value < $minimum ||
        $value > $maximum ||
        (in_array($name, ['co2', 'light', 'presence'], true) && floor($value) !== $value)
    )
    {
        http_response_code(400);

        exit(
            json_encode([
                'error' => "Waarde buiten bereik: $name"
            ])
        );
    }

    $values[$name] = $value;
}

// Optioneel meettijdstip (Unix-tijd) voor metingen uit de offline buffer
// van de ESP32. Zonder timestamp gebruikt de database "nu".
$timestamp = null;

if (isset($_POST['timestamp']) && $_POST['timestamp'] !== '')
{
    if (!ctype_digit((string)$_POST['timestamp']))
    {
        http_response_code(400);
        exit(json_encode(['error' => 'Ongeldige timestamp']));
    }

    $timestamp = (int)$_POST['timestamp'];
    $now = time();

    // Niet in de toekomst (2 min speling) en niet ouder dan 7 dagen
    if ($timestamp > $now + 120 || $timestamp < $now - 7 * 86400)
    {
        http_response_code(400);
        exit(json_encode(['error' => 'Timestamp buiten bereik']));
    }
}

try
{
    $connection = database_connection();

    $timestampSql = $timestamp === null
        ? 'CURRENT_TIMESTAMP'
        : 'TO_TIMESTAMP(:timestamp)';

    $statement = $connection->prepare(
        "INSERT INTO measurements
        (
            temperature,
            humidity,
            pressure,
            co2,
            light,
            presence,
            timestamp
        )
        VALUES
        (
            :temperature,
            :humidity,
            :pressure,
            :co2,
            :light,
            :presence,
            $timestampSql
        )"
    );

    $parameters = [
        ':temperature' => $values['temperature'],
        ':humidity'    => $values['humidity'],
        ':pressure'    => $values['pressure'],
        ':co2'         => (int)$values['co2'],
        ':light'       => (int)$values['light'],
        ':presence'    => (int)$values['presence']
    ];

    if ($timestamp !== null)
    {
        $parameters[':timestamp'] = $timestamp;
    }

    $statement->execute($parameters);

    echo json_encode([
        'status' => 'ok'
    ]);
}
catch (Throwable $error)
{
    error_log('Meting opslaan mislukt: ' . $error->getMessage());

    http_response_code(500);

    echo json_encode([
        'error' => 'Meting kon niet worden opgeslagen.'
    ]);
}
