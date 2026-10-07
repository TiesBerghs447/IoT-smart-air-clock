<?php

function database_connection(): PDO
{
    return new PDO(
        sprintf(
            "pgsql:host=%s;port=%s;dbname=%s",
            getenv("DB_HOST"),
            getenv("DB_PORT"),
            getenv("DB_NAME")
        ),
        getenv("DB_USER"),
        getenv("DB_PASSWORD"),
        [
            PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION
        ]
    );
}