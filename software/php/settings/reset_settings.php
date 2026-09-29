<?php

copy(
    "default_config.php",
    "config.php"
);

header(
    "Location: settings.php"
);

exit();

?>