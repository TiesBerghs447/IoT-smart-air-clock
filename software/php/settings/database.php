<?php

function database_connection(): mysqli
{
    mysqli_report(MYSQLI_REPORT_OFF);

    $connection = new mysqli(
        "sql311.infinityfree.com",
        "if0_43039618",
        "WGy8FBhO7jHv",
        "if0_43039618_smartairclock"
    );

    if ($connection->connect_error) {
        throw new RuntimeException("Database connection failed");
    }

    if (!$connection->set_charset("utf8mb4")) {
        throw new RuntimeException("Database charset setup failed");
    }

    return $connection;
}
