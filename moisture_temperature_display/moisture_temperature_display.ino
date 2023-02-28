#include <LCD_I2C.h>
#include <DHT.h>
// #include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

#define sensorPin A0
#define DHTPIN 2
#define DHTTYPE DHT22

LCD_I2C lcd(0x27);  // default LCD I2C address
DHT dht(DHTPIN, DHTTYPE);  // initialize DHT sensor
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085); //Initialize address for BMP180

const int dryValue = 634; // moisture sensor dry value
const int wetValue = 321; // moisture sensor wet value
int intervals = (dryValue - wetValue)/3;  // intervals very wet, wet, dry
const float SLevelPressure = 1012;  // sea level pressure in hPa

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin();
  lcd.backlight();
}

void loop() {  
  
  delay(3000);  // delay between measurements
  // temperature sensor
  float cT = dht.readTemperature();
  float h = dht.readHumidity();
  float fT = dht.readTemperature(true);
  // check read failure of DHT sensor
  if (isnan(cT) || isnan(fT) || isnan(h)){
    lcd.print(F("DHT Failure"));    
  }
  else{
    lcd.print(cT, 1);
    lcd.print("C, ");
    lcd.print(h, 1);
    lcd.print("%");
  }
  
  // pressure sensor
  float cP = readPressure();
  if (cP < 0) {
    lcd.print("P?")
  }
  else  {
    lcd.print(cP, 0)
    lcd.print("hPa")
  }

  // soil moisture sensor
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

float readPressure() {
  /* Get a new sensor event */
  sensors_event_t event;
  bmp.getEvent(&event);
  float pressure

  if (event.pressure)  {
    pressure = event.pressure;  // read pressure from BMP180    
    return pressure;
  }
  else  {    
    return -1;
  }
}
