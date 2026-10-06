<?php

return [
    // Set this on the server, or provide ESP_API_KEY as an environment variable.
    "device_api_key" => "",

    // CO2 thresholds in ppm.
    "co2_good_max" => 800,
    "co2_warning_max" => 1200,

    // Sensor upload and dashboard refresh intervals in seconds.
    "upload_interval" => 30,
    "refresh_interval" => 5,
    "config_sync_interval" => 60,

    // Audio settings.
    "audio_enabled" => true,
    "audio_volume" => 20,
    "audio_track" => 1,

    // Alarm time in 24-hour format.
    "alarm_enabled" => true,
    "alarm_hour" => 7,
    "alarm_minute" => 0,

    // Hardware outputs.
    "display_enabled" => true,
    "green_enabled" => true,
    "orange_enabled" => true,
    "red_enabled" => true,
];