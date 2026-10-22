<?php

function web_auth_start_session(): void
{
    if (session_status() !== PHP_SESSION_NONE) {
        return;
    }

    $isHttps =
        (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off')
        || ($_SERVER['HTTP_X_FORWARDED_PROTO'] ?? '') === 'https';

    session_set_cookie_params([
        'httponly' => true,
        'secure' => $isHttps,
        'samesite' => 'Lax'
    ]);

    session_start();
}

function web_auth_app_path(): string
{
    $scriptName =
        str_replace('\\', '/', $_SERVER['SCRIPT_NAME'] ?? '/index.php');

    $directory =
        dirname($scriptName);

    if (
        basename($directory) === 'settings'
        || basename($directory) === 'data'
    ) {
        $directory = dirname($directory);
    }

    $directory =
        str_replace('\\', '/', $directory);

    return rtrim($directory, '/');
}

function web_auth_csrf_token(): string
{
    web_auth_start_session();

    if (!isset($_SESSION['web_auth_csrf_token'])) {
        $_SESSION['web_auth_csrf_token'] =
            bin2hex(random_bytes(32));
    }

    return $_SESSION['web_auth_csrf_token'];
}

function web_auth_require(bool $jsonResponse = false): void
{
    web_auth_start_session();

    header('Cache-Control: no-store, no-cache, must-revalidate, max-age=0');

    if (!empty($_SESSION['web_authenticated'])) {
        return;
    }

    if ($jsonResponse) {
        http_response_code(401);
        header('Content-Type: application/json; charset=utf-8');
        echo json_encode(['error' => 'Log in om deze gegevens te bekijken.']);
        exit;
    }

    $requestUri =
        $_SERVER['REQUEST_URI'] ?? '';

    if (
        is_string($requestUri)
        && str_starts_with($requestUri, '/')
        && !str_starts_with($requestUri, '//')
    ) {
        $_SESSION['web_auth_return_to'] = $requestUri;
    }

    header('Location: ' . web_auth_app_path() . '/login.php');
    exit;
}

function web_auth_validate_csrf(): bool
{
    web_auth_start_session();

    $submittedToken =
        $_POST['csrf_token'] ?? '';

    return is_string($submittedToken)
        && isset($_SESSION['web_auth_csrf_token'])
        && hash_equals($_SESSION['web_auth_csrf_token'], $submittedToken);
}
