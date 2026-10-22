<?php

/*
 * Zet alle instellingen terug naar de defaults en stuurt daarna terug naar de pagina.
 */
require_once __DIR__ . '/config_store.php';
require_once __DIR__ . '/web_auth.php';
web_auth_require();

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    header('Allow: POST');
    exit('Gebruik de fabrieksinstellingenknop op de instellingenpagina.');
}

if (!web_auth_validate_csrf()) {
    http_response_code(400);
    exit('De sessie is verlopen. Vernieuw de pagina en probeer opnieuw.');
}

try {
    save_app_config(include __DIR__ . '/default_config.php');
} catch (RuntimeException $error) {
    error_log($error->getMessage());
    http_response_code(500);
    exit('Fabrieksinstellingen konden niet worden opgeslagen.');
}

header(
    "Location: settings.php"
);

exit();

?>