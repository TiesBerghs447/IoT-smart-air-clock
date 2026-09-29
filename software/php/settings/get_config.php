<?php
header('Content-Type: application/json');

$config = include('config.php');

echo json_encode($config);

?>