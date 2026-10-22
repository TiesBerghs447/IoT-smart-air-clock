<?php

ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

if (session_status() === PHP_SESSION_NONE)
{
    session_start();
}

require_once __DIR__ . '/settings/database.php';

if (!isset($_SESSION['alarm_csrf_token']))
{
    $_SESSION['alarm_csrf_token'] =
        bin2hex(random_bytes(32));
}

$csrfToken =
    $_SESSION['alarm_csrf_token'];

$alarms = [];

$message = null;

$messageType = 'error';

$connection = null;

function alarm_posted_time(
    string $field,
    int $maximum
): int
{
    $value =
        $_POST[$field] ?? null;

    if (
        !is_string($value)
        || !preg_match('/^\d{1,2}$/D', $value)
    )
    {
        throw new InvalidArgumentException(
            'Vul een geldige alarmtijd in.'
        );
    }

    $time = (int)$value;

    if (
        $time < 0 ||
        $time > $maximum
    )
    {
        throw new InvalidArgumentException(
            'De opgegeven alarmtijd is ongeldig.'
        );
    }

    return $time;
}

if ($_SERVER['REQUEST_METHOD'] === 'POST')
{
    try
    {
        $submittedToken =
            $_POST['csrf_token'] ?? '';

        if (
            !is_string($submittedToken)
            || !hash_equals(
                $csrfToken,
                $submittedToken
            )
        )
        {
            throw new InvalidArgumentException(
                'Je sessie is verlopen. Vernieuw de pagina en probeer opnieuw.'
            );
        }

        $connection =
            database_connection();

        $action =
            $_POST['action'] ?? '';

        if ($action === 'add')
        {
            $hour =
                alarm_posted_time(
                    'hour',
                    23
                );

            $minute =
                alarm_posted_time(
                    'minute',
                    59
                );

            $enabled =
                isset($_POST['enabled']);

            $count =
                (int)$connection
                    ->query(
                        'SELECT COUNT(*) FROM alarms'
                    )
                    ->fetchColumn();

            if ($count >= 5)
            {
                throw new InvalidArgumentException(
                    'Er kunnen maximaal vijf alarmen worden ingesteld.'
                );
            }

            $duplicate =
                $connection->prepare(
                    'SELECT COUNT(*) FROM alarms
                     WHERE hour = :hour
                     AND minute = :minute'
                );

            $duplicate->execute([
                ':hour' => $hour,
                ':minute' => $minute
            ]);

            if (
                (int)$duplicate->fetchColumn() > 0
            )
            {
                throw new InvalidArgumentException(
                    'Er bestaat al een alarm op dit tijdstip.'
                );
            }

            $insert =
                $connection->prepare(
                    'INSERT INTO alarms
                     (hour, minute, enabled)
                     VALUES
                     (:hour, :minute, :enabled)'
                );

            $insert->execute([
                ':hour' => $hour,
                ':minute' => $minute,
                ':enabled' => $enabled
            ]);

            header(
                'Location: alarms.php?status=added'
            );

            exit;
        }

        $id =
            (int)($_POST['id'] ?? 0);

        if ($id <= 0)
        {
            throw new InvalidArgumentException(
                'Ongeldig alarm.'
            );
        }

        if ($action === 'update')
        {
            $hour =
                alarm_posted_time(
                    'hour',
                    23
                );

            $minute =
                alarm_posted_time(
                    'minute',
                    59
                );

            $enabled =
                isset($_POST['enabled']);

            $duplicate =
                $connection->prepare(
                    'SELECT COUNT(*)
                     FROM alarms
                     WHERE hour = :hour
                     AND minute = :minute
                     AND id <> :id'
                );

            $duplicate->execute([
                ':hour' => $hour,
                ':minute' => $minute,
                ':id' => $id
            ]);

            if (
                (int)$duplicate->fetchColumn() > 0
            )
            {
                throw new InvalidArgumentException(
                    'Er bestaat al een alarm op dit tijdstip.'
                );
            }

            $update =
                $connection->prepare(
                    'UPDATE alarms
                     SET
                        hour = :hour,
                        minute = :minute,
                        enabled = :enabled
                     WHERE id = :id'
                );

            $update->execute([
                ':hour' => $hour,
                ':minute' => $minute,
                ':enabled' => $enabled,
                ':id' => $id
            ]);

            header(
                'Location: alarms.php?status=updated'
            );

            exit;
        }

        if ($action === 'delete')
        {
            $delete =
                $connection->prepare(
                    'DELETE FROM alarms
                     WHERE id = :id'
                );

            $delete->execute([
                ':id' => $id
            ]);

            header(
                'Location: alarms.php?status=deleted'
            );

            exit;
        }

        throw new InvalidArgumentException(
            'Onbekende actie.'
        );
    }
    catch (InvalidArgumentException $error)
    {
        $message =
            $error->getMessage();
    }
    catch (Throwable $error)
    {
        error_log(
            'Alarmbeheer mislukt: ' .
            $error->getMessage()
        );

        $message =
            'De alarmen konden niet worden bijgewerkt.';
    }
}

if (
    isset($_GET['status']) &&
    is_string($_GET['status'])
)
{
    $messages = [
        'added' => 'Alarm toegevoegd.',
        'updated' => 'Alarm gewijzigd.',
        'deleted' => 'Alarm verwijderd.'
    ];

    if (
        isset(
            $messages[$_GET['status']]
        )
    )
    {
        $message =
            $messages[
                $_GET['status']
            ];

        $messageType =
            'success';
    }
}

try
{
    $connection =
        $connection ??
        database_connection();

    $alarms =
        $connection
        ->query(
            '
            SELECT
                id,
                hour,
                minute,
                enabled
            FROM alarms
            ORDER BY hour, minute
            '
        )
        ->fetchAll(
            PDO::FETCH_ASSOC
        );
}
catch (Throwable $error)
{
    error_log(
        'Alarmen laden mislukt: ' .
        $error->getMessage()
    );

    $message =
        'De alarmen konden niet worden geladen.';
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