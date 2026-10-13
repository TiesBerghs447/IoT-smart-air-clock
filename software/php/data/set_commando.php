<?php

header('Content-Type: application/json; charset=utf-8');

if ($_SERVER['REQUEST_METHOD'] !== 'POST')
{
    http_response_code(405);
    header('Allow: POST');
    echo json_encode(['error' => 'Alleen POST is toegestaan']);
    exit;
}

$ledMode = $_POST['led_mode'] ?? 'co2';

if (
    $ledMode !== 'co2' &&
    $ledMode !== 'remote'
)
{
    $ledMode = 'co2';
}

$red   = max(0, min(255, (int)($_POST['red'] ?? 0)));
$green = max(0, min(255, (int)($_POST['green'] ?? 0)));
$blue  = max(0, min(255, (int)($_POST['blue'] ?? 0)));

$data = [
    'led_mode' => $ledMode,
    'red' => $red,
    'green' => $green,
    'blue' => $blue
];

if (file_put_contents(
    __DIR__ . '/command.json',
    json_encode(
        $data,
        JSON_PRETTY_PRINT
    ),
    LOCK_EX
) === false)
{
    http_response_code(500);
    echo json_encode(['error' => 'Commando kon niet worden opgeslagen']);
    exit;
}

echo json_encode([
    'status' => 'ok'
]);