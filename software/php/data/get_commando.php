<?php

/*
 * Geeft het laatst opgeslagen LED-commando terug als JSON; levert CO2-modus
 * met zwart als standaard wanneer er nog geen commando is opgeslagen.
 */
header('Content-Type: application/json; charset=utf-8');

// Alleen de ESP32 (met API-sleutel) mag het LED-commando ophalen.
require_once __DIR__ . '/../settings/device_auth.php';
require_device_api_key();

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