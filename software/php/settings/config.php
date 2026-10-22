<?php

/*
 * Serverinstellingen en terugvalwaarden.
 * Alarmen worden niet meer hier opgeslagen,
 * maar in de PostgreSQL tabel "alarms".
 */

return [

    // API sleutel voor de ESP32
    "device_api_key" =>
        "smartairclock2026api123456789012345",

    // CO2 drempelwaarden
    "co2_good_max" =>
        800,

    "co2_warning_max" =>
        1200,

    // Intervallen (seconden)
    "upload_interval" =>
        30,

    "refresh_interval" =>
        5,

    "config_sync_interval" =>
        60,

    // Audio
    "audio_enabled" =>
        true,

    "audio_volume" =>
        20,

    "audio_track" =>
        1,

    // Hardware
    "display_enabled" =>
        true,

    "green_enabled" =>
        true,

    "orange_enabled" =>
        true,

    "red_enabled" =>
        true
];