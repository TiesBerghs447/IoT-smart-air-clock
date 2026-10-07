<?php

function database_connection(): PDO
{
    return new PDO(
        "pgsql:host=db.dwnseqlgrlzwjnbqlnoo.supabase.co;port=5432;dbname=postgres",
        "postgres",
        "@/NQXrFzpyS9TXW",
        [
            PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION
        ]
    );
}