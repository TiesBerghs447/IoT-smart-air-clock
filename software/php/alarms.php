<?php

session_start();

require_once __DIR__ . '/settings/database.php';

if (!isset($_SESSION['alarm_csrf_token'])) {
    $_SESSION['alarm_csrf_token'] = bin2hex(random_bytes(32));
}

$csrfToken = $_SESSION['alarm_csrf_token'];
$alarms = [];
$message = null;
$messageType = 'error';
$connection = null;

function alarm_posted_time(string $field, int $maximum): int
{
    $value = $_POST[$field] ?? null;

    if (!is_string($value) || !preg_match('/^\d{1,2}$/D', $value)) {
        throw new InvalidArgumentException('Vul een geldige alarmtijd in.');
    }

    $time = (int) $value;
    if ($time < 0 || $time > $maximum) {
        throw new InvalidArgumentException('De opgegeven alarmtijd is ongeldig.');
    }

    return $time;
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    try {
        $submittedToken = $_POST['csrf_token'] ?? '';
        if (
            !is_string($submittedToken)
            || !hash_equals($csrfToken, $submittedToken)
        ) {
            throw new InvalidArgumentException('Je sessie is verlopen. Vernieuw de pagina en probeer opnieuw.');
        }

        $connection = database_connection();
        $action = $_POST['action'] ?? '';

        if ($action === 'add') {
            $hour = alarm_posted_time('hour', 23);
            $minute = alarm_posted_time('minute', 59);

            $count = (int) $connection->query('SELECT COUNT(*) FROM alarms')->fetchColumn();
            if ($count >= 5) {
                throw new InvalidArgumentException('Er kunnen maximaal vijf alarmen worden ingesteld.');
            }

            $duplicate = $connection->prepare(
                'SELECT COUNT(*) FROM alarms WHERE hour = :hour AND minute = :minute'
            );
            $duplicate->execute([':hour' => $hour, ':minute' => $minute]);
            if ((int) $duplicate->fetchColumn() > 0) {
                throw new InvalidArgumentException('Er bestaat al een alarm op dit tijdstip.');
            }

            $insert = $connection->prepare(
                'INSERT INTO alarms (hour, minute) VALUES (:hour, :minute)'
            );
            $insert->execute([':hour' => $hour, ':minute' => $minute]);

            header('Location: alarms.php?status=added');
            exit;
        }

        $oldHour = alarm_posted_time('old_hour', 23);
        $oldMinute = alarm_posted_time('old_minute', 59);
        $existing = $connection->prepare(
            'SELECT COUNT(*) FROM alarms WHERE hour = :hour AND minute = :minute'
        );
        $existing->execute([':hour' => $oldHour, ':minute' => $oldMinute]);
        $existingCount = (int) $existing->fetchColumn();

        if ($existingCount !== 1) {
            throw new InvalidArgumentException(
                $existingCount === 0
                    ? 'Dit alarm bestaat niet meer. Vernieuw de pagina.'
                    : 'Er zijn meerdere alarmen met deze tijd. Pas de dubbele tijden eerst aan in de database.'
            );
        }

        if ($action === 'update') {
            $hour = alarm_posted_time('hour', 23);
            $minute = alarm_posted_time('minute', 59);

            $duplicate = $connection->prepare(
                'SELECT COUNT(*) FROM alarms
                 WHERE hour = :hour AND minute = :minute
                   AND (hour <> :old_hour OR minute <> :old_minute)'
            );
            $duplicate->execute([
                ':hour' => $hour,
                ':minute' => $minute,
                ':old_hour' => $oldHour,
                ':old_minute' => $oldMinute,
            ]);
            if ((int) $duplicate->fetchColumn() > 0) {
                throw new InvalidArgumentException('Er bestaat al een alarm op dit tijdstip.');
            }

            $update = $connection->prepare(
                'UPDATE alarms SET hour = :hour, minute = :minute
                 WHERE hour = :old_hour AND minute = :old_minute'
            );
            $update->execute([
                ':hour' => $hour,
                ':minute' => $minute,
                ':old_hour' => $oldHour,
                ':old_minute' => $oldMinute,
            ]);

            header('Location: alarms.php?status=updated');
            exit;
        }

        if ($action === 'delete') {
            $delete = $connection->prepare(
                'DELETE FROM alarms WHERE hour = :hour AND minute = :minute'
            );
            $delete->execute([':hour' => $oldHour, ':minute' => $oldMinute]);

            header('Location: alarms.php?status=deleted');
            exit;
        }

        throw new InvalidArgumentException('Onbekende actie.');
    } catch (InvalidArgumentException $error) {
        $message = $error->getMessage();
    } catch (Throwable $error) {
        error_log('Alarmer beheren mislukt: ' . $error->getMessage());
        $message = 'De alarmen konden niet worden bijgewerkt. Controleer de databaseverbinding en probeer opnieuw.';
    }
}

