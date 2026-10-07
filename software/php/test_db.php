<?php

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