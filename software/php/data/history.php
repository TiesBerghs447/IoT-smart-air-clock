<?php

header('Content-Type: application/json; charset=utf-8');

require_once __DIR__ . '/../settings/web_auth.php';
web_auth_require(true);

require_once __DIR__ . '/../settings/database.php';

$range = $_GET['range'] ?? 'month';

if (!is_string($range)) {
    http_response_code(400);
    echo json_encode(['error' => 'Ongeldige periode.']);
    exit;
}

$periods = [
    'hour' => ['seconds' => 3600, 'interval' => '1 hour'],
    'day' => ['seconds' => 86400, 'interval' => '1 day'],
    'week' => ['seconds' => 604800, 'interval' => '7 days'],
    'month' => ['seconds' => 2592000, 'interval' => '30 days'],
];

if ($range === 'custom') {
    $startInput = $_GET['start'] ?? null;
    $endInput = $_GET['end'] ?? null;

    if (!is_string($startInput) || !is_string($endInput)) {
        http_response_code(400);
        echo json_encode(['error' => 'Vul een begin- en einddatum in.']);
        exit;
    }

    $start = DateTimeImmutable::createFromFormat('!Y-m-d', $startInput);
    $startErrors = DateTimeImmutable::getLastErrors();
    $end = DateTimeImmutable::createFromFormat('!Y-m-d', $endInput);
    $endErrors = DateTimeImmutable::getLastErrors();

    if (
        !$start
        || !$end
        || $start->format('Y-m-d') !== $startInput
        || $end->format('Y-m-d') !== $endInput
        || ($startErrors && ($startErrors['warning_count'] || $startErrors['error_count']))
        || ($endErrors && ($endErrors['warning_count'] || $endErrors['error_count']))
        || $start > $end
    ) {
        http_response_code(400);
        echo json_encode(['error' => 'Kies een geldig datumbereik.']);
        exit;
    }

    $endExclusive = $end->modify('+1 day');
    $rangeSeconds = $endExclusive->getTimestamp() - $start->getTimestamp();
    $bucketSeconds = max(1, (int)ceil($rangeSeconds / 1200));
    $whereClause = 'timestamp >= ? AND timestamp < ?';
    $queryParameters = [
        $bucketSeconds,
        $start->format('Y-m-d H:i:s'),
        $endExclusive->format('Y-m-d H:i:s'),
        $bucketSeconds,
    ];
} elseif (isset($periods[$range])) {
    $period = $periods[$range];
    $bucketSeconds = max(1, (int)ceil($period['seconds'] / 1200));
    $whereClause = "timestamp >= CURRENT_TIMESTAMP - INTERVAL '{$period['interval']}'";
    $queryParameters = [$bucketSeconds, $bucketSeconds];
} else {
    http_response_code(400);
    echo json_encode(['error' => 'Ongeldige periode.']);
    exit;
}

try
{
    $connection = database_connection();
    $statement = $connection->prepare(
        "WITH samples AS (
            SELECT
                FLOOR(EXTRACT(EPOCH FROM timestamp) / CAST(? AS DOUBLE PRECISION))::bigint AS bucket,
                temperature,
                humidity,
                pressure,
                co2
            FROM measurements
            WHERE $whereClause
        )
        SELECT
            TO_CHAR(TO_TIMESTAMP(bucket * CAST(? AS DOUBLE PRECISION)), 'YYYY-MM-DD HH24:MI') AS timestamp,
            AVG(temperature) AS temperature,
            AVG(humidity) AS humidity,
            AVG(pressure) AS pressure,
            AVG(co2) AS co2
        FROM samples
        GROUP BY bucket
        ORDER BY bucket ASC"
    );
    $statement->execute($queryParameters);
    $measurements = $statement->fetchAll(PDO::FETCH_ASSOC);

    echo json_encode($measurements);
}
catch (Throwable $error)
{
    error_log('Historiek ophalen mislukt: ' . $error->getMessage());
    http_response_code(500);
    echo json_encode([
        'error' => 'De historiek kon niet worden opgehaald.'
    ]);
}