library al geinstaleerd van tobias

aansluitschema met pwm:
MH-Z19B      ESP32-S3
---------------------
VCC      -> 5V
GND      -> GND

TX       -> GPIO16
RX       -> GPIO17

PWM      -> GPIO18 (optioneel)

code met pwm:
#include <SoftwareSerial.h>
#include <MHZ.h>

// PWM pin (optioneel)
#define CO2_IN 18

// UART pinnen ESP32-S3
#define MH_Z19_RX 16
#define MH_Z19_TX 17

MHZ co2(MH_Z19_RX, MH_Z19_TX, CO2_IN, MHZ19B);

void setup() {
  Serial.begin(115200);

  pinMode(CO2_IN, INPUT);

  delay(100);

  Serial.println("MH-Z19B CO2 Sensor");

  // Debug inschakelen indien nodig
  // co2.setDebug(true);

  if (co2.isPreHeating()) {
    Serial.print("Preheating");

    while (co2.isPreHeating()) {
      Serial.print(".");
      delay(5000);
    }

    Serial.println();
    Serial.println("Sensor klaar!");
  }
}

void loop() {

  Serial.print("\n----- Tijd sinds opstart: ");
  Serial.print(millis() / 1000);
  Serial.println(" s -----");

  int ppm_uart = co2.readCO2UART();

  Serial.print("CO2 (UART): ");

  if (ppm_uart > 0) {
    Serial.print(ppm_uart);
    Serial.println(" ppm");
  } else {
    Serial.println("n/a");
  }

  int ppm_pwm = co2.readCO2PWM();

  Serial.print("CO2 (PWM): ");
  Serial.print(ppm_pwm);
  Serial.println(" ppm");

  int temperature = co2.getLastTemperature();

  Serial.print("Sensor temperatuur: ");

  if (temperature > 0) {
    Serial.print(temperature);
    Serial.println(" °C");
  } else {
    Serial.println("n/a");
  }

  delay(5000);
}

aansluitschema zonder pwm:
MH-Z19B      ESP32-S3
---------------------
VCC      -> 5V
GND      -> GND
TX       -> GPIO16
RX       -> GPIO17

code zonder pwm:
#include <MHZ.h>

// RX = ESP32 ontvangt van TX MH-Z19B
// TX = ESP32 verstuurt naar RX MH-Z19B
#define MH_Z19_RX 16
#define MH_Z19_TX 17

MHZ co2(MH_Z19_RX, MH_Z19_TX, MHZ19B);

void setup() {
  Serial.begin(115200);

  delay(100);

  Serial.println("MH-Z19B");

  if (co2.isPreHeating()) {
    Serial.print("Preheating");

    while (co2.isPreHeating()) {
      Serial.print(".");
      delay(5000);
    }

    Serial.println();
    Serial.println("Sensor klaar!");
  }
}

void loop() {

  int ppm = co2.readCO2UART();

  Serial.print("CO2: ");

  if (ppm > 0) {
    Serial.print(ppm);
    Serial.println(" ppm");
  } else {
    Serial.println("n/a");
  }

  int temperature = co2.getLastTemperature();

  Serial.print("Sensor temperatuur: ");

  if (temperature > 0) {
    Serial.print(temperature);
    Serial.println(" C");
  } else {
    Serial.println("n/a");
  }

  Serial.println("----------------");

  delay(5000);
}