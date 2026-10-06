<?php

function require_device_api_key(): void
{
    $apiKey = getenv('ESP_API_KEY');

    if (!is_string($apiKey) || $apiKey === '') {
        $config = include __DIR__ . '/config.php';
        $apiKey = $config['device_api_key'] ?? '';
    }

    if (!is_string($apiKey) || strlen($apiKey) < 32) {
        http_response_code(503);
        echo json_encode(['error' => 'ESP API-sleutel is niet ingesteld']);
        exit;
    }

    $providedKey = $_SERVER['HTTP_X_API_KEY'] ?? '';

    if (!is_string($providedKey) || !hash_equals($apiKey, $providedKey)) {
        http_response_code(401);
        echo json_encode(['error' => 'Ongeldige API-sleutel']);
        exit;
    }
}