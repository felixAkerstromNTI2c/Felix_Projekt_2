#include "arduino_secrets.h"
#include "thingProperties.h"
#include <Wire.h>
#include "Adafruit_SGP30.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme; // I2C
Adafruit_SGP30 sgp; // I2C

/*
Formel som beräknar den absoluta luftfuktigheten utifrån den relativa luftfuktigheten 
och temperaturen. Används av gassensornn för att förbättra dess noggranhet. 
*/
uint32_t getAbsoluteHumidity(float temperature, float humidity) {
  const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
  const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity);
  return absoluteHumidityScaled;
}

void setup() {
  Serial.begin(115200);
  delay(1500);

  // Definerad i thingProperties.h
  initProperties();

  // Koppla till Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  setDebugMessageLevel(2); // Felsökning av WIFI koppling
  ArduinoCloud.printDebugInfo(); // Felsökning

  bme.begin(0x77); // I2C adress
  sgp.begin();
}

int counter = 0;

void loop() {
  Serial.print("void loop");
  ArduinoCloud.update();
  updateReadings(); // Kör funktionen som uppdaterar variablerna

  delay(1500);

  /*
  if-satserna kollar om mätningarna lyckats och skriver ut om det misslyckats i seial monitorn
  */
  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }
  if (! sgp.IAQmeasureRaw()) {
    Serial.println("Raw Measurement failed");
    return;
  }
  

  delay(1000);

}

/*
Funktion som läser av värdena för klimatet och tilldelar de avläsna värdena 
till de variabler som uppdateras till IoT Cloud.
Returnerar: void
*/
void updateReadings(){
  air_quality = sgp.TVOC;
  co2 = sgp.eCO2;
  temp = bme.readTemperature();
  hum = bme.readHumidity();
  pres= bme.readPressure() / 100.0F;
  
// Uppdaterar värdena temp och hum värdena till gassensorn, vilket förbättrar dess noggranhet.
  sgp.setHumidity(getAbsoluteHumidity(temp, hum)); 
}










