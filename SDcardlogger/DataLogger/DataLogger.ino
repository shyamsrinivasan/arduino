#include <DS3231.h>
#include <Wire.h>
#include <DHT.h>
#include <SD.h>

#define DHTPIN 2
#define DHTTYPE DHT22

//Initialize DS3231 clock
DS3231 clock;
bool century = false;
bool h12Flag;
bool pmFlag;

//SD card CS pin connected to pin 4 of Arduino
const int chipSelect = 4; 

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

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

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(clock.getYear(), DEC); //Store year on SD card
    dataFile.print(","); //Move to next column using a ","

    dataFile.print(clock.getMonth(century)); //Store month on SD card
    dataFile.print(","); //Move to next column using a ","

    dataFile.print(clock.getDate()); //Store date on SD card
    dataFile.print(","); //Move to next column using a ","

    dataFile.print(clock.getHour(h12Flag, pmFlag), DEC); //store hour on SD card
    dataFile.print(",");
    dataFile.print(clock.getMinute(), DEC);   //store minute on SD card
    dataFile.print(",");
    dataFile.print(clock.getSecond(), DEC);   //store second on SD card
    dataFile.print(",");

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    dataFile.print(t); //Store date on SD card
    dataFile.print(","); //Move to next column using a ","

    dataFile.print(h); //Store date on SD card
    dataFile.print(","); //Move to next column using a ","

    // Compute heat index in Celsius (isFahreheit = false)
    dataFile.print(dht.computeHeatIndex(t, h, false));  //Store date on SD card
    dataFile.print(","); //Move to next column using a ","

    dataFile.println(); //End of Row move to next row
    dataFile.close(); //Close the file
  }
  else
  Serial.println("OOPS!! SD card writing failed");
}

void setup() {
  // Setup Serial connection
  Serial.begin(9600);
  dht.begin();
  Initialize_SDcard();
  Initialize_RTC();
  // PLX-DAQ data logging to excel
  //Initialize_PlxDaq();
}

void loop() {
  readDHT22();
  Write_SDcard();
  delay(5000);
}

void Initialize_RTC()
{
  Wire.begin();
}

// Read DHT sensir data
void readDHT22(){
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

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C ")); 
  Serial.println();
}
