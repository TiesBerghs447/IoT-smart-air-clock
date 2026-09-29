let co2Chart;
let tempChart;

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

    if(co2 < 800)
    {
        status.innerText =
            "Uitstekende luchtkwaliteit";

        status.className =
            "good";
    }
    else if(co2 < 1200)
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

async function loadHistory()
{
    try
    {
        const response =
            await fetch("data/history.php");

        if (!response.ok)
        {
            throw new Error("Historiek ophalen mislukt");
        }

        const data =
            await response.json();

        const labels =
            data.map(
                row => row.timestamp
            );

        const co2Values =
            data.map(
                row => row.co2
            );

        const tempValues =
            data.map(
                row => row.temperature
            );

        createCO2Chart(
            labels,
            co2Values
        );

        createTempChart(
            labels,
            tempValues
        );
    }
    catch(error)
    {
        console.log(error);
    }
}

function createCO2Chart(labels, values)
{
    new Chart(
        document.getElementById("co2Chart"),
        {
            type: "line",

            data:
            {
                labels: labels,

                datasets:
                [
                    {
                        label: "CO₂ ppm",

                        data: values,

                        borderColor: "#38bdf8",

                        backgroundColor:
                        "rgba(56,189,248,0.2)",

                        fill: true,

                        tension: 0.4
                    }
                ]
            }
        }
    );
}

function createTempChart(labels, values)
{
    new Chart(
        document.getElementById("tempChart"),
        {
            type: "line",

            data:
            {
                labels: labels,

                datasets:
                [
                    {
                        label:
                        "Temperatuur °C",

                        data: values,

                        borderColor: "#22c55e",

                        backgroundColor:
                        "rgba(34,197,94,0.2)",

                        fill: true,

                        tension: 0.4
                    }
                ]
            }
        }
    );
}

updateData();

loadHistory();

setInterval(
    updateData,
    5000
);