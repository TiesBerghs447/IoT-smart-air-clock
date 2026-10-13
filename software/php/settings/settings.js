const colorInput = document.getElementById("rgb-color");
const colorStatus = document.getElementById("rgb-status");
const remoteToggle = document.getElementById("remote-toggle");
let remoteMode = false;

function toHex(value) {
    const channel = Math.max(0, Math.min(255, Number(value) || 0));
    return channel.toString(16).padStart(2, "0");
}

function setStatus(message) {
    colorStatus.textContent = message;
}

function updateRemoteButton() {
    remoteToggle.textContent = remoteMode
        ? "Remote mode uitschakelen"
        : "Remote mode inschakelen";
    remoteToggle.setAttribute("aria-pressed", String(remoteMode));
}

async function sendCommand(mode) {
    const red = parseInt(colorInput.value.slice(1, 3), 16);
    const green = parseInt(colorInput.value.slice(3, 5), 16);
    const blue = parseInt(colorInput.value.slice(5, 7), 16);

    const response = await fetch("../data/set_commando.php", {
        method: "POST",
        headers: {
            "Content-Type": "application/x-www-form-urlencoded"
        },
        body: new URLSearchParams({
            led_mode: mode,
            red: String(red),
            green: String(green),
            blue: String(blue)
        })
    });

    const result = await response.json();
    if (!response.ok || result.status !== "ok") {
        throw new Error(result.error || `HTTP ${response.status}`);
    }
}

async function loadColor() {
    try {
        const response = await fetch("../data/get_commando.php");
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
        }

        const command = await response.json();
        colorInput.value = `#${toHex(command.red)}${toHex(command.green)}${toHex(command.blue)}`;
        remoteMode = command.led_mode === "remote";
        updateRemoteButton();
        setStatus(`Huidige kleur: ${colorInput.value.toUpperCase()}`);
    } catch (error) {
        setStatus(`Huidige kleur kon niet worden geladen: ${error.message}`);
    }
}

colorInput.addEventListener("change", async () => {
    setStatus("Kleur wordt verstuurd...");

    try {
        await sendCommand(remoteMode ? "remote" : "co2");
        setStatus(`Kleur verzonden: ${colorInput.value.toUpperCase()}`);
    } catch (error) {
        setStatus(`Kleur kon niet worden verstuurd: ${error.message}`);
    }
});

remoteToggle.addEventListener("click", async () => {
    const nextMode = !remoteMode;
    setStatus("Remote mode wordt bijgewerkt...");

    try {
        await sendCommand(nextMode ? "remote" : "co2");
        remoteMode = nextMode;
        updateRemoteButton();
        setStatus(
            remoteMode
                ? `Remote mode actief: ${colorInput.value.toUpperCase()}`
                : "Remote mode uitgeschakeld; de LED volgt de CO₂-waarde."
        );
    } catch (error) {
        setStatus(`Remote mode kon niet worden bijgewerkt: ${error.message}`);
    }
});

updateRemoteButton();
loadColor();
