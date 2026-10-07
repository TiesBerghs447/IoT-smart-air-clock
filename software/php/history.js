function formatTimestamp(timestamp)
{
    if (!timestamp)
    {
        return "--";
    }

    const value = String(timestamp);
    const dateTime = value.match(/^(\d{4}-\d{2}-\d{2})[ T](\d{2}:\d{2}:\d{2})/);

    return dateTime
        ? `${dateTime[1]} ${dateTime[2]}`
        : value.replace(/\.\d+(?=Z|[+-]\d{2}(?::?\d{2})?$|$)/, "").replace("T", " ");
}

function createHistoryChart(canvasId, label, labels, values, color)
{
    new Chart(
        document.getElementById(canvasId),
        {
            type: "line",
            data: {
                labels,
                datasets: [{
                    label,
                    data: values,
                    borderColor: color,
                    backgroundColor: `${color}33`,
                    fill: true,
                    tension: 0.4
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: true
            }
        }
    );
}

async function loadHistory()
{
    const status = document.getElementById("historyStatus");

    try
    {
        const response = await fetch("data/history.php");

        if (!response.ok)
        {
            throw new Error("Historiek ophalen mislukt");
        }

        const measurements = await response.json();
        const labels = measurements.map(
            measurement => formatTimestamp(measurement.timestamp)
        );

        createHistoryChart(
            "co2Chart",
            "CO₂ (ppm)",
            labels,
            measurements.map(measurement => measurement.co2),
            "#38bdf8"
        );
        createHistoryChart(
            "temperatureChart",
            "Temperatuur (°C)",
            labels,
            measurements.map(measurement => measurement.temperature),
            "#22c55e"
        );
        createHistoryChart(
            "humidityChart",
            "Vochtigheid (%)",
            labels,
            measurements.map(measurement => measurement.humidity),
            "#a855f7"
        );
        createHistoryChart(
            "pressureChart",
            "Luchtdruk (hPa)",
            labels,
            measurements.map(measurement => measurement.pressure),
            "#f97316"
        );

        status.textContent = measurements.length
            ? `De laatste ${measurements.length} metingen worden getoond.`
            : "Er zijn nog geen metingen beschikbaar.";
    }
    catch (error)
    {
        console.error(error);
        status.textContent = "De historiek kon niet worden geladen. Probeer het later opnieuw.";
    }
}

loadHistory();
