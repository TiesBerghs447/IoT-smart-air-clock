<?php

/*
 * Centrale opslag voor website-instellingen: leest defaults en overschrijft
 * die met settings/config.json of het pad uit SETTINGS_FILE.
 */
function app_config_path(): string
{
    $configuredPath = getenv('SETTINGS_FILE');

    return is_string($configuredPath) && $configuredPath !== ''
        ? $configuredPath
        : __DIR__ . '/config.json';
}

function load_app_config(): array
{
    $config = include __DIR__ . '/config.php';
    $configPath = app_config_path();

    if (!is_file($configPath)) {
        return $config;
    }

    $contents = file_get_contents($configPath);
    if ($contents === false) {
        throw new RuntimeException('Instellingenbestand kon niet worden gelezen.');
    }

    try {
        $storedConfig = json_decode($contents, true, 512, JSON_THROW_ON_ERROR);
    } catch (JsonException $error) {
        throw new RuntimeException('Instellingenbestand bevat ongeldige JSON.', 0, $error);
    }

    if (!is_array($storedConfig)) {
        throw new RuntimeException('Instellingenbestand moet een JSON-object bevatten.');
    }

    $allowedSettings = include __DIR__ . '/default_config.php';
    unset($allowedSettings['device_api_key'], $storedConfig['device_api_key']);
    $storedConfig = array_intersect_key($storedConfig, $allowedSettings);

    return array_merge($config, $storedConfig);
}

function save_app_config(array $config): void
{
    $defaults = include __DIR__ . '/default_config.php';
    unset($defaults['device_api_key'], $config['device_api_key']);
    $settings = array_intersect_key($config, $defaults);
    $json = json_encode($settings, JSON_PRETTY_PRINT | JSON_THROW_ON_ERROR);
    $configPath = app_config_path();

    $writeWarning = null;
    set_error_handler(
        static function ($severity, $message) use (&$writeWarning) {
            $writeWarning = $message;
            return true;
        }
    );

    $bytesWritten = file_put_contents($configPath, $json . PHP_EOL, LOCK_EX);
    restore_error_handler();

    if ($bytesWritten === false) {
        if ($writeWarning !== null) {
            error_log('Instellingen opslaan mislukt: ' . $writeWarning);
        }

        throw new RuntimeException(
            'Instellingen konden niet worden opgeslagen. Controleer of de map van SETTINGS_FILE schrijfbaar is.'
        );
    }
}
