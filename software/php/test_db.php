<?php
echo "VERSION 999";
echo "<pre>";

echo "HOST = " . getenv("DB_HOST") . "\n";
echo "PORT = " . getenv("DB_PORT") . "\n";
echo "USER = " . getenv("DB_USER") . "\n";

echo "</pre>";