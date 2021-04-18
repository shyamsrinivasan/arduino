#include <Wire.h>
#include <TimeLib.h>
#include <DS3231.h>
#include <DS1307RTC.h>
#include <DHT.h>
#include <SD.h>

#define DHTPIN 2
#define DHTTYPE DHT22

//Initialize DS1307 clock
RTClib rtc;

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

// Initialize SD card module
void Initialize_SDcard()
{
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("LoggerCD.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println("Year,Month,Date,Hour,Minute,Second,Temperature,Humidity,Heat Index"); //Write the first row of the excel file
    dataFile.close();
  }
}

//Write data to SD card
void Write_SDcard()
{
    // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("LoggerCD.txt", FILE_WRITE);

  // read current date and time
  DateTime now = rtc.now();

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  
  // if the file is available, write to it:
  if (dataFile) {
     //Store year on SD card
    dataFile.print(now.year(), DEC);
    dataFile.print(","); //Move to next column using a ","

    //Store month on SD card
    dataFile.print(now.month(), DEC); 
    dataFile.print(","); 

    //Store date on SD card
    dataFile.print(now.day(), DEC); 
    dataFile.print(","); 

    //store hour on SD card
    dataFile.print(now.hour(), DEC); 
    dataFile.print(",");
    //store minute on SD card
    dataFile.print(now.minute(), DEC);   
    dataFile.print(",");
    //store second on SD card
    dataFile.print(now.second(), DEC);   
    dataFile.print(",");    

    dataFile.print(t); //Store date on SD card
    dataFile.print(","); //Move to next column using a ","

    dataFile.print(h); //Store date on SD card
    dataFile.print(","); //Move to next column using a ","

    // Compute heat index in Celsius (isFahreheit = false)
    dataFile.print(dht.computeHeatIndex(t, h, false));  //Store date on SD card
    dataFile.print(","); //Move to next column using a ","

    dataFile.println(); //End of Row move to next row
    dataFile.close(); //Close the file

    Serial.println("Data Write successful");
  }
  else {    
    Serial.println("OOPS!! SD card writing failed");
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(", ");    
    Serial.print(now.hour(), DEC);
    Serial.print(":");
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print(",");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(",");
    Serial.print("RH: ");
    Serial.print(h);
    Serial.println();    
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  Initialize_SDcard();
  Wire.begin();
  // while (!Serial) ; // wait for serial

}

void loop() {
  readDHT22();
  Write_SDcard();
  delay(5000);  
}


// Read DHT sensor data
void readDHT22(){
  // Wait a few seconds between measurements.
  delay(5000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  Serial.println("Sensor read successful");

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Serial.print(F("Humidity: "));
  // Serial.print(h);
  // Serial.print(F("%  Temperature: "));
  // Serial.print(t);
  // Serial.print(F("°C ")); 
  // Serial.println();
}
