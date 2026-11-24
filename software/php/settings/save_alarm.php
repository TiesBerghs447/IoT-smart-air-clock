<?php

/*
 * Ontvangt een alarm dat op de ESP32 (met de knoppen) is gemaakt of gewijzigd.
 * Zonder dit endpoint bestonden zulke alarmen alleen op het toestel en wiste
 * de volgende configuratie-sync ze weer.
 *
 * POST (x-www-form-urlencoded): hour, minute, enabled (0/1), soundID, optioneel id
 * Antwoord: {"id": <database-id>}
 *   409 = maximaal aantal alarmen bereikt (de ESP neemt dan de serverlijst over)
 */

header('Content-Type: application/json; charset=utf-8');

require_once __DIR__ . '/device_auth.php';

if ($_SERVER['REQUEST_METHOD'] !== 'POST')
{
    http_response_code(405);
    header('Allow: POST');
    echo json_encode(['error' => 'Alleen POST is toegestaan']);
    exit;
}

require_device_api_key();

require_once __DIR__ . '/database.php';

const MAX_ALARMS = 5;

// Zelfde geluid-id's als alarm_sound_options() in alarms.php
// en de tabel sounds[] in de firmware (alarm.cpp).
const ALLOWED_SOUND_IDS = [1, 2, 4, 5, 7];

function posted_int(string $name, int $minimum, int $maximum): int
{
    $value = $_POST[$name] ?? null;

    if (!is_string($value) || !preg_match('/^\d{1,9}$/D', $value))
    {
        http_response_code(400);
        exit(json_encode(['error' => "Ongeldige waarde: $name"]));
    }

    $number = (int)$value;

    if ($number < $minimum || $number > $maximum)
    {
        http_response_code(400);
        exit(json_encode(['error' => "Waarde buiten bereik: $name"]));
    }

    return $number;
}

$hour = posted_int('hour', 0, 23);
$minute = posted_int('minute', 0, 59);
$enabled = posted_int('enabled', 0, 1) === 1;
$soundId = posted_int('soundID', 1, 99);

if (!in_array($soundId, ALLOWED_SOUND_IDS, true))
{
    $soundId = 1;
}

$id = isset($_POST['id']) ? posted_int('id', 1, 999999999) : null;

try
{
    $db = database_connection();
    $db->beginTransaction();

    $parameters = [
        ':hour' => $hour,
        ':minute' => $minute,
        ':enabled' => $enabled ? 'true' : 'false',
        ':soundID' => $soundId
    ];

    // 1. Bestaand alarm bijwerken
    if ($id !== null)
    {
        $update = $db->prepare(
            'UPDATE alarms
             SET hour = :hour, minute = :minute, enabled = :enabled, "soundID" = :soundID
             WHERE id = :id'
        );

        $update->execute($parameters + [':id' => $id]);

        if ($update->rowCount() > 0)
        {
            $db->commit();
            echo json_encode(['id' => $id]);
            exit;
        }

        // Alarm was intussen op de website verwijderd -> als nieuw toevoegen
    }

    // 2. Er bestaat al een alarm op dit tijdstip -> dat bijwerken
    $existing = $db->prepare(
        'SELECT id FROM alarms WHERE hour = :hour AND minute = :minute LIMIT 1'
    );
    $existing->execute([':hour' => $hour, ':minute' => $minute]);
    $existingId = $existing->fetchColumn();

    if ($existingId !== false)
    {
        $update = $db->prepare(
            'UPDATE alarms SET enabled = :enabled, "soundID" = :soundID WHERE id = :id'
        );
        $update->execute([
            ':enabled' => $parameters[':enabled'],
            ':soundID' => $soundId,
            ':id' => (int)$existingId
        ]);

        $db->commit();
        echo json_encode(['id' => (int)$existingId]);
        exit;
    }

    // 3. Nieuw alarm, maar niet meer dan MAX_ALARMS
    $count = (int)$db->query('SELECT COUNT(*) FROM alarms')->fetchColumn();

    if ($count >= MAX_ALARMS)
    {
        $db->rollBack();
        http_response_code(409);
        echo json_encode(['error' => 'Maximaal ' . MAX_ALARMS . ' alarmen']);
        exit;
    }

    $insert = $db->prepare(
        'INSERT INTO alarms (hour, minute, enabled, "soundID")
         VALUES (:hour, :minute, :enabled, :soundID)
         RETURNING id'
    );
    $insert->execute($parameters);

    $newId = (int)$insert->fetchColumn();

    $db->commit();

    echo json_encode(['id' => $newId]);
}
catch (Throwable $error)
{
    if (isset($db) && $db->inTransaction())
    {
        $db->rollBack();
    }

    error_log('Alarm opslaan mislukt: ' . $error->getMessage());

    http_response_code(500);

    echo json_encode(['error' => 'Alarm kon niet worden opgeslagen.']);
}
