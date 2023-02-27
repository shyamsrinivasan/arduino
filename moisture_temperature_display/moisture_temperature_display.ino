#include <LCD_I2C.h>
#include <DHT.h>

#define sensorPin A0
#define DHTPIN 2
#define DHTTYPE DHT22

LCD_I2C lcd(0x27);  // default LCD I2C address
DHT dht(DHTPIN, DHTTYPE);  // initialize DHT sensor

const int dryValue = 634; // moisture sensor dry value
const int wetValue = 321; // moisture sensor wet value
int intervals = (dryValue - wetValue)/3;  // intervals very wet, wet, dry

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin();
  lcd.backlight();
}

void loop() {  
  
  delay(3000);  // delay between measurements
  float cT = dht.readTemperature();
  float h = dht.readHumidity();
  float fT = dht.readTemperature(true);
  // check read failure of DHT sensor
  if (isnan(cT) || isnan(fT) || isnan(h)){
    lcd.print(F("DHT Failure"));    
  }
  else{
    lcd.print(cT);
    lcd.print("C, ");
    lcd.print(h);
    lcd.print("%");
  }

  int moisture = readSensor();
  // check read failure of Soil moisture sensor
  if (isnan(moisture)){
    lcd.setCursor(0, 1);
    lcd.print(F("Soil Sense Err"));
  }

  float moisture_percent = map(moisture, 634, 321, 0, 100);
  String moisture_category;
  int moisture_level;

  if(moisture > wetValue && moisture < (wetValue + intervals))
  {
    moisture_category = "Very Wet";
    moisture_level = 3;
  }
  else if(moisture > (wetValue + intervals) && moisture < (dryValue - intervals))
  {
    moisture_category = "Wet";
    moisture_level = 2;
  }
  else if(moisture < dryValue && moisture > (dryValue - intervals))
  {
    moisture_category = "Dry";
    moisture_level = 1;
  }

  // print moisture levels  
  lcd.setCursor(0, 1);  
  lcd.print(moisture_category);  
  delay(3000);
  lcd.clear();
}

int readSensor() {  
  // read analog value from sensor
  int val = analogRead(sensorPin);
  return val;
}
