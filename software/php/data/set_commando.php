<?php

header('Content-Type: application/json; charset=utf-8');

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

file_put_contents(
    __DIR__ . '/command.json',
    json_encode(
        $data,
        JSON_PRETTY_PRINT
    )
);

echo json_encode([
    'status' => 'ok'
]);