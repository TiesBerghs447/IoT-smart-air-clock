const chartDefinitions = {
    co2Chart: { label: "CO₂ (ppm)", key: "co2", color: "#38bdf8" },
    temperatureChart: { label: "Temperatuur (°C)", key: "temperature", color: "#22c55e" },
    humidityChart: { label: "Vochtigheid (%)", key: "humidity", color: "#a855f7" },
    pressureChart: { label: "Luchtdruk (hPa)", key: "pressure", color: "#f97316" }
};

const charts = new Map();

function formatTimestamp(timestamp, showDate)
{
    const match = String(timestamp).match(/^(\d{4})-(\d{2})-(\d{2})[ T](\d{2}:\d{2})/);

    if (!match)
    {
        return "Datum onbekend";
    }

    return showDate
        ? `${match[3]}-${match[2]} ${match[4]}`
        : match[4];
}

function formatTooltipTimestamp(timestamp)
{
    const match = String(timestamp).match(/^(\d{4})-(\d{2})-(\d{2})[ T](\d{2}:\d{2})/);
    return match
        ? `${match[3]}-${match[2]}-${match[1]} ${match[4]}`
        : "Datum onbekend";
}

function renderChart(canvasId, measurements, showDate)
{
    const definition = chartDefinitions[canvasId];
    const labels = measurements.map(
        measurement => formatTimestamp(measurement.timestamp, showDate)
    );
    const data = {
        labels,
        datasets: [{
            label: definition.label,
            data: measurements.map(measurement => Number(measurement[definition.key])),
            borderColor: definition.color,
            backgroundColor: `${definition.color}22`,
            borderWidth: 2,
            fill: true,
            tension: 0.35,
            cubicInterpolationMode: "monotone",
            pointRadius: 0,
            pointHoverRadius: 4,
            pointHitRadius: 8
        }]
    };

    const options = {
        responsive: true,
        maintainAspectRatio: true,
        interaction: {
            intersect: false,
            mode: "index"
        },
        plugins: {
            tooltip: {
                callbacks: {
                    title(items)
                    {
                        const measurement = measurements[items[0].dataIndex];
                        return formatTooltipTimestamp(measurement.timestamp);
                    }
                }
            }
        },
        scales: {
            x: {
                ticks: {
                    autoSkip: true,
                    maxTicksLimit: 6,
                    maxRotation: 0,
                    minRotation: 0
                }
            }
        }
    };

    if (charts.has(canvasId))
    {
        charts.get(canvasId).destroy();
    }

    charts.set(canvasId, new Chart(
        document.getElementById(canvasId),
        { type: "line", data, options }
    ));
}

async function loadHistory(controls)
{
    const canvasId = controls.dataset.chart;
    const chart = chartDefinitions[canvasId];
    const status = controls.closest(".chart-container").querySelector(".chart-status");
    const range = controls.querySelector(".history-range").value;
    const parameters = new URLSearchParams({ range });
    let showDate = range === "week" || range === "month";

    if (range === "custom")
    {
        const start = controls.querySelector(".history-start").value;
        const end = controls.querySelector(".history-end").value;

        if (!start || !end)
        {
            status.textContent = "Vul een begin- en einddatum in.";
            return;
        }

        if (start > end)
        {
            status.textContent = "De begindatum moet op of voor de einddatum liggen.";
            return;
        }

        parameters.set("start", start);
        parameters.set("end", end);
        showDate = start !== end;
    }

    status.textContent = "Historiek laden...";

    try
    {
        const response = await fetch(`data/history.php?${parameters.toString()}`, {
            cache: "no-store"
        });
        const result = await response.json();

        if (!response.ok)
        {
            throw new Error(result.error || "Historiek ophalen mislukt.");
        }

        renderChart(canvasId, result, showDate);
        status.textContent = result.length
            ? `${result.length} meetpunten getoond.`
            : "Er zijn geen metingen voor deze periode.";
    }
    catch (error)
    {
        console.error(error);
        status.textContent = error.message || "De historiek kon niet worden geladen. Probeer het later opnieuw.";
    }
}

document.querySelectorAll(".history-controls").forEach(controls =>
{
    const rangeSelect = controls.querySelector(".history-range");
    const customRange = controls.querySelector(".history-custom-range");

    controls.addEventListener("submit", event =>
    {
        event.preventDefault();
    });

    rangeSelect.addEventListener("change", () =>
    {
        const isCustom = rangeSelect.value === "custom";
        customRange.hidden = !isCustom;

        if (!isCustom)
        {
            loadHistory(controls);
        }
    });

    controls.querySelector(".history-apply").addEventListener("click", () =>
    {
        loadHistory(controls);
    });

    loadHistory(controls);
});
