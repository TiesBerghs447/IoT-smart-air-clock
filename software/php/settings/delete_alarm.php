<?php

require_once __DIR__ . '/device_auth.php';
require_device_api_key();

require_once __DIR__ . '/database.php';

if ($_SERVER['REQUEST_METHOD'] !== 'POST')
{
    http_response_code(405);
    exit;
}

$id = (int)($_POST['id'] ?? 0);

if ($id <= 0)
{
    http_response_code(400);
    exit;
}

$db = database_connection();

$stmt = $db->prepare(
    'DELETE FROM alarms
     WHERE id = :id'
);

$stmt->execute([
    ':id' => $id
]);

echo json_encode([
    'success' => true
]);