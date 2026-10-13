<?php

/*
 * Zet alle instellingen terug naar de defaults en stuurt daarna terug naar de pagina.
 */
require_once __DIR__ . '/config_store.php';

save_app_config(include __DIR__ . '/default_config.php');

header(
    "Location: settings.php"
);

exit();

?>