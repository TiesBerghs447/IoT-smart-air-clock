let config = null;
let refreshTimer;

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

async function loadConfig()
{
    try
    {
        const response =
            await fetch(
                "settings/get_config.php"
            );

        if (!response.ok)
        {
            throw new Error("Instellingen ophalen mislukt");
        }

        config =
            await response.json();
    }
    catch(error)
    {
        console.log(error);
    }
}

async function updateData()
{
    try
    {
        const response =
            await fetch("data/latest.php");

        if (!response.ok)
        {
            throw new Error("Metingen ophalen mislukt");
        }

        const data =
            await response.json();

        document.getElementById("co2")
            .innerText = data.co2 ?? "--";

        document.getElementById("temperature")
            .innerText = data.temperature ?? "--";

        document.getElementById("humidity")
            .innerText = data.humidity ?? "--";

        document.getElementById("pressure")
            .innerText = data.pressure ?? "--";
        document.getElementById(
            "lastUpdate"
        ).innerText =
            formatTimestamp(data.timestamp);
        const deviceStatus = document.getElementById("deviceStatus");
        const status = data.device_status ?? "unknown";
        deviceStatus.innerText = status === "online"
            ? "Online"
            : status === "offline" ? "Offline" : "Onbekend";
        deviceStatus.className = `value device-status ${status}`;
        deviceStatus.title = data.seconds_since_update === null
            ? "Nog geen meting ontvangen"
            : `Laatste meting ${data.seconds_since_update} seconden geleden`;
        updateStatus(data.co2);
    }
    catch(error)
    {
        console.log(error);
    }
}

function updateStatus(co2)
{
    const status =
        document.getElementById("statusText");

    if (co2 === null)
    {
        status.innerText = "Wachten op meting";
        status.className = "warning";
        return;
    }

    const thresholds = config ?? { co2_good_max: 800, co2_warning_max: 1200 };

    if(co2 < thresholds.co2_good_max)
    {
        status.innerText =
            "Uitstekende luchtkwaliteit";

        status.className =
            "good";
    }
    else if(co2 < thresholds.co2_warning_max)
    {
        status.innerText =
            "Ventilatie aanbevolen";

        status.className =
            "warning";
    }
    else
    {
        status.innerText =
            "Slechte luchtkwaliteit";

        status.className =
            "danger";
    }
}

(async () =>
{
    await loadConfig();

    updateData();

    const refreshSeconds = Number(config?.refresh_interval ?? 5);
    refreshTimer = setInterval(
        updateData,
        Math.min(Math.max(refreshSeconds, 1), 300) * 1000
    );
})();