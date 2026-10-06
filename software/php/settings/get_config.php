<?php

header('Content-Type: application/json; charset=utf-8');

$config = include('config.php');

/*
 * Deze sleutel mag niet naar de ESP of browser gestuurd worden.
 */
unset($config['device_api_key']);

$config['device_enabled'] =
    $config['device_enabled'] ?? true;

echo json_encode($config);

?>