<?php

ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

session_start();


session_start();

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