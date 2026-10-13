const colorInput = document.getElementById("rgb-color");
const colorStatus = document.getElementById("rgb-status");

function toHex(value) {
    const channel = Math.max(0, Math.min(255, Number(value) || 0));
    return channel.toString(16).padStart(2, "0");
}

function setStatus(message) {
    colorStatus.textContent = message;
}

async function loadColor() {
    try {
        const response = await fetch("../data/get_commando.php");
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
        }

        const command = await response.json();
        colorInput.value = `#${toHex(command.red)}${toHex(command.green)}${toHex(command.blue)}`;
        setStatus(
            command.led_mode === "remote"
                ? `Actieve kleur: ${colorInput.value.toUpperCase()}`
                : "Selecteer een kleur om de LED te bedienen."
        );
    } catch (error) {
        setStatus("Huidige kleur kon niet worden geladen.");
    }
}

colorInput.addEventListener("change", async () => {
    const red = parseInt(colorInput.value.slice(1, 3), 16);
    const green = parseInt(colorInput.value.slice(3, 5), 16);
    const blue = parseInt(colorInput.value.slice(5, 7), 16);

    setStatus("Kleur wordt verstuurd...");

    try {
        const response = await fetch("../data/set_commando.php", {
            method: "POST",
            headers: {
                "Content-Type": "application/x-www-form-urlencoded"
            },
            body: new URLSearchParams({
                led_mode: "remote",
                red: String(red),
                green: String(green),
                blue: String(blue)
            })
        });

        const result = await response.json();
        if (!response.ok || result.status !== "ok") {
            throw new Error(result.error || `HTTP ${response.status}`);
        }

        setStatus(`Kleur verzonden: ${colorInput.value.toUpperCase()}`);
    } catch (error) {
        setStatus("Kleur kon niet worden verstuurd.");
    }
});

loadColor();
