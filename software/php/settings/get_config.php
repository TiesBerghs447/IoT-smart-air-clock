<?php

/*
 * Levert de huidige instellingen als JSON aan de browser en laat de API-sleutel weg.
 */
header('Content-Type: application/json; charset=utf-8');
header('Cache-Control: no-store, no-cache, must-revalidate, max-age=0');

require_once __DIR__ . '/config_store.php';

$config = load_app_config();
$config['refresh_interval'] =
    max(1, min(300, (int)($config['refresh_interval'] ?? 5)));
$config['config_sync_interval'] =
    max(15, min(3600, (int)($config['config_sync_interval'] ?? 60)));

/*
 * Deze sleutel mag niet naar de ESP of browser gestuurd worden.
 */
unset($config['device_api_key']);

$config['device_enabled'] =
    $config['device_enabled'] ?? true;

echo json_encode($config);

?>