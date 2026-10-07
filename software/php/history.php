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
            <li><a href="history.php" aria-current="page">Historiek</a></li>
            <li><a href="settings/settings.php">Instellingen</a></li>
        </ul>
    </nav>

    <main class="container">
        <h1 class="page-title">Historiek</h1>
        <p class="subtitle">Historische metingen van de luchtkwaliteit</p>

        <p id="historyStatus" class="subtitle" role="status" aria-live="polite">
            Historiek laden...
        </p>

        <section class="chart-container" aria-labelledby="co2ChartTitle">
            <h2 id="co2ChartTitle">CO₂ (ppm)</h2>
            <canvas id="co2Chart" aria-label="Historische CO₂-metingen" role="img"></canvas>
        </section>

        <section class="chart-container" aria-labelledby="temperatureChartTitle">
            <h2 id="temperatureChartTitle">Temperatuur (°C)</h2>
            <canvas id="temperatureChart" aria-label="Historische temperatuurmetingen" role="img"></canvas>
        </section>

        <section class="chart-container" aria-labelledby="humidityChartTitle">
            <h2 id="humidityChartTitle">Vochtigheid (%)</h2>
            <canvas id="humidityChart" aria-label="Historische vochtigheidsmetingen" role="img"></canvas>
        </section>

        <section class="chart-container" aria-labelledby="pressureChartTitle">
            <h2 id="pressureChartTitle">Luchtdruk (hPa)</h2>
            <canvas id="pressureChart" aria-label="Historische luchtdrukmetingen" role="img"></canvas>
        </section>
    </main>

    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="history.js"></script>
</body>

</html>
