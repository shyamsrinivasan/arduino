#include <Wire.h>
#include <Adafruit_Sensor.h>
#include<Adafruit_BMP085_U.h>
#include <LCD_I2C.h>

/*To assign a unique ID, simply
   provide an appropriate value in the constructor below */

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085); /*0x77*/
LCD_I2C lcd(0x27);

void setup() 
{
  Serial.begin(9600);
  Serial.println("Pressure Sensor Test"); Serial.println("");
  
  lcd.begin(); // If you are using more I2C devices using the Wire library use lcd.begin(false)
  // this stop the library(LCD_I2C) from calling Wire.begin()
  lcd.backlight();
  
  /* Initialise the sensor */
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP180 detected ... Check your wiring or I2C ADDR!");
    lcd.print("Ooops! No BMP180");
    while(1);
  }  
}

void loop() 
{
  lcd.clear();
  
  /* Get a new sensor event */ 
  sensors_event_t event;
  bmp.getEvent(&event);

  if (event.pressure)
  {   
    /* Calculating altitude with reasonable accuracy requires pressure    *
     * sea level pressure for your position at the moment the data is     *
     * converted, as well as the ambient temperature in degress           *
     * celcius.  If you don't have these values, a 'generic' value of     *
     * 1013.25 hPa can be used (defined as SENSORS_PRESSURE_SEALEVELHPA   *
     * in sensors.h), but this isn't ideal and will give variable         *
     * results from one day to the next.                                  *
     *                                                                    *
     * You can usually find the current SLP value by looking at weather   *
     * websites or from environmental information centers near any major  *
     * airport.                                                           *
     *                                                                    *
     * For example, for Paris, France you can check the current mean      *
     * pressure and sea level at: http://bit.ly/16Au8ol                   */
     
    /* First we get the current temperature from the BMP085 */
    float temperature;
    bmp.getTemperature(&temperature);
    /* Then convert the atmospheric pressure, and SLP to altitude         */
    /* Update this next line with the current SLP for better results      */
    float seaLevelPressure = 1009;
    
    /* Display atmospheric pressue in hPa, current atmospheric temperature and Altitude */
    Serial.print("Sea level pressure:   ");
    Serial.print(seaLevelPressure);
    Serial.print(" hPa\n");
    Serial.print("Pressure:    ");
    Serial.print(event.pressure);
    Serial.println(" hPa");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");   
    Serial.print("Altitude:    "); 
    Serial.print(bmp.pressureToAltitude(seaLevelPressure, event.pressure)); 
    Serial.println(" m");
    Serial.println("");

    lcd.print(event.pressure);
    lcd.print(F("hPa"));
    lcd.setCursor(0,1);
    /*lcd.print(altitude);
    lcd.print(F("m"));*/
  }
  else
  {
    Serial.println("Sensor error");
    lcd.print("Sensor error");
  }
  delay(5000);

}
