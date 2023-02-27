#include <LCD_I2C.h>

#define sensorPin A0

LCD_I2C lcd(0x27);  // default LCD I2C address

const int dryValue = 634;
const int wetValue = 321;
int intervals = (dryValue - wetValue)/3;

void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
}

void loop() {  
  delay(3000);  // delay between measurements
  int moisture = readSensor();
  float moisture_percent = map(moisture, 634, 321, 0, 100);
  char moisture_category;
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
  lcd.print(F("Level="));
  lcd.print(moisture_level);  
  delay(3000);
  lcd.clear();
}

int readSensor() {  
  // read analog value from sensor
  int val = analogRead(sensorPin);
  return val;
}
