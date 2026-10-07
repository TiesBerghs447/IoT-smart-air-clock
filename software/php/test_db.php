<?php

echo "HOST: " . getenv("DB_HOST") . "<br>";
echo "PORT: " . getenv("DB_PORT") . "<br>";
echo "DB: " . getenv("DB_NAME") . "<br>";
echo "USER: " . getenv("DB_USER") . "<br>";

require_once __DIR__ . '/settings/database.php';

try
{
    $db = database_connection();

    echo "Database OK";
}
catch(Throwable $e)
{
    echo $e->getMessage();
}
