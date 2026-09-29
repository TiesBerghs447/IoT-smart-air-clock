<?php

$config = include("config.php");

if ($_SERVER["REQUEST_METHOD"] == "POST")
{
    $config["co2_good_max"] =
        intval($_POST["co2_good_max"]);

    $config["co2_warning_max"] =
        intval($_POST["co2_warning_max"]);

    $config["upload_interval"] =
        intval($_POST["upload_interval"]);

    $config["refresh_interval"] =
        intval($_POST["refresh_interval"]);

    $config["audio_volume"] =
        intval($_POST["audio_volume"]);

    $config["alarm_hour"] =
        intval($_POST["alarm_hour"]);

    $config["alarm_minute"] =
        intval($_POST["alarm_minute"]);

    $config["audio_enabled"] =
        isset($_POST["audio_enabled"]);

    $config["alarm_enabled"] =
        isset($_POST["alarm_enabled"]);

    $config["display_enabled"] =
        isset($_POST["display_enabled"]);

    $config["green_enabled"] =
        isset($_POST["green_enabled"]);

    $config["orange_enabled"] =
        isset($_POST["orange_enabled"]);

    $config["red_enabled"] =
        isset($_POST["red_enabled"]);

    $content =
        "<?php\n\nreturn "
        . var_export($config, true)
        . ";\n";

    file_put_contents(
        "config.php",
        $content
    );

    $saved = true;
}

?>

<!DOCTYPE html>
<html lang="nl">

<head>

<meta charset="UTF-8">

<title>Instellingen</title>

<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="stylesheet" href="../stylesheet.css">

</head>

<body>

<nav class="navbar">

    <div class="logo">
        🌬 Smart Air Clock
    </div>

    <ul class="nav-links">
        <li><a href="../index.php">Dashboard</a></li>
        <li><a href="settings.php" aria-current="page">Instellingen</a></li>
    </ul>

</nav>

<div class="settings-container">

<h1>⚙ Instellingen</h1>

<?php if(isset($saved)): ?>
<div class="success">
    Instellingen opgeslagen
</div>
<?php endif; ?>

<form method="POST">

<div class="settings-card">

<h2>CO₂ Grenzen</h2>

<label>Groen tot</label>
<input
type="number"
name="co2_good_max"
value="<?= $config['co2_good_max']; ?>">

<label>Oranje tot</label>
<input
type="number"
name="co2_warning_max"
value="<?= $config['co2_warning_max']; ?>">

</div>

<div class="settings-card">

<h2>Upload</h2>

<label>Upload interval (s)</label>

<input
type="number"
name="upload_interval"
value="<?= $config['upload_interval']; ?>">

</div>

<div class="settings-card">

<h2>Dashboard</h2>

<label>Refresh interval (s)</label>

<input
type="number"
name="refresh_interval"
value="<?= $config['refresh_interval']; ?>">

</div>

<div class="settings-card">

<h2>Audio</h2>

<label>
<input
type="checkbox"
name="audio_enabled"
<?= $config['audio_enabled'] ? 'checked' : ''; ?>>
 Audio actief
</label>

<label>Volume</label>

<input
type="number"
min="0"
max="30"
name="audio_volume"
value="<?= $config['audio_volume']; ?>">

</div>

<div class="settings-card">

<h2>Alarm</h2>

<label>
<input
type="checkbox"
name="alarm_enabled"
<?= $config['alarm_enabled'] ? 'checked' : ''; ?>>
 Alarm actief
</label>

<label>Uur</label>

<input
type="number"
min="0"
max="23"
name="alarm_hour"
value="<?= $config['alarm_hour']; ?>">

<label>Minuut</label>

<input
type="number"
min="0"
max="59"
name="alarm_minute"
value="<?= $config['alarm_minute']; ?>">

</div>

<div class="settings-card">

<h2>RGB LED</h2>

<label><input type="checkbox" name="green_enabled" <?= $config['green_enabled'] ? 'checked' : ''; ?>> Groen</label>

<label><input type="checkbox" name="orange_enabled" <?= $config['orange_enabled'] ? 'checked' : ''; ?>> Oranje</label>

<label><input type="checkbox" name="red_enabled" <?= $config['red_enabled'] ? 'checked' : ''; ?>> Rood</label>

</div>

<br>

<button class="save-btn" type="submit">

Opslaan

</button>

reset_settings.php

<button
type="button"
class="reset-btn">

Fabrieksinstellingen

</button>

</a>

</form>

</div>

</body>

</html>