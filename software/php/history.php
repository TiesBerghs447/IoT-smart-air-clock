<?php

require_once __DIR__ . '/settings/web_auth.php';
web_auth_require();
$webAuthCsrfToken = web_auth_csrf_token();
?>
<!DOCTYPE html>
<html lang="nl">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Historiek | Smart Air Clock</title>
    <link rel="stylesheet" href="stylesheet.css">
</head>

<body>
    <nav class="navbar">
        <div class="logo">
            🌬 Smart Air Clock
        </div>

        <ul class="nav-links">
            <li><a href="index.php">Dashboard</a></li>
            <li><a href="alarms.php">Alarmen</a></li>
            <li><a href="history.php" aria-current="page">Historiek</a></li>
            <li><a href="settings/settings.php">Instellingen</a></li>
            <li>
                <form method="post" action="logout.php">
                    <input type="hidden" name="csrf_token" value="<?= htmlspecialchars($webAuthCsrfToken, ENT_QUOTES, 'UTF-8'); ?>">
                    <button class="logout-btn" type="submit">Uitloggen</button>
                </form>
            </li>
        </ul>
    </nav>

    <main class="container">
        <h1 class="page-title">Historiek</h1>
        <p class="subtitle">Kies per grafiek een periode of stel zelf datums in.</p>
        <section class="chart-container" aria-labelledby="co2ChartTitle">
            <div class="chart-heading">
                <h2 id="co2ChartTitle">CO₂ (ppm)</h2>
                <form class="history-controls" data-chart="co2Chart">
                    <label class="sr-only" for="co2Range">Periode voor CO₂</label>
                    <select id="co2Range" class="history-range">
                        <option value="hour">Afgelopen uur</option>
                        <option value="day">Afgelopen dag</option>
                        <option value="week">Afgelopen week</option>
                        <option value="month" selected>Afgelopen maand</option>
                        <option value="custom">Zelf datums kiezen</option>
                    </select>
                    <div class="history-custom-range" hidden>
                        <label>Van <input class="history-start" type="date" aria-label="Begindatum voor CO₂"></label>
                        <label>Tot en met <input class="history-end" type="date" aria-label="Einddatum voor CO₂"></label>
                        <button class="history-apply" type="button">Toon</button>
                    </div>
                </form>
            </div>
            <p class="chart-status" role="status" aria-live="polite">Historiek laden...</p>
            <canvas id="co2Chart" aria-label="Historische CO₂-metingen" role="img"></canvas>
        </section>

        <section class="chart-container" aria-labelledby="temperatureChartTitle">
            <div class="chart-heading">
                <h2 id="temperatureChartTitle">Temperatuur (°C)</h2>
                <form class="history-controls" data-chart="temperatureChart">
                    <label class="sr-only" for="temperatureRange">Periode voor temperatuur</label>
                    <select id="temperatureRange" class="history-range">
                        <option value="hour">Afgelopen uur</option>
                        <option value="day">Afgelopen dag</option>
                        <option value="week">Afgelopen week</option>
                        <option value="month" selected>Afgelopen maand</option>
                        <option value="custom">Zelf datums kiezen</option>
                    </select>
                    <div class="history-custom-range" hidden>
                        <label>Van <input class="history-start" type="date" aria-label="Begindatum voor temperatuur"></label>
                        <label>Tot en met <input class="history-end" type="date" aria-label="Einddatum voor temperatuur"></label>
                        <button class="history-apply" type="button">Toon</button>
                    </div>
                </form>
            </div>
            <p class="chart-status" role="status" aria-live="polite">Historiek laden...</p>
            <canvas id="temperatureChart" aria-label="Historische temperatuurmetingen" role="img"></canvas>
        </section>

        <section class="chart-container" aria-labelledby="humidityChartTitle">
            <div class="chart-heading">
                <h2 id="humidityChartTitle">Vochtigheid (%)</h2>
                <form class="history-controls" data-chart="humidityChart">
                    <label class="sr-only" for="humidityRange">Periode voor vochtigheid</label>
                    <select id="humidityRange" class="history-range">
                        <option value="hour">Afgelopen uur</option>
                        <option value="day">Afgelopen dag</option>
                        <option value="week">Afgelopen week</option>
                        <option value="month" selected>Afgelopen maand</option>
                        <option value="custom">Zelf datums kiezen</option>
                    </select>
                    <div class="history-custom-range" hidden>
                        <label>Van <input class="history-start" type="date" aria-label="Begindatum voor vochtigheid"></label>
                        <label>Tot en met <input class="history-end" type="date" aria-label="Einddatum voor vochtigheid"></label>
                        <button class="history-apply" type="button">Toon</button>
                    </div>
                </form>
            </div>
            <p class="chart-status" role="status" aria-live="polite">Historiek laden...</p>
            <canvas id="humidityChart" aria-label="Historische vochtigheidsmetingen" role="img"></canvas>
        </section>

        <section class="chart-container" aria-labelledby="pressureChartTitle">
            <div class="chart-heading">
                <h2 id="pressureChartTitle">Luchtdruk (hPa)</h2>
                <form class="history-controls" data-chart="pressureChart">
                    <label class="sr-only" for="pressureRange">Periode voor luchtdruk</label>
                    <select id="pressureRange" class="history-range">
                        <option value="hour">Afgelopen uur</option>
                        <option value="day">Afgelopen dag</option>
                        <option value="week">Afgelopen week</option>
                        <option value="month" selected>Afgelopen maand</option>
                        <option value="custom">Zelf datums kiezen</option>
                    </select>
                    <div class="history-custom-range" hidden>
                        <label>Van <input class="history-start" type="date" aria-label="Begindatum voor luchtdruk"></label>
                        <label>Tot en met <input class="history-end" type="date" aria-label="Einddatum voor luchtdruk"></label>
                        <button class="history-apply" type="button">Toon</button>
                    </div>
                </form>
            </div>
            <p class="chart-status" role="status" aria-live="polite">Historiek laden...</p>
            <canvas id="pressureChart" aria-label="Historische luchtdrukmetingen" role="img"></canvas>
        </section>
    </main>

    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="history.js"></script>
</body>

</html>
