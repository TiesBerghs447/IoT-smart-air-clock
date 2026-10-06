<?php

header('Content-Type: application/json; charset=utf-8');
require_once __DIR__ . '/device_auth.php';

if ($_SERVER['REQUEST_METHOD'] !== 'GET') {
    http_response_code(405);
    header('Allow: GET');
    echo json_encode(['error' => 'Alleen GET is toegestaan']);
    exit;
}

require_device_api_key();

$config = include __DIR__ . '/config.php';
$deviceConfig = [
    'co2_good_max' => $config['co2_good_max'],
    'co2_warning_max' => $config['co2_warning_max'],
    'upload_interval' => $config['upload_interval'],
    'config_sync_interval' => $config['config_sync_interval'],
    'audio_enabled' => $config['audio_enabled'],
    'audio_volume' => $config['audio_volume'],
    'audio_track' => $config['audio_track'],
    'alarm_enabled' => $config['alarm_enabled'],
    'alarm_hour' => $config['alarm_hour'],
    'alarm_minute' => $config['alarm_minute'],
    'display_enabled' => $config['display_enabled'],
    'green_enabled' => $config['green_enabled'],
    'orange_enabled' => $config['orange_enabled'],
    'red_enabled' => $config['red_enabled'],
];

echo json_encode($deviceConfig);