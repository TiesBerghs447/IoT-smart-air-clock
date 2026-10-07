<!DOCTYPE html>
<html lang="nl">

<head>

    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">

    <title>Smart Air Clock</title>

    <link rel="stylesheet" href="stylesheet.css">
</head>

<body>

    <nav class="navbar">

    <div class="logo">
        🌬 Smart Air Clock
    </div>

    <ul class="nav-links">

        <li>
            <a href="index.php">Dashboard</a>
        </li>

        <li>
            <a href="settings/settings.php">Instellingen</a>
        </li>

        <li>
            <a href="history.php">Historiek</a>
        </li>

        <li>
            #
                Systeem
            </a>
        </li>

    </ul>

</nav>

    <div class="container">

        <h1 class="page-title">
            Dashboard
        </h1>

        <p class="subtitle">
            Live luchtkwaliteitsmonitor
        </p>

        <div class="dashboard">

            <div class="card">

                <h2>CO₂</h2>

                <div class="value">
                    <span id="co2">--</span>
                </div>

                <div class="unit">
                    ppm
                </div>

            </div>

            <div class="card">

                <h2>Temperatuur</h2>

                <div class="value">
                    <span id="temperature">--</span>
                </div>

                <div class="unit">
                    °C
                </div>

            </div>

            <div class="card">

                <h2>Vochtigheid</h2>

                <div class="value">
                    <span id="humidity">--</span>
                </div>

                <div class="unit">
                    %
                </div>

            </div>

            <div class="card">

                <h2>Luchtdruk</h2>

                <div class="value">
                    <span id="pressure">--</span>
                </div>

                <div class="unit">
                    hPa
                </div>

            </div>
            <div class="card">

                <h2>Laatste Update</h2>

                <div
                    class="value"
                    id="lastUpdate">

                    --

                </div>

            </div>
            <div class="card">

                <h2>ESP32 Status</h2>

                <div
                    id="deviceStatus"
                    class="value device-status unknown">

                    --

                </div>

            </div>

        </div>

        <div class="status">

            <strong>Status:</strong>

            <span id="statusText" class="good">
                Uitstekend
            </span>

        </div>

    </div>

    <script src="script.js"></script>

</body>

</html>