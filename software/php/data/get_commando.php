<?php

header('Content-Type: application/json; charset=utf-8');

$file = __DIR__ . '/command.json';

if (!file_exists($file))
{
    echo json_encode([
    'led_mode' => 'co2',
    'red' => 0,
    'green' => 0,
    'blue' => 0
    ]);
    exit;
}

echo file_get_contents($file);