<?php

/*
 * Levert de huidige instellingen als JSON aan de browser en laat de API-sleutel weg.
 */
header('Content-Type: application/json; charset=utf-8');

require_once __DIR__ . '/config_store.php';

$config = load_app_config();

/*
 * Deze sleutel mag niet naar de ESP of browser gestuurd worden.
 */
unset($config['device_api_key']);

$config['device_enabled'] =
    $config['device_enabled'] ?? true;

echo json_encode($config);

?>