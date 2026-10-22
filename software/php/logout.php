<?php

require_once __DIR__ . '/settings/web_auth.php';
web_auth_start_session();

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    header('Allow: POST');
    exit('Gebruik de uitlogknop op de website.');
}

if (!web_auth_validate_csrf()) {
    http_response_code(400);
    exit('De sessie is verlopen. Vernieuw de pagina en probeer opnieuw.');
}

$_SESSION = [];

if (ini_get('session.use_cookies')) {
    $cookie = session_get_cookie_params();
    setcookie(
        session_name(),
        '',
        time() - 42000,
        $cookie['path'],
        $cookie['domain'],
        $cookie['secure'],
        $cookie['httponly']
    );
}

session_destroy();
header('Location: ' . web_auth_app_path() . '/login.php');
exit;
