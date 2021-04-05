#include <DHT.h>
#include <LCD_I2C.h>

#define DHTPIN 2    // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22
LCD_I2C lcd(0x27);  // Default address of most PCF8574 modules, change accordingly

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(9600);
  dht.begin();

  lcd.begin(); // If you are using more I2C devices using the Wire library use lcd.begin(false)
  // this stop the library(LCD_I2C) from calling Wire.begin()
  lcd.backlight();


}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  //Serial.print(F("Humidity: "));
  //Serial.print(h);
  //Serial.print(F("%  Temperature: "));
  //Serial.print(t);
  //Serial.print(F("°C "));
  //Serial.print(f);
  //Serial.print(F("°F  Heat index: "));
  //Serial.print(hic);
  //Serial.print(F("°C "));
  //Serial.print(hif);
  //Serial.println(F("°F"));

  lcd.print(F("T:"));
  lcd.print(t);
  lcd.print(F("C,"));
  lcd.print(hic);
  lcd.print(F("C"));
  //lcd.print(f);
  //lcd.print("F");
  lcd.setCursor(0,1);
  lcd.print(F("RH:"));
  lcd.print(h);
  lcd.print(F("%"));
  //lcd.print(F("HI:"));
  
  
  delay(10000);
  lcd.clear();
  //delay(1000);
  //lcd.setCursor(2, 1);
}
