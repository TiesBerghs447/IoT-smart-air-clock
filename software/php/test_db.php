<?php

echo "<pre>";

var_dump($_ENV);

echo "\n\n";

var_dump(getenv("DB_HOST"));
var_dump(getenv("DB_PORT"));
var_dump(getenv("DB_USER"));

echo "</pre>";