if (isset($_GET['status']) && is_string($_GET['status'])) {
    $statusMessages = [
        'added' => 'Alarm toegevoegd.',
        'updated' => 'Alarm gewijzigd.',
        'deleted' => 'Alarm verwijderd.',
    ];
    if (isset($statusMessages[$_GET['status']])) {
        $message = $statusMessages[$_GET['status']];
        $messageType = 'success';
    }
}

try {
    $connection = $connection ?? database_connection();
    $alarms = $connection->query(
        'SELECT hour, minute FROM alarms ORDER BY hour, minute'
    )->fetchAll(PDO::FETCH_ASSOC);
} catch (Throwable $error) {
    error_log('Alarmlijst laden mislukt: ' . $error->getMessage());
    $message = 'De alarmen konden niet worden geladen. Controleer de databaseverbinding.';
    $messageType = 'error';
}

?>
<!DOCTYPE html>
<html lang="nl">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Alarmen - Smart Air Clock</title>
    <link rel="stylesheet" href="stylesheet.css">
</head>

<body>
    <nav class="navbar">
        <div class="logo">🌬 Smart Air Clock</div>
        <ul class="nav-links">
            <li><a href="index.php">Dashboard</a></li>
            <li><a href="alarms.php" aria-current="page">Alarmen</a></li>
            <li><a href="settings/settings.php">Instellingen</a></li>
            <li><a href="history.php">Historiek</a></li>
        </ul>
    </nav>

    <main class="settings-container alarm-container">
        <h1>Alarmen</h1>
        <p class="subtitle">Bekijk en beheer de alarmtijden die naar je Smart Air Clock worden verstuurd.</p>

        <?php if ($message !== null): ?>
            <div class="<?= htmlspecialchars($messageType, ENT_QUOTES, 'UTF-8'); ?>" role="status">
                <?= htmlspecialchars($message, ENT_QUOTES, 'UTF-8'); ?>
            </div>
        <?php endif; ?>

        <section class="settings-card">
            <h2>Nieuw alarm instellen</h2>
            <?php if (count($alarms) < 5): ?>
                <form method="post" class="alarm-form">
                    <input type="hidden" name="csrf_token" value="<?= htmlspecialchars($csrfToken, ENT_QUOTES, 'UTF-8'); ?>">
                    <input type="hidden" name="action" value="add">
                    <div class="alarm-time-fields">
                        <label>Uur
                            <input type="number" name="hour" min="0" max="23" required>
                        </label>
                        <label>Minuut
                            <input type="number" name="minute" min="0" max="59" required>
                        </label>
                    </div>
                    <button class="save-btn" type="submit">Alarm toevoegen</button>
                </form>
            <?php else: ?>
                <p>Je hebt het maximum van vijf alarmen bereikt.</p>
            <?php endif; ?>
        </section>

        <section class="settings-card">
            <h2>Ingestelde alarmen (<?= count($alarms); ?>/5)</h2>
            <?php if ($alarms === []): ?>
                <p>Er zijn nog geen alarmen ingesteld.</p>
            <?php else: ?>
                <div class="alarm-list">
                    <?php foreach ($alarms as $alarm): ?>
                        <?php
                        $hour = (int) $alarm['hour'];
                        $minute = (int) $alarm['minute'];
                        ?>
                        <div class="alarm-item">
                            <div class="alarm-display">
                                <strong><?= sprintf('%02d:%02d', $hour, $minute); ?></strong>
                            </div>
                            <form method="post" class="alarm-form">
                                <input type="hidden" name="csrf_token" value="<?= htmlspecialchars($csrfToken, ENT_QUOTES, 'UTF-8'); ?>">
                                <input type="hidden" name="action" value="update">
                                <input type="hidden" name="old_hour" value="<?= $hour; ?>">
                                <input type="hidden" name="old_minute" value="<?= $minute; ?>">
                                <div class="alarm-time-fields">
                                    <label>Uur
                                        <input type="number" name="hour" min="0" max="23" value="<?= $hour; ?>" required>
                                    </label>
                                    <label>Minuut
                                        <input type="number" name="minute" min="0" max="59" value="<?= $minute; ?>" required>
                                    </label>
                                </div>
                                <button class="save-btn" type="submit">Wijzigen</button>
                            </form>
                            <form method="post" class="alarm-delete-form" onsubmit="return confirm('Dit alarm verwijderen?');">
                                <input type="hidden" name="csrf_token" value="<?= htmlspecialchars($csrfToken, ENT_QUOTES, 'UTF-8'); ?>">
                                <input type="hidden" name="action" value="delete">
                                <input type="hidden" name="old_hour" value="<?= $hour; ?>">
                                <input type="hidden" name="old_minute" value="<?= $minute; ?>">
                                <button class="reset-btn" type="submit">Verwijderen</button>
                            </form>
                        </div>
                    <?php endforeach; ?>
                </div>
            <?php endif; ?>
        </section>
    </main>
</body>

</html>